/**
 * @ingroup uos3-proto
 * @ingroup eps
 *
 * @file eps.c
 *
 * @{
 */

#include "board.h"
#include "../uart.h"
#include "../rtc.h"
#include "../eps.h"

// DEBUG_NOTES ( REMOVE LATER )
#include "../../firmware.h"

typedef struct eps_master_packet_t {
  uint8_t register_id:7;
  bool write:1;
  uint8_t value_l;
  uint8_t value_h;
  uint8_t crc_l;
  uint8_t crc_h;
} eps_master_packet_t;

typedef struct eps_slave_packet_single_t {
  bool 		reserved_a:1;
  uint8_t register_id:7;
  bool 		reserved_b:1;
  uint8_t register_quantity:7;
  uint16_t value;
  uint16_t crc;
} eps_slave_packet_single_t;

static eps_master_packet_t eps_master_packet;
static eps_slave_packet_single_t eps_slave_packet_single;

static bool EPS_readRegister(uint8_t register_id, eps_slave_packet_single_t *data);
static bool EPS_writeRegister(uint8_t register_id, uint8_t register_value);
static uint16_t EPS_crc(uint8_t *message, int32_t offset, int32_t length);


void EPS_init(void)
{
  	UART_init(UART_EPS, EPS_BAUDRATE);
}



bool EPS_selfTest(void)
{
	/* Ref: https://github.com/uos3/eps-firmware/issues/1 */
	uint8_t attempts = 0;		//Writing to register and retrying if failed
	while (attempts < 3 && !EPS_readRegister(EPS_KNOWN_VAL, &eps_slave_packet_single)) {
		attempts++;
	}
	if (attempts > 2) {
		EPS_COMMS_FAILS++;
	}
	return (eps_slave_packet_single.value == EPS_ID);	//Should give a value of 0x42

}
 
bool EPS_getInfo(uint16_t *output, uint8_t regID)
{
  UART_puts(UART_INTERFACE, "\r\nTrying to get reg data\r\n");
  uint8_t attempts = 0;		//Writing to register and retrying if failed
  /*while (attempts < 3 && !EPS_readRegister(regID, &eps_slave_packet_single)) {
	  attempts++;
  }
  if (attempts <3) {
	  *output = eps_slave_packet_single.value;
	  return true;
  }
  EPS_COMMS_FAILS++;
  return false;*/
  if(EPS_readRegister(regID, &eps_slave_packet_single)){
	  *output = eps_slave_packet_single.value;
	  return true;
  }else{
	  return false;
  }
}

bool EPS_togglePowerRail(uint8_t regVal)	{
	//A 1 will toggle the state of the relevant rail in the input byte, rail bit positions specified in header
	uint8_t attempts = 0;
	while (attempts < 3 && !EPS_writeRegister(EPS_REG_SW_ON, regVal)) {
		attempts++;
	}
	if (attempts > 2) {
		EPS_COMMS_FAILS++;
		return false;
	}
	return true;
}
//when received the command to turn off the MCU rail, the EPS turns it off and after 5ss turns it back ON - this operation was confirmed!
bool EPS_setPowerRail(uint8_t regVal) {		//Requires intended states for every rail
	uint8_t state = EPS_getPowerRail();
	state ^= regVal;		//Finding the difference between the current states and the targets 
	return EPS_togglePowerRail(state);

}

bool EPS_resetPowerRail(uint8_t regVal)	{
	if (EPS_togglePowerRail(regVal))	{
		Delay_ms(2000);
		return EPS_togglePowerRail(regVal);
	}
	return false
}

uint8_t EPS_getPowerRail() {
	uint8_t attempts = 0;
	while (attempts < 3 && !EPS_readRegister(EPS_REG_SW_ON, &eps_slave_packet_single)) {
		attempts++;
	}
	if (attempts > 2) {
		EPS_COMMS_FAILS++;
		return 0;
	}
	return eps_slave_packet_single.value;
}


static bool EPS_readRegister(uint8_t register_id, eps_slave_packet_single_t *data)
{
	char c;
	uint8_t temp;
	uint16_t crc, bytes_expected, bytes_received;
	uint32_t timeout, current_time;
	
	/* Construct Master Read Packet */
	eps_master_packet.write = false;
	eps_master_packet.register_id = (register_id & 0x7F); // filters first 7 bits
	eps_master_packet.value_l = 1; // no. of registers to read
	eps_master_packet.value_h = 0;
	crc = EPS_crc((uint8_t *)&eps_master_packet, 0, 3);
	eps_master_packet.crc_l = (uint8_t)crc & 0xFF;
	eps_master_packet.crc_h = (uint8_t)((crc >> 8) & 0xFF);

	/* Send Master Read Packet */
	UART_putb(UART_EPS, (char *)&eps_master_packet, 5);

	bytes_expected = 6;
	bytes_received = 0;
	RTC_getTime(&current_time);
	timeout = current_time;
	while(current_time < (timeout + 2))
	{
		if(UART_getc_nonblocking(UART_EPS, &c))
		{
      //UART_puts(UART_INTERFACE, "\r\nHeard back from EPS.\r\n");

			((char *)(data))[bytes_received] = c;
			if(++bytes_received == bytes_expected)
			{
				break;
			}
		}
		RTC_getTime(&current_time);
	}
	if(bytes_received != bytes_expected)
	{
		return false;
	}

	crc = EPS_crc((uint8_t *)data, 0, 4);
	if(crc != data->crc)
	{
		return false;
	}

	return true;
}

static bool EPS_writeRegister(uint8_t register_id, uint8_t register_value)
{
	char c;	
	uint8_t temp;
	uint16_t crc, bytes_expected, bytes_received;
	uint32_t timeout, current_time;
	//eps_slave_packet_single_t *data;
	
	/* Construct Master Read Packet */
	eps_master_packet.write = true;
	eps_master_packet.register_id = (uint8_t)(register_id & 0x7F);
	eps_master_packet.value_l = register_value;
	eps_master_packet.value_h = 0;
	crc = EPS_crc((uint8_t *)&eps_master_packet, 0, 3);
	eps_master_packet.crc_l = (uint8_t)crc & 0xFF;
	eps_master_packet.crc_h = (uint8_t)((crc >> 8) & 0xFF);

	/* Send Master Read Packet */
	temp = (eps_master_packet.write<<7) | (eps_master_packet.register_id);
	//UART_putb(UART_EPS, (char *)&eps_master_packet, 5);
	UART_putc(UART_EPS, (char)temp);	//print each part of the message separately to avoid complicated casting
	//UART_putb(UART_EPS, ((char *)&eps_master_packet+1), 5); //use this eventually to start from the value_l variable
	UART_putc(UART_EPS, (char)eps_master_packet.value_l);
	UART_putc(UART_EPS, (char)eps_master_packet.value_h);
	UART_putc(UART_EPS, (char)eps_master_packet.crc_l);
	UART_putc(UART_EPS, (char)eps_master_packet.crc_h);
	
	bytes_expected = 6;
	bytes_received = 0;
	RTC_getTime(&current_time);
	timeout = current_time;
	while(current_time < (timeout + 2))
	{
		if(UART_getc_nonblocking(UART_EPS, &c))
		{
			((char *)(&eps_slave_packet_single))[bytes_received] = c;
			if(++bytes_received == bytes_expected)
			{
				break;
			}
		}
		RTC_getTime(&current_time);
	}
	if(bytes_received != bytes_expected)
	{
		return false;
	}

	crc = EPS_crc((uint8_t *)&eps_slave_packet_single, 0, 4);
	if(crc != (&eps_slave_packet_single)->crc)
	{
		return false;
	}

	return true;
}

/* Implementation copied from eps-firmware/PowerMcu/PowerMcu/util/crc.c */
// Generate cyclic reduncancy check code
static uint16_t EPS_crc(uint8_t *message, int32_t offset, int32_t length)
{
	uint16_t crcFull = 0xFFFF;
	int32_t i, j;

	for (i = offset; i < length; i++)
	{
		crcFull = (uint16_t)(crcFull ^ message[i]);

		for (j = 0; j < 8; j++)
		{
			uint8_t crcLsb = (uint8_t)(crcFull & 0x0001);
			crcFull = (uint16_t)((crcFull >> 1) & 0x7FFF);

			if (crcLsb == 1)
				crcFull = (uint16_t)(crcFull ^ 0xA001);
		}
	}
	return crcFull;
}

/**
 * @}
 */
