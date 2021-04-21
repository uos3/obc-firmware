/**
 * @file Eps_private.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief EPS Component Module private definitions.
 * 
 * Task ref: [UT_2.10.7]
 * 
 * @version 0.1
 * @date 2021-02-17
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_EPS_PRIVATE_H
#define H_EPS_PRIVATE_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdint.h>

/* Internal incldues */
#include "components/eps/Eps_public.h"

/* -------------------------------------------------------------------------   
 * TYPES
 * ------------------------------------------------------------------------- */

/**
 * @brief The byte representing the state of the EPS OCP (over-current 
 * protection) rails.
 * 
 * This is a bitfield, in which each bit represents the state of a particular
 * rail of the EPS, either on (1) or off (0). The bitmasks are defined as
 * EPS_OCP_RAIL_x_MASK. 
 */
typedef uint8_t Eps_OcpByte;

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief The resistance of the R1 resistor in the ADC voltage sense circuit.
 */
#define EPS_ADC_VOLTAGE_SENSE_R1_OHMS ((double)(309000.0))

/**
 * @brief The resistance of the R2 resistor in the ADC voltage sense circuit.
 */
#define EPS_ADC_VOLTAGE_SENSE_R2_OHMS ((double)(100000.0))

/**
 * @brief The conversion factor to map ADC vsense scaledints to volts.
 */
#define EPS_ADC_VOLTAGE_SENSE_SCALEDINT_TO_VOLTS ((double)(2.5 / 1024.0))

/**
 * @brief Bitshift for the Radio TX rail of an Eps_OcpByte variable.
 * 
 * As per [DDF_1_15] this is for OCP rail 1.
 */
#define EPS_OCP_RAIL_RADIO_TX_SHIFT (0)

/**
 * @brief Bitshift for the Radio RX rail of an Eps_OcpByte variable.
 * 
 * As per [DDF_1_15] this is for OCP rail 2.
 */
#define EPS_OCP_RAIL_RADIO_RX_CAMERA_SHIFT (1)

/**
 * @brief Bitshift for the Camera rail of an Eps_OcpByte variable.
 * 
 * As per [DDF_1_15] this is for OCP rail 3.
 */
#define EPS_OCP_RAIL_EPS_MCU_SHIFT (2)

/**
 * @brief Bitshift for the OBC rail of an Eps_OcpByte variable.
 * 
 * As per [DDF_1_15] this is for OCP rail 4.
 */
#define EPS_OCP_RAIL_OBC_SHIFT (3)

/**
 * @brief Bitshift for the GNSS RX rail of an Eps_OcpByte variable.
 * 
 * As per [DDF_1_15] this is for OCP rail 5.
 */
#define EPS_OCP_RAIL_GNSS_RX_SHIFT (4)

/**
 * @brief Bitshift for the GNSS LNA rail of an Eps_OcpByte variable.
 * 
 * As per [DDF_1_15] this is for OCP rail 6.
 */
#define EPS_OCP_RAIL_GNSS_LNA_SHIFT (5)

/**
 * @brief Bitmask for the Radio TX rail of an Eps_OcpByte variable.
 * 
 * As per [DDF_1_15] this is for OCP rail 1.
 */
#define EPS_OCP_RAIL_RADIO_TX_MASK (1 << EPS_OCP_RAIL_RADIO_TX_SHIFT)

/**
 * @brief Bitmask for the Radio RX/Camera rail of an Eps_OcpByte variable.
 * 
 * As per [DDF_1_15] this is for OCP rail 2.
 */
#define EPS_OCP_RAIL_RADIO_RX_CAMERA_MASK (1 << EPS_OCP_RAIL_RADIO_RX_CAMERA_SHIFT)

/**
 * @brief Bitmask for the EPS MCU rail of an Eps_OcpByte variable.
 * 
 * As per [DDF_1_15] this is for OCP rail 3.
 */
#define EPS_OCP_RAIL_EPS_MCU_MASK (1 << EPS_OCP_RAIL_EPS_MCU_SHIFT)

/**
 * @brief Bitmask for the OBC rail of an Eps_OcpByte variable.
 * 
 * As per [DDF_1_15] this is for OCP rail 4.
 */
#define EPS_OCP_RAIL_OBC_MASK (1 << EPS_OCP_RAIL_OBC_SHIFT)

/**
 * @brief Bitmask for the GNSS RX rail of an Eps_OcpByte variable.
 * 
 * As per [DDF_1_15] this is for OCP rail 5.
 */
#define EPS_OCP_RAIL_GNSS_RX_MASK (1 << EPS_OCP_RAIL_GNSS_RX_SHIFT)

/**
 * @brief Bitmask for the GNSS LNA rail of an Eps_OcpByte variable.
 * 
 * As per [DDF_1_15] this is for OCP rail 6.
 */
#define EPS_OCP_RAIL_GNSS_LNA_MASK (1 << EPS_OCP_RAIL_GNSS_LNA_SHIFT)

/**
 * @brief Length of the payload for EPS_UART_DATA_TYPE_TC_COLLECT_HK_DATA.
 * 
 */
#define EPS_UART_TC_COLLECT_HK_DATA_PL_LENGTH (0)

/**
 * @brief Length of the payload for EPS_UART_DATA_TYPE_TC_SET_OCP_STATE.
 * 
 */
#define EPS_UART_TC_SET_OCP_STATE_PL_LENGTH (1)

/**
 * @brief Length of the payload for EPS_UART_DATA_TYPE_TC_SEND_BATT_CMD.
 * 
 */
#define EPS_UART_TC_SEND_BATT_CMD_PL_LENGTH (3)

/**
 * @brief Length of the payload for EPS_UART_DATA_TYPE_TC_SET_CONFIG.
 */
#define EPS_UART_TC_SET_CONFIG_PL_LENGTH (sizeof(Eps_ConfigData))

/**
 * @brief Length of the payload for EPS_UART_DATA_TYPE_TC_RESET_OCP.
 */
#define EPS_UART_TC_RESET_OCP_PL_LENGTH (1)

/**
 * @brief Length of the EPS_UART_DATA_TYPE_TM_HK_DATA payload in bytes.
 * 
 * From [SW_ICD]
 */
#define EPS_UART_TM_HK_DATA_PL_LENGTH (105)

/**
 * @brief Length of the EPS_UART_DATA_TYPE_TM_BATT_REPLY payload in bytes.
 */
#define EPS_UART_TM_BATT_REPLY_PL_LENGTH (0)

/**
 * @brief Length of the EPS_UART_DATA_TYPE_TM_LOADED_CONFIG payload in bytes.
 */
#define EPS_UART_TM_LOADED_CONFIG_PL_LENGTH (0)

/**
 * @brief Length of the EPS_UART_DATA_TYPE_TM_OCP_STATE payload in bytes.
 */
#define EPS_UART_TM_OCP_STATE_PL_LENGTH (1)

/**
 * @brief Length of the EPS_UART_DATA_TYPE_TM_OCP_TRIPPED payload in bytes.
 */
#define EPS_UART_TM_OCP_TRIPPED_PL_LENGTH (1)

/**
 * @brief Shunt resistor value for the ADC current sense on all solar panels.
 */
#define EPS_ADC_SHUNT_RESIST_SOLAR_PANELS_OHMS ((double)0.043)

/**
 * @brief Shunt resistor value for the ADC current sense on OPC rail 1.
 */
#define EPS_ADC_SHUNT_RESIST_OCP_RAIL_1_OHMS ((double)0.050)

/**
 * @brief Shunt resistor value for the ADC current sense on OPC rail 2.
 */
#define EPS_ADC_SHUNT_RESIST_OCP_RAIL_2_OHMS ((double)0.062)

/**
 * @brief Shunt resistor value for the ADC current sense on OPC rail 3.
 */
#define EPS_ADC_SHUNT_RESIST_OCP_RAIL_3_OHMS ((double)6.0)

/**
 * @brief Shunt resistor value for the ADC current sense on OPC rail 4.
 */
#define EPS_ADC_SHUNT_RESIST_OCP_RAIL_4_OHMS ((double)0.442)

/**
 * @brief Shunt resistor value for the ADC current sense on OPC rail 5.
 */
#define EPS_ADC_SHUNT_RESIST_OCP_RAIL_5_OHMS ((double)0.036)

/**
 * @brief Shunt resistor value for the ADC current sense on OPC rail 6.
 */
#define EPS_ADC_SHUNT_RESIST_OCP_RAIL_6_OHMS ((double)0.124)

/**
 * @brief Shunt resistor value for the ADC current sense on the 3V3 supply.
 */
#define EPS_ADC_SHUNT_RESIST_3V3_SUPPLY_OHMS ((double)0.250)

/**
 * @brief Shunt resistor value for the ADC current sense on the 5V supply.
 * 
 */
#define EPS_ADC_SHUNT_RESIST_5V_SUPPLY_OHMS ((double)0.124)

/**
 * @brief Shunt resistor value for the ADC current sense on the charge line.
 * 
 */
#define EPS_ADC_SHUNT_RESIST_CHARGE_OHMS ((double)0.250)

/**
 * @brief EPS command timeout duration in seconds.
 */
#define EPS_COMMAND_TIMEOUT_S ((double)(4.0))

/* -------------------------------------------------------------------------   
 * STRUCTS
 * ------------------------------------------------------------------------- */

/**
 * @brief Build a new UART header from the given data type.
 * 
 * The frame number is determined automatically, and incremented.
 * 
 * @param data_type_in The data type of the frame to construct.
 * @param p_header_out Pointer to the location to store the header.
 */
void Eps_build_uart_header(
    Eps_UartDataType data_type_in,
    uint8_t *p_header_out
);

/**
 * @brief Append the CRC to the end of the given data frame.
 * 
 * After appending the length of the frame will be length_without_crc_in +
 * EPS_UART_CRC_LENGTH.
 * 
 * NOTE: The frame buffer must be large enough to contain the frame + the crc.
 * 
 * @param p_frame_in Frame to append the CRC to
 * @param length_without_crc_in The length of the frame without the CRC
 */
void Eps_append_crc_to_frame(
    uint8_t *p_frame_in,
    size_t length_without_crc_in
);

/**
 * @brief Check that the given UART frame is correct by recomputing the CRC
 * and checking it with the CRC in the frame.
 * 
 * @param p_frame_in The frame to check
 * @param length_in The length of the frame (including CRC bytes)
 * @return bool True if the frame is valid (CRCs match), false otherwise
 */
bool Eps_check_uart_frame(
    uint8_t *p_frame_in,
    size_t length_in
);

/**
 * @brief Parse the provided HK data packet data into an `Eps_HkData` struct.
 * 
 * @param p_data_in Raw data to parse. Shall be of length
 * EPS_UART_TM_HK_DATA_PL_LENGTH. 
 * @param p_hk_data_out Pointer to the HK data struct to populate
 * @return bool True if successful, false if failure.
 */
bool Eps_parse_hk_data(
    uint8_t *p_data_in,
    Eps_HkData *p_hk_data_out
);

/**
 * @brief Converts between a 10-bit ADC scaled integer voltage sense value and
 * the voltage of the sensed line.
 * 
 * @param voltage_scaledint_in The ADC scaled int to convert
 * @return double The calculated voltage in Volts.
 */
double Eps_adc_voltage_sense_scaledint_to_volts(
    uint16_t voltage_scaledint_in
);

/**
 * @brief Converts between a 10-bit ADC scaled integer voltage sense value and
 * the current draw of the sensed line.
 * 
 * @param voltage_scaledint_in The ADC scaled int to convert
 * @param shunt_resistor_ohms_in The resistance of the shunt resistor for this
 * voltage sense value.
 * @return double The calculated current in Amperes.
 */
double Eps_adc_voltage_sense_scaledint_to_amps(
    uint16_t voltage_scaledint_in,
    double shunt_resistor_ohms_in
);

/**
 * @brief Convert from an Eps_OcpState struct to an Eps_OcpByte.
 * 
 * @param state_in The state to convert
 * @return Eps_OcpByte The converted byte.
 */
Eps_OcpByte Eps_ocp_state_to_ocp_byte(Eps_OcpState state_in);

/**
 * @brief Convert from an Eps_OcpByte to an Eps_OcpState struct.
 * 
 * @param byte_in The byte to convert
 * @return Eps_OcpState The converted state
 */
Eps_OcpState Eps_ocp_byte_to_ocp_state(Eps_OcpByte byte_in);

/**
 * @brief Parse the given bytes into an Eps_BattStatus struct.
 * 
 * @param p_data_in Pointer to the bytes to convert. Shall be 2 bytes long.
 * @return Eps_BattStatus The convered battery status
 */
Eps_BattStatus Eps_parse_batt_status(uint8_t *p_data_in);

/**
 * @brief Process a recieved header from the EPS
 * 
 * @return bool True on success, false on failure. DP.EPS.ERROR is set
 * appropriately. 
 */
bool Eps_process_uart_header(void);

/**
 * @brief Process a recieved payload (and CRC) from the EPS
 * 
 * @return bool True on success, false on failure. DP.EPS.ERROR is set
 * appropriately. 
 */
bool Eps_process_uart_payload(void);

/**
 * @brief Setup the UART driver to receive the bytes for the given payload
 * type. 
 * 
 * This function expects that the header for the payload is already present in
 * DP.EPS.EPS_REPLY, and will use this to recieve the correct number of bytes.
 * 
 * @return bool True on success, false on failure. DP.EPS.ERROR is set
 * appropriately. 
 */
bool Eps_start_uart_receive_payload(void);

/**
 * @brief Process the reply to the last sent EPS command.
 * 
 * @return bool True on success, false on failure. DP.EPS.ERROR is set
 * appropriately. 
 */
bool Eps_process_reply(void);

/**
 * @brief Simple helper function to tidy up if a reply has the wrong data type.
 */
void Eps_handle_incorrect_reply_data_type(void);

#endif /* H_EPS_PRIVATE_H */