/**
 * File purpose:        Timers operation demo
 * Last modification:   22/08/2019
 * Status:              Ready for the test
 */

/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include <stdio.h>
#include <stdlib.h>

#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "inc/hw_memmap.h"
 
#include "../board/memory_map.h"
#include "../board/radio.h"

volatile uint32_t time;
char output[200];

void timer0(){
TimerIntClear(WTIMER0_BASE, TIMER_TIMA_TIMEOUT);
RTC_getTime(&time);
sprintf(output, ">>> Timer0 Interrupt - Time is %u\r\n", time);
UART_puts(UART_INTERFACE, output);
  }
void timer1(){
TimerIntClear(WTIMER1_BASE, TIMER_TIMA_TIMEOUT);
RTC_getTime(&time);
sprintf(output, ">>> Timer1 Interrupt - Time is %u\r\n", time);
UART_puts(UART_INTERFACE, output);
  }
void timer2(){
TimerIntClear(WTIMER2_BASE, TIMER_TIMA_TIMEOUT);
RTC_getTime(&time);
sprintf(output, ">>> Timer 2 Interrupt - Time is %u\r\n", time);
UART_puts(UART_INTERFACE, output);
  }
void timer3(){
TimerIntClear(WTIMER3_BASE, TIMER_TIMA_TIMEOUT);
RTC_getTime(&time);
sprintf(output, ">>> Timer 3 Interrupt - Time is %u\r\n", time);
UART_puts(UART_INTERFACE, output);
  }
void timer4(){
TimerIntClear(WTIMER4_BASE, TIMER_TIMA_TIMEOUT);
RTC_getTime(&time);
sprintf(output, ">>> Timer 4 Interrupt - Time is %u\r\n", time);
UART_puts(UART_INTERFACE, output);
  }
void timer5(){
TimerIntClear(WTIMER5_BASE, TIMER_TIMA_TIMEOUT);
RTC_getTime(&time);
sprintf(output, ">>> Timer 5 Interrupt - Time is %u\r\n", time);
UART_puts(UART_INTERFACE, output);
  }


int main(void){
    Board_init();
    watchdog_update = 0xFF;
    enable_watchdog_kick();

    RTC_init();
    UART_init(UART_INTERFACE, 9600);
    UART_puts(UART_INTERFACE, ">>>>>>>>Timers Demo\r\n");
    timer_init();

uint64_t Period;
Period = SysCtlClockGet();
sprintf(output, "Period is %u\n\r", Period);
UART_puts(UART_INTERFACE, output);

        //Timer0 set up - target period 120
        TimerLoadSet64(WTIMER0_BASE, Period*120);
        TimerIntRegister(WTIMER0_BASE, TIMER_A, timer0);
        RTC_getTime(&time);
        sprintf(output, "Timer0 is Starting, Period 120s - Time is %u\r\n", time);
        UART_puts(UART_INTERFACE, output);
        TimerEnable(WTIMER0_BASE,TIMER_A);

        //Timer1 set up - target period 300s
        TimerLoadSet64(WTIMER1_BASE, Period*80);
        TimerIntRegister(WTIMER1_BASE, TIMER_A, timer1);
        RTC_getTime(&time);
        sprintf(output, "Timer1 is Starting, Period 80s - Time is %u\r\n", time);
        UART_puts(UART_INTERFACE, output);
        TimerEnable(WTIMER1_BASE,TIMER_A);

        //Timer2 set up - target period 20s
        TimerLoadSet64(WTIMER2_BASE, Period*20);
        TimerIntRegister(WTIMER2_BASE, TIMER_A, timer2);
        RTC_getTime(&time);
        sprintf(output, "Timer2 is Starting, Period 20s - Time is %u\r\n", time);
        UART_puts(UART_INTERFACE, output);
        TimerEnable(WTIMER2_BASE,TIMER_A);

        //Timer3 set up - target period 40s
        TimerLoadSet64(WTIMER3_BASE, Period*40);
        TimerIntRegister(WTIMER3_BASE, TIMER_A, timer3);
        RTC_getTime(&time);
        sprintf(output, "Timer3 is Starting, Period 40s - Time is %u\r\n", time);
        UART_puts(UART_INTERFACE, output);
        TimerEnable(WTIMER3_BASE,TIMER_A);
    while (1)
    {
        watchdog_update = 0xFF;
    }
    
}