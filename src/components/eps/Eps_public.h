/**
 * @file Eps_public.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief EPS Component Module
 * 
 * Task ref: [UT_2.10.7]
 * 
 * Note: In this header Eps refers to the software module, and EPS the physical
 * system.
 * 
 * The Eps component provides a command and telemetry interface for the EPS
 * microcontroller (MCU) on the EPS board. There are two physical connections
 * between the EPS MCU and the OBC:
 * 
 *                  +-----------+    UART     +-----------+
 *                  |           +<----------->+           |
 *                  |  EPS MCU  |             |    OBC    |
 *                  |           +<------------+           |
 *                  +-----------+  INTERRUPT  +-----------+
 * 
 * The UART line provides two way communications between the two chips, and the
 * INTERRUPT provides watchdog functionality. This watchdog functionality is
 * handled by the Wdt driver, leaving the Eps component to deal with the UART.
 * 
 * The UART connection is modelled as a request-response system, in which the
 * OBC will send requests to the EPS, and the EPS will respond to those
 * requests. The full details are covered in [SW_ICD].
 * 
 * The Eps component itself (this software module) is modelled as a state
 * machine, which allows us to support the non-blocking UART driver. The module
 * is broken down into the following states:
 * 
 *  - `EPS_STATE_INIT` - Initialisation state, in which the UART connection is
 *    initialised and the EPS configuration is updated to be in line with the
 *    one loaded as a part of the configuration file. This state contains the
 *    following substates:
 *      - `EPS_STATE_INIT_SEND_CFG` - The first state, in which the config file
 *        is sent to the EPS.
 *      - `EPS_STATE_INIT_WAIT_CFG_REPLY` - The second state, in which the
 *        Eps module waits for the EPS to send the correct config file back.
 *  - `EPS_STATE_IDLE` - Idle state, in which the Eps is waiting for commands
 *    from the software system.
 *  - `EPS_STATE_REQUEST` - State in which the Eps will send a request over the
 *    UART.
 *  - `EPS_STATE_WAIT_REPLY` - State in which the Eps will wait for the reply
 *    to the most recent request.
 * 
 * The Eps maintains a queue of commands to be sent to the EPS. The operation
 * of this queue is synchronous FIFO, meaning that commands will be sent in the
 * order they are queued up, but a command reply must be recieved before the
 * next command can be sent.
 * 
 * A user can send commands to the EPS by using one of the public functions
 * provided by this module. Data can be retrieved by accessing the appropriate
 * DataPool parameters, which always contain the most up to date information.
 * 
 * Successful completion of a command is signaled by the event system, through
 * the `EVT_EPS_COMMAND_COMPLETE` event.
 * 
 * It is possible for commands to fail, for example if the EPS failed to
 * respond in the configured timeout. The success of a command is signalled by
 * the `DP.EPS.COMMAND_STATUS` variable, which will be `EPS_COMMAND_OK` or 
 * `EPS_COMMAND_FAILED`, depending on the success of the command. If the status
 * is failed the user should attempt the operation again. TODO: What if second
 * fails too? An `EVT_EPS_COMMAND_FAILED` event is raised for FDIR purposes, 
 * and should not be polled by the user.
 * 
 * References:
 * [DDF_1_15] - UoS3_DDF_20181203_v1.15 - Design Definition File
 * [SW_ICD] - UoS3_SoftwareIcd_Working - Software Interface Control Document
 * [BATT_UM] - CS-SBAT2-30 User Manual - Battery User Manual
 * 
 * @version 0.1
 * @date 2021-02-15
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_EPS_PUBLIC_H
#define H_EPS_PUBLIC_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* External includes */
#include <stdint.h>
#include <stdbool.h>

/* Internal includes */
#include "components/eps/Eps_dp_struct.h"
#include "components/eps/Eps_errors.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief The maximum length (in bytes) of a UART frame in the EPS system.
 */
#define EPS_MAX_UART_FRAME_LENGTH (256)

/**
 * @brief The length (in bytes) of the UART frame header.
 */
#define EPS_UART_HEADER_LENGTH (3)

/**
 * @brief Position of the frame number byte within the UART header.
 */
#define EPS_UART_HEADER_FRAME_NUMBER_POS (0)

/**
 * @brief Position of the data type byte within the UART header.
 */
#define EPS_UART_HEADER_DATA_TYPE_POS (1)

/**
 * @brief Position of the payload length byte within the UART header.
 */
#define EPS_UART_HEADER_PAYLOAD_LENGTH_POS (2)

/**
 * @brief The maximum length (in bytes) of a UART frame payload data section, 
 * i.e. the whole frame - the header.
 */
#define EPS_MAX_UART_PAYLOAD_LENGTH (EPS_MAX_UART_FRAME_LENGTH - EPS_UART_HEADER_LENGTH)

/* -------------------------------------------------------------------------   
 * STRUCTURES
 * ------------------------------------------------------------------------- */

/**
 * @brief EPS Configuration Data
 * 
 * This struct defines the configuration data that is used by the EPS. It will
 * be sent to the EPS during the initialisation process.
 */
typedef struct _Eps_ConfigData {

} Eps_ConfigData;

/**
 * @brief EPS Battery Command.
 * 
 * A command which can be sent to the EPS's Battery I2C [BATT_UM].
 */
typedef struct _Eps_BattCmd {

    /**
     * @brief First byte, the command type.
     */
    uint8_t type;

    /**
     * @brief Second byte, the value associated with the command.
     */
    uint8_t value;

} Eps_BattCmd;

/**
 * @brief EPS OCP rail state.
 * 
 * This struct defines the state of the OCP rails, either On (true) or Off
 * (false). 
 */
typedef struct _Eps_OcpState {

    /**
     * @brief Status of the Radio TX rail.
     * 
     * As per [DDF_1_15] this is for OCP rail 1.
     */
    bool radio_tx;

    /**
     * @brief Status of the Radio RX rail.
     * 
     * As per [DDF_1_15] this is for OCP rail 2.
     */
    bool radio_rx;

    /**
     * @brief Status of the Camera rail.
     * 
     * As per [DDF_1_15] this is for OCP rail 3.
     */
    bool camera;

    /**
     * @brief Status of the OBC rail.
     * 
     * As per [DDF_1_15] this is for OCP rail 4.
     */
    bool obc;

    /**
     * @brief Status of the GNSS RX rail.
     * 
     * As per [DDF_1_15] this is for OCP rail 5.
     */
    bool gnss_rx;

    /**
     * @brief Status of the GNSS LNA rail.
     * 
     * As per [DDF_1_15] this is for OCP rail 6.
     */
    bool gnss_lna;

} Eps_OcpState;

/**
 * @brief Represents the status reported by the battery.
 * 
 * [BATT_UM]
 */
typedef struct _Eps_BattStatus {

    /**
     * @brief If false the last command was OK, otherwise the last command type
     * was an unrecognised byte.
     * 
     * Note that reading the battery status will clear this value.
     */
    bool unknown_command_type;

    /**
     * @brief If false the last command value was OK, otherwise the last 
     * command value was an unrecognised byte.
     * 
     * Note that reading the battery status will clear this value.
     */
    bool unknown_command_value;

    /**
     * @brief If true the result of an ADC (Analogue to Digital Converter) read
     * operation is not yet ready.
     * 
     * Note that reading the battery status will clear this value.
     */
    bool adc_result_not_ready;

    /**
     * @brief If true the external oscillator has failed.
     */
    bool oscillator_failure;

    /**
     * @brief If true a watchdog reset has occured.
     * 
     * Note that reading the battery status will clear this value.
     */
    bool watchdog_reset_occured;

    /**
     * @brief If true NO power on reset has occured.
     * 
     * Note that reading the battery status will clear this value.
     */
    bool power_on_reset_not_occured;

    /**
     * @brief If true NO brown out reset has occured.
     * 
     * Note that reading the battery status will clear this value.
     */
    bool brown_out_reset_not_occured;

    /**
     * @brief If true an I2C error has occured
     * 
     * Note that reading the battery status will clear this value.
     */
    bool i2c_error;

    /**
     * @brief If true an I2C write collision has occured
     * 
     * Note that reading the battery status will clear this value.
     */
    bool i2c_write_collision;

    /**
     * @brief If true an I2C overflow has occured
     * 
     * Note that reading the battery status will clear this value.
     */
    bool i2c_overflow;

} Eps_BattStatus;

/**
 * @brief EPS Housekeeping data
 * 
 * This struct defines all the standard housekeeping telemetry data that is
 * provided by the EPS MCU.
 */
typedef struct _Eps_HkData {

    /**
     * @brief State of the OCP rails
     */
    Eps_OcpState ocp_state;

    /**
     * @brief Status of the battery
     */
    Eps_BattStatus batt_status;

} Eps_HkData;

/* -------------------------------------------------------------------------   
 * ENUMS
 * ------------------------------------------------------------------------- */

/**
 * @brief Possible states of the Eps module.
 */
typedef enum _Eps_State {

    /**
     * @brief State in which the Eps module will initialise the EPS by sending
     * the configuration data.
     * 
     * No commands may be sent in this state.
     */
    EPS_STATE_INIT_SEND_CFG = 0,

    /**
     * @brief State in which the Eps module waits for the EPS to reply with the
     * configuration data sent in EPS_STATE_INIT_SEND_CFG.
     * 
     * No commands may be sent in this state.
     */
    EPS_STATE_INIT_WAIT_CFG_REPLY = 1,

    /**
     * @brief Idle state, in which the Eps module waits for commands from the
     * user. 
     */
    EPS_STATE_IDLE = 2,

    /**
     * @brief State in which the Eps module will send a request to the EPS.
     * 
     * No commands may be sent in this state.
     */
    EPS_STATE_REQUEST = 3,

    /**
     * @brief State in which the Eps module is waiting for the reply from the
     * EPS.
     * 
     * No commands may be sent in this state.
     */
    EPS_STATE_WAIT_REPLY = 4,

} Eps_State;

/**
 * @brief Types of data frames that can be sent or recieved by the EPS.
 */
typedef enum _Eps_UartDataType {
    /**
     * @brief Command instructing EPS to collect housekeeping data.
     * 
     * Expected reply is EPS_UART_DATA_TYPE_TM_HK_DATA.
     */
    EPS_UART_DATA_TYPE_TC_COLLECT_HK_DATA = 1,

    /**
     * @brief Command instructing EPS to set the provided configuration.
     * 
     * Expected reply is EPS_UART_DATA_TYPE_TM_LOADED_CONFIG.
     */
    EPS_UART_DATA_TYPE_TC_SET_CONFIG = 2,

    /**
     * @brief Command instructing EPS to change the OCP rail state to the given
     * values.
     * 
     * Expected reply is EPS_UART_DATA_TYPE_TM_OCP_STATE.
     */
    EPS_UART_DATA_TYPE_TC_SET_OCP_STATE = 3,

    /**
     * @brief Command instructing EPS to send the given command to the battery.
     * 
     * Expected reply is EPS_UART_DATA_TYPE_TM_BATT_REPLY.
     */
    EPS_UART_DATA_TYPE_TC_SEND_BATT_CMD = 4,

    /**
     * @brief Reply containing housekeeping data.
     * 
     * Generated with an EPS_UART_DATA_TYPE_TM_HK_DATA request.
     */
    EPS_UART_DATA_TYPE_TM_HK_DATA = 129,

    /**
     * @brief Reply containing the loaded configuration file of the EPS.
     * 
     * Generated with an EPS_UART_DATA_TYPE_TC_SET_CONFIG request.
     */
    EPS_UART_DATA_TYPE_TM_LOADED_CONFIG = 130,

    /**
     * @brief Reply containing the state of the OCP rails.
     * 
     * Generated with an EPS_UART_DATA_TYPE_TC_SET_OCP_STATE request.
     * 
     * Note: normal telemetry collection of OCP state is through the
     * housekeeping data, this reply exists to ensure that the EPS sets the
     * correct state of the OCP rails after an
     * EPS_UART_DATA_TYPE_TC_SET_OCP_STATE command.
     */
    EPS_UART_DATA_TYPE_TM_OCP_STATE = 131,

    /**
     * @brief Reply containing the Battery's reply.
     * 
     * Generated with an EPS_UART_DATA_TYPE_TC_SEND_BATT_CMD request.
     */
    EPS_UART_DATA_TYPE_TM_BATT_REPLY = 132
    
} Eps_UartDataType;

/**
 * @brief The status of a completed command.
 */
typedef enum _Eps_CommandStatus {
    /**
     * @brief Indicates that no command has been completed yet.
     */
    EPS_COMMAND_NONE = 0,

    /**
     * @brief Indicates that a command has finished successfully.
     */
    EPS_COMMAND_SUCCESS = 1,

    /**
     * @brief Indicates that a command has failed.
     * 
     * See `DP.EPS.ERROR_CODE` for the cause of this failure.
     */
    EPS_COMMAND_FAILURE = 2

} Eps_CommandStatus;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Initialises the EPS module.
 * 
 * @return bool True on success, false on error. See DP.EPS.ERROR_CODE for the
 * cause of the error.
 */
bool Eps_init(void);

/**
 * @brief Step the EPS component.
 * 
 * This function will execute the EPS state machine.
 * 
 * @return bool True on success, false on error. See DP.EPS.ERROR_CODE for the
 * cause of the error.
 */
bool Eps_step(void);

/**
 * @brief Sends a Housekeeping data collection request to the EPS.
 * 
 * This function will add an HK data request command to the queue. See
 * `Eps_public.h` for how to use this function.
 * 
 * @return bool True on success, false on failure. See DP.EPS.ERROR_CODE for
 * error cause.
 */
bool Eps_collect_hk_data(void);

/**
 * @brief Sends a command to the EPS to set the state of the OCP rails.
 * 
 * @param ocp_state_in The state that the OCP rails should be set to
 * @return bool True on success, false on failure. See DP.EPS.ERROR_CODE for
 * error cause.
 */
bool Eps_set_ocp_state(Eps_OcpState ocp_state_in);

/**
 * @brief Sends a command to the EPS to send a particular command to the
 * battery. 
 * 
 * @param batt_cmd_in The battery command to send.
 * @return bool True on success, false on failure. See DP.EPS.ERROR_CODE for
 * error cause.
 */
bool Eps_send_battery_command(Eps_BattCmd batt_cmd_in);

#endif /* H_EPS_PUBLIC_H */