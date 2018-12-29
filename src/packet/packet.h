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

// Compile-time assertions in < C11, Source: https://stackoverflow.com/a/3385694
#define STATIC_ASSERT(COND,MSG) typedef char static_assertion_##MSG[(!!(COND))*2-1]
#define COMPILE_TIME_ASSERT3(X,L) STATIC_ASSERT(X,static_assertion_at_line_##L)
#define COMPILE_TIME_ASSERT2(X,L) COMPILE_TIME_ASSERT3(X,L)
#define COMPILE_TIME_ASSERT(X)    COMPILE_TIME_ASSERT2(X,__LINE__)

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

COMPILE_TIME_ASSERT(sizeof(packet_telemetry_1024)==128);

typedef struct packet_telemetry_1024_hash {
  uint8_t data[110];
  uint8_t hash[16];
  uint16_t _crc;
} packet_telemetry_1024_hash;

COMPILE_TIME_ASSERT(sizeof(packet_telemetry_1024_hash)==128);

typedef struct packet_telemetry_1024_crc {
  uint8_t data[126];
  uint16_t crc;
} packet_telemetry_1024_crc;

COMPILE_TIME_ASSERT(sizeof(packet_telemetry_1024_crc)==128);

// Telecommand with 256 bit (: 128b hash: 10 bytes data, 64b hash: 18 bytes data


typedef struct packet_telecommand_512 {
  uint16_t spacecraft;
  uint16_t type;
  uint8_t data[42]; /* 336 bits */
  uint8_t hash[16];
  uint16_t crc;
} packet_telecommand_512;

COMPILE_TIME_ASSERT(sizeof(packet_telecommand_512)==64);

typedef struct packet_telecommand_512_hash {
  uint8_t data[46];
  uint8_t hash[16];
  uint16_t _crc;
} packet_telecommand_512_hash;

COMPILE_TIME_ASSERT(sizeof(packet_telecommand_512_hash)==64);

typedef struct packet_telecommand_512_crc {
  uint8_t data[62];
  uint16_t crc;
} packet_telecommand_512_crc;

COMPILE_TIME_ASSERT(sizeof(packet_telecommand_512_crc)==64);

typedef enum {
	PACKET_TYPE_NULL = 0
} packet_type;

void Packet_telecommand_512_encode(packet_telecommand_512 *input_packet, uint8_t *output_buffer, const uint16_t origin, const uint8_t *key, uint32_t key_length);

bool Packet_telecommand_512_decode(uint8_t *input_buffer, packet_telecommand_512 *output_packet, const uint16_t destination, const uint8_t *key, uint32_t key_length);

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
void Packet_telemetry_1024_encode(packet_telemetry_1024 *input_packet, uint8_t *output_buffer, const uint16_t origin, const uint8_t *key, uint32_t key_length);

bool Packet_telemetry_1024_decode(uint8_t *input_buffer, packet_telemetry_1024 *output_packet, const uint16_t destination, const uint8_t *key, uint32_t key_length);

void Packet_sign_shake128(const uint8_t *input, uint32_t input_length, const uint8_t *key, uint32_t key_length, uint8_t *output);

#endif /* __PACKET_FORMAT_H__ */