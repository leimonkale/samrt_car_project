#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "motor.h"
#include "uart.h"
#include "hcsr04.h"

Motor_HandleTypeDef motor1, motor2;
HCSR04_HandleTypeDef hcsr04;
/**
 * @brief  初始化超声波模块结构体
 */
void Hcsr04_Init(void)
{
	hcsr04.TIMx = TIM2;
	hcsr04.ECHO_GPIOx = GPIOA;
	hcsr04.ECHO_GPIO_Pin = GPIO_Pin_4;
	hcsr04.TRIG_GPIOx = GPIOA;
	hcsr04.TRIG_GPIO_Pin = GPIO_Pin_5;
	
	HCSR04_Init(&hcsr04);
}

/**
 * @brief  初始化电机1结构体
 */
void Motor1_Init(void) {
    motor1.TIMx = TIM3;
    motor1.TIM_Channel = TIM_Channel_1;
    motor1.IN1_GPIOx = GPIOA;
    motor1.IN1_GPIO_Pin = GPIO_Pin_0;
    motor1.IN2_GPIOx = GPIOA;
    motor1.IN2_GPIO_Pin = GPIO_Pin_1;
	motor1.dir = MOTOR_FORWARD;
	motor1.speed = 30;
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
	motor2.dir = MOTOR_FORWARD;
	motor2.speed = 30;
    Motor_Init(&motor2);
}

int main()
{
	char buf[128];
	Hcsr04_Init();
	Motor1_Init();
	Motor2_Init();
	
	uart_init(115200);
	
	Motor_Start(&motor1);
	Motor_Start(&motor2);
	
	while(1){
		
		if(flag2){
			
		}
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
