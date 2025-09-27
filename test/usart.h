#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"

void USART1_Init(uint32_t bound);
void USART1_SendByte(uint8_t data);
void USART1_SendStr(char *str);

#endif /* __USART_H */
    
	

