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

    /* 
     * Using the check value from the CRC catalogue
     * https://reveng.sourceforge.io/crc-catalogue/17plus.htm#crc.cat.crc-32c 
     */
    Crypto_Crc32 calculated_crc = 0;
    uint8_t test_vector[] = "123456789";
    Crypto_get_crc32(test_vector, strlen(test_vector), &calculated_crc);
    assert_int_equal(calculated_crc, 0xe3069283);
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