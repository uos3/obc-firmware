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

int main(void) {

    Kernel_init_critical_modules();

    DEBUG_INF("GPIO Test");


    if (Gpio_init(GPIO_PINF1, 1, GPIO_MODE_OUTPUT) != ERROR_NONE) {
        Debug_exit(1);
    }
    
    if (Gpio_init(GPIO_PINF0, 1, GPIO_MODE_INPUT) != ERROR_NONE) {
        Debug_exit(1);
    }

    Led_set(GPIO_PINF1, true);

    Debug_exit(1);
    /* Return 0 if no errors occured up to this point. */
    return 0;
}
