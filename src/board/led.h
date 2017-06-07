#ifndef __LED_H__
#define __LED_H__

#include <stdint.h>
#include <stdbool.h>

void LED_on(uint8_t led_number);
void LED_off(uint8_t led_number);
void LED_set(uint8_t led_number, bool state);

#endif /*  __LED_H__ */