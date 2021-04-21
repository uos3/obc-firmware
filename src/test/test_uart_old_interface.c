/**
 * @file test_uart_old_interface.c
 * @author Leon Galanakis (leongalanakis@gmail.com)
 * @brief UART test with old interface (no uDMA)
 * 
 * 
 * 
 * @version 0.1
 * @date 2021-04-21
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
#include "drivers/udma/Udma_public.h"
#include "components/led/Led_public.h"
#include "drivers/gpio/Gpio_public.h"
#include "util/debug/Debug_public.h"
#include "system/event_manager/EventManager_public.h"
#include "system/kernel/Kernel_public.h"

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

int main(void) {
    uint8_t i;
    uint32_t data_size;
    uint8_t send_data[8];
    uint8_t recv_data[8];
    uint8_t test_step;
    uint8_t num_attempts;

    data_size = 64;
    test_step = 0;
    num_attempts = 0;

    send_data[0] = 0;
    send_data[1] = 1;
    send_data[2] = 2;
    send_data[3] = 3;
    send_data[4] = 4;
    send_data[5] = 5;
    send_data[6] = 6;
    send_data[7] = 7;

    recv_data[0] = 10;
    recv_data[1] = 11;
    recv_data[2] = 12;
    recv_data[3] = 13;
    recv_data[4] = 14;
    recv_data[5] = 15;
    recv_data[6] = 16;
    recv_data[7] = 17;


    Kernel_init_critical_modules();

    DEBUG_INF(" ===== Uart test =====");

    /* Initialise the UART devices. */
    if (Uart_init_specific(UART_DEVICE_ID_TEST) != ERROR_NONE) {
        DEBUG_ERR("Failed to initialise the UART devices.");
        Debug_exit(1);
    }

    /* Main loop */
    while (true) {
        switch(test_step) {
            /* Step 0 is to send the bytes */
            case 0:
                if (Uart_put_buffer(UART_DEVICE_ID_TEST, 8, send_data) != ERROR_NONE) {
                    Debug_exit(1);
                }
                if (Uart_step() != ERROR_NONE) {
                    Debug_exit(1);
                }
                DEBUG_INF("Sending bytes");
                /* Increment the step number and move on to next case */
                test_step++;
                break;
            /* Step 1 is to check if the correct event has been raised (and
             * automatically clear it with poll_event). Will check 3 times
             * before failing. */
            case 1:
                if (EventManager_poll_event(EVT_UART_TEST_TX_COMPLETE)) {
                    DEBUG_INF("Bytes have been sent");
                }
                else {
                    if (num_attempts <= 3) {
                        DEBUG_INF("TX Complete event has NOT been raised. Retrying");
                        num_attempts++;
                    }
                    else {
                        DEBUG_INF("TX Not complete after max num attempts. Exiting.");
                        Debug_exit(1);
                        break;
                    }
                }
                /* Reset the number of attempts for the next step */
                num_attempts = 0;
                /* Increment the step number and move on to next case */
                test_step++;
                break;
            /* Step 2 is to receive the bytes */
            case 2:
                if (Uart_recv_bytes(UART_DEVICE_ID_TEST, recv_data, data_size) != ERROR_NONE) {
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
                if (EventManager_poll_event(EVT_UART_TEST_RX_COMPLETE)) {
                    DEBUG_INF("Bytes have been received");
                }
                else {
                    if (num_attempts <= 3) {
                        DEBUG_INF("RX Complete event has NOT been raised. Retrying");
                        num_attempts++;
                        break;
                    }
                    else {
                        DEBUG_INF("RX Not complete after max num attempts. Exiting.");
                        Debug_exit(1);
                        break;
                    }
                }
                num_attempts = 0;
                test_step++;
            /* Step 4 is to check that the data received is equal to the data
             * which was sent, as all this test is doing is sending and
             * receiving the same data. */
            case 4:
                for (i = 0; i < data_size; ++i) {
                    if (recv_data[i] != send_data[i]) {
                        DEBUG_INF(
                            "%d from recvd data does not equal expected value of %d",
                            recv_data[i],
                            send_data[i]
                        );
                        Debug_exit(1);
                    }
                }
                /* Increment the step number and move on to next case */
                test_step++;
                break;
            default:
                DEBUG_INF("Test complete");
        }
    }

    EventManager_clear_all_events();

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
