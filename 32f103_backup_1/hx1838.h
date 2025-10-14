#ifndef __HX1838_H
#define __HX1838_H

#include "stm32f10x.h"

// 硬件引脚定义（根据实际电路修改）
#define HX1838_PORT        GPIOA               /* 红外接收引脚端口 */
#define HX1838_PIN         GPIO_Pin_2          /* 红外接收引脚 */
#define HX1838_RCC         RCC_APB2Periph_GPIOA /* 端口时钟 */
#define HX1838_EXTI_PORT   GPIO_PortSourceGPIOA /* EXTI端口源 */
#define HX1838_EXTI_PIN    GPIO_PinSource2      /* EXTI引脚源 */
#define HX1838_EXTI_LINE   EXTI_Line2           /* EXTI中断线 */
#define HX1838_IRQn        EXTI2_IRQn           /* 中断向量号 */

// 红外按键值定义（根据遥控器实际编码修改）
#define KEY_NONE           0xFF    /* 无按键 */
#define KEY_POWER          0x0C    /* 电源键 */
#define KEY_MENU           0x18    /* 菜单键 */
#define KEY_TEST           0x5E    /* 测试键 */
#define KEY_PLUS           0x08    /* 加键 */
#define KEY_MINUS          0x1C    /* 减键 */
#define KEY_NEXT           0x5A    /* 下一曲键 */
#define KEY_PREV           0x42    /* 上一曲键 */
#define KEY_0              0x4A    /* 数字0键 */
#define KEY_1              0x00    /* 数字1键 */
#define KEY_2              0x01    /* 数字2键 */
#define KEY_3              0x02    /* 数字3键 */
#define KEY_4              0x03    /* 数字4键 */
#define KEY_5              0x04    /* 数字5键 */
#define KEY_6              0x05    /* 数字6键 */
#define KEY_7              0x06    /* 数字7键 */
#define KEY_8              0x07    /* 数字8键 */
#define KEY_9              0x09    /* 数字9键 */

/**
 * @brief  初始化HX1838红外接收模块，配置GPIO、EXTI中断和定时器
 */
void HX1838_Init(void);

/**
 * @brief  获取红外遥控器按键值
 * @return 按键值（定义的KEY_xxx），无按键时返回KEY_NONE
 */
uint8_t HX1838_GetKey(void);

#endif

