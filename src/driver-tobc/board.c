 /**
 * @ingroup uos3-proto
 * @ingroup board
 *
 * @file uos3-proto/board.c
 * @brief Board Initialisation Driver - uos3-proto board
 *
 * @{
 */

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_types.h"
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"

#include "../driver/board.h"

void Board_init(void)
{
	/* 8MHz TCXO, Internal PLL runs at 400MHz, so use divide-by-5 to get 80MHz Core Clock */
    //SysCtlClockSet(SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_SYSDIV_5 | SYSCTL_XTAL_8MHZ);
    SysCtlClockSet(SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_SYSDIV_5 | SYSCTL_XTAL_8MHZ);
}

/**
 * @}
 */