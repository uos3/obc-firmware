#ifndef __LED_H__
#define __LED_H__

#include <stdint.h>
#include <stdbool.h>

#define LED_A	0x0
#define LED_B	0x1

void LED_on(uint8_t led_number);
void LED_off(uint8_t led_number);
void LED_set(uint8_t led_number, bool state);

#endif /*  __LED_H__ */