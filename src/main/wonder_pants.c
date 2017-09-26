// now modified to watchdog kick on interrupt

// a home for all the orphaned functions and routines that are written without hardware available to check whether they actually work...

//wonder pants indeed. Let's hope the hardware works according to the spec with no bugs, functions or any other real-life occurrences

// not recommended or particularly reliable without further testing, especially for a satellite, but hey let's press on with wonder pants!

// S Lucarotti 25/9/17

#include "../firmware.h"


#define DISP3(x,y,z) UART_putstr(DEBUG_SERIAL,x,y,z); // x and z are strings, y is a number shown as signed base10 16bit
#define DISP2(y,z) UART_putstr(DEBUG_SERIAL,NULL,y,z); 
#define DISP1(x) UART_puts(DEBUG_SERIAL,x);

#define GPS_SERIAL UART_PC104_HEADER
#define CAM_SERIAL UART_CAM_HEADER

#define DEBUG_SERIAL GPS_SERIAL


// Antenna burn through code

void antenna_initialise()
 {
 	// enable GPIO PB5 as output and PB4 as input
 	// this shouldn't be necessary, but here just in case
 }

void deploy_antenna(void) // one 5s burn is the description // may need to be clear whether gate needs 2,4 or 8mA and set accordingly
 {
  GPIO_set(GPIO_PB5);
  Delay_ms(5000);
  GPIO_reset(GPIO_PB5);
 }

 bool read_antenna_deployment_switch(void)
 {
  return GPIO_read(GPIO_PB4);
 }

// Radio Thermistor read code // on I2C0 (PB2,PB3 SCL,SDA)

 void initialise_radio_thermistors(void)
  {
  	InitI2C0(); // initialise correct interface on processor - both temperature sensors on same i2c line
  }

 signed int read_pct2075_temp(void) // radio amplifier temperature slave address 1001 000 (to 1001 111) depending on pins
 {  //  register with temp data is 00, others are configuration, hysteresis, overtemperature shutdown, tilde (sampling rate) and temp conversion
 	// temp is 2 byte, MSB then LSB, 2s complement. each is .125C bottom 5 bits ignored
 	// to read 1. scl sda free, send slave address, pointer ,write bit
 	// then read slave address, then receive 2 bytes

 const unsigned char pct2075_address=0x48; // %1001000;
 signed int data = I2CReceive16(pct2075_address,0,I2C0_BASE);
 return data;
 }

 //signed int read_

 //void tmp_

int main(void)
{
    Board_init();
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); // turn on clock

    UART_init(DEBUG_SERIAL, 115200);  UART_puts(DEBUG_SERIAL,"\n\n\r    Wonder Pants!\n\r");
    UART_init(CAM_SERIAL, 115200);    UART_puts(CAM_SERIAL,"\n\n\r    Wonder Pants!\n\r");
    
    Delay_ms(1000); // so can be clearly seen when watchdog kicking

    setupwatchdoginterrupt();	// kick the watchdog on interrupt

 //   antenna_initialise();
    initialise_radio_thermistors();

	while(1)
    {
   // 	deploy_antenna();
   // 	bool x=read_antenna_deployment_switch();

    	signed int temp=read_pct2075_temp();

    	DISP3("Temp = ",temp,"\r\n");
     }

}
