#include <stdint.h>
#include <stdbool.h>
#include "../driver/gpio.h"

typedef enum {GPIO_MODE_INPUT, GPIO_MODE_OUTPUT} GPIO_mode;

void GPIO_set(uint8_t gpio_number){

}

void GPIO_reset(uint8_t gpio_number){
}
	

void GPIO_write(uint8_t gpio_number, bool state){

}

bool GPIO_read(uint8_t gpio_number){
	return true;
}

bool GPIO_set_risingInterrupt(uint8_t gpio_number, void (*interrupt_callback)(void)){
	return true;
}

void GPIO_reset_interrupt(uint8_t gpio_number){

}
