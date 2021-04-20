/**
 * @file test_gpio.c
 * @author Leon Galanakis (leongalanakis@gmail.com)
 * @brief GPIO test
 * 
 * 
 * 
 * @version 0.1
 * @date 2021-04-16
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
#include "drivers/uart/Uart_public.h"
#include "drivers/udma/Udma_public.h"
#include "components/led/Led_public.h"
#include "drivers/gpio/Gpio_public.h"
#include "util/debug/Debug_public.h"
#include "system/event_manager/EventManager_public.h"
#include "system/kernel/Kernel_public.h"

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

int num_toggles;

void Led_toggle_red(void);

/* The function to be called in the interrupt */
void Led_toggle_red(void) {
    if (Led_toggle(LED_LAUNCHPAD) != ERROR_NONE) {
        Debug_exit(1);
    }
    /* Increment the number of toggles to keep track of (the test is over once
     * this value reaches the arbitrary value of 5) */
    num_toggles++;
    DEBUG_DBG("SWITCH 1 has been pressed (times pressed: %d)", num_toggles);
}

int main(void) {

    num_toggles = 0;
    uint8_t *pf0_value_out;
    GPIO_PIN_INDEX *input_pins_in[1];
    GPIO_PIN_INDEX *output_pins_in[1];

    /* Initialise the critical modules such as board and debug */
    Kernel_init_critical_modules();

    /* Set the values of pins to be initialised */
    input_pins_in[0] = GPIO_PINF4;
    output_pins_in[0] = GPIO_PINF1;
    pf0_value_out = 1;

    DEBUG_INF(" ===== GPIO Test =====");

    /* Initialise the input and output pins to their respective mode, exiting
     * if the functions return an error. */
    if (Gpio_init(output_pins_in, 1, GPIO_MODE_OUTPUT) != ERROR_NONE) {
        Debug_exit(1);
    }
    if (Gpio_init(input_pins_in, 1, GPIO_MODE_INPUT) != ERROR_NONE) {
        Debug_exit(1);
    }

    /* Set the LED initially to be ON. Exit if the function returns an error.*/
    if (Led_set((uint8_t)LED_LAUNCHPAD, 1) != ERROR_NONE) {
        Debug_exit(1);
    }

    /* Set the rising interrupt to run Led_toggle_red every time switch 1
     * (GPIO PIN F4) is pressed, exiting if the function returns an error. */
    if (Gpio_set_rising_interrupt(GPIO_PINF4, Led_toggle_red) != ERROR_NONE) {
        Debug_exit(1);
    }

    while (num_toggles < 5) {
        /* The delay prevents 'bouncing' of the signal, as the mechanical
         * button may not give a perfect signal of 1 or 0. */
        SysCtlDelay(7000000);
    }

    DEBUG_DBG(" ===== Test complete =====");
    /* Return 0 if no errors occured up to this point. */
    return 0;
}
