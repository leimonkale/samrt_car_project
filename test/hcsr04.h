#ifndef __HCSR04_H
#define __HCSR04_H

#include "stm32f10x.h"

// 超声波模块结构体定义
typedef struct {
    GPIO_TypeDef* TRIG_GPIOx;   // 触发引脚GPIO
    uint16_t TRIG_GPIO_Pin;     // 触发引脚
    GPIO_TypeDef* ECHO_GPIOx;   // 回响引脚GPIO
    uint16_t ECHO_GPIO_Pin;     // 回响引脚
    TIM_TypeDef* TIMx;          // 用于计时的定时器
    TIM_TypeDef* TIMx_NVIC;     // 用于中断的定时器
    uint32_t timeout_us;        // 超时时间(微秒)
    float distance;             // 当前距离(厘米)
} HCSR04_HandleTypeDef;

// 函数声明
void HCSR04_Init(HCSR04_HandleTypeDef* hcsr04);
uint8_t HCSR04_Measure(HCSR04_HandleTypeDef* hcsr04);
float HCSR04_GetDistance(HCSR04_HandleTypeDef* hcsr04);
void HCSR04_InitTimerMeasure(HCSR04_HandleTypeDef* hcsr04);
void TIMx_NVIC_IRQHandler(void);

#endif /* __HCSR04_H */
    

