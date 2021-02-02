/**
 * @ingroup gpio
 * 
 * @file Gpio_public.h
 * @author Leon Galanakis (lg5g16@soton.ac.uk)
 * @brief Public header for the GPIO driver.
 * 
 * @version 0.1
 * @date 2020-12-06
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef H_GPIO_PUBLIC_H
#define H_GPIO_PUBLIC_H

/* TODO: Add board defines and constants for GPIO - most likely in board.h? */
/* TODO: Add gpio mode enum (or struct?) */
/* TODO: Check parts of prev. year's gpio.c which should be moved into gpio.h */

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/* TODO: Check for potential loss of efficiency/warnings that may arise due
 * to these values being defined twice (once in the tivaware library and once
 * here). */

/* The following values define the respective GPIOs */

#define GPIOA 0xf0000800
#define GPIOB 0xf0000801
#define GPIOC 0xf0000802
#define GPIOD 0xf0000803
#define GPIOE 0xf0000804
#define GPIOF 0xf0000805

/* The following values define the respective GPIO ports */

#define GPIO_PORTA_BASE 0x40004000
#define GPIO_PORTB_BASE 0x40005000
#define GPIO_PORTC_BASE 0x40006000
#define GPIO_PORTD_BASE 0x40007000
#define GPIO_PORTE_BASE 0x40024000
#define GPIO_PORTF_BASE 0x40025000

/* The following values define the respective GPIO pins */

#define GPIO_PIN_0 0x00000001
#define GPIO_PIN_1 0x00000002
#define GPIO_PIN_2 0x00000004
#define GPIO_PIN_3 0x00000008
#define GPIO_PIN_4 0x00000010
#define GPIO_PIN_5 0x00000020
#define GPIO_PIN_6 0x00000040
#define GPIO_PIN_7 0x00000080

/* The following values are to be passed in to the GPIOIntEnable() and
 * GPIOIntDisable() functions (ui32IntFlags parameter) */

#define GPIO_INT_PIN_0 0x00000001
#define GPIO_INT_PIN_1 0x00000002
#define GPIO_INT_PIN_2 0x00000004
#define GPIO_INT_PIN_3 0x00000008
#define GPIO_INT_PIN_4 0x00000010
#define GPIO_INT_PIN_5 0x00000020
#define GPIO_INT_PIN_6 0x00000040
#define GPIO_INT_PIN_7 0x00000080

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdint.h>
#include <stdbool.h>

/* Internal includes */
#include "drivers/gpio/Gpio_errors.h"

/* -------------------------------------------------------------------------   
 * ENUMS
 * ------------------------------------------------------------------------- */

typedef enum _Gpio_Mode {

    GPIO_MODE_INPUT,

    GPIO_MODE_OUTPUT

} Gpio_Mode;

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Defines a GPIO module
 * 
 */
typedef struct _Gpio_Module {
    uint32_t peripheral;
    uint32_t port;
    uint8_t pin;
    uint8_t interrupt_pin;
    Gpio_Mode mode;
    bool initialised;
    /* Prev year has void (*int_function)(void); in a different struct.
     * TODO: Check what its purpose is */
} Gpio_Module;

/**
 * @brief State of the GPIO driver
 * TODO: Does this have to be separate to GPIO module? I have created it as
 * a seperate struct so there can be a global state, so other modules can check
 * if it has been (for example) initialised.
 * 
 */
typedef struct _Gpio {
    
    /**
     * @brief Flag to show if the Gpio has been initalised or not
     * 
     */
    bool initialised;

} Gpio;

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/**
 * @brief Global instance of the GPIO driver state
 * 
 */
extern Gpio GPIO;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Writes the state of a specified GPIO pin to HIGH or LOW.
 * 
 * @param gpio_id_number GPIO pin ID number.
 * @param gpio_state State to write the pin to (true = HIGH, false = LOW)
 * @return ErrorCode Return code.
 */
ErrorCode Gpio_write(uint8_t gpio_id_number, bool gpio_state);

/**
 * @brief Reads the input value of a specified GPIO pin.
 * 
 * @param gpio_id_number GPIO pin ID number.
 * @param p_gpio_value_out Input value of GPIO pin (true = HIGH, false = LOW).
 * @return ErrorCode Return code.
 */
ErrorCode Gpio_read(uint8_t gpio_id_number, bool *p_gpio_value_out);

/**
 * @brief Sets up a rising edge interrupt on a specified GPIO pin.
 * 
 * @param gpio_id_number GPIO pin ID number.
 * @param interrupt_callback Pointer to the function which is called in the interrupt.
 * @return ErrorCode Return code.
 */
ErrorCode Gpio_set_rising_interrupt(uint8_t gpio_id_number, void *interrupt_callback(void));

/**
 * @brief Removes the interrupt from a specified GPIO pin.
 * 
 * @param gpio_id_number GPIO pin ID number.
 * @return ErrorCode Return code.
 */
ErrorCode Gpio_reset_interrupt(uint8_t gpio_id_number);

#endif /* H_GPIO_PUBLIC_H */