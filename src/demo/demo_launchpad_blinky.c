/**
 * __DEMO__ - Not for flight usage
 * 
 * # TM4C Launchpad Blinky Demo
 * 
 * This demo file is used to verify the build system functionality with the
 * TM4C launchpad. It will toggle the onboard LED if SW1 is pressed, and keep
 * the LED on if SW2 is pressed. If buttons are released the LED is turned off.
 * 
 * Originally written by Yusef Karim, adapted for UoS3.
 * 
 * File version: 0.1.0
 * Last modified: 2020-10-02
 * Author: Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * Status: In pogress
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

#include <stdint.h>
#include "inc/tm4c123gh6pm.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/* 
 * Using the 400MHz PLL we get the bus frequency by dividing
 * 400MHz / (SYSDIV2+1). Therefore, 400MHz/(4+1) = 80 MHz bus frequency
 */
#define SYSDIV2 (4)

/* 
 * Pin used for SW1
 */
#define GPIO_SW1 (0x10);

/* 
 * Pin used for SW2
 */
#define GPIO_SW2 (0x01);

/* 
 * Pin used for the RED led 
 */
#define GPIO_LED_RED (0x02);

/* 
 * Pin used for the BLUE led 
 */
#define GPIO_LED_BLUE (0x04);

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/* 
 * Store the SW1 and SW2 input values.
 */
unsigned long SW1, SW2;


/* -------------------------------------------------------------------------   
 * FUNCTION PRE-DECLARATIONS
 * ------------------------------------------------------------------------- */

/*
 * Initialise GPIO Port F, which contains the LED and switch pins.
 */
void PortF_init(void);

/*
 * Initialise the PLL to enable system clock.
 */
void Pll_init(void);

/*
 * Initialise the systick.
 */
void SysTick_init(void);

/*
 * TODO
 */
void SysTick_wait_ms(unsigned long ms);

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/*
 *  Main entry point for the programme
 */
int main(void) {
    
    /* Init the system */
    Pll_init();
    SysTick_init();
    PortF_init();

    /* Main loop */
    while (1) {

        /* Read values of the switches, SW1 at PF4 and SW2 at PF0 */
        SW1 = GPIO_PORTF_DATA_R & GPIO_SW1;
        SW2 = GPIO_PORTF_DATA_R & GPIO_SW2;

        /* Checks if SW1 has been pressed, SW1 uses negative logic */
        if (!SW1) {
            /* Toggle PF2, the blue LED */
            GPIO_PORTF_DATA_R ^= GPIO_LED_BLUE;

            /* Wait to disable the LED */
            SysTick_wait_ms(500);
        }
        else {
            /* Clear PF2 (blue LED) */
            GPIO_PORTF_DATA_R &= ~GPIO_LED_BLUE;
        }
        
        /* Checks if SW2 has been pressed, SW2 uses negative logic */
        if (!SW2) {
            /* Set PF2, the red LED */
            GPIO_PORTF_DATA_R |= GPIO_LED_RED;  
        }
        else {
            /* Clear PF2, the red LED */
            GPIO_PORTF_DATA_R &= ~GPIO_LED_RED;
        }
    }
}

/*
 * NOTE: These functions haven't been changed from Karim's versions and do not
 * follow the writing standards.
 * 
 * TODO: Update to follow writing standards.
 */

void PortF_init(void) {
    volatile unsigned long delay;
    SYSCTL_RCGC2_R |= 0x00000020;   //F clock
    delay = SYSCTL_RCGC2_R;         //delay
    GPIO_PORTF_LOCK_R = 0x4C4F434B; //Unlock Port F
    GPIO_PORTF_CR_R = 0x1F;         //Allow changes to PF4-0
    GPIO_PORTF_AMSEL_R = 0x00;      //Disable analog functions
    GPIO_PORTF_PCTL_R = 0x00000000; //GPIO clear bit PCTL
    GPIO_PORTF_DIR_R = 0x06;        //PF2, PF1 output, PF4,PF0 input
    GPIO_PORTF_AFSEL_R = 0x00;      //Clear alt functions
    GPIO_PORTF_PUR_R |= 0x11;       //Enable pull-up resistor on PF4 and PF0
    GPIO_PORTF_DEN_R |= 0x17;       //Enable digital pin PF4, PF0 and PF2-1
}

void Pll_init(void)
{
    SYSCTL_RCC2_R |= SYSCTL_RCC2_USERCC2;   //Use RCC2 for advanced features
    SYSCTL_RCC2_R |= SYSCTL_RCC2_BYPASS2;   //Bypass PLL during initialization
    SYSCTL_RCC_R &= ~SYSCTL_RCC_XTAL_M;     //Clear XTAL field
    SYSCTL_RCC_R += SYSCTL_RCC_XTAL_16MHZ;  //Use 16Mhz crystal
    SYSCTL_RCC2_R &= ~SYSCTL_RCC2_OSCSRC2_M;//Clear oscillator source field
    SYSCTL_RCC2_R += SYSCTL_RCC2_OSCSRC2_MO;//Use main oscillator source field
    SYSCTL_RCC2_R &= ~SYSCTL_RCC2_PWRDN2;   //Activate PLL by clearing PWRDN

    //As explained above we use 400MHz PLL and specified SYSDIV2 for 80MHz clock
    SYSCTL_RCC2_R |= SYSCTL_RCC2_DIV400;    //Use 400MHz PLL
    //Clear system clock divider field then configure 80Mhz clock
    SYSCTL_RCC2_R = (SYSCTL_RCC2_R&~0x1FC00000) + (SYSDIV2<<22);
    //Wait for the PLL to lock by polling PLLLRIS
    while((SYSCTL_RIS_R&SYSCTL_RIS_PLLLRIS) == 0){};
    SYSCTL_RCC2_R &= ~SYSCTL_RCC2_BYPASS2;  //Enable PLL by clearing BYPASS
}

void SysTick_init(void)
{
    NVIC_ST_CTRL_R = 0;     //Disable SysTick during setup
    NVIC_ST_CURRENT_R = 0;  //Any write clears current
    //Enable SysTick with core clock, i.e. NVIC_ST_CTRL_R = 0x00000005
    NVIC_ST_CTRL_R = NVIC_ST_CTRL_ENABLE+NVIC_ST_CTRL_CLK_SRC;
}

void SysTick_wait_ms(unsigned long ms)
{
    //Clock is set to 80MHz, each Systick takes 1/80MHz = 12.5ns
    //To get 1ms count down delay, take (1ms)/(12.5ns)
    unsigned long delay = 80000;
    unsigned long i;

    for(i = 0; i < ms; i++)
    {
        NVIC_ST_RELOAD_R = delay - 1;   //Reload value is the number of counts to wait
        NVIC_ST_CURRENT_R = 0;          //Clears current
        //Bit 16 of STCTRL is set to 1 if SysTick timer counts down to zero
        while((NVIC_ST_CTRL_R&0x00010000) == 0){}
    }
}
