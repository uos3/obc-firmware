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

#define SW_ON 0x02

// DEBUG_NOTES ( REMOVE LATER )
#include "../../firmware.h"
#define UART_INTERFACE UART_GNSS

typedef struct eps_master_packet_t {
  bool write:1;
  uint8_t register_id:7;
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
static bool EPS_writeRegister(uint8_t register_id, uint8_t register_value,
  eps_slave_packet_single_t *data);
static uint16_t EPS_crc(uint8_t *message, int32_t offset, int32_t length);


void EPS_init(void)
{
  UART_init(UART_EPS, EPS_BAUDRATE);
}

bool EPS_selfTest(void)
{
	/* Ref: https://github.com/uos3/eps-firmware/issues/1 */
	//EPS_readRegister(EPS_REG_RESERVED, &eps_slave_packet_single);
  //return (eps_slave_packet_single.value == EPS_ID);

  return true;
}

bool EPS_getBatteryInfo(uint16_t *output, uint8_t type)
{
  UART_puts(UART_INTERFACE, "\r\nTrying to get battery info\r\n");
	if(EPS_readRegister(type, &eps_slave_packet_single))
	{
		*output = eps_slave_packet_single.value;
		return true;
	}
	return false;
}

bool EPS_setPowerRail(uint8_t type, bool on){
  return EPS_writeRegister(SW_ON, type, &eps_slave_packet_single);
}

static bool EPS_readRegister(uint8_t register_id, eps_slave_packet_single_t *data)
{
	char c;
	uint16_t crc, bytes_expected, bytes_received;
	uint32_t timeout, current_time;

	/* Construct Master Read Packet */
	eps_master_packet.write = false;
	eps_master_packet.register_id = (uint8_t)(register_id & 0x7F); // filters first 7 bits
	eps_master_packet.value_l = 1; // no. of registers to read
	eps_master_packet.value_h = 0;
	crc = EPS_crc((uint8_t *)&eps_master_packet, 0, 3);
	eps_master_packet.crc_l = (uint8_t)crc & 0xFF;
	eps_master_packet.crc_h = (uint8_t)((crc >> 8) & 0xFF);

	/* Send Master Read Packet */
  UART_puts(UART_INTERFACE, "\r\nSending Master Read Packet\r\n");
	UART_putb(UART_EPS, (char *)&eps_master_packet, 5);
  UART_puts(UART_INTERFACE, "\r\nSend to EPS\r\n");

	bytes_expected = 6;
	bytes_received = 0;
	RTC_getTime(&current_time);
	timeout = current_time;
	while(current_time < (timeout + 2))
	{
		if(UART_getc_nonblocking(UART_EPS, &c))
		{
      UART_puts(UART_INTERFACE, "\r\nHeard back from EPS.\r\n");

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

static bool EPS_writeRegister(uint8_t register_id, uint8_t register_value, eps_slave_packet_single_t *data)
{
	char c;
	uint16_t crc, bytes_expected, bytes_received;
	uint32_t timeout, current_time;

	/* Construct Master Read Packet */
	eps_master_packet.write = true;
	eps_master_packet.register_id = (uint8_t)(register_id & 0x7F);
	eps_master_packet.value_l = register_value;
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
