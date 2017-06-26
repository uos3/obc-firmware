//#define PART_TM4C123GH6PM
#define TARGET_IS_BLIZZARD_RB1
#define TARGET_TM4C123
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/rom.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/adc.h"
#include "driverlib/uart.h"
#include "driverlib/ssi.h"


#include "../firmware.h"

uint32_t micros(void) {
	return 0xFFFFFFFF - ROM_TimerValueGet(WTIMER4_BASE, TIMER_B);
}

void delay(unsigned long nTime) {
	ROM_TimerLoadSet(WTIMER5_BASE, TIMER_A, nTime * 10);

	ROM_TimerIntEnable(WTIMER5_BASE, TIMER_TIMA_TIMEOUT);
	ROM_TimerEnable(WTIMER5_BASE, TIMER_A);
	ROM_SysCtlSleep();

	ROM_TimerIntDisable(WTIMER5_BASE, TIMER_TIMA_TIMEOUT);
	ROM_TimerDisable(WTIMER5_BASE, TIMER_A);
}

void delayInterrupt(void) {
	ROM_TimerIntClear(WTIMER5_BASE, TIMER_TIMA_TIMEOUT);
}

void delayMicroseconds(uint32_t nTime) {
	if (!nTime)
		return;
	else if (nTime < 3)
		SysCtlDelay(12 * nTime);    // empirical
	else {
		// Compensates for latency
		uint32_t endtime = micros() + nTime - 1;
		while (micros() < endtime)
			;
	}
}

void resetMillis(void) {
	ROM_TimerLoadSet(WTIMER4_BASE, TIMER_A, 0xFFFFFFFF);
}

uint32_t millis(void) {
	return (0xFFFFFFFF - ROM_TimerValueGet(WTIMER4_BASE, TIMER_A)) / 2;
}

void resetMicros(void) {
	ROM_TimerLoadSet(WTIMER4_BASE, TIMER_B, 0xFFFFFFFF);
}

void UARTputc(uint8_t UART, char c) {
	ROM_UARTCharPut(UART, c);
}

void UARTputs(uint8_t UART, const char *str) {
	int a = 0;
	while (str[a] != '\0') {
		if (str[a] == '\n') {
			UARTputc(UART, '\r');
			UARTputc(UART, '\n');
		} else
			UARTputc(UART, str[a]);
		a++;
	}
}

/*************************************/
/* Set ImageSize :
 /* <1> 0x22 : 160*120
 /* <2> 0x11 : 320*240
 /* <3> 0x00 : 640*480
 /* <4> 0x1D : 800*600
 /* <5> 0x1C : 1024*768
 /* <6> 0x1B : 1280*960
 /* <7> 0x21 : 1600*1200
 /************************************/
void SetImageSizeCmd(uint8_t Size) {
	UARTCharPut(UART3_BASE, 0x56);
	UARTCharPut(UART3_BASE, 0x00);
	UARTCharPut(UART3_BASE, 0x54);
	UARTCharPut(UART3_BASE, 0x01);
	UARTCharPut(UART3_BASE, Size);
}

/*************************************/
/* Set BaudRate :
 /* <1>　0xAE  :   9600
 /* <2>　0x2A  :   38400
 /* <3>　0x1C  :   57600
 /* <4>　0x0D  :   115200
 /* <5>　0xAE  :   128000
 /* <6>　0x56  :   256000
 /*************************************/
void SetBaudRateCmd(uint8_t baudrate) {
	UARTCharPut(UART3_BASE, 0x56);
	UARTCharPut(UART3_BASE, 0x00);
	UARTCharPut(UART3_BASE, 0x24);
	UARTCharPut(UART3_BASE, 0x03);
	UARTCharPut(UART3_BASE, 0x01);
	UARTCharPut(UART3_BASE, baudrate);
}

void SendResetCmd() {
	UARTCharPut(UART3_BASE, 0x56);
	UARTCharPut(UART3_BASE, 0x00);
	UARTCharPut(UART3_BASE, 0x26);
	UARTCharPut(UART3_BASE, 0x00);
}

void SendTakePhotoCmd() {
	UARTCharPut(UART3_BASE, 0x56);
	UARTCharPut(UART3_BASE, 0x00);
	UARTCharPut(UART3_BASE, 0x36);
	UARTCharPut(UART3_BASE, 0x01);
	UARTCharPut(UART3_BASE, 0x00);
}

void StopTakePhotoCmd() {
	UARTCharPut(UART3_BASE, 0x56);
	UARTCharPut(UART3_BASE, 0x00);
	UARTCharPut(UART3_BASE, 0x36);
	UARTCharPut(UART3_BASE, 0x01);
	UARTCharPut(UART3_BASE, 0x03);
}

int main(void) {
	/* 8MHz TCXO, Internal PLL runs at 400MHz, so use divide-by-5 to get 80MHz Core Clock */
    SysCtlClockSet(SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_SYSDIV_5 | SYSCTL_XTAL_8MHZ);

    
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
	(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER4);
	ROM_TimerConfigure(WTIMER4_BASE,
			(TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_ONE_SHOT | TIMER_CFG_B_ONE_SHOT));
	ROM_TimerPrescaleSet(WTIMER4_BASE, TIMER_A, 39999);      // 0.5 ms per cycle
	ROM_TimerPrescaleSet(WTIMER4_BASE, TIMER_B, 79);           // 1 us per cycle
	ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_WTIMER4);
	ROM_TimerEnable(WTIMER4_BASE, TIMER_A);
	ROM_TimerEnable(WTIMER4_BASE, TIMER_B);

	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER5);
	ROM_TimerConfigure(WTIMER5_BASE,
			(TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_ONE_SHOT | TIMER_CFG_B_ONE_SHOT));
	ROM_TimerPrescaleSet(WTIMER5_BASE, TIMER_A, 7999);       // 0.1 ms per cycle
	ROM_TimerPrescaleSet(WTIMER5_BASE, TIMER_B, 7);          // 0.1 us per cycle
	TimerIntRegister(WTIMER5_BASE, TIMER_A, delayInterrupt); // Attach interrupt to function
	ROM_IntMasterEnable();	// Enable interrupts
	ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_WTIMER5);

	//camera
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinConfigure(GPIO_PC6_U3RX);
	GPIOPinConfigure(GPIO_PC7_U3TX);
	GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);

	UARTCharPut(UART0_BASE, 'S');

    LED_on(LED_B);
    Delay_ms(500);
    LED_off(LED_B);

    WDT_kick();

	//UARTCharPut(UART0_BASE, 'S');
	uint8_t rep;
	/*
	 while (UARTCharsAvail(UART3_BASE)) {
	 rep = UARTCharGet(UART3_BASE);
	 //UARTCharPut(UART0_BASE, rep);
	 }

	 //UARTCharPut(UART0_BASE, '-');

	 UARTCharPut(UART3_BASE, 0x56);
	 UARTCharPut(UART3_BASE, 0x00);
	 UARTCharPut(UART3_BASE, 0x26);
	 UARTCharPut(UART3_BASE, 0x00);

	 //UARTCharPut(UART0_BASE, '_');
	 delay(100);
	 while (UARTCharsAvail(UART3_BASE)) {
	 rep = UARTCharGet(UART3_BASE);
	 //UARTCharPut(UART0_BASE, rep);
	 }
	 delay(3000);
	 //UARTCharPut(UART0_BASE, 'R');

	 SetImageSizeCmd(0x21);
	 delay(1000);

	 //camera ready
	 //uint8_t size320[]={0x56,0x00,0x31,0x05,0x04,0x01,0x00,0x19,0x11} ;
	 //	UARTputs(UART3_BASE,)
	 UARTCharPut(UART3_BASE, 0x56);
	 UARTCharPut(UART3_BASE, 0x00);
	 UARTCharPut(UART3_BASE, 0x36);
	 UARTCharPut(UART3_BASE, 0x01);
	 UARTCharPut(UART3_BASE, 0x00);
	 delay(1000);
	 while (UARTCharsAvail(UART3_BASE)) {
	 rep = UARTCharGet(UART3_BASE);
	 //UARTCharPut(UART0_BASE, rep);
	 }

	 //get size

	 UARTCharPut(UART3_BASE, 0x56);
	 UARTCharPut(UART3_BASE, 0x00);
	 UARTCharPut(UART3_BASE, 0x34);
	 UARTCharPut(UART3_BASE, 0x01);
	 UARTCharPut(UART3_BASE, 0x00);
	 delay(1000);
	 while (UARTCharsAvail(UART3_BASE)) {
	 delayMicroseconds(100);
	 rep = UARTCharGet(UART3_BASE);
	 //UARTCharPut(UART0_BASE, rep);
	 }*/

	//UARTCharPut(UART0_BASE, 's');
	UARTConfigSetExpClk(UART3_BASE, SysCtlClockGet(), 38400,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	delay(200);
    WDT_kick();

	SendResetCmd();	//Wait 2-3 second to send take picture command
	UARTCharPut(UART0_BASE, 'R');

	delay(1000);
    WDT_kick();

	delay(1000);
    WDT_kick();

	UARTConfigSetExpClk(UART3_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	SetBaudRateCmd(0x2A);
	UARTCharPut(UART0_BASE, 'B');

	delay(100);
    WDT_kick();

	//mySerial.begin(38400);

	UARTConfigSetExpClk(UART3_BASE, SysCtlClockGet(), 38400,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	delay(100);
    WDT_kick();

	SetImageSizeCmd(0x21);
	UARTCharPut(UART0_BASE, 'I');

	delay(1000);
    WDT_kick();
	//image qual
	/*UARTCharPut(UART3_BASE, 0x56);
	UARTCharPut(UART3_BASE, 0x00);
	UARTCharPut(UART3_BASE, 0x31);
	UARTCharPut(UART3_BASE, 0x05);
	UARTCharPut(UART3_BASE, 0x01);
	UARTCharPut(UART3_BASE, 0x01);
	UARTCharPut(UART3_BASE, 0x12);
	UARTCharPut(UART3_BASE, 0x04);
	UARTCharPut(UART3_BASE, 0x01);*/
	//delay(1000);

	SendTakePhotoCmd();
	UARTCharPut(UART0_BASE, 'C');
	delay(500);
    WDT_kick();

	UARTCharPut(UART3_BASE, 0x56);
	UARTCharPut(UART3_BASE, 0x00);
	UARTCharPut(UART3_BASE, 0x34);
	UARTCharPut(UART3_BASE, 0x01);
	UARTCharPut(UART3_BASE, 0x00);
	UARTCharPut(UART0_BASE, 'X');
	delay(100);
    WDT_kick();

	while (UARTCharsAvail(UART3_BASE)) {
		delayMicroseconds(100);
		rep = UARTCharGet(UART3_BASE);
		//UARTCharPut(UART0_BASE, rep);
    	WDT_kick();
	}
	UARTCharPut(UART0_BASE, 'A');
	delay(100);
    WDT_kick();

	uint8_t incomingbyte;
	long int j = 0, k = 0, count = 0, i = 0x0000;
	uint8_t MH, ML, MHH;
	uint8_t EndFlag = 0;
	uint8_t a[32];

	while (!EndFlag) {
		j = 0;
		k = 0;
		count = 0;

		MHH = (i >> 16) & 0xFF;
		MH = (i >> 8) & 0xFF;
		ML = i & 0xFF;
		UARTCharPut(UART3_BASE, 0x56);
		UARTCharPut(UART3_BASE, 0x00);
		UARTCharPut(UART3_BASE, 0x32);
		UARTCharPut(UART3_BASE, 0x0c);
		UARTCharPut(UART3_BASE, 0x00);
		UARTCharPut(UART3_BASE, 0x0a);
		UARTCharPut(UART3_BASE, 0x00);
		UARTCharPut(UART3_BASE, MHH);
		UARTCharPut(UART3_BASE, MH);
		UARTCharPut(UART3_BASE, ML);
		UARTCharPut(UART3_BASE, 0x00);
		UARTCharPut(UART3_BASE, 0x00);
		UARTCharPut(UART3_BASE, 0x00);
		UARTCharPut(UART3_BASE, 0x20);
		UARTCharPut(UART3_BASE, 0x00);
		UARTCharPut(UART3_BASE, 0x0a);
		i += 0x20;
		UARTCharPut(UART0_BASE, '|');

		//delay(10);
		while (UARTCharsAvail(UART3_BASE) || k < (5 + 32 + 5)) {
			UARTCharPut(UART0_BASE, '-');
			while(!UARTCharsAvail(UART3_BASE))
			{
				UARTCharPut(UART0_BASE, 'N');
    			Delay_ms(200);
    			WDT_kick();
			}
				UARTCharPut(UART0_BASE, 'Y');
			incomingbyte = UARTCharGet(UART3_BASE);
				UARTCharPut(UART0_BASE, 'P');
			k++;
			//delay(1); //250 for regular
			//delayMicroseconds(100);
//	if (k==5) {
			//		delayMicroseconds(100);
			//		}

			//if (k==37) {
			//	delayMicroseconds(100);
			//}
			if ((k > 5) && (j < 32) && (!EndFlag)) {
				a[j] = incomingbyte;
				if ((a[j - 1] == 0xFF) && (a[j] == 0xD9)) //tell if the picture is finished
						{
					EndFlag = 1;
				}
				j++;
				count++;
			}
    		WDT_kick();
		}

		for (j = 0; j < count; j++) {
			//if (a[j] < 0x10)
			//			Serial.print("0");
			//	Serial.print(a[j], HEX);    // observe the image through serial port
			//Serial.print(" ");
			while (UARTBusy(UART0_BASE)) {
    			WDT_kick();
			}
			UARTCharPut(UART0_BASE, a[j]);
		}
    	WDT_kick();
	}

}
