/**
 * @ingroup uos3-proto
 * @ingroup board
 *
 * @file uos3-proto/board.c
 * @brief Board Initialisation Driver - uos3-proto board
 *
 * @{
 */

#include "../board.h"

#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"

#include "driverlib/sysctl.h"

void Board_init(void)
{
	/* 8MHz TCXO, Internal PLL runs at 400MHz, so use divide-by-5 to get 80MHz Core Clock */
    //SysCtlClockSet(SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_SYSDIV_5 | SYSCTL_XTAL_8MHZ);
    SysCtlClockSet(SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_SYSDIV_5 | SYSCTL_XTAL_8MHZ);
}

//Dont forget to set last_WDT in each demo/mission code/driver this is used. Try to avoid using in drivers
void WDT_wait_to_kick(uint64_t *last_WDT){
    uint64_t timeout = 100;
    while(!RTC_timerElapsed_ms(last_WDT, timeout));
    WDT_kick();
    RTC_getTime_ms(&last_WDT);
}


/**
 * @}
 */