#include "delay.h"
#include "stm32f10x.h"  

void my_delay_ms(int ms)
{
    int i, j;
    for(i=0; i<ms; i++)
        for(j=0; j<8050; j++);
}

void systick_init(void)
{
    //systick 24位 最大计数值: 16,777,215

//    SysTick_CLKSourceConfig 设置 SysTick 时钟源
    //SysTick 时钟源为 AHB 时钟/8 ( 72MHz/8 = 9MHz )  
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    //SysTick->CTRL |= (1<<2);  //SysTick 时钟源为 AHB 时钟
    
    SysTick->CTRL &= ~(1<<1); //倒计数到0后, 不产生中断
    
}


void systick_delay_us(int us)
{
    SysTick->LOAD = 9*us; //装载数
    SysTick->VAL = 0;
    SysTick->CTRL |= (1<<0); //开始倒计数
    while( (SysTick->CTRL & (1<<16)) == 0); //等待数到0  等待时间到达
    SysTick->CTRL &= ~(1<<16);
}

//ms: 0~1864
void systick_delay_ms(int ms)
{
    SysTick->LOAD = 9000*ms; //装载数
    SysTick->VAL = 0;
    SysTick->CTRL |= (1<<0); //开始倒计数
    //等待数到0  等待时间到达
    while( (SysTick->CTRL & (1<<16)) == 0);
    SysTick->CTRL &= ~(1<<16);
}

