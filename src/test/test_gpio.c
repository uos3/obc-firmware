/**
 * @file test_power.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Power Application main test executable
 * 
 * Task ref: [UT_2.11.6]
 * 
 * @version 0.1
 * @date 2021-02-26
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Internal includes */
#include "drivers/gpio/Gpio_errors.h"
#include "drivers/gpio/Gpio_private.h"
#include "drivers/gpio/Gpio_public.h"
#include "components/led/Led_errors.h"
#include "components/led/Led_private.h"
#include "components/led/Led_public.h"

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

int main(void) {
    /* TODO: Unfinished */

    /* Initialise the LED. */
    Gpio_init(LED_LEDS[LED_TEMP_1].gpio_pin, 1, GPIO_MODE_INPUT);

    /* Set the rising interrupt to toggle the LED state when an interrupt is
     * detected. */
    Gpio_set_rising_interrupt(LED_LEDS[LED_TEMP_1].gpio_pin, Led_toggle(LED_LEDS[LED_TEMP_1].gpio_pin));

    /* Return 1 if no errors occured up to this point. */
    return 1;
}