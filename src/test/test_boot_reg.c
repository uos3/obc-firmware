/**
 * @file test_boot_reg.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Tests boot registers of the TM4C in order to debug flashing issues.
 * 
 * @version 0.1
 * @date 2021-03-18
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdint.h>
#include <stdbool.h>

/* External includes */
#include "driverlib/sysctl.h"

/* Internal includes */
#include "drivers/board/Board_public.h"
#include "util/debug/Debug_public.h"

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

int main(void) {

    uint32_t reset_cause;

    /* Board and debug */
    Board_init();
    if (!Debug_init()) {
        Debug_exit(1);
    }

    /* Get the reset cause */
    reset_cause = SysCtlResetCauseGet();

    DEBUG_INF("Reset cause = 0x%08X", reset_cause);

    return 0;
}