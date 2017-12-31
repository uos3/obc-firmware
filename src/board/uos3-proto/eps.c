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

#define EPS_BAUDRATE		57600

#define EPS_REG_CONFIG			0x00
#define EPS_REG_STATUS			0x01
#define EPS_REG_SW_ON				0x02
#define EPS_REG_POWER				0x03
#define EPS_REG_BAT_V				0x04
#define EPS_REG_BAT_I				0x05
#define EPS_REG_BAT_T				0x06
#define EPS_REG_SOLAR_N1_I	0x07
#define EPS_REG_SOLAR_N2_I	0x08
#define EPS_REG_SOLAR_S1_I	0x09
#define EPS_REG_SOLAR_S2_I	0x0A
#define EPS_REG_SOLAR_E1_I	0x0B
#define EPS_REG_SOLAR_E2_I	0x0C
#define EPS_REG_SOLAR_W1_I	0x0D
#define EPS_REG_SOLAR_W2_I	0x0E
#define EPS_REG_SOLAR_T1_I	0x0F
#define EPS_REG_SOLAR_T2_I	0x10
#define EPS_REG_RESERVED 		0x11 // Maybe make known ID? ( https://github.com/uos3/eps-firmware/issues/1 )
#define EPS_REG_SOLAR_2_V		0x12
#define EPS_REG_RAIL1_BOOT	0x13
#define EPS_REG_RAIL1_FAIL	0x14
#define EPS_REG_RAIL1_V			0x15
#define EPS_REG_RAIL1_I			0x16
#define EPS_REG_RAIL2_BOOT	0x17
#define EPS_REG_RAIL2_FAIL	0x18
#define EPS_REG_RAIL2_V			0x19
#define EPS_REG_RAIL2_I			0x1A
#define EPS_REG_RAIL3_BOOT	0x1B
#define EPS_REG_RAIL3_FAIL	0x1C
#define EPS_REG_RAIL3_V			0x1D
#define EPS_REG_RAIL3_I			0x1E
#define EPS_REG_RAIL4_BOOT	0x1F
#define EPS_REG_RAIL4_FAIL	0x20
#define EPS_REG_RAIL4_V			0x21
#define EPS_REG_RAIL4_I			0x22
#define EPS_REG_RAIL5_BOOT	0x23
#define EPS_REG_RAIL5_FAIL	0x24
#define EPS_REG_RAIL5_V			0x25
#define EPS_REG_RAIL5_I			0x26
#define EPS_REG_RAIL6_BOOT	0x27
#define EPS_REG_RAIL6_FAIL	0x28
#define EPS_REG_RAIL6_V			0x29
#define EPS_REG_RAIL6_I			0x2A
#define EPS_REG_SUPPLY_3_V	0x2B
#define EPS_REG_SUPPLY_3_I	0x2C
#define EPS_REG_SUPPLY_5_V	0x2D
#define EPS_REG_SUPPLY_5_I	0x2E
#define EPS_REG_CHARGE_I		0x2F
#define EPS_REG_MPPT_BUS_V	0x30
#define EPS_REG_CRC_ER_CNT	0x31
#define EPS_REG_DROP_CNT		0x32

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

bool EPS_getBatteryVoltage(uint16_t *voltage)
{
	if(EPS_readRegister(EPS_REG_BAT_V, &eps_slave_packet_single))
	{
		*voltage = eps_slave_packet_single.value;
		return true;
	}
	return false;
}

static bool EPS_readRegister(uint8_t register_id, eps_slave_packet_single_t *data)
{
	char c;
	uint16_t crc, bytes_expected, bytes_received;
	uint32_t timeout, current_time;

	/* Construct Master Read Packet */
	eps_master_packet.write = false;
	eps_master_packet.register_id = (uint8_t)(register_id & 0x7F);
	eps_master_packet.value_l = 1;
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