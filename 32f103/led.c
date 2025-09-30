#include "stm32f10x.h"      //右键插入可以快速选入            // Device header
#include "led.h"
// 50MHz 主频下的粗略延时

void led_init(void){
	GPIO_InitTypeDef init;
	//1、管脚的时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	//管脚的初始化
	init.GPIO_Pin = GPIO_Pin_13 ;   //填写管脚
	init.GPIO_Speed = GPIO_Speed_50MHz;   //填写速率
	init.GPIO_Mode = GPIO_Mode_Out_PP;    //设置工作模式
	GPIO_Init(GPIOC,&init); 
	
	init.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 |GPIO_Pin_0;   //填写管脚
	init.GPIO_Speed = GPIO_Speed_50MHz;   //填写速率
	init.GPIO_Mode = GPIO_Mode_Out_PP;    //设置工作模式
	GPIO_Init(GPIOA,&init); 
	}
	
