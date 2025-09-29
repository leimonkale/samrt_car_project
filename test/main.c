#include "stm32f10x.h"
#include "string.h"
#include "stdio.h"

// 全局变量：接收缓冲区及标志
uint8_t recv_buf[64] = {0};
uint8_t recv_len = 0;
uint8_t recv_complete = 0;

// 初始化PA9(TX)和PA10(RX)为USART1功能
void uart_init(uint32_t baudrate) {
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);

    // 配置PA9为复用推挽输出（TX）
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 配置PA10为上拉输入（RX）
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入，确保空闲电平稳定
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 配置USART1
    USART_InitStruct.USART_BaudRate = baudrate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStruct);

    // 使能接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // 配置中断优先级
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    // 使能USART1
    USART_Cmd(USART1, ENABLE);
}

// 发送一个字节
void uart_send_byte(uint8_t data) {
    USART_SendData(USART1, data);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

// 发送字符串
void uart_send_str(uint8_t *str) {
    while (*str) {
        uart_send_byte(*str++);
    }
}

// USART1中断服务函数：接收数据并回传
void USART1_IRQHandler(void) {
    uint8_t data;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
        data = USART_ReceiveData(USART1);
        
        // 接收到回车换行时，标记接收完成并发送反馈
        if (data == '\n' || data == '\r') {
            recv_buf[recv_len] = '\0';  // 字符串结束符
            uart_send_str((uint8_t*)"收到: ");
            uart_send_str(recv_buf);
            uart_send_str((uint8_t*)"\r\n");
            
            // 重置缓冲区
            recv_len = 0;
            memset(recv_buf, 0, sizeof(recv_buf));
        } else {
            // 存储数据（防止溢出）
            if (recv_len < 63) {
                recv_buf[recv_len++] = data;
            }
        }
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

int main(void) {
    // 初始化UART（波特率115200）
    uart_init(115200);
    
    // 发送测试提示
    uart_send_str((uint8_t*)"PA10测试开始，请发送数据...\r\n");
    
    while (1) {
        // 主循环无需处理，所有逻辑在中断中完成
    }
}

// 若需要使用标准库printf，可添加以下代码（需勾选MicroLIB）
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE {
    uart_send_byte((uint8_t)ch);
    return ch;
}

