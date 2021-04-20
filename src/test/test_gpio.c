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

void Led_toggle_red(void) {
    if (Led_toggle(LED_LAUNCHPAD) != ERROR_NONE) {
        Debug_exit(1);
    }
    num_toggles++;
    DEBUG_DBG("SW2 Pressed");
}

int main(void) {

    num_toggles = 0;
    uint8_t *pf0_value_out;
    GPIO_PIN_INDEX *input_pins_in[1];
    GPIO_PIN_INDEX *output_pins_in[1];

    Kernel_init_critical_modules();

    input_pins_in[0] = GPIO_PINF0;
    output_pins_in[0] = GPIO_PINF4;
    pf0_value_out = 1;

    DEBUG_INF("GPIO Test");

    if (Gpio_init(output_pins_in, 1, GPIO_MODE_OUTPUT) != ERROR_NONE) {
        Debug_exit(1);
    }
    
    if (Gpio_init(input_pins_in, 1, GPIO_MODE_INPUT) != ERROR_NONE) {
        Debug_exit(1);
    }

    if (Led_set((uint8_t)LED_LAUNCHPAD, 1) != ERROR_NONE) {
        Debug_exit(1);
    }

    if (Gpio_set_rising_interrupt(GPIO_PINF4, Led_toggle_red) != ERROR_NONE) {
        Debug_exit(1);
    }

    while (num_toggles < 5) {
        if (Gpio_read(GPIO_PINF4, pf0_value_out) != ERROR_NONE) {
            Debug_exit(1);
        }
        else {
            DEBUG_INF("Value = %d", pf0_value_out);
            if (pf0_value_out == 0) {
                Led_toggle_red();
            }
        }
        SysCtlDelay(7000000);
    }

    DEBUG_DBG("Test complete");
    /* Return 0 if no errors occured up to this point. */
    return 0;
}
