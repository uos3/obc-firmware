/* @file util.h
 *
 * @ingroup tests
 *
 * @{
 */

#ifndef __UTIL_TEST_H__
#define __UTIL_TEST_H__

#include "../../firmware.h"

bool test_crc(void);
bool test_pn9(void);
bool test_shake(void);

bool util_tests(void);

#endif /* __UTIL_TEST_H__ */

/**
 * @}
 */