/**
 * @ingroup debug
 * 
 * @file Debug_public.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Provides debug logging functionality to the software.
 * @version 0.1
 * @date 2020-11-01
 * 
 * @copyright Copyright (c) 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef TARGET_UNIX
#include <time.h>
#endif

/* External includes */
#ifdef TARGET_TM4C
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#endif

/* Internal includes */
#include "system/data_pool/DataPool_public.h"
#include "util/debug/Debug_public.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

#ifdef TARGET_UNIX
static struct timespec DEBUG_INIT_TIME;
#endif

/* -------------------------------------------------------------------------   
 * CONSTANTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Names given to different log levels.
 */
static const char *Debug_level_names[] = {
  "ERR", "WRN", "---", "DBG", "TRC"
};

/**
 * @brief Terminal colour codes used to log messages
 */
static const char *Debug_level_colours[] = {
   "\x1b[31m", "\x1b[33m", "", "\x1b[36m", "\x1b[35m",
};

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

bool Debug_init(void) {

    /* If on UNIX set the init time */
    #ifdef TARGET_UNIX
    if (clock_gettime(CLOCK_MONOTONIC_RAW, &DEBUG_INIT_TIME) != 0) {
        return false;
    }
    #endif
    /* If on TM4C */
    #ifdef TARGET_TM4C

    /* Note: UART1 is used here since this is the one that's connected to pins
     * on the launchpad, this should be changed when building for the TM4C. 
     */

    /* Check the system clock has been init */
    if (!DP.BOARD_INITIALISED) {
        /* Can't print a debug statement here since the debug system relies on
         * the clock being setup.
         * 
         * FIXME: Trigger hardfault? */
        return false;
    }

    /* Enable the GPIO for the LED, so we can signal the device is booted and
     * debug initialised */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);

    /* Enable the GPIO peripheral for UART1 and UART1 itself */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    /* Configure the GPIO for UART output */
    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    /* Configure the UART for 115,200, 8-N-1 operation. */
    UARTConfigSetExpClk(
        UART1_BASE, 
        SysCtlClockGet(), 
        115200,
        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE)
    );

    /* Turn the LED on to show the device is ready */
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
    #endif

    return true;
}

#ifdef TARGET_UNIX
void Debug_log_unix(
    uint8_t level, 
    const char *p_file, 
    uint32_t line, 
    const char *p_fmt, 
    ...
) {
    va_list args;
    va_start(args, p_fmt);

    /* Calculate time since init */
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    int64_t delta_ms = 
        (now.tv_sec - DEBUG_INIT_TIME.tv_sec) * 1000
        + (now.tv_nsec - DEBUG_INIT_TIME.tv_nsec) / 1000000;

    /* Remove the file path up to src/ */
    char *p_file_stripped = strstr(p_file, "src");

    /* Print the prefix */
    printf(
        "[%10lu %s%s\x1b[0m] %s:%d ",
        delta_ms,
        Debug_level_colours[level], 
        Debug_level_names[level],
        p_file_stripped + 4,
        line
    );

    /* Print the message */
    vprintf(p_fmt, args);

    /* Print newline */
    printf("\n");
}
#endif

#ifdef TARGET_TM4C
void Debug_log_tm4c(
    uint8_t level, 
    const char *p_file, 
    uint32_t line, 
    const char *p_fmt, 
    ...
) {

    va_list args;
    va_start(args, p_fmt);

    /* String to print into */
    char str[512] = {0};

    /* TODO: Get the time from the RTC */

    /* Remove the file path up to src/ */
    char *p_file_stripped = strstr(p_file, "src");

    /* Put the the prefix into the string */
    sprintf(
        str,
        "[---------- %s%s\x1b[0m] %s:%ld ",
        Debug_level_colours[level], 
        Debug_level_names[level],
        p_file_stripped + 4,
        line
    );

    /* Add the message */
    vsprintf(&str[0] + strlen(str), p_fmt, args);

    /* Add the carriage return/newline */
    sprintf(&str[0] + strlen(str), "\r\n");

    /* Iterate over the string and print the characters to the UART */
    for (size_t i = 0; i < strlen(str); ++i) {
        UARTCharPut(UART1_BASE, str[i]);
    }
}
#endif

void Debug_exit(int error_code) {
    #ifdef TARGET_UNIX
    exit(error_code);
    #endif
    #ifdef TARGET_TM4C
    (void) error_code;
    __asm("BKPT");
    #endif
}

void Debug_hex_string(uint8_t *p_bytes_in, char *p_str_out, size_t length_in) {
    char byte[4];

    for (size_t i = 0; i < length_in; i++) {
        sprintf(byte, "%02X ", p_bytes_in[i]);
        strcat(p_str_out, byte);
    }
}