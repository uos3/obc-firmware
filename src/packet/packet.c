/**
 * @ingroup packet
 *
 * @file packet.c
 *
 * @{
 */

#include "../firmware.h"

uint8_t *Packet_telemetry_1024_encode(packet_telemetry_1024 *packet, uint16_t origin, uint8_t *key, uint32_t key_length)
{
  packet->spacecraft = origin;

  Packet_sign_shake128(((packet_telemetry_1024_hash *)packet)->data, 110, key, key_length, packet->hash);

  Packet_crc16(((packet_telemetry_1024_crc *)packet)->data, 126, &packet->crc);

  Packet_pn9_xor((uint8_t *)packet, 128);

  /* TODO: r=1/4, k=1024, n=4096 Polar FEC */

  Packet_interleave_64x64((uint8_t *)packet);

  return (uint8_t *)packet;
}

uint8_t *Packet_telecommand_512_encode(packet_telecommand_512 *packet, uint16_t origin, uint8_t *key, uint32_t key_length)
{
	packet->spacecraft = origin;

  Packet_sign_shake128(((packet_telecommand_512_hash *)packet)->data, 46, key, key_length, packet->hash);

  Packet_crc16(((packet_telecommand_512_crc *)packet)->data, 62, &packet->crc);

  Packet_pn9_xor((uint8_t *)packet, 64);

  /* TODO: r=1/2, k=256, n=512 131.0-B-2 LDPC FEC */

  Packet_interleave_32x32((uint8_t *)packet);

  return (uint8_t *)packet;
}

bool Packet_telemetry_1024_decode(uint8_t *input_buffer, packet_telemetry_1024 *output, uint8_t *key, uint32_t key_length)
{

  Packet_uninterleave_32x32(input_buffer);

  /* TODO: r=1/4, k=1024, n=4096 Polar FEC */

  Packet_pn9_xor(input_buffer, 128);

  memcpy(input_buffer, output, 128);

  uint16_t packet_crc;
  Packet_crc16(((packet_telemetry_1024_crc *)output)->data, 126, &packet_crc);
  if(memcmp(&packet_crc, &output->crc, 16) != 0)
  {
  	return false;
  }

  uint8_t packet_hash[16];
  Packet_sign_shake128(((packet_telemetry_1024_hash *)output)->data, 110, key, key_length, packet_hash);
  if(memcmp(packet_hash, output->hash, 16) != 0)
  {
  	return false;
  }

  return true;
}

/**
 * @}
 */