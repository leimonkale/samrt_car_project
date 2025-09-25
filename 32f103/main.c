#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "motor.h"
#include "uart.h"
#include "hcsr04.h"

Motor_HandleTypeDef motor1, motor2;
HCSR04_HandleTypeDef hcsr04;

/**
 * @brief  初始化电机1结构体
 * @param  无
 * @retval 无
 */
void Motor1_Init(void) {
    motor1.TIMx = TIM3;
    motor1.TIM_Channel = TIM_Channel_1;
    motor1.IN1_GPIOx = GPIOA;
    motor1.IN1_GPIO_Pin = GPIO_Pin_0;
    motor1.IN2_GPIOx = GPIOA;
    motor1.IN2_GPIO_Pin = GPIO_Pin_1;
    Motor_Init(&motor1);
}

/**
 * @brief  初始化电机2结构体
 * @param  motor: 电机结构体指针
 * @retval 无
 */
void Motor2_Init(void) {
    motor2.TIMx = TIM3;
    motor2.TIM_Channel = TIM_Channel_2;
    motor2.IN1_GPIOx = GPIOA;
    motor2.IN1_GPIO_Pin = GPIO_Pin_2;
    motor2.IN2_GPIOx = GPIOA;
    motor2.IN2_GPIO_Pin = GPIO_Pin_3;
    Motor_Init(&motor2);
}

int main()
{
	char buf[128];
	Motor1_Init();
	Motor2_Init();
	Motor_Init(&motor1);
	Motor_Init(&motor2);
	
	uart_init(115200);
	
	while(1){
		if(flag2){}
		if(flag3){}
		if(flag4){}
		if(flag5){}
		if(flag6){}
		if(flag7){}
		
		//sprintf(buf,"模式运行中...%d %d %d %d %d %d\n",flag2,flag3,flag4,flag5,flag6,flag7);
		uart_send_str(buf);
	
		my_delay_ms(10);
	
	}
}
