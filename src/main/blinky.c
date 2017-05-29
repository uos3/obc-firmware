/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"


#include "../platform/led.h"

//! A very simple example that blinks the on-board LED.

uint32_t pui32DataTx[10];
uint32_t pui32DataRx[10];

int main(void)
{

    volatile uint32_t ui32Loop;
	
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinConfigure(GPIO_PF2_SSI1CLK);
    GPIOPinConfigure(GPIO_PF3_SSI1FSS);
    GPIOPinConfigure(GPIO_PF0_SSI1RX);
    GPIOPinConfigure(GPIO_PF1_SSI1TX);
	GPIOPinTypeSSI(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 |
                   GPIO_PIN_3);
	SSIConfigSetExpClk(SSI1_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER, 1000000, 8);
	
	
	SSIEnable(SSI1_BASE);
	
	while(SSIDataGetNonBlocking(SSI1_BASE, &pui32DataRx[0]))
    {
    }
	
	pui32DataTx[0] = 's';
    pui32DataTx[1] = 'p';
    pui32DataTx[2] = 'i';
	
	
	
    // Loop forever.
    while(1)
    {
        // Turn on the LED.
        LED_off(LED_B);


    /* On period */
    for(i = 0; i < 300000; i++) {};

    LED_off(LED_B);

        // Delay for a bit.
        for(ui32Loop = 0; ui32Loop < 300000; ui32Loop++) {};
		
		uint8_t ui32Index;
		for(ui32Index = 0; ui32Index < 3; ui32Index++)
			SSIDataPut(SSI1_BASE, pui32DataTx[ui32Index]);
		while(SSIBusy(SSI1_BASE));
    }
}
