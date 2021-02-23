/**
 * @file demo_eps.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Demonstrates usage of the Eps functionality
 * @version 0.1
 * @date 2021-02-18
 * 
 * @copyright Copyright (c) 2021
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "drivers/board/Board_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/event_manager/EventManager_public.h"
#include "components/eps/Eps_public.h"
#include "drivers/timer/Timer_public.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

void get_str_from_bytes(uint8_t *p_bytes_in, char *p_str_out, int length) {
    char byte[4];

    for (int i = 0; i < length; i++) {
        sprintf(byte, "%02X ", p_bytes_in[i]);
        strcat(p_str_out, byte);
    }
}

/* -------------------------------------------------------------------------   
 * MAIN
 * ------------------------------------------------------------------------- */

int main(void) {
    ErrorCode error = ERROR_NONE;
    bool exit_loop = false;
    Event timer_event;
    double timeout_duration = 0.1;
    Eps_OcpState ocp_state = {0};
    char request_str[EPS_MAX_UART_FRAME_LENGTH*3 + 1] = {0};

    /* Init system critical modules */
    if (!DataPool_init()) {
        Debug_exit(1);
    }
    if (!Board_init()) {
        Debug_exit(1);
    }
    if (!Debug_init()) {
        Debug_exit(1);
    }
    if (!EventManager_init()) {
        Debug_exit(1);
    }

    DEBUG_INF("Eps Demo");

    /* Init the EPS */
    if (!Eps_init()) {
        Debug_exit(1);
    }

    /* Start timer to timeout the demo */
    if (Timer_init() != ERROR_NONE) {
        Debug_exit(1);
    }
    if (Timer_start_one_shot(timeout_duration, &timer_event) != ERROR_NONE) {
        Debug_exit(1);
    }

    /* Main loop */
    while (!exit_loop) {

        /* If initialised build a test ocp command and print it to the screen */
        if (DP.EPS.INITIALISED) {
            ocp_state.radio_rx_camera = true;
            ocp_state.obc = true;
            ocp_state.gnss_rx = true;

            if (!Eps_set_ocp_state(ocp_state)) {
                Debug_exit(1);
            }

            get_str_from_bytes(
                DP.EPS.EPS_REQUEST, 
                request_str, 
                DP.EPS.EPS_REQUEST_LENGTH
            );

            DEBUG_INF("DP.EPS.EPS_REQUEST = %s", request_str);

            request_str[0] = 0;
            break;
        }

        /* Step the EPS */
        if (!Eps_step()) {
            Debug_exit(1);
        }

        /* If the timer has fired exit the loop */
        if (!EventManager_poll_event(timer_event, &exit_loop)) {
            Debug_exit(1);
        }
    }

    return 0;
}