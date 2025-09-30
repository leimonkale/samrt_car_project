#ifndef __DELAY_H
#define __DELAY_H

void my_delay_ms(int ms);
void systick_init(void);
void systick_delay_us(int us);
void systick_delay_ms(int ms);

#endif
