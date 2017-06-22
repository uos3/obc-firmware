#include "../board.h"

#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"

#include "driverlib/sysctl.h"

void Board_init(void)
{
	/* 8MHz TCXO, Internal PLL runs at 400MHz, so use divide-by-5 to get 80MHz Core Clock */
    SysCtlClockSet(SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_SYSDIV_5 | SYSCTL_XTAL_8MHZ);
}