#include "delay.h"

void my_delay_ms(int ms)
{
    int i, j;
    for(i=0; i<ms; i++)
        for(j=0; j<8050; j++);
}
