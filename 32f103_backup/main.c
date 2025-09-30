#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "motor.h"
#include "uart.h"
#include "hcsr04.h"
#include "engin.h"

int flag_motor1 = 0;
int flag_motor2 = 0;

Motor_HandleTypeDef motor1, motor2;
HCSR04_HandleTypeDef hcsr04;
/**
 * @brief  初始化超声波模块结构体
 */
void Hcsr04_Init(void)
{
	hcsr04.TIMx = TIM2;
	hcsr04.ECHO_GPIOx = GPIOB;
	hcsr04.ECHO_GPIO_Pin = GPIO_Pin_14;
	hcsr04.TRIG_GPIOx = GPIOB;
	hcsr04.TRIG_GPIO_Pin = GPIO_Pin_15;
	
	HCSR04_Init(&hcsr04);
}

/**
 * @brief  初始化电机1结构体
 */
void Motor1_Init(void) {
    motor1.TIMx = TIM3;
    motor1.TIM_Channel = TIM_Channel_1;
	motor1.PWM_GPIOx = GPIOA;
	motor1.PWM_GPIO_Pin = GPIO_Pin_6;
    motor1.IN1_GPIOx = GPIOB;
    motor1.IN1_GPIO_Pin = GPIO_Pin_6;
    motor1.IN2_GPIOx = GPIOB;
    motor1.IN2_GPIO_Pin = GPIO_Pin_7;
	motor1.dir = MOTOR_FORWARD;
	motor1.speed = 70;
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
	motor2.PWM_GPIOx = GPIOA;
	motor2.PWM_GPIO_Pin = GPIO_Pin_7;
    motor2.IN1_GPIOx = GPIOB;
    motor2.IN1_GPIO_Pin = GPIO_Pin_8;
    motor2.IN2_GPIOx = GPIOB;
    motor2.IN2_GPIO_Pin = GPIO_Pin_9;
	motor2.dir = MOTOR_FORWARD;
	motor2.speed = 70;
    Motor_Init(&motor2);
}

//设置舵机
//TIM_SetCompare1(TIM1, 2000);
int main()
{
//	int i = 0;
	int flag1 = 1;
	char buf[128];
	Hcsr04_Init();
	Motor1_Init();
	Motor2_Init();
	gpio_init();
	tim1_init();
		
	uart_init(115200);
	
	TIM_SetCompare1(TIM1, 1500);
	
	while(1){
		HCSR04_Measure(&hcsr04);
		sprintf(buf,"模式运行中...%d %d %d %d %d %d\n",flag2,flag3,flag4,flag5,flag6,flag7);
		uart_send_str(buf);
		sprintf(buf, "Distance: %.2f cm\r\n", hcsr04.distance);
		//sprintf(buf,"%d",uart_recv_byte());
		uart_send_str(buf);
		if(hcsr04.distance < 10 && hcsr04.distance > 0){
			Motor_Stop(&motor1);
			Motor_Stop(&motor2);
			my_delay_ms(1000);
			TIM_SetCompare1(TIM1, 500);
			my_delay_ms(1000);
			HCSR04_Measure(&hcsr04);
			if(hcsr04.distance < 10 && hcsr04.distance > 0){
				my_delay_ms(1000);
				TIM_SetCompare1(TIM1, 2500);
				my_delay_ms(1000);
				HCSR04_Measure(&hcsr04);
				if(hcsr04.distance < 10 && hcsr04.distance > 0){
					motor1.dir = MOTOR_BACKWARD;
					motor2.dir = MOTOR_BACKWARD;
					Motor_Start(&motor1);
					Motor_Start(&motor2);
					my_delay_ms(3000);
					Motor_Stop(&motor1);
					Motor_Stop(&motor2);
					my_delay_ms(1000);
					HCSR04_Measure(&hcsr04);
					if(hcsr04.distance < 10 && hcsr04.distance > 0){
						
					}else{
						TIM_SetCompare1(TIM1, 1500);
						motor1.dir = MOTOR_BACKWARD;
						motor2.dir = MOTOR_FORWARD;
						Motor_Start(&motor1);
						Motor_Start(&motor2);
						my_delay_ms(1500);
						Motor_SetDirection(&motor1,MOTOR_FORWARD);
					}
				}
				else{
					TIM_SetCompare1(TIM1, 1500);
					motor1.dir = MOTOR_BACKWARD;
					motor2.dir = MOTOR_FORWARD;
					Motor_Start(&motor1);
					Motor_Start(&motor2);
					my_delay_ms(1500);
					Motor_SetDirection(&motor1,MOTOR_FORWARD);
				}
			
			}
			else{
				TIM_SetCompare1(TIM1, 1500);
				motor1.dir = MOTOR_FORWARD;
				motor2.dir = MOTOR_BACKWARD;
				Motor_Start(&motor1);
				Motor_Start(&motor2);
				my_delay_ms(1500);
				Motor_SetDirection(&motor2,MOTOR_FORWARD);
			}
			
			
		} 
		
		if(flag1){
		/*motor1.dir = MOTOR_FORWARD;
		motor2.dir = MOTOR_FORWARD;
		motor1.speed = 70;
		motor2.speed = 70;
		Motor_Start(&motor1);
	    Motor_Start(&motor2);
		my_delay_ms(5000);
		motor1.dir = MOTOR_BACKWARD;
		motor1.speed = 100;
		motor2.speed = 100;
		Motor_Start(&motor1);
	    Motor_Start(&motor2);
		my_delay_ms(5000);*/
		motor1.dir = MOTOR_FORWARD;
		motor2.dir = MOTOR_FORWARD;
		motor1.speed = 100;
		motor2.speed = 85;
		Motor_Start(&motor1);
	    Motor_Start(&motor2);
		flag1 = 0;
		}
		if(flag2){
			Motor_SetSpeed(&motor1,100);
			Motor_SetSpeed(&motor2,85);
			Motor_Start(&motor1);
			Motor_Start(&motor2);
			flag2 = 0;
			
		}
		if(flag3){
			Motor_Stop(&motor1);
			Motor_Stop(&motor2);
			my_delay_ms(500);
			
			flag3 = 0;
		}
		if(flag4){
			Motor_SetSpeed(&motor2,90);
			flag4 = 0;
		}
		if(flag5){
			
		}
		if(flag6){
			
		}
		if(flag7){}
		
		my_delay_ms(10);
	
	}
}
