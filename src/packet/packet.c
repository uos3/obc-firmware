/**
 * @ingroup packet
 *
 * @file packet.c
 *
 * @{
 */

#include "../firmware.h"

uint8_t *Packet_telecommand_512_encode(packet_telecommand_512 *packet, uint16_t origin, uint8_t *key, uint32_t key_length)
{
  packet->spacecraft = origin;

  Packet_sign_shake128(((packet_telecommand_512_hash *)packet)->data, 46, key, key_length, packet->hash);

  packet->crc = Util_crc16(((packet_telecommand_512_crc *)packet)->data, 62);

  Util_pn9((uint8_t *)packet, 64);

  /* TODO: Add encoder for 2x k=256 r=1/2 blocks (TC512) LDPC */

  Packet_interleave_32x32((uint8_t *)packet);

  return (uint8_t *)packet;
}

bool Packet_telecommand_512_decode(uint8_t *input_buffer, packet_telecommand_512 *output, uint8_t *key, uint32_t key_length)
{
  Packet_uninterleave_32x32(input_buffer);

  /* TODO: Add hard decoder for 2x k=256 r=1/2 blocks (TC512) LDPC */

  Util_pn9(input_buffer, 64);

  memcpy(input_buffer, output, 64);

  uint16_t packet_crc;
  packet_crc = Util_crc16(((packet_telecommand_512_crc *)output)->data, 62);
  if(memcmp(&packet_crc, &output->crc, 16) != 0)
  {
    return false;
  }

  uint8_t packet_hash[16];
  Packet_sign_shake128(((packet_telecommand_512_hash *)output)->data, 46, key, key_length, packet_hash);
  if(memcmp(packet_hash, output->hash, 16) != 0)
  {
    return false;
  }

  return true;
}


uint8_t *Packet_telemetry_1024_encode(packet_telemetry_1024 *packet, uint16_t origin, uint8_t *key, uint32_t key_length)
{
  packet->spacecraft = origin;

  Packet_sign_shake128(((packet_telemetry_1024_hash *)packet)->data, 110, key, key_length, packet->hash);

  packet->crc = Util_crc16(((packet_telemetry_1024_crc *)packet)->data, 126);

  Util_pn9((uint8_t *)packet, 128);

  /* TODO: Add encoder for Matt's r=1/3 Turbo FEC */

  Packet_interleave_64x64((uint8_t *)packet);

  // 3072-bit output

  return (uint8_t *)packet;
}

bool Packet_telemetry_1024_decode(uint8_t *input_buffer, packet_telemetry_1024 *output, uint8_t *key, uint32_t key_length)
{

  Packet_uninterleave_64x64(input_buffer);

  // 3072-bit input

  /* TODO: Add hard-decoder for Matt's r=1/3 Turbo FEC (not used on the spacecraft) */

  Util_pn9(input_buffer, 128);

  memcpy(input_buffer, output, 128);

  uint16_t packet_crc;
  packet_crc = Util_crc16(((packet_telemetry_1024_crc *)output)->data, 126);
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