/**
 * @ingroup board
 * 
 * @file Board_public.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Main driver providing basic TM4C123GPMH board support.
 * 
 * This driver is responsible for setting up the system clock on
 * initialisation.
 * 
 * @version 0.1
 * @date 2020-11-11
 * 
 * @copyright Copyright (c) 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* External includes */
#ifdef TARGET_TM4C
#include "driverlib/sysctl.h"
#endif

/* Internal includes */
#include "system/data_pool/DataPool_public.h"
#include "drivers/board/Board_public.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

bool Board_init(void) {
    /* Check that the datapool has been init */
    if (!DP.INITIALISED) {
        /* Can't print a debug statement here since the debug system relies on
         * the clock being setup.
         * 
         * FIXME: Trigger hardfault? */
        return false;
    }

    /* Nothing needs to be done on linux */
    
    /* On the TM4C we need to initialise the system clock.
     * 
     * When the TM4C first boots the clock is configured from the Precision
     * Internal Oscillator (PIOSC) which runs at 16 MHz, in order to get the
     * target of 80 MHz we have to enable the Main Oscillator (MOSC) [TM4C
     * Manual 5.2.5.1].
     * 
     * The PLL runs at a fixed frequency of 400 MHz, so to get the 80 MHz we
     * require we must divide the PLL output by 5 [TM4C Manual 5.2.5.5]. In
     * addition we tell the system that the TOBC board uses an 8 MHz external
     * crystal [TOBC_20200201_v1.3.pdf]. Note that the launchpad uses a 16 MHz
     * crystal instead.
     */
    #ifdef TARGET_TM4C_TOBC
    SysCtlClockSet(
        SYSCTL_USE_PLL 
        | SYSCTL_OSC_MAIN 
        | SYSCTL_SYSDIV_5 
        | SYSCTL_XTAL_8MHZ
    );
    #elif TARGET_TM4C_LAUNCHPAD
    SysCtlClockSet(
        SYSCTL_USE_PLL 
        | SYSCTL_OSC_MAIN 
        | SYSCTL_SYSDIV_5 
        | SYSCTL_XTAL_16MHZ
    ); 
    #endif

    /* Set the DP flag */
    DP.BOARD_INITIALISED = true;

    return true;
}