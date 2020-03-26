/**
 * @ingroup packet
 *
 * @file packet.c
 *
 * @{
 */

#include "../firmware.h"

void Packet_sign_shake128(const uint8_t *input, uint32_t input_length, const uint8_t *key, uint32_t key_length, uint8_t *output)
{
  util_shake_ctx_t shake_ctx;
  Util_shake_init(&shake_ctx, 16);

  Util_shake_update(&shake_ctx, input, input_length);
  Util_shake_update(&shake_ctx, key, key_length);

  Util_shake_out(&shake_ctx, output);
}

void Packet_telecommand_512_encode(packet_telecommand_512 *input_packet, uint8_t *output_buffer, const uint16_t origin, const uint8_t *key, uint32_t key_length)
{
  input_packet->spacecraft = origin;

  /* Authentication hash */
  Packet_sign_shake128(((packet_telecommand_512_hash *)input_packet)->data, 46, key, key_length, input_packet->hash);

  /* Integrity CRC */
  input_packet->crc = Util_crc16(((packet_telecommand_512_crc *)input_packet)->data, 62);

  /* Whiten input data to improve FEC performance */
  Util_pn9((uint8_t *)input_packet, 0, 64);

  /* There's no CCSDS k=512, r=1/2 LDPC, so we're appending 2x k=256 r=1/2 blocks */
  labrador_ldpc_copy_encode(LABRADOR_LDPC_CODE_TC512, (uint8_t *)input_packet, output_buffer);
  labrador_ldpc_copy_encode(LABRADOR_LDPC_CODE_TC512, &((uint8_t *)input_packet)[32], &output_buffer[64]);

  /* Interleave to improve FEC performance */
  Packet_interleave_32x32(output_buffer);
}

bool Packet_telecommand_512_decode(uint8_t *input_buffer, packet_telecommand_512 *output_packet, const uint16_t destination, const uint8_t *key, uint32_t key_length)
{
  uint8_t ldpc_wa[LABRADOR_LDPC_BF_WORKING_LEN_TC512];
  uint8_t ldpc_out[LABRADOR_LDPC_OUTPUT_LEN_TC512];

  Packet_uninterleave_32x32(input_buffer);

  /* Attempt decode of first half of packet */
  if(!labrador_ldpc_decode_bf(LABRADOR_LDPC_CODE_TC512,
                             input_buffer,
                             (uint8_t *)output_packet,
                             ldpc_wa,
                             50, /* Iterations to use. TODO: Vary this for CPU budget */
                             NULL))
  {
    return false;
  }

  /* Un-whiten first half of packet */
  Util_pn9((uint8_t *)output_packet, 0, 32);

  /* Check of the spacecraft id matching our destination, easy and cheap initial sanity check */
  if(output_packet->spacecraft != destination)
  {
    return false;
  }

  /* Attempt decode of second half of packet */
  if(!labrador_ldpc_decode_bf(LABRADOR_LDPC_CODE_TC512,
                             &input_buffer[64],
                             ldpc_out,
                             ldpc_wa,
                             50, /* Iterations to use. TODO: Vary this for CPU budget */
                             NULL))
  {
    return false;
  }

  memcpy(&((uint8_t *)output_packet)[32], ldpc_out, 32);

  /* Un-whiten second half of packet */
  Util_pn9(&((uint8_t *)output_packet)[32], 32, 32);

  /* Check packet CRC */
  if(output_packet->crc != Util_crc16(((packet_telecommand_512_crc *)output_packet)->data, 62))
  {
    return false;
  }

  /* Check packet authentication hash */
  uint8_t packet_hash[16];
  Packet_sign_shake128(((packet_telecommand_512_hash *)output_packet)->data, 46, key, key_length, packet_hash);
  if(memcmp(packet_hash, output_packet->hash, 16) != 0)
  {
    return false;
  }

  return true;
}


void Packet_telemetry_1024_encode(packet_telemetry_1024 *input_packet, uint8_t *output_buffer, const uint16_t origin, const uint8_t *key, uint32_t key_length)
{
  input_packet->spacecraft = origin;

  Packet_sign_shake128(((packet_telemetry_1024_hash *)input_packet)->data, 110, key, key_length, input_packet->hash);

  input_packet->crc = Util_crc16(((packet_telemetry_1024_crc *)input_packet)->data, 126);

  Util_pn9((uint8_t *)input_packet, 0, 128);

  /* TODO: Add encoder for Matt's r=1/3 Turbo FEC */
  /* packet[1024b] -> output_buffer[3072b] */
  memcpy(output_buffer, input_packet, sizeof(packet_telemetry_1024));

  /* TODO: Need a rectangular interleaver */
  //Packet_interleave_64x64((uint8_t *)output_buffer);
}

bool Packet_telemetry_1024_decode(uint8_t *input_buffer, packet_telemetry_1024 *output_packet, const uint16_t destination, const uint8_t *key, uint32_t key_length)
{
  /* TODO: Need a rectangular interleaver */
  //Packet_uninterleave_64x64(input_buffer);

  /* TODO: Add hard-decoder for Matt's r=1/3 Turbo FEC (not used on the spacecraft) */
  /* input_buffer[3072b] -> output_packet [1024b] */
  memcpy(output_packet, input_buffer, sizeof(packet_telemetry_1024));

  Util_pn9((uint8_t *)output_packet, 0, 128);

  /* Check of the spacecraft id matching our destination, easy and cheap initial sanity check */
  if(output_packet->spacecraft != destination)
  {
    return false;
  }

  if(output_packet->crc != Util_crc16(((packet_telemetry_1024_crc *)output_packet)->data, 126))
  {
  	return false;
  }

  uint8_t packet_hash[16];
  Packet_sign_shake128(((packet_telemetry_1024_hash *)output_packet)->data, 110, key, key_length, packet_hash);
  if(memcmp(packet_hash, output_packet->hash, 16) != 0)
  {
  	return false;
  }

  return true;
}

/**
 * @}
 */