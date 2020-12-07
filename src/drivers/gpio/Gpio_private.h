/**
 * @ingroup gpio
 * 
 * @file Gpio_private.h
 * @author Leon Galanakis (lg5g16@soton.ac.uk)
 * @brief Private header for the GPIO driver.
 * 
 * @version 0.1
 * @date 2020-12-07
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef H_GPIO_PUBLIC_H
#define H_GPIO_PUBLIC_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdint.h>
#include <stdbool.h>

/* Internal includes */
/* TODO: Why does this come up as an error? */
/* #include "drivers/gpio/Gpio_public.h" */

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

typedef struct _Gpio_Module {
    uint32_t peripheral;
    uint32_t port;
    uint8_t pin;
    uint8_t interrupt_pin;
} Gpio_Module;

#endif /* H_GPIO_PUBLIC_H */