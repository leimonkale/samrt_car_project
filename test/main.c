#include "stm32f10x.h"
#include "hcsr04.h"
#include "usart.h"
#include <stdio.h>

// 定义超声波模块结构体实例
HCSR04_HandleTypeDef hcsr04;

// 延时函数（简单实现）
void Delay_ms(uint32_t ms) {
    uint32_t i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 7200; j++);
}

int main(void) {
    // 初始化系统时钟（72MHz）
    SystemInit();
    
    // 初始化串口（波特率115200）
    USART1_Init(115200);
    
    // 配置超声波模块参数
    hcsr04.TRIG_GPIOx = GPIOB;
    hcsr04.TRIG_GPIO_Pin = GPIO_Pin_15;
    hcsr04.ECHO_GPIOx = GPIOB;
    hcsr04.ECHO_GPIO_Pin = GPIO_Pin_14;
    hcsr04.TIMx = TIM2;          // 用于计时的定时器
    hcsr04.TIMx_NVIC = TIM4;     // 用于定时中断的定时器
    hcsr04.timeout_us = 30000;   // 超时时间30ms
    
    // 初始化超声波模块
    HCSR04_Init(&hcsr04);
    
    // 初始化定时测量（每100ms自动测量一次）
    HCSR04_InitTimerMeasure(&hcsr04);
    
    USART1_SendStr("HC-SR04 Test Start...\r\n");
    
    // 主循环
    while (1) {
        char buf[50];
        // 格式化距离信息
        sprintf(buf, "Distance: %.2f cm\r\n", HCSR04_GetDistance(&hcsr04));
        // 发送到串口
        USART1_SendStr(buf);
        // 延时500ms
        Delay_ms(500);
    }
}
    

