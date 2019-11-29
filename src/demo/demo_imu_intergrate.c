/**
 * File purpose:        IMU functionality demo
 * Last modification:   22/08/2019
 * Status:              Ready for the test
 */

/* firmware.h contains all relevant headers */
#include "../firmware.h"

#include <stdio.h>

uint64_t start_timestamp;
uint64_t current_time;
uint64_t timeout;

int main(void)
{
  int16_t x, y, z;
  int16_t new_gx_offset, new_gy_offset, new_gz_offset;
  int16_t mag_bias_x, mag_bias_y, mag_bias_z;
  int16_t mag_scale_x, mag_scale_y, mag_scale_z;
  char output[200];
  timeout = 30000;

  Board_init();
  watchdog_update = 0xFF;
  enable_watchdog_kick();
  UART_init(UART_INTERFACE, 9600);
  Delay_ms(2000);
  UART_puts(UART_INTERFACE, "\r\n>>>>>>>> Board Initialized\r\n");

  for(int i=0;i<3;i++)
  {LED_on(LED_B); Delay_ms(300); LED_off(LED_B);} //blink the LED three times

  IMU_Init(); // TO-DO: add data handling for null and 0 if I2C communication fails
  UART_puts(UART_INTERFACE, "\r\n>>>>>>>> IMU Initialized\r\n");

  if(!IMU_selftest()) //self test can return false if fails.
  {
    while(1) {
      UART_puts(UART_INTERFACE, "\r\n>>>>>>>> Selftest: FAIL\r\n");
      Delay_ms(3000);
    };
  } else {
    UART_puts(UART_INTERFACE, "\r\n>>>>>>>> Selftest: Pass\r\n");
  }
  /* Prints the gyro offset once*/
  int16_t gx_offset = (int16_t)I2CReceive16(I2C_IMU, 0x68, MPU_GYRO_X_OFFS_H);
	int16_t gy_offset = (int16_t)I2CReceive16(I2C_IMU, 0x68, MPU_GYRO_Y_OFFS_H);
	int16_t gz_offset = (int16_t)I2CReceive16(I2C_IMU, 0x68, MPU_GYRO_Z_OFFS_H);
  sprintf(output, ">>> Current Gyro Offsets: X %+06d, Y %+06d, Z %+06d\r\n", gx_offset,gy_offset, gz_offset);
  UART_puts(UART_INTERFACE, output);

  for(int i=0; i<5; i++)
  {
    //LED_on(LED_A); //led doesnt turn on on EQM board for unknown reasons.
    LED_on(LED_B);

    IMU_read_gyro(&x, &y, &z);
    sprintf(output,">>> Gyro:  X %+06d, Y %+06d, Z %+06d\r\n", x, y, z);
    UART_puts(UART_INTERFACE, output);

    IMU_read_magno(&x, &y, &z);
    sprintf(output,">>> Magno: X %+06d, Y %+06d, Z %+06d\r\n", x, y, z);
    UART_puts(UART_INTERFACE, output);
    UART_puts(UART_INTERFACE, "--------------------------------\r\n");

    //LED_off(LED_B);
    watchdog_update = 0xFF;
    Delay_ms(1000);
    for(int i=0;i<3;i++)
    {LED_off(LED_B); Delay_ms(300); LED_on(LED_B); Delay_ms(300);} //blink the LED three times
  }
  LED_off(LED_B);


  UART_puts(UART_INTERFACE, "\r\n>>>>>>>> Calibration Started\r\n");
  IMU_calibrate_gyro(&new_gx_offset, &new_gy_offset, &new_gz_offset);
  UART_puts(UART_INTERFACE, "\r\n>>>>>>>> Calibration Completed\r\n");
  sprintf(output, ">>> New Gyro Offsets: X %+06d, Y %+06d, Z %+06d\r\n", new_gx_offset,new_gy_offset, new_gz_offset);
  UART_puts(UART_INTERFACE, output);
  sprintf(output, ">>> Old Gyro Offsets: X %+06d, Y %+06d, Z %+06d\r\n", gx_offset,gy_offset, gz_offset);
  UART_puts(UART_INTERFACE, output);


  IMU_calibrate_magno(&mag_bias_x, &mag_bias_y, &mag_bias_z, &mag_scale_x, &mag_scale_y, &mag_scale_z);
  sprintf(output, ">>> Magno Hard Iron Offsets: X %+06d, Y %+06d, Z %+06d\r\n", mag_bias_x,mag_bias_y, mag_bias_z);
  UART_puts(UART_INTERFACE, output);
  Delay_ms(1000);
  sprintf(output, ">>> Magno Soft Iron Offsets: X %+06d, Y %+06d, Z %+06d\r\n", mag_scale_x,mag_scale_y, mag_scale_z);
  UART_puts(UART_INTERFACE, output);
  Delay_ms(5000);

  while(1){

    IMU_read_gyro(&x, &y, &z);
    sprintf(output,">>> Gyro:  X %+06d, Y %+06d, Z %+06d\r\n", x, y, z);
    UART_puts(UART_INTERFACE, output);

    IMU_read_magno(&x, &y, &z);
    sprintf(output,">>> Magno: X %+06d, Y %+06d, Z %+06d\r\n", x-mag_bias_x, y-mag_bias_y, z-mag_bias_z);
    UART_puts(UART_INTERFACE, output);
    UART_puts(UART_INTERFACE, "--------------------------------\r\n");

    /* Placholder code in the event that the output reading with the bias is over the limit in both maximum and minimum*/
    /*if(x-mag_bias_x > 32767 || x-mag_bias_x < -32767 || y-mag_bias_y > 32767 || y-mag_bias_y <-32767 || z-mag_bias_y <-32767 || z-mag_bias_y > 32767){
      sprintf(output,">>> Magno: X %+06d, Y %+06d, Z %+06d\r\n", x-mag_bias_x, y-mag_bias_y, z-mag_bias_z);
      UART_puts(UART_INTERFACE, "\r\n>>>>>>>> Calibration Exceeded maximum reading! Figure Eight Pattern must be redone!\r\n");
      IMU_calibrate_magno(&mag_bias_x, &mag_bias_y, &mag_bias_z, &mag_scale_x, &mag_scale_y, &mag_scale_z);
      sprintf(output, ">>> Magno Hard Iron Offsets: X %+06d, Y %+06d, Z %+06d\r\n", mag_bias_x,mag_bias_y, mag_bias_z);
      UART_puts(UART_INTERFACE, output);
      Delay_ms(1000);
      sprintf(output, ">>> Magno Soft Iron Offsets: X %+06d, Y %+06d, Z %+06d\r\n", mag_scale_x,mag_scale_y, mag_scale_z);
      UART_puts(UART_INTERFACE, output);
      Delay_ms(5000);
    }*/

    watchdog_update = 0xFF;
    Delay_ms(1000);
    for(int i=0;i<3;i++)
    {LED_off(LED_B); Delay_ms(300); LED_on(LED_B); Delay_ms(300);} //blink the LED three times
  }

}
