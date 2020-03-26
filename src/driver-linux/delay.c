#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include "../driver/delay.h"


void Delay_ms(uint32_t milliseconds){
	sleep(milliseconds/1000);
}

void Delay_us(uint32_t microseconds){
	sleep(microseconds/1000/1000);
}
