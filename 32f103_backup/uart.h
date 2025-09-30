#ifndef __UART_H
#define __UART_H

#include <stdio.h>
#include <string.h>

extern volatile int flag2 ;
extern volatile int flag3;
extern volatile int flag4 ;
extern volatile int flag5 ;
extern volatile int flag6 ;
extern volatile int flag7 ;
void uart_init(int BaudRate);
void uart_send_byte(char data);
void uart_send_str(char *str);


unsigned char uart_recv_byte(void);
void uart_get_recvbuf(unsigned char *buf);

#endif
