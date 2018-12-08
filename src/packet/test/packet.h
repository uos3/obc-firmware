/* @file packet.h
 *
 * @ingroup tests
 *
 * @{
 */

#ifndef __PACKET_TEST_H__
#define __PACKET_TEST_H__

#include "../../firmware.h"

bool test_interleave(void);
bool test_crc16(void);
bool test_sign(void);
bool test_ldpc(void);

bool packet_tests(void);

#endif /* __PACKET_TEST_H__ */

/**
 * @}
 */