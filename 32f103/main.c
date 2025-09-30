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

/// 定义觅光判断范围
#define FRONT_MIN 1000   // 正前方的最小差值
#define FRONT_MAX 1200 // 正前方的最大差值
#define SAMPLE_TIMES 5 

/// 定义方向枚举
typedef enum {
    DIR_LEFT = 0,       // 向左
    DIR_FRONT,          // 正前方
    DIR_RIGHT           // 向右
} Light_Direction;

/**
 * @brief  DHT11 PB10设置管脚为输入模式
 */
void gpio_setIn()		
{
	GPIO_InitTypeDef init;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	init.GPIO_Pin = GPIO_Pin_10;
	init.GPIO_Speed = GPIO_Speed_50MHz;
	init.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &init);
}
/**
 * @brief  DHT11 PB10设置管脚为输出模式
 */
void gpio_setOut()		
{
	GPIO_InitTypeDef init;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	init.GPIO_Pin = GPIO_Pin_10;
	init.GPIO_Speed = GPIO_Speed_50MHz;
	init.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &init);
}
/**
 * @brief  多采集取平均（减少ADC抖动，采集5次，去极值后平均）
 */
int dht11_read(int *hum, int *tem)
{
	int cnt, i, j;
	char buf[5] = {0};
	
	//设置管脚为输出功能
	gpio_setOut();
	
	//拉低电平，持续 >= 18ms (20ms)
	GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_RESET);
	systick_delay_ms(20);
	
	//拉高电平 30(us)
	GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_SET);
	systick_delay_us(30);
	
	//设置管脚为 输入模式
	gpio_setIn();
	
	//读取管脚电平，判断DHT11是否响应
	if( GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0){
		
		//等待低电平结束
		while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0);
		//等待高电平结束
		while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 1);
		
		for(i = 0; i < 5; i++){		//循环5次读取 5字节数据
			for(j = 7;j >= 0; j--){
				cnt = 0;
				//等待DHT11发送数据时 低电平结束
				while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0);
		
				//读取DHT11发送数据时 高电平的时长
				while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 1){
					cnt++;
					systick_delay_us(10);
				}
				if(cnt > 5){
					buf[i] = buf[i] | (1 << j);
				}
			}
		}
		//校验5字节中前 4字节之和的低 8位 是否等于 第5字节
		if( ((buf[0]+buf[1]+buf[2]+buf[3]) & 0xff) == buf[4]){
			*hum = buf[0];
			*tem = buf[2];
			return 0;
		}
	}
	return -1;
}

/**
 * @brief  多采集取平均（减少ADC抖动，采集5次，去极值后平均）
 */
uint16_t ADC_Get_Average(ADC_TypeDef* ADCx, uint8_t channel, uint8_t times) {
    uint16_t buf[SAMPLE_TIMES];
    uint16_t sum = 0, temp;
	uint8_t i;
	uint8_t j;
    
    // 采集times次数据
    for (i = 0; i < times; i++) {
        ADC_RegularChannelConfig(ADCx, channel, 1, ADC_SampleTime_239Cycles5); // 配置通道
        ADC_SoftwareStartConvCmd(ADCx, ENABLE);                                // 启动转换
        while (ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC) == RESET);                // 等待完成
        buf[i] = ADC_GetConversionValue(ADCx);                                 // 存数据
    }
    
    // 排序（去极值：去掉最大和最小，避免极端噪声）
    for (i = 0; i < times-1; i++) {
        for (j = 0; j < times-1-i; j++) {
            if (buf[j] > buf[j+1]) {
                temp = buf[j];
                buf[j] = buf[j+1];
                buf[j+1] = temp;
            }
        }
    }
    
    // 求和（去掉第一个最小和最后一个最大）
    for (i = 1; i < times-1; i++) {
        sum += buf[i];
    }
    
    // 返回平均值
    return sum / (times - 2);
}
/**
 * @brief  觅光方向输出
 */
Light_Direction Get_Light_Direction(void) {
    uint16_t adc_left, adc_right;
    int32_t diff; // 差值（int32_t避免负数溢出）
    
    // 采集左右ADC值（各采集5次，去极值平均，抗抖动）
    adc_left = ADC_Get_Average(ADC1, ADC_Channel_3, 5);  // 左侧：ADC1_IN3（PA3）
    adc_right = ADC_Get_Average(ADC2, ADC_Channel_5, 5); // 右侧：ADC2_IN7（PA7）
    
    // 计算差值
    diff = adc_right - adc_left;
    
    // 判断方向
    if (diff < FRONT_MIN) {
        return DIR_LEFT;       //  向左
    } else if (diff > FRONT_MAX) {
        return DIR_RIGHT;      //  向右
    } else {
        return DIR_FRONT;      //  正前方
    }
}

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
			Motor_SetSpeed(&motor2,100);
			Motor_Start(&motor1);
			Motor_Start(&motor2);
			flag2 = 0;
			
		}
		if(flag3){
			//Motor_SetSpeed(&motor1,50);
			Motor_Stop(&motor2);
			flag3 = 0;
		}
		if(flag4){
			Motor_Stop(&motor1);
			//Motor_SetSpeed(&motor2,50);
			flag4 = 0;
		}
		if(flag5){
			//Motor_Stop(&motor2);
			flag5 = 0;
		}
		if(flag6){
			Motor_Stop(&motor2);
			Motor_Stop(&motor1);
			my_delay_ms(5000);
			Motor_Start(&motor2);
			my_delay_ms(800);
			flag6 = 0;
		}
		if(flag7){}
		
		my_delay_ms(10);
	
	}
}
