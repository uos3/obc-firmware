/**
 * File purpose:        IMU calibration script
 * Last modification:   22/08/2019
 * Status:              Ready for use
 * 
 * This code calibrates the IMU chip, the IMU require calibration after power up, as it loses its offsets after power down
 * You can implemenent calibration is any file, just by calling the function "IMU_calibrate_gyro". However, make sure that the board is flat and not moving
 */

/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include <stdio.h>
int main(void)
{
    int16_t x, y, z, i = 10;
    char output[200];

    Board_init();
    watchdog_update = 0xFF;
    enable_watchdog_kick();
    UART_init(UART_INTERFACE, 9600);

    for(int i=0;i<3;i++)
    {LED_on(LED_A); Delay_ms(100); LED_off(LED_A);} //blink the LED three times

    IMU_Init();
    UART_puts(UART_INTERFACE, "\r\n>>>>>>>> Calibration Start\r\n");
    if(!IMU_selftest())
    {
        UART_puts(UART_INTERFACE, "\r\n>>>>>>>> Selftest: FAIL\r\n");
        while(1) {};
    }
    /*Calibrate gyroscope */
    int16_t gx_offset, gy_offset, gz_offset;
    IMU_calibrate_gyro(&gx_offset, &gy_offset, &gz_offset);
    sprintf(output, ">>> Current Gyro Offsets: X %+06d, Y %+06d, Z %+06d\r\n", gx_offset,gy_offset, gz_offset);
    UART_puts(UART_INTERFACE, output);

    while(i)
    {
    LED_on(LED_B);

    IMU_read_gyro(&x, &y, &z);
    sprintf(output,">>> Gyro:  X %+06d, Y %+06d, Z %+06d\r\n", x, y, z);
    UART_puts(UART_INTERFACE, output);

    IMU_read_magno(&x, &y, &z);
    sprintf(output,">>> Magno: X %+06d, Y %+06d, Z %+06d\r\n", x, y, z);
    UART_puts(UART_INTERFACE, output);
    UART_puts(UART_INTERFACE, "--------------------------------\r\n");

    LED_off(LED_B);
    watchdog_update = 0xFF;
    Delay_ms(1000);
    i--;
    }

}
