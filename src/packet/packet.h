/**
 * @defgroup packet
 *
 * @brief Packet Functions & Definitions
 *
 * @details 
 *
 * @ingroup packet
 */

#ifndef __PACKET_FORMAT_H__
#define __PACKET_FORMAT_H__

#include "../firmware.h"

#define PACKET_PREAMBLE_WORD 0x55555555
#define PACKET_PREAMBLE_LENGTH 4

#define PACKET_SYNC_WORD 0x42A6F8B3
#define PACKET_SYNC_LENGTH 4

#define PACKET_SPACECRAFT_ID 0x4242
#define PACKET_CODED_DATA_LENGTH 1024

typedef struct packet_telemetry_1024 {
  uint16_t spacecraft;
  uint16_t type;
  uint8_t data[106]; /* 848 bits */
  uint8_t hash[16];
  uint16_t crc;
} packet_telemetry_1024;

typedef struct packet_telemetry_1024_hash {
  uint8_t data[110];
  uint8_t hash[16];
  uint16_t _crc;
} packet_telemetry_1024_hash;

typedef struct packet_telemetry_1024_crc {
  uint8_t data[126];
  uint16_t crc;
} packet_telemetry_1024_crc;


typedef struct packet_telecommand_512 {
  uint16_t spacecraft;
  uint16_t type;
  uint8_t data[42]; /* 336 bits */
  uint8_t hash[16];
  uint16_t crc;
} packet_telecommand_512;

typedef struct packet_telecommand_512_hash {
  uint8_t data[110];
  uint8_t hash[16];
  uint16_t _crc;
} packet_telecommand_512_hash;

typedef struct packet_telecommand_512_crc {
  uint8_t data[126];
  uint16_t crc;
} packet_telecommand_512_crc;

typedef enum {
	PACKET_TYPE_NULL = 0
} packet_type;

/**
 * @function
 * @ingroup packet
 *
 * Prepare a data-populated packet struct for transmission.
 *
 * Before using this function, the data and type fields must be populated.
 *
 * Returns a binary buffer for direct transmission (excluding preamble and sync)
 *
 * @param packet Pointer to the packet struct to be prepared
 * @param origin Spacecraft ID of origin
 * @param key Pointer to the key to be signed with
 * @param key_length Length of the key in bytes
 * @returns Pointer to the packet buffer ready for transmission
 */
uint8_t *Packet_telemetry_1024_encode(packet_telemetry_1024 *packet, uint16_t origin, uint8_t *key, uint32_t key_length);

uint8_t *Packet_telecommand_512_encode(packet_telecommand_512 *packet, uint16_t origin, uint8_t *key, uint32_t key_length);

bool Packet_telemetry_1024_decode(uint8_t *input_buffer, packet_telemetry_1024 *output, uint8_t *key, uint32_t key_length);

#endif /* __PACKET_FORMAT_H__ */