/**
 * @ingroup gpio
 * 
 * @file Gpio_public_linux.c
 * @author Leon Galanakis (lg5g16@soton.ac.uk)
 * @brief Public GPIO driver for the firmware.
 * 
 * Implements the GPIO driver for Linux.
 * 
 * @version 0.1
 * @date 2020-12-09
 * 
 * @copyright Copyright (c) 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard Lirary */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Internal */
#include "drivers/gpio/Gpio_public.h"
#include "drivers/gpio/Gpio_private.h"
#include "drivers/gpio/Gpio_errors.h"
#include "util/debug/Debug_public.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

ErrorCode Gpio_init(
    uint32_t *p_gpio_pins_in,
    size_t num_gpio_pins_in,
    Gpio_Mode mode_in
) {
    DEBUG_DBG("GPIO initialise attempted when running on linux, ignoring.");
    return ERROR_NONE;
}

ErrorCode Gpio_write(uint8_t gpio_id_number, bool gpio_state_in) {
    DEBUG_DBG("GPIO write attempted when running on linux, ignoring.");
    return ERROR_NONE;
}

ErrorCode Gpio_read(uint8_t gpio_id_number, bool *p_gpio_value_out) {
    DEBUG_DBG("GPIO read attempted when running on linux, ignoring.");
    return ERROR_NONE;
}

ErrorCode Gpio_set_rising_interrupt(
    uint8_t gpio_id_number,
    void *interrupt_callback(void)
) {
        
    DEBUG_DBG("GPIO set rising interrupt attempted when running on linux,\
    ignoring.");
    return ERROR_NONE;
}

ErrorCode GPIO_reset_interrupt(uint8_t gpio_id_numer) {
    DEBUG_DBG("GPIO reset interrupt attempted when running on linux,\
    ignoring.");
    return ERROR_NONE;
}