/**
 * @ingroup uart
 * 
 * @file Uart_public.h
 * @author Leon Galanakis (lg5g16@soton.ac.uk)
 * @brief Public header for the UART driver.
 * 
 * @version 0.1
 * @date 2021-02-16
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_UART_PUBLIC_H
#define H_UART_PUBLIC_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard Lirary */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Internal */
#include "drivers/uart/Uart_errors.h"
#include "drivers/uart/Uart_events.h"
#include "drivers/gpio/Gpio_public.h"
#include "system/event_manager/EventManager_public.h"

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------   
 * ENUMS
 * ------------------------------------------------------------------------- */

/* Add indexes */
typedef enum _Uart_DeviceId {
    UART_DEVICE_ID_GNSS = 0,
    UART_DEVICE_ID_CAM = 1,
    UART_DEVICE_ID_EPS = 2,
    UART_DEVICE_ID_TEST = 3,
} Uart_DeviceId;

/* Add status enum */
typedef enum _Uart_Status {
    UART_STATUS_NONE = 0,
    UART_STATUS_COMPLETE = 1,
    UART_STATUS_IN_PROGRESS = 2,
    UART_STATUS_UDMA_TRANSFER_ERROR = 3, /* add many error kinds */
} Uart_Status;

/*
 * 0. STATUS = NONE
 *  - USER: Call Uart_send_bytes()
 * 1. STATUS = IN_PROGRESS (in background via uDMA)
 *  - interrupt -> set status as either succes/error, try to raise complete
 *    event 
 * 2. STATUS = SUCCESS / ERROR
 *  - step -> if STATUS = success or error, and event not raised, raise event
 *  - USER: poll_event(COMPLETE)
 *         - Uart_get_tx_status(DEVICE) -> return success/error
 *                                      -> reset status NONE
 */

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */
/**
 * @brief Defines a UART.
 */
typedef struct _Uart_Device {
    uint32_t gpio_peripheral;
    uint32_t uart_peripheral;
    uint32_t gpio_base;
    uint32_t uart_base;
    uint32_t uart_pin_rx_func;
    uint32_t uart_pin_tx_func;
    GPIO_PIN_INDEX gpio_pin_rx;
    GPIO_PIN_INDEX gpio_pin_tx;
    uint8_t udma_channel_tx;
    uint8_t udma_channel_rx;
    Uart_Status uart_status_tx;
    Uart_Status uart_status_rx;
    Event uart_event;
    uint32_t udma_mode;
    uint32_t baud_rate;
    bool initialised;
} Uart_Device;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Calls Uart_init_specific for all UART devices.
 * 
 * If not all UARTs are initialised correctly an error is returned for
 * monitoring purposes only. If all UARTs fail to initialise an error is also
 * returned, but indicates failure of the UART system itself.
 *
 * @return ErrorCode If no error, ERROR_NONE, otherwise UART_ERROR_x.
 */
ErrorCode Uart_init(void);

ErrorCode Uart_step(void);
    /* In Uart_step():
     * 
     * - Check all devices with a status that isn't UART_STATUS_IN_PROGRESS or
     *   UART_STATUS_NONE 
     * - if that device's event hasn't been raised, raise it again
     */

/**
 * @brief Initialise (or re-init) a specific UART device.
 * 
 * @param uart_id_in The device to re-initialise.
 * @return ErrorCode TODO
 */
ErrorCode Uart_init_specific(Uart_DeviceId uart_id_in);

/**
 * @brief Send bytes to a UART device
 * 
 * Event EVT_UART_SEND_COMPLETE is raised when the bytes have been sent to the
 * device. This could either be successful or it could fail, so use
 * Uart_get_status() for the device to check.
 * 
 * TODO: Change this comment to be about the EVT_UART_device_TX_COMPLETE
 * 
 * @param uart_id_in 
 * @param p_data_in 
 * @param length_in 
 * @return ErrorCode If no error, ERROR_NONE, otherwise UART_ERROR_x. 
 */
ErrorCode Uart_send_bytes(
    Uart_DeviceId uart_id_in,
    uint8_t *p_data_in, 
    size_t length_in
);

/**
 * @brief Receive bytes from a UART device
 * 
 * TODO: Add note on specific event
 * TODO: Add note on how p_data_out __MUST__ exist statically
 * 
 * @param uart_id_in 
 * @param p_data_out 
 * @param length_in 
 * @return ErrorCode If no error, ERROR_NONE, otherwise UART_ERROR_x.
 */
ErrorCode Uart_recv_bytes(
    Uart_DeviceId uart_id_in,
    uint8_t *p_data_out,
    size_t length_in
);

/**
 * @brief Gets the status of the UART to check if the uDMA transfer completed
 * successfuly.
 * 
 * @param uart_id_in 
 * @param p_status_out 
 * @return ErrorCode If no error, ERROR_NONE, otherwise UART_ERROR_x.
 */
ErrorCode Uart_get_status(
    Uart_DeviceId uart_id_in,
    Uart_Status p_status_out
);


/* -------------------------------------------------------------------------   
 * TODO: FUNCTIONS BELOW ARE TEMPORARY - WILL BE REPLACED BY NEW INTERFACE
 * ------------------------------------------------------------------------- */

/**
 * @brief Receieves a byte from the RX FIFO of the specified UART peripheral.
 * 
 * @param uart_id_number_in UART peripheral ID number.
 * @param is_blocking_in bool to check if the function should be nonblocking.
 * @param recvd_byte_out Byte received from the UART peripheral.
 * @return ErrorCode 
 */
ErrorCode Uart_get_char(uint8_t uart_id_number_in, char *recvd_byte_out);

/**
 * @brief Sends a byte from the specified UART peripheral.
 * 
 * @param uart_id_number_in UART peripheral ID number.
 * @param is_blocking_in bool to check if the function should be nonblocking.
 * @param byte_out Byte to send.
 * @return ErrorCode 
 */
ErrorCode Uart_put_char(uint8_t uart_id_number_in, char byte_out);

/**
 * @brief Sends a buffer with a specified length from the specified UART
 *  peripheral.
 * 
 * @param uart_id_number_in UART peripheral ID number.
 * @param is_blocking_in bool to check if the function should be nonblocking.
 * @param buffer_length_in Length of buffer to send (in bytes).
 * @param buffer_out Pointer to the buffer/string to send.
 * @return ErrorCode 
 */
ErrorCode Uart_put_buffer(uint8_t uart_id_number_in, size_t buffer_length_in, char *buffer_out);

/**
 * @brief Checks the TX FIFO busy status of the specified UART peripheral.
 * 
 * @param uart_id_number_in UART peripheral ID number.
 * @return ErrorCode 
 */
ErrorCode Uart_busy_check_tx(uint8_t uart_id_number_in);

/**
 * @brief Checks the RX FIFO busy status of the specified UART peripheral.
 * 
 * @param uart_id_number_in UART peripheral ID number.
 * @return ErrorCode 
 */
ErrorCode Uart_busy_check_rx(uint8_t uart_id_number_in);

#endif /* H_UART_PUBLIC_H */