#include "hx1838.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_exti.h"
#include "misc.h"
#include "delay.h"

// 红外接收状态变量
static uint32_t ir_data = 0;        /* 存储接收到的32位数据 */
static uint8_t  ir_bit_count = 0;   /* 已接收位计数器 */
static uint8_t  ir_state = 0;       /* 接收状态机状态 */
static uint32_t last_time = 0;      /* 上一次中断时间戳 */
static uint8_t  key_value = KEY_NONE;/* 解码后的按键值 */
static uint8_t  key_ready = 0;      /* 按键就绪标志，1表示有新按键 */

/**
 * @brief  计算两个时间戳的差值（微秒）
 */
static uint32_t get_time_diff(uint32_t now, uint32_t last) {
    if (now >= last) {
        return now - last;
    } else {
        // 处理定时器溢出（32位计数器）
        return (0xFFFFFFFF - last + now);
    }
}

/**
 * @brief  EXTI中断服务程序（红外接收信号边沿触发）
 */
void EXTI1_IRQHandler(void) {
    if (EXTI_GetITStatus(HX1838_EXTI_LINE) != RESET) {
        uint32_t now = TIM4->CNT;               /* 读取TIM4计数器值（微秒级） */
        uint32_t diff = get_time_diff(now, last_time);
		uint8_t level;
        last_time = now;

        // 读取当前接收脚的电平
        level = GPIO_ReadInputDataBit(HX1838_PORT, HX1838_PIN);

        // 解析NEC协议红外信号
        switch (ir_state) {
            case 0:     // 等待引导码低电平（9ms）
                if (!level && diff > 8500 && diff < 9500) {
                    ir_state = 1;  // 进入引导码高电平等待状态
                }
                break;
            
            case 1:     // 等待引导码高电平（4.5ms）
                if (level && diff > 4000 && diff < 5000) {
                    ir_state = 2;          // 进入数据接收状态
                    ir_data = 0;           // 重置数据缓存
                    ir_bit_count = 0;      // 重置位计数器
                } else {
                    ir_state = 0;          // 信号异常，重置状态
                }
                break;
            
            case 2:     // 接收数据位（560us低电平 + 数据位高电平）
                if (!level) {
                    break;  // 忽略低电平阶段
                }
                
                // 根据高电平持续时间判断数据位（NEC协议）
                if (diff > 1000) {         // 1680us高电平 -> 数据1
                    ir_data = (ir_data << 1) | 1;
                    ir_bit_count++;
                } else if (diff > 300) {   // 560us高电平 -> 数据0
                    ir_data = (ir_data << 1) | 0;
                    ir_bit_count++;
                } else {
                    ir_state = 0;          // 信号异常，重置状态
                    break;
                }
                
                // 32位数据接收完成（地址8位 + 地址反8位 + 命令8位 + 命令反8位）
                if (ir_bit_count >= 32) {
                    uint8_t addr = (ir_data >> 24) & 0xFF;
                    uint8_t addr_inv = (ir_data >> 16) & 0xFF;
                    uint8_t cmd = (ir_data >> 8) & 0xFF;
                    uint8_t cmd_inv = ir_data & 0xFF;
                    
                    // 验证地址和命令的反码（确保接收正确）
                    if ((addr == (uint8_t)~addr_inv) && (cmd == (uint8_t)~cmd_inv)) {
                        key_value = cmd;    // 存储有效命令
                        key_ready = 1;      // 置位按键就绪标志
                    }
                    ir_state = 0;          // 重置状态机
                }
                break;
        }

        EXTI_ClearITPendingBit(HX1838_EXTI_LINE);  // 清除中断标志
    }
}

/**
 * @brief  初始化TIM4为微秒级计数器（用于计时）
 * @note   使用TIM4的计数器功能，不使用特定通道的输出比较/输入捕获
 */
static void TIM4_Init(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  // 使能TIM4时钟
    
    // 配置TIM4：72MHz / 72 = 1MHz计数频率（1us每计数）
    TIM_TimeBaseStructure.TIM_Period = 0xFFFFFFFF;        // 计数范围（0~65535）
    TIM_TimeBaseStructure.TIM_Prescaler = 71;         // 预分频器
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  // 向上计数
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    
    TIM_Cmd(TIM4, ENABLE);  // 启动定时器
}

/**
 * @brief  初始化HX1838红外接收模块
 * @note   配置GPIO为输入、EXTI中断为双边沿触发、初始化TIM4定时器
 */
void HX1838_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    TIM4_Init();  // 初始化TIM4作为计时计数器
    
    // 使能GPIO和AFIO时钟
    RCC_APB2PeriphClockCmd(HX1838_RCC | RCC_APB2Periph_AFIO, ENABLE);
    
    // 配置GPIO为上拉输入
    GPIO_InitStructure.GPIO_Pin = HX1838_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HX1838_PORT, &GPIO_InitStructure);
    
    // 配置EXTI中断映射
    GPIO_EXTILineConfig(HX1838_EXTI_PORT, HX1838_EXTI_PIN);
    
    // 配置EXTI中断参数
    EXTI_InitStructure.EXTI_Line = HX1838_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;       // 中断模式
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  // 双边沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    // 配置NVIC中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = HX1838_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;         // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief  获取红外遥控按键值
 * @return 有效按键值（KEY_xxx），无按键时返回KEY_NONE
 */
uint8_t HX1838_GetKey(void) {
    if (key_ready) {
        key_ready = 0;  // 清除就绪标志
        return key_value;
    }
    return KEY_NONE;
}
