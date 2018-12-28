/**
 * @defgroup radio
 *
 * @brief Radio Peripheral Driver
 *
 * @details Hardware Radio Peripheral Driver.
 *
 * @ingroup drivers
 */

#ifndef __RADIO_H__
#define __RADIO_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct radio_config_t {
	double frequency; // MHz
	double power; // dBm
} radio_config_t;


void Radio_tx_msk(radio_config_t *radio_config, uint8_t *data_buffer, uint32_t data_length, void *end_of_tx_handler);

void Radio_tx_fsk(radio_config_t *radio_config, uint8_t *data_buffer, uint32_t data_length, void *end_of_tx_handler);

void Radio_tx_morse(radio_config_t *radio_config, uint8_t *text_buffer, uint32_t text_length, void *end_of_tx_handler);

void Radio_tx_standby(radio_config_t *radio_config);

void Radio_tx_off(radio_config_t *radio_config);


void Radio_rx_receive(radio_config_t *radio_config, void *received_packet_handler);

void Radio_rx_standby(radio_config_t *radio_config);

void Radio_rx_off(radio_config_t *radio_config);


#endif /*  __RADIO_H__ */