#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "../driver/gpio.h"
#include "../utility/debug.h"

void GPIO_set(uint8_t gpio_number){

}

void GPIO_reset(uint8_t gpio_number){
}
	

void GPIO_write(uint8_t gpio_number, bool state){
	#ifdef VERBOSE_IO
	printf("GPIO: write attempt on %d to state %1d\n", 
	gpio_number, state);
	#endif
}

bool GPIO_read(uint8_t gpio_number){
	return true;
}

bool GPIO_set_risingInterrupt(uint8_t gpio_number, void (*interrupt_callback)(void)){
	return true;
}

void GPIO_reset_interrupt(uint8_t gpio_number){

}
