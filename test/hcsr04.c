#include "hcsr04.h"
#include "stm32f10x.h"
#include <stddef.h>

static HCSR04_HandleTypeDef* g_hcsr04 = NULL;

void HCSR04_Init(HCSR04_HandleTypeDef* hcsr04) {
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    // 使能GPIO时钟
    if (hcsr04->TRIG_GPIOx == GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    else if (hcsr04->TRIG_GPIOx == GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    else if (hcsr04->TRIG_GPIOx == GPIOC) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    if (hcsr04->ECHO_GPIOx == GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    else if (hcsr04->ECHO_GPIOx == GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    else if (hcsr04->ECHO_GPIOx == GPIOC) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    // 使能定时器时钟
    if (hcsr04->TIMx == TIM1) RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    else if (hcsr04->TIMx == TIM2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    else if (hcsr04->TIMx == TIM3) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    else if (hcsr04->TIMx == TIM4) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    
    // 配置TRIG引脚(推挽输出)
    GPIO_InitStructure.GPIO_Pin = hcsr04->TRIG_GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(hcsr04->TRIG_GPIOx, &GPIO_InitStructure);
    
    // 配置ECHO引脚(上拉输入)
    GPIO_InitStructure.GPIO_Pin = hcsr04->ECHO_GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(hcsr04->ECHO_GPIOx, &GPIO_InitStructure);
    
    // 初始化TRIG为低电平
    GPIO_ResetBits(hcsr04->TRIG_GPIOx, hcsr04->TRIG_GPIO_Pin);
    
    // 配置定时器(1us计数)
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(hcsr04->TIMx, &TIM_TimeBaseStructure);
    
    // 启动定时器
    TIM_Cmd(hcsr04->TIMx, ENABLE);
    
    // 初始化参数
    hcsr04->timeout_us = 30000;
    hcsr04->distance = 0.0f;
}

uint8_t HCSR04_Measure(HCSR04_HandleTypeDef* hcsr04) {
    uint32_t start_time, end_time, duration;
    uint32_t i;
    
    // 发送10us触发脉冲
    GPIO_SetBits(hcsr04->TRIG_GPIOx, hcsr04->TRIG_GPIO_Pin);
    for (i = 0; i < 720; i++) __NOP();
    GPIO_ResetBits(hcsr04->TRIG_GPIOx, hcsr04->TRIG_GPIO_Pin);
    
    // 等待ECHO上升沿
    start_time = 0;
    while (GPIO_ReadInputDataBit(hcsr04->ECHO_GPIOx, hcsr04->ECHO_GPIO_Pin) == 0) {
        if (start_time++ > hcsr04->timeout_us) {
            hcsr04->distance = -1.0f;
            return 1;
        }
        while (hcsr04->TIMx->CNT < 1);
        hcsr04->TIMx->CNT = 0;
    }
    
    // 记录上升沿时间
    start_time = hcsr04->TIMx->CNT;
    
    // 等待ECHO下降沿
    while (GPIO_ReadInputDataBit(hcsr04->ECHO_GPIOx, hcsr04->ECHO_GPIO_Pin) == 1) {
        if (hcsr04->TIMx->CNT - start_time > hcsr04->timeout_us) {
            hcsr04->distance = -1.0f;
            return 1;
        }
    }
    
    // 计算持续时间(us)
    end_time = hcsr04->TIMx->CNT;
    duration = end_time - start_time;
    
    // 计算距离(厘米)
    hcsr04->distance = (duration * 0.03432f) / 2.0f;
    
    return 0;
}

float HCSR04_GetDistance(HCSR04_HandleTypeDef* hcsr04) {
    return hcsr04->distance;
}

void HCSR04_InitTimerMeasure(HCSR04_HandleTypeDef* hcsr04) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    g_hcsr04 = hcsr04;
    
    // 使能中断定时器时钟
    if (hcsr04->TIMx_NVIC == TIM1) RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    else if (hcsr04->TIMx_NVIC == TIM2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    else if (hcsr04->TIMx_NVIC == TIM3) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    else if (hcsr04->TIMx_NVIC == TIM4) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    
    // 配置定时器为100ms中断一次
    TIM_TimeBaseStructure.TIM_Period = 9999;
    TIM_TimeBaseStructure.TIM_Prescaler = 719;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(hcsr04->TIMx_NVIC, &TIM_TimeBaseStructure);
    
    // 使能定时器更新中断
    TIM_ITConfig(hcsr04->TIMx_NVIC, TIM_IT_Update, ENABLE);
    
    // 配置中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = (hcsr04->TIMx_NVIC == TIM1) ? TIM1_UP_IRQn :
                                         (hcsr04->TIMx_NVIC == TIM2) ? TIM2_IRQn :
                                         (hcsr04->TIMx_NVIC == TIM3) ? TIM3_IRQn : TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // 启动定时器
    TIM_Cmd(hcsr04->TIMx_NVIC, ENABLE);
}

// 定时器中断服务函数（需与启动文件中的中断向量对应）
void TIM4_IRQHandler(void) {
    TIMx_NVIC_IRQHandler();
}

void TIMx_NVIC_IRQHandler(void) {
    if (g_hcsr04 == NULL) return;
    
    if (TIM_GetITStatus(g_hcsr04->TIMx_NVIC, TIM_IT_Update) != RESET) {
        HCSR04_Measure(g_hcsr04);
        TIM_ClearITPendingBit(g_hcsr04->TIMx_NVIC, TIM_IT_Update);
    }
}
    

