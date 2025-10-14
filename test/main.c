#include "stm32f10x.h"

// 红外模块引脚定义（PB3）
#define HX1838_PORT        GPIOB
#define HX1838_PIN         GPIO_Pin_3
#define HX1838_RCC         RCC_APB2Periph_GPIOB
#define HX1838_EXTI_PORT   GPIO_PortSourceGPIOB
#define HX1838_EXTI_PIN    GPIO_PinSource3
#define HX1838_EXTI_LINE   EXTI_Line3
#define HX1838_IRQn        EXTI3_IRQn

// 全局变量：用于记录电平状态和时间
char uart_buf[64];                   // 串口发送缓冲区
static uint32_t last_time = 0;       // 上一次电平跳变的时间戳（微秒）
static uint8_t last_level = 1;       // 上一次的电平状态（初始高电平，HX1838默认空闲高）

// 延时函数相关变量（仅用于主循环防卡死，不影响信号捕获）
static uint8_t fac_us = 0;
static uint16_t fac_ms = 0;

/****************************** 基础延时函数（保留，避免主循环过快） ******************************/
void delay_init(void) {
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    fac_us = SystemCoreClock / 8000000;
    fac_ms = (uint16_t)fac_us * 1000;
}

void delay_ms(uint32_t ms) {
    while (ms--) {
        uint32_t temp;
        SysTick->LOAD = 1000 * fac_us;
        SysTick->VAL = 0x00;
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
        do { temp = SysTick->CTRL; } while ((temp & 0x01) && !(temp & (1 << 16)));
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
        SysTick->VAL = 0x00;
    }
}

/****************************** 串口函数（仅用于打印，波特率115200） ******************************/
void USART1_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    // 使能USART1和GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // 配置TX引脚（PA9，复用推挽）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置串口参数（115200波特率，8位数据，1停止位，无校验）
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx; // 仅发送模式（足够打印信息）
    USART_Init(USART1, &USART_InitStructure);

    // 使能串口
    USART_Cmd(USART1, ENABLE);
}

// 串口发送字符串（简化版，确保能打印）
void USART1_Print(char *str) {
    while (*str) {
        USART_SendData(USART1, (uint16_t)*str);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); // 等待发送完成
        str++;
    }
}

/****************************** TIM4定时器（仅用于时间戳，1us精度） ******************************/
void TIM4_Init(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    // 使能TIM4时钟（APB1总线，72MHz）
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    // 配置TIM4：72MHz / 72 = 1MHz（1us计数1次），计数范围最大（避免频繁溢出）
    TIM_TimeBaseStructure.TIM_Period = 0xFFFFFFFF; // 16位定时器实际最大0xFFFF，但这样写减少溢出
    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    // 启动定时器
    TIM_Cmd(TIM4, ENABLE);
}

// 获取当前时间戳（微秒）
uint32_t Get_Timestamp(void) {
    return TIM4->CNT;
}

/****************************** PB3引脚+EXTI中断配置（仅捕获电平跳变） ******************************/
void PB3_Exti_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 1. 使能GPIO和AFIO时钟（AFIO用于EXTI中断映射）
    RCC_APB2PeriphClockCmd(HX1838_RCC | RCC_APB2Periph_AFIO, ENABLE);

    // 2. 配置PB3为上拉输入（HX1838空闲时为高电平，有信号时跳变）
    GPIO_InitStructure.GPIO_Pin = HX1838_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入，确保空闲高电平
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HX1838_PORT, &GPIO_InitStructure);

    // 3. 配置EXTI中断映射（PB3 -> EXTI3）
    GPIO_EXTILineConfig(HX1838_EXTI_PORT, HX1838_EXTI_PIN);

    // 4. 配置EXTI3为双边沿触发（上升沿+下降沿，捕获所有电平跳变）
    EXTI_InitStructure.EXTI_Line = HX1838_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // 关键：双边沿
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // 5. 配置NVIC中断（优先级别设高，避免被其他中断抢占）
    NVIC_InitStructure.NVIC_IRQChannel = HX1838_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 最高抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 初始化时间戳和初始电平
    last_time = Get_Timestamp();
    last_level = GPIO_ReadInputDataBit(HX1838_PORT, HX1838_PIN);
    // 打印初始状态
    sprintf(uart_buf, "Init: PB3 Level = %d, Timestamp = %lu us\r\n", last_level, last_time);
    USART1_Print(uart_buf);
}

/****************************** EXTI3中断服务函数（核心：捕获电平跳变并打印） ******************************/
void EXTI3_IRQHandler(void) {
    // 确保是PB3触发的中断（防止其他中断线干扰）
    if (EXTI_GetITStatus(HX1838_EXTI_LINE) != RESET) {
        uint32_t current_time = Get_Timestamp(); // 当前时间戳（微秒）
        uint8_t current_level = GPIO_ReadInputDataBit(HX1838_PORT, HX1838_PIN); // 当前电平
        uint32_t time_diff = current_time - last_time; // 两次跳变的时间差

        // 打印：时间差 + 上一次电平 + 当前电平（直观看到信号变化）
        sprintf(uart_buf, "Diff: %lu us | Last Level: %d -> Current Level: %d\r\n", 
                time_diff, last_level, current_level);
        USART1_Print(uart_buf);

        // 更新“上一次”的时间和电平，为下一次跳变做准备
        last_time = current_time;
        last_level = current_level;

        // 清除中断标志（必须做，否则会重复触发中断）
        EXTI_ClearITPendingBit(HX1838_EXTI_LINE);
    }
}

/****************************** 主函数（仅初始化，无其他逻辑） ******************************/
int main(void) {
    // 1. 配置系统时钟（72MHz，必须先初始化，否则定时器/串口频率错误）
    RCC_HSEConfig(RCC_HSE_ON);
    while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET); // 等待外部晶振稳定
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); // HSE 8MHz *9 =72MHz
    RCC_PLLCmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); // 等待PLL稳定
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); // 系统时钟切换为PLL（72MHz）

    // 2. 初始化基础外设
    delay_init();    // 延时初始化（主循环用）
    USART1_Init();   // 串口初始化（打印用）
    TIM4_Init();     // 定时器初始化（时间戳用）
    PB3_Exti_Init(); // PB3中断初始化（捕获电平用）

    // 3. 主循环：仅打印“等待信号”（证明程序在运行）
    USART1_Print("PB3 Signal Monitor Start! Press IR Remote...\r\n");
    while (1) {
        // 每隔3秒打印一次“程序正常运行”，避免误以为程序卡死
        static uint32_t tick = 0;
        if (tick++ >= 300) { // delay_ms(10) * 300 = 3000ms = 3秒
            USART1_Print("Program running... Wait for PB3 signal\r\n");
            tick = 0;
        }
        delay_ms(10);
    }
}