#include <stdint.h>
#include <stdbool.h>

#include "../board/led.h"

//! A very simple example that blinks the on-board LED.

int main(void)
{
    volatile uint32_t ui32Loop;

    // Loop forever.
    while(1)
    {
        // Turn on the LED.
        LED_off(LED_B);

        // Delay for a bit.
        for(ui32Loop = 0; ui32Loop < 100000; ui32Loop++) {};

        // Turn off the LED.
        LED_on(LED_B);

        // Delay for a bit.
        for(ui32Loop = 0; ui32Loop < 300000; ui32Loop++) {};
    }
}
