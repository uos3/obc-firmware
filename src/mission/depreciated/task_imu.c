#include <stdio.h>
#include <stdint.h>
#include "../firmware.h"


int8_t get_imu_data(){
    LED_on(LED_B)
    UART_puts(UART_INTERFACE, "Getting IMU payload data...\r\n");
  
    // TODO: Generate dataset id
	//add_telemetry((uint16_t)0, 2);

	// TODO: Get timestamp
	//add_telemetry((uint32_t)0, 4);

	// TODO: Get IMU temperature based on Ed's new driver

	// Take multiple samples
	char gyro_string[20] = "", accel_string[20] = "";
	int16_t accel_data[3], gyro_data[3];
	IMU_read_accel(accel_data[0], accel_data[1], accel_data[2]);
	IMU_read_gyro(gyro_data[0], gyro_data[1], gyro_data[2]);

	//add_telemetry((int16_t)&accel_data, 6); // 2bytes * 3
	//add_telemetry((int16_t)&gyro_data, 6); // 2bytes * 3
    sprintf(gyro_string, "GYRO : X: %" PRId16 ", Y: %" PRId16 " Z: %" PRId16 "\r\n", gyro_data[0], gyro_data[1], gyro_data[2]);
    sprintf(accel_string, "ACCEL : X: %" PRId16 ", Y: %" PRId16 " Z: %" PRId16 "\r\n", accel_data[0], accel_data[1], accel_data[2]);

    UART_puts(UART_INTERFACE, gyro_string);
    UART_puts(UART_INTERFACE, accel_string);

	//Delay_ms(500); // TODO: update time once it has been determined


	//end_telemetry();

    
    UART_puts(UART_INTERFACE, "Finished getting IMU payload data.\r\n");
    LED_off(LED_B);
    return 0;
}