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
    uint8_t *send_data;
    uint8_t *recv_data;
    uint8_t test_step;
    uint8_t num_attempts;
    bool test_complete;


    test_complete = false;
    data_size = 1;
    test_step = 0;
    num_attempts = 0;

    send_data = (uint8_t*) malloc(data_size * sizeof(uint8_t));
    recv_data = (uint8_t*) malloc(data_size * sizeof(uint8_t));

    send_data[0] = 14;
    recv_data[0] = 10;


    Kernel_init_critical_modules();

    DEBUG_INF(" ===== Uart test =====");

    /* Initialise the UART devices. */
    if (Uart_init_specific(UART_DEVICE_ID_TEST) != ERROR_NONE) {
        DEBUG_ERR("Failed to initialise the UART devices.");
        Debug_exit(1);
        free(send_data);
        free(recv_data);
        return 1;
    }

    /* Initialise the uDMA */
    if (Udma_init() != ERROR_NONE) {
        DEBUG_ERR("Failed to initialise the uDMA.");
        Debug_exit(1);
        free(send_data);
        free(recv_data);
        return 1;
    }

    DEBUG_DBG("Element 0 of recv_data is %d", recv_data[0]);
    DEBUG_DBG("Element 0 of sent data was %d", send_data[0]);

    /* Main loop */
    while (test_complete == false) {
        switch(test_step) {
            /* Step 0 is to send the bytes */
            case 0:
                DEBUG_INF("----- STEP 0 -----");
                if (Uart_send_bytes(UART_DEVICE_ID_TEST, send_data, data_size) != ERROR_NONE) {
                    Debug_exit(1);
                    DEBUG_ERR("Failed to send bytes");
                    free(send_data);
                    free(recv_data);
                    return 1;
                }
                if (Uart_step() != ERROR_NONE) {
                    Debug_exit(1);
                    DEBUG_ERR("Step function failed");
                    free(send_data);
                    free(recv_data);
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
                if (num_attempts < 3) {
                    if (EventManager_poll_event(EVT_UART_TEST_TX_COMPLETE)) {
                        DEBUG_INF("Bytes have been sent");
                        test_step++;
                        break;
                    }
                    else {
                        num_attempts++;
                    }
                }
                else {
                    DEBUG_INF("TX Not complete after max num attempts. Exiting.");
                    Debug_exit(1);
                    free(send_data);
                    free(recv_data);
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
                DEBUG_INF("----- STEP 3 -----");
                if (num_attempts < 3) {
                    if (EventManager_poll_event(EVT_UART_TEST_RX_COMPLETE)) {
                        DEBUG_INF("Bytes have been received");
                        test_step++;
                        break;
                    }
                    else {
                        num_attempts++;
                    }
                }
                else {
                    DEBUG_INF("RX Not complete after max num attempts. Exiting.");
                    Debug_exit(1);
                    free(send_data);
                    free(recv_data);
                    return 1;
                }
                num_attempts = 0;
                test_step++;
            /* Step 4 is to check that the data received is equal to the data
             * which was sent, as all this test is doing is sending and
             * receiving the same data. */
            case 4:
                DEBUG_INF("----- STEP 4 -----");
                for (i = 0; i < data_size; ++i) {
                    if (recv_data[i] != send_data[i]) {
                        DEBUG_INF("Send and recv data not equal");
                    }
                    DEBUG_DBG("Element %d of recv_data is %d", i, recv_data[i]);
                    DEBUG_DBG("Element %d of sent data was %d", i, send_data[i]);
                }
                /* Increment the step number and move on to next case */
                test_step++;
                break;
            default:
                DEBUG_INF(" ----- TEST COMPLETE AT STEP %d -----", test_step);
                test_complete = true;
        }
    }

    if (EventManager_clear_all_events()) {
        DEBUG_INF("Events have been cleared");
    }
    else {
        DEBUG_ERR("Error when clearing events");
        free(send_data);
        free(recv_data);
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
    free(send_data);
    free(recv_data);

    /* Return 0 if no errors occured up to this point. */
    return 0;
}
