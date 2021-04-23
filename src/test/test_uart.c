/**
 * @file test_uart.c
 * @author Leon Galanakis (leongalanakis@gmail.com)
 * @brief uDMA UART test
 * 
 * 
 * 
 * @version 0.1
 * @date 2021-03-16
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Internal includes */
#include "drivers/uart/Uart_public.h"
#include "drivers/uart/Uart_private.h"
#include "drivers/udma/Udma_public.h"
#include "components/led/Led_public.h"
#include "drivers/gpio/Gpio_public.h"
#include "util/debug/Debug_public.h"
#include "system/event_manager/EventManager_public.h"
#include "system/kernel/Kernel_public.h"

#define UART_DEVICE UART_DEVICE_ID_GNSS
#define UART_RX_EVT EVT_UART_GNSS_RX_COMPLETE
#define UART_TX_EVT EVT_UART_GNSS_TX_COMPLETE

#define DATA_LENGTH (2)

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

int main(void) {
    uint8_t i;
    uint8_t send_data[DATA_LENGTH] = {0};
    uint8_t recv_data[DATA_LENGTH] = {0};
    uint8_t test_step;
    uint8_t num_attempts;
    uint8_t max_num_attemps;
    bool test_complete;
    bool recv_matches_sent = true;

    test_complete = false;
    test_step = 0;
    num_attempts = 0;
    max_num_attemps = 100;

    for (i = 0; i < DATA_LENGTH; i++) {
        send_data[i] = i;
    }

    Kernel_init_critical_modules();

    DEBUG_INF(" ===== Uart test =====");

    /* Initialise the UART devices. */
    if (Uart_init_specific(UART_DEVICE) != ERROR_NONE) {
        DEBUG_ERR("Failed to initialise the UART devices.");
        Debug_exit(1);
        return 1;
    }

    /* Initialise the uDMA */
    if (Udma_init() != ERROR_NONE) {
        DEBUG_ERR("Failed to initialise the uDMA.");
        Debug_exit(1);
        return 1;
    }

    /* Main loop */
    while (test_complete == false) {
        switch(test_step) {
            /* Step 0 is to send the bytes */
            case 0:
                DEBUG_INF("----- STEP 0 -----");
                if (Uart_send_bytes(UART_DEVICE, send_data, DATA_LENGTH) != ERROR_NONE) {
                    DEBUG_ERR("Failed to send bytes");
                    Debug_exit(1);
                    return 1;
                }
                if (Uart_step() != ERROR_NONE) {
                    DEBUG_ERR("Step function failed");
                    Debug_exit(1);
                    return 1;
                }
                DEBUG_INF("Sending bytes");
                /* Increment the step number and move on to next case */
                test_step++;
                break;
            /* Step 1 is to check if the correct event has been raised (and
             * automatically clear it with poll_event). Will check 3 times
             * before failing. */
            case 1:
                DEBUG_INF("----- STEP 1 -----");
                if (num_attempts < max_num_attemps) {
                    if (EventManager_poll_event(UART_TX_EVT)) {
                        DEBUG_INF("Bytes have been sent");
                        test_step++;
                        break;
                    }
                    else {
                        num_attempts++;
                        break;
                    }
                }
                else {
                    DEBUG_INF("TX Not complete after max num attempts. Exiting.");
                    Debug_exit(1);
                    return 1;
                }
                /* Reset the number of attempts for the next step */
                num_attempts = 0;
                /* Increment the step number and move on to next case */
                test_step++;
                break;
            /* Step 2 is to receive the bytes */
            case 2:
                DEBUG_INF("----- STEP 2 -----");
                if (Uart_recv_bytes(UART_DEVICE, recv_data, DATA_LENGTH) != ERROR_NONE) {
                    Debug_exit(1);
                }
                if (Uart_step() != ERROR_NONE) {
                    Debug_exit(1);
                }
                /* Increment the step number and move on to next case */
                test_step++;
                break;
            /* Step 3 is to check if the correct event has been raised after
             * receiving bytes. */
            case 3:
                DEBUG_INF("----- STEP 3 -----");
                if (num_attempts < max_num_attemps) {
                    if (EventManager_poll_event(UART_RX_EVT)) {
                        DEBUG_INF("Bytes have been received");
                        test_step++;
                        break;
                    }
                    else {
                        num_attempts++;
                        break;
                    }
                }
                else {
                    DEBUG_INF("RX Not complete after max num attempts. Exiting.");
                    Debug_exit(1);
                    return 1;
                }
                num_attempts = 0;
                test_step++;
            /* Step 4 is to check that the data received is equal to the data
             * which was sent, as all this test is doing is sending and
             * receiving the same data. */
            case 4:
                DEBUG_INF("----- STEP 4 -----");
                DEBUG_INF("index: [sent, received]");
                for (i = 0; i < DATA_LENGTH; ++i) {
                    if (recv_data[i] != send_data[i]) {
                        DEBUG_ERR("%3d: [0x%02X, 0x%02X]", i, send_data[i], recv_data[i]);
                        recv_matches_sent = false;
                    }
                    else {
                        DEBUG_INF("%3d: [0x%02X, 0x%02X]", i, send_data[i], recv_data[i]);
                    }
                }
                /* Increment the step number and move on to next case */
                test_step++;
                break;
            default:
                DEBUG_INF(" ----- TEST COMPLETE AT STEP %d -----", test_step);
                test_complete = true;
                if (recv_matches_sent) {
                    DEBUG_INF(" ---- TEST PASSED ---- ");
                }
                else {
                    DEBUG_ERR(" ---- TEST FAILED ---- ");
                }
        }
    }

    if (EventManager_clear_all_events()) {
        DEBUG_INF("Events have been cleared");
    }
    else {
        DEBUG_ERR("Error when clearing events");
        return 1;
    }

    #if 0
    /* Send the data to the UART RX associated with the UART device.
     * TODO: Change which device for send and receive below. */
    if (Uart_send_bytes(UART_DEVICE_ID_TEST, send_data, data_size) != ERROR_NONE) {
        Uart_test_tx_int_handler();
        /*Udma_interrupt_handler(UART_DEVICE_ID_TEST, data_size)*/
        DEBUG_ERR("Failed to send bytes to UART device.");
    }

    if (EventManager_is_event_raised(EVT_UART_TEST_TX_COMPLETE)) {
        /* Receive the data from the UART TX associated with the UART device. */
        if (Uart_recv_bytes(UART_DEVICE_ID_TEST, recv_data, data_size) != ERROR_NONE) {
            Uart_test_rx_int_handler();
            /*Udma_interrupt_handler(UART_DEVICE_ID_TEST, data_size)*/
            DEBUG_ERR("Failed to receive bytes from UART device.");
        }
        else {
            if(EventManager_is_event_raised(EVT_UART_TEST_RX_COMPLETE)) {
                /* Loop through the data size, making sure that the data received from the
                * TX is the same as what was sent to the RX (the raspberry pi should re
                * echo the data it received, and send the same data back) */
                for (i = 0; i < data_size; ++i) {
                    if (recv_data[i] != send_data[i]) {
                        DEBUG_ERR("Unexpected receive data - should be the same as sent.");
                        break;
                    }
                }
            }
        }
    }
    else {
        DEBUG_ERR("EventManager_is_event_raised failed.");
        return 1;
    }
    #endif

    /* Return 0 if no errors occured up to this point. */
    return 0;
}
