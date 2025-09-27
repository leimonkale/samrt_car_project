#include "stm32f10x.h" 
#include "engin.h"
void gpio_init(void){
    GPIO_InitTypeDef init;
    // PB5 时钟使能
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    // 复用功能时钟使能（关键：复用推挽输出需开启AFIO时钟）
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    // PB5 引脚初始化（TIM3_CH2）
    init.GPIO_Pin = GPIO_Pin_5;
    init.GPIO_Speed = GPIO_Speed_50MHz;
    init.GPIO_Mode = GPIO_Mode_AF_PP;  // 复用推挽输出（正确）
    GPIO_Init(GPIOB, &init);
}

void tim3_init(void){
    TIM_TimeBaseInitTypeDef BaseInit;
    TIM_OCInitTypeDef OcInit;

    // TIM3 时钟使能
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // 定时器基础配置（改为向上计数，更直观）
    BaseInit.TIM_Prescaler = 71;        // 预分频：72MHz/(71+1)=1MHz（1μs/计数）
    BaseInit.TIM_Period = 19999;         // 周期：2001μs（约500Hz）
    BaseInit.TIM_ClockDivision = TIM_CKD_DIV1; // 不分频
    BaseInit.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数（推荐）
    TIM_TimeBaseInit(TIM3, &BaseInit);

    // PWM配置
    OcInit.TIM_Pulse = 1500;            // 比较值：1000μs高电平（占空比~50%）
    OcInit.TIM_OCMode = TIM_OCMode_PWM1; // PWM1：计数 < CCR 时输出高电平
    OcInit.TIM_OCPolarity = TIM_OCPolarity_High; // 高电平有效
    OcInit.TIM_OutputState = ENABLE;    // 使能输出
    TIM_OC1Init(TIM3, &OcInit);

    // 使能预装载（稳定PWM输出）
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);

    TIM_Cmd(TIM3, ENABLE);              // 启动定时器
}



