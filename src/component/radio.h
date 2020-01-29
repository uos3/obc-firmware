

#ifndef __RADIO_H__
#define __RADIO_H__


#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "cc112x.h"
#include "cc112x_spi.h"
//#include "../firmware.h"
//#include "../cc1125/cc1125.h"


/*
Giles' Notes
http://www.ti.com/lit/ug/swru295e/swru295e.pdf
38.4MHz XOSC
38.4kHz RC osc
To sniff use eWOR with PQT or CS termination (prob PQT)
Only set freq registers in idle mode
*/

//And indicator in dBm of the received signal power from -128 to 128, -128 indicates invalid value
double RSSI;

typedef struct radio_config_t{
	const double frequency; // MHz
	double power; // dBm
	cc112x_fsk_symbolrate_t symbolRate;
}radio_config_t ;

typedef enum {
	RADIO_STATUS_OK,
	RADIO_STATUS_BUSY,
	RADIO_STATUS_WRONGPART
} Radio_Status_t;

Radio_Status_t Radio_tx_msk(radio_config_t *radio_config, uint8_t *data_buffer, uint32_t data_length, void *end_of_tx_handler(void));

Radio_Status_t Radio_tx_fsk(radio_config_t *radio_config, uint8_t *data_buffer, uint32_t data_length, void *end_of_tx_handler(void));

Radio_Status_t Radio_tx_morse(radio_config_t *radio_config, uint8_t *text_buffer, uint32_t text_length, void *end_of_tx_handler(void));

Radio_Status_t Radio_tx_off(radio_config_t *radio_config);

Radio_Status_t Radio_rx_receive(radio_config_t *radio_config, uint8_t *receive_buffer, uint32_t receive_length, void received_packet_handler(void));

Radio_Status_t Radio_rx_off(radio_config_t *radio_config);

void cw_tone_on(radio_config_t *radio_config);

void cw_tone_off(void);




#endif /*  __RADIO_H__ */ 