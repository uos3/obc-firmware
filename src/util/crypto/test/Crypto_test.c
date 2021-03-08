/**
 * @ingroup crypto
 * 
 * @file EventManager_test.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Test functionality of the Crypto module.
 * @version 1.0
 * @date 2020-12-18
 * 
 * @copyright Copyright (c) 2020
 * 
 * @defgroup crypto_test Crypto Test
 * @{
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdint.h>

/* External library includes */
#include <cmocka.h>

/* Internal includes */
#include "util/crypto/Crypto_public.h"

/* -------------------------------------------------------------------------   
 * TESTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Test that CRC values are calculated correctly
 * 
 * @param state cmocka state
 */
static void Crypto_test_crc_value(void **state) {
    (void) state;
    Crypto_Crc32 calculated_crc32 = 0;
    Crypto_Crc16 calcualted_crc16 = 0;
    /* 
     * Using the check value from the CRC catalogue
     * https://reveng.sourceforge.io/crc-catalogue/17plus.htm#crc.cat.crc-32c 
     */
    Crypto_get_crc32("123456789", strlen("123456789"), &calculated_crc32);
    assert_int_equal(calculated_crc32, 0xe3069283);

    /* Check CRC-16 compliance with ECSS-E-ST-70-41 B.1.5 */
    uint8_t test_vectors[4][6] = {
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0xAB, 0xCD, 0xEF, 0x01, 0x00, 0x00},
        {0x14, 0x56, 0xF8, 0x9A, 0x00, 0x01}
    };
    size_t test_vector_sizes[4] = {2, 3, 4, 6};
    Crypto_Crc16 expected_crc16[4] = {0x1D0F, 0xCC9C, 0x04A2, 0x7FD5};

    for (int i = 0; i < 4; i++) {
        Crypto_get_crc16(
            test_vectors[i], 
            test_vector_sizes[i], 
            &calcualted_crc16
        );
        assert_int_equal(calcualted_crc16, expected_crc16[i]);
    }
}

/* -------------------------------------------------------------------------   
 * TEST GROUP
 * ------------------------------------------------------------------------- */

/**
 * @brief Tests to run for the event manager. 
 */
const struct CMUnitTest crypto_tests[] = {
    /* No setup for the init test, which calls init itself */
    cmocka_unit_test(Crypto_test_crc_value)
};


/** @} */ /* End of event_manager_test */