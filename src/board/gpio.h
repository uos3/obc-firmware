#ifndef __GPIO_H__
#define __GPIO_H__

#include <stdint.h>
#include <stdbool.h>

typedef enum {GPIO_MODE_INPUT, GPIO_MODE_OUTPUT} GPIO_mode;

void GPIO_set(uint8_t gpio_number);
void GPIO_reset(uint8_t gpio_number);
void GPIO_write(uint8_t gpio_number, bool state);
bool GPIO_read(uint8_t gpio_number);

#endif /*  __GPIO_H__ */