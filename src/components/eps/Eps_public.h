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
 * The Eps can only handle one command at a time, so the user must wait until
 * the currently executing command is completed before sending the next one.
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
#include "util/crypto/Crypto_public.h"
#include "components/eps/Eps_errors.h"
#include "components/eps/Eps_events.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief The maximum length (in bytes) of a UART frame in the EPS system.
 */
#define EPS_MAX_UART_FRAME_LENGTH (128)

/**
 * @brief The length (in bytes) of the UART frame header.
 */
#define EPS_UART_HEADER_LENGTH (2)

/**
 * @brief Position of the frame number byte within the UART header.
 */
#define EPS_UART_HEADER_FRAME_NUMBER_POS (0)

/**
 * @brief Position of the data type byte within the UART header.
 */
#define EPS_UART_HEADER_DATA_TYPE_POS (1)

/**
 * @brief The length (in bytes) of the CRC used in the UART frame.
 */
#define EPS_UART_CRC_LENGTH (sizeof(Crypto_Crc16))

/**
 * @brief The maximum length (in bytes) of a UART frame payload data section, 
 * i.e. the whole frame - the header - the crc.
 */
#define EPS_MAX_UART_PAYLOAD_LENGTH \
    (EPS_MAX_UART_FRAME_LENGTH - EPS_UART_HEADER_LENGTH - EPS_UART_CRC_LENGTH)

#define EPS_BATT_CMD_DISABLE_HEATER ((Eps_BattCmd){5, 1})

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
    /**
     * @brief an Eps_OcpByte that shows whether or not a rail will be reset 
     * after an OCP event.
     */
    uint8_t reset_rail_after_ocp;

    /**
     * @brief Length of the TOBC watchdog timer in EPS MCU Clock Cycles.
     * 
     * Divide by 4096 for a value in seconds.
     */
    uint16_t tobc_timer_length;
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
     * @brief Second byte, the first value associated with the command.
     */
    uint8_t value_a;

    /**
     * @brief Third byte, the second value associated with the command.
     */
    uint8_t value_b;

} Eps_BattCmd;

/**
 * @brief EPS OCP rail state.
 * 
 * This struct defines the state of the OCP rails, either On (true) or Off
 * (false). 
 * 
 * TODO: Danny says the DDF is out of date, find a better reference
 */
typedef struct _Eps_OcpState {

    /**
     * @brief Status of the Radio TX rail.
     * 
     * As per [DDF_1_15] this is for OCP rail 1.
     */
    bool radio_tx;

    /**
     * @brief Status of the Radio RX/Camera rail.
     * 
     * As per [DDF_1_15] this is for OCP rail 2.
     */
    bool radio_rx_camera;

    /**
     * @brief Status of the EPS MCU rail.
     * 
     * As per [DDF_1_15] this is for OCP rail 3.
     */
    bool eps_mcu;

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
     * @brief Status of the battery.
     */
    uint16_t batt_status;

    /**
     * @brief Scaledint representing the battery's voltage in raw ADC values.
     */
    uint16_t batt_output_voltage_scaledint;

    /**
     * @brief Scaledint representing the magnitude of the battery's output 
     * current in rawn ADC values.
     */
    uint16_t batt_current_magnitude_scaledint;

    /**
     * @brief An integer representing the direction of the battery's current.
     */
    uint16_t batt_current_direction;

    /**
     * @brief A scaled integer representing the battery's motherboard
     * temperature in raw ADC values.
     */
    uint16_t batt_motherboard_temp_scaledint;

    /**
     * @brief Scaled integer representing the output current of the battery's
     * 5V bus in raw ADC values.
     */
    uint16_t batt_5v_current_scaledint;

    /**
     * @brief Scaled integer representing the voltage of the battery's 5V bus
     * in raw ADC values.
     */
    uint16_t batt_5v_voltage_scaledint;

    /**
     * @brief Scaled integer representing the output current of the battery's
     * 3V3 bus in raw ADC values.
     */
    uint16_t batt_3v3_current_scaledint;

    /**
     * @brief Scaled integer representing the voltage of the battery's 3.3V bus
     * in raw ADC values.
     */
    uint16_t batt_3v3_voltage_scaledint;

    /**
     * @brief A scaled integer representing the temperature of the battery's
     * daughterboard in raw ADC values.
     */
    uint16_t batt_daughterboard_temp_scaledint;

    /**
     * @brief An integer representing the state of the heater on the battery's
     * daughterboard. 
     */
    uint16_t batt_daughterboard_heater_status;

    /**
     * @brief A scaled integer representing the temperature of the EPS MCU in
     * raw ADC values.
     */
    uint16_t eps_temp_scaledint;

    /* Note the ordering here is based on that of the MUXes, which explains why
     * things aren't logically grouped */

    /* TODO: doc comment when ICD updated */
    uint16_t pv_top1_csense;

    /* TODO: doc comment when ICD updated */
    uint16_t pv_top2_csense;

    /* TODO: doc comment when ICD updated */
    uint16_t ocp5_csense;

    /* TODO: doc comment when ICD updated */
    uint16_t ocp6_csense;

    /* TODO: doc comment when ICD updated */
    uint16_t ocp6_vsense;

    /* TODO: doc comment when ICD updated */
    uint16_t ocp4_csense;

    /* TODO: doc comment when ICD updated */
    uint16_t ocp4_vsense;

    /* TODO: doc comment when ICD updated */
    uint16_t ocp5_vsense;

    /* TODO: doc comment when ICD updated */
    uint16_t ocp3_csense;

    /* TODO: doc comment when ICD updated */
    uint16_t ocp3_vsense;

    /* TODO: doc comment when ICD updated */
    uint16_t sys_5v_csense;

    /* TODO: doc comment when ICD updated */
    uint16_t sys_3v3_csense;

    /* TODO: doc comment when ICD updated */
    uint16_t ocp2_csense;

    /* TODO: doc comment when ICD updated */
    uint16_t ocp2_vsense;

    /* TODO: doc comment when ICD updated */
    uint16_t ocp1_vsense;

    /* TODO: doc comment when ICD updated */
    uint16_t pv_north2_csense;

    /* TODO: doc comment when ICD updated */
    uint16_t pv_north1_csense;

    /* TODO: doc comment when ICD updated */
    uint16_t charge_csense;

    /* TODO: doc comment when ICD updated */
    uint16_t pv_west1_csense;

    /* TODO: doc comment when ICD updated */
    uint16_t mppt_bus_vsense;

    /* TODO: doc comment when ICD updated */
    uint16_t mppt2_lower_pv_vsense;

    /* TODO: doc comment when ICD updated */
    uint16_t mppt2_mid_pv_vsense;

    /* TODO: doc comment when ICD updated */
    uint16_t pv_west2_csense;

    /* TODO: doc comment when ICD updated */
    uint16_t pv_south2_csense;

    /* TODO: doc comment when ICD updated */
    uint16_t pv_south1_csense;

    /* TODO: doc comment when ICD updated */
    uint16_t uvp_5v_vsense;

    /* TODO: doc comment when ICD updated */
    uint16_t uvp_3v3_vsense;

    /* TODO: doc comment when ICD updated */
    uint16_t vbatt_vsense;

    /* TODO: doc comment when ICD updated */
    uint16_t ocp1_csense;

    /* TODO: doc comment when ICD updated */
    uint16_t pv_east2_csense;

    /* TODO: doc comment when ICD updated */
    uint16_t pv_east1_csense;

    /* TODO: doc comment when ICD updated */
    uint16_t mppt1_lower_pv_vsense;

    /* TODO: doc comment when ICD updated */
    uint16_t mppt3_lower_pv_vsense;

    /* TODO: doc comment when ICD updated */
    uint16_t mppt1_mid_pv_vsense;

    /* TODO: doc comment when ICD updated */
    uint16_t mppt3_mid_pv_vsense;

    /**
     * @brief Contains 0 if the flash is working, 1 if there was an error 
     * reading from it.
     */
    uint8_t log_flash_error;

    /**
     * @brief Number of times OCP1 has tripped since the last housekeeping 
     * request.
     */
    uint8_t log_ocp1_trip_count;

    /**
     * @brief Number of times OCP2 has tripped since the last housekeeping 
     * request.
     */
    uint8_t log_ocp2_trip_count;

    /**
     * @brief Number of times OCP3 has tripped since the last housekeeping 
     * request.
     */
    uint8_t log_ocp3_trip_count;

    /**
     * @brief Number of times OCP4 has tripped since the last housekeeping 
     * request.
     */
    uint8_t log_ocp4_trip_count;

    /**
     * @brief Number of times OCP5 has tripped since the last housekeeping 
     * request.
     */
    uint8_t log_ocp5_trip_count;

    /**
     * @brief Number of times OCP6 has tripped since the last housekeeping 
     * request.
     */
    uint8_t log_ocp6_trip_count;

    /**
     * @brief Number of times the EPS MCU has restarted since the last 
     * housekeeping request.
     */
    uint8_t log_reboot_count;

    /**
     * @brief Number of times the TOBC watchdog timer has elapsed since the 
     * last housekeeping request.
     */
    uint8_t log_tobc_time_count;

    /**
     * @brief The state of the OCP rails.
     */
    uint8_t ocp_rail_state;

} Eps_HkData;

/* -------------------------------------------------------------------------   
 * ENUMS
 * ------------------------------------------------------------------------- */

/**
 * @brief Possible states of the Eps module.
 */
typedef enum _Eps_State {

    /**
     * @brief Idle state, in which the Eps module waits for commands from the
     * user. 
     */
    EPS_STATE_IDLE = 1,

    /**
     * @brief State in which the Eps module will send a request to the EPS.
     * 
     * No commands may be sent in this state.
     */
    EPS_STATE_REQUEST = 2,

    /**
     * @brief State in which the Eps module is waiting for the reply from the
     * EPS.
     * 
     * No commands may be sent in this state.
     */
    EPS_STATE_WAIT_REPLY = 3

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
     * @brief Command instructing the EPS to reset (turn off then on) a
     * selection of OCP rails.
     * 
     * Expected reply is EPS_UART_DATA_TYPE_TM_OCP_STATE
     */
    EPS_UART_DATA_TYPE_TC_RESET_OCP = 5,

    /**
     * @brief Data type indicating that the OBC has been reset and the 
     * communications chain should also be reset.
     * 
     * This has no payload or CRC.
     */
    EPS_UART_DATA_TYPE_TC_RESET_COMMUNICATIONS = 126,

    /**
     * @brief Data type indicating that the previous header has been received 
     * and the receiving end is now ready to receive the following payload 
     * bytes. 
     * 
     * Send by both the OBC only with no payload or CRC.
     */
    EPS_UART_DATA_TYPE_TC_CONTINUE = 127,

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
    EPS_UART_DATA_TYPE_TM_BATT_REPLY = 132,

    /**
     * @brief Reply indicating that the OCP rail associated with the
     * Eps_OcpByte has been tripped.
     * 
     * Generated as an unsolicited TM packet by the EPS.
     */
    EPS_UART_DATA_TYPE_TM_OCP_TRIPPED = 133,

    /**
     * @brief EPS_TM that returns a data type that the EPS thinks 
     * is invalid.
     * 
     * Generated on any command failure.
     */
    EPS_UART_DATA_TYPE_TM_INVALID_DATA_TYPE = 134,

    /**
     * @brief EPS_TM sent when the length of the received header is not 2.
     * 
     * Generated on any command failure.
     */
    EPS_UART_DATA_TYPE_TM_INVALID_HEADER_LENGTH = 135,

    /**
     * @brief EPS_TM sent when the length of the received farme is less than 
     * that specified by the data type.
     * 
     * Generated on any command failure.
     */
    EPS_UART_DATA_TYPE_TM_INVALID_PAYLOAD_LENGTH = 136,

    /**
     * @brief EPS_TM sent when the CRC of the frame received by the EPS is 
     * incorrect. 
     * 
     * Generated on any command failure.
     */
    EPS_UART_DATA_TYPE_TM_CRC_FAIL = 137,

    /**
     * @brief Unsolicited EPS_TM sent when the CRC of the EPS's flash has 
     * failed. This shall prompt a new config to be sent.
     * 
     * Generated on failure of the EPS flash CRC.
     */
    EPS_UART_DATA_TYPE_TM_FLASH_READ_FAIL = 138,
    
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
     * @brief Indicates that the Eps is still waiting for the EPS to reply to
     * the last command.
     */
    EPS_COMMAND_IN_PROGRESS = 1,

    /**
     * @brief Indicates that a command has finished successfully.
     */
    EPS_COMMAND_SUCCESS = 2,

    /**
     * @brief Indicates that a command has failed.
     * 
     * See `DP.EPS.ERROR_CODE` for the cause of this failure.
     */
    EPS_COMMAND_FAILURE = 3

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
 * @brief Send the EPS configuration.
 * 
 * @return bool True on success, false on error. See DP.EPS.ERROR_CODE for the
 * cause of the error.
 */
bool Eps_send_config(void);

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

/**
 * @brief Sends a command to the EPS to reset the OCP rails described by the
 * given OcpState. A reset is defined as turning the rail off then on again.
 * 
 * @param ocp_state_in For each rail: true to reset, false to not reset.
 * @return bool True on success, false on failure. See DP.EPS.ERROR_CODE for
 * error cause. 
 */
bool Eps_reset_ocp(Eps_OcpState ocp_state_in);

/**
 * @brief Determines if the two EPS OCP states match.
 * 
 * @param p_a_in Pointer to the first state.
 * @param p_b_in Pointer to the second state.
 * @return bool True if they match, false if they don't
 */
bool Eps_do_ocp_states_match(
    Eps_OcpState *p_a_in,
    Eps_OcpState *p_b_in
);

#endif /* H_EPS_PUBLIC_H */