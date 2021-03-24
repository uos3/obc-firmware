
/**
 * @ingroup data_pool
 * 
 * @file DataPool_generated.c
 * @author Generated by DataPool_generate.py
 * @brief Generated code to assist with DataPool usage.
 * 
 * This file was generated from DataPool_struct.h by DataPool_generate.py.
 * 
 * @version Generated from DataPool_struct.h version 0.1
 * @date 2021-03-24
 * 
 * @copyright Copyright (c) UoS3 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stddef.h>

/* Internal includes */
#include "util/debug/Debug_public.h"
#include "system/data_pool/DataPool_public.h"
#include "system/data_pool/DataPool_generated.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

bool DataPool_get(
    DataPool_Id id_in,
    void **pp_data_out,
    DataPool_DataType *p_data_type_out,
    size_t *p_data_size_out
) {

    switch (id_in) {
    
    
    /* DP.INITIALISED */
    case 0x0001:
        *pp_data_out = &DP.INITIALISED;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.BOARD_INITIALISED */
    case 0x0002:
        *pp_data_out = &DP.BOARD_INITIALISED;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.EVENTMANAGER.INITIALISED */
    case 0x0c01:
        *pp_data_out = &DP.EVENTMANAGER.INITIALISED;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.EVENTMANAGER.ERROR_CODE */
    case 0x0c02:
        *pp_data_out = &DP.EVENTMANAGER.ERROR_CODE;
        *p_data_type_out = DATAPOOL_DATATYPE_ERRORCODE;
        *p_data_size_out = sizeof(ErrorCode);
        return true;


    /* DP.EVENTMANAGER.MAX_EVENTS_REACHED */
    case 0x0c03:
        *pp_data_out = &DP.EVENTMANAGER.MAX_EVENTS_REACHED;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.EVENTMANAGER.NUM_RAISED_EVENTS */
    case 0x0c04:
        *pp_data_out = &DP.EVENTMANAGER.NUM_RAISED_EVENTS;
        *p_data_type_out = DATAPOOL_DATATYPE_UINT8_T;
        *p_data_size_out = sizeof(uint8_t);
        return true;


    /* DP.EVENTMANAGER.EVENT_LIST_SIZE */
    case 0x0c05:
        *pp_data_out = &DP.EVENTMANAGER.EVENT_LIST_SIZE;
        *p_data_type_out = DATAPOOL_DATATYPE_SIZE_T;
        *p_data_size_out = sizeof(size_t);
        return true;


    /* DP.IMU.INITIALISED */
    case 0x9401:
        *pp_data_out = &DP.IMU.INITIALISED;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.IMU.ERROR_CODE */
    case 0x9402:
        *pp_data_out = &DP.IMU.ERROR_CODE;
        *p_data_type_out = DATAPOOL_DATATYPE_ERRORCODE;
        *p_data_size_out = sizeof(ErrorCode);
        return true;


    /* DP.IMU.I2C_ERROR_CODE */
    case 0x9403:
        *pp_data_out = &DP.IMU.I2C_ERROR_CODE;
        *p_data_type_out = DATAPOOL_DATATYPE_ERRORCODE;
        *p_data_size_out = sizeof(ErrorCode);
        return true;


    /* DP.IMU.STATE */
    case 0x9404:
        *pp_data_out = &DP.IMU.STATE;
        *p_data_type_out = DATAPOOL_DATATYPE_IMU_STATE;
        *p_data_size_out = sizeof(Imu_State);
        return true;


    /* DP.IMU.SUBSTATE */
    case 0x9405:
        *pp_data_out = &DP.IMU.SUBSTATE;
        *p_data_type_out = DATAPOOL_DATATYPE_IMU_SUBSTATE;
        *p_data_size_out = sizeof(Imu_SubState);
        return true;


    /* DP.IMU.COMMAND */
    case 0x9406:
        *pp_data_out = &DP.IMU.COMMAND;
        *p_data_type_out = DATAPOOL_DATATYPE_IMU_COMMAND;
        *p_data_size_out = sizeof(Imu_Command);
        return true;


    /* DP.IMU.GYROSCOPE_DATA */
    case 0x9407:
        *pp_data_out = &DP.IMU.GYROSCOPE_DATA;
        *p_data_type_out = DATAPOOL_DATATYPE_IMU_VECINT16;
        *p_data_size_out = sizeof(Imu_VecInt16);
        return true;


    /* DP.IMU.GYROSCOPE_DATA_VALID */
    case 0x9408:
        *pp_data_out = &DP.IMU.GYROSCOPE_DATA_VALID;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.IMU.MAGNETOMETER_DATA */
    case 0x9409:
        *pp_data_out = &DP.IMU.MAGNETOMETER_DATA;
        *p_data_type_out = DATAPOOL_DATATYPE_IMU_VECINT16;
        *p_data_size_out = sizeof(Imu_VecInt16);
        return true;


    /* DP.IMU.MAGNE_SENSE_ADJUST_DATA */
    case 0x940a:
        *pp_data_out = &DP.IMU.MAGNE_SENSE_ADJUST_DATA;
        *p_data_type_out = DATAPOOL_DATATYPE_IMU_VECUINT8;
        *p_data_size_out = sizeof(Imu_VecUint8);
        return true;


    /* DP.IMU.MAGNETOMETER_DATA_VALID */
    case 0x940b:
        *pp_data_out = &DP.IMU.MAGNETOMETER_DATA_VALID;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.IMU.TEMPERATURE_DATA */
    case 0x940c:
        *pp_data_out = &DP.IMU.TEMPERATURE_DATA;
        *p_data_type_out = DATAPOOL_DATATYPE_INT16_T;
        *p_data_size_out = sizeof(int16_t);
        return true;


    /* DP.IMU.TEMPERATURE_DATA_VALID */
    case 0x940d:
        *pp_data_out = &DP.IMU.TEMPERATURE_DATA_VALID;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.MEMSTOREMANAGER.INITIALISED */
    case 0x1001:
        *pp_data_out = &DP.MEMSTOREMANAGER.INITIALISED;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.MEMSTOREMANAGER.ERROR_CODE */
    case 0x1002:
        *pp_data_out = &DP.MEMSTOREMANAGER.ERROR_CODE;
        *p_data_type_out = DATAPOOL_DATATYPE_ERRORCODE;
        *p_data_size_out = sizeof(ErrorCode);
        return true;


    /* DP.MEMSTOREMANAGER.EEPROM_ERROR_CODE */
    case 0x1003:
        *pp_data_out = &DP.MEMSTOREMANAGER.EEPROM_ERROR_CODE;
        *p_data_type_out = DATAPOOL_DATATYPE_ERRORCODE;
        *p_data_size_out = sizeof(ErrorCode);
        return true;


    /* DP.MEMSTOREMANAGER.CFG_FILE_1_OK */
    case 0x1004:
        *pp_data_out = &DP.MEMSTOREMANAGER.CFG_FILE_1_OK;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.MEMSTOREMANAGER.CFG_FILE_2_OK */
    case 0x1005:
        *pp_data_out = &DP.MEMSTOREMANAGER.CFG_FILE_2_OK;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.MEMSTOREMANAGER.CFG_FILE_3_OK */
    case 0x1006:
        *pp_data_out = &DP.MEMSTOREMANAGER.CFG_FILE_3_OK;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.MEMSTOREMANAGER.USE_BACKUP_CFG */
    case 0x1007:
        *pp_data_out = &DP.MEMSTOREMANAGER.USE_BACKUP_CFG;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.MEMSTOREMANAGER.PERS_DATA_DIRTY */
    case 0x1008:
        *pp_data_out = &DP.MEMSTOREMANAGER.PERS_DATA_DIRTY;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.MEMSTOREMANAGER.PERS_FILE_1_OK */
    case 0x1009:
        *pp_data_out = &DP.MEMSTOREMANAGER.PERS_FILE_1_OK;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.MEMSTOREMANAGER.PERS_FILE_2_OK */
    case 0x100a:
        *pp_data_out = &DP.MEMSTOREMANAGER.PERS_FILE_2_OK;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.MEMSTOREMANAGER.PERS_FILE_3_OK */
    case 0x100b:
        *pp_data_out = &DP.MEMSTOREMANAGER.PERS_FILE_3_OK;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.EPS.INITIALISED */
    case 0x8801:
        *pp_data_out = &DP.EPS.INITIALISED;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.EPS.ERROR_CODE */
    case 0x8802:
        *pp_data_out = &DP.EPS.ERROR_CODE;
        *p_data_type_out = DATAPOOL_DATATYPE_ERRORCODE;
        *p_data_size_out = sizeof(ErrorCode);
        return true;


    /* DP.EPS.STATE */
    case 0x8803:
        *pp_data_out = &DP.EPS.STATE;
        *p_data_type_out = DATAPOOL_DATATYPE_EPS_STATE;
        *p_data_size_out = sizeof(Eps_State);
        return true;


    /* DP.EPS.CONFIG_SYNCED */
    case 0x8804:
        *pp_data_out = &DP.EPS.CONFIG_SYNCED;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.EPS.NEW_REQUEST */
    case 0x8805:
        *pp_data_out = &DP.EPS.NEW_REQUEST;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.EPS.EPS_REQUEST[EPS_MAX_UART_FRAME_LENGTH] */
    case 0x8806:
        *pp_data_out = &DP.EPS.EPS_REQUEST[EPS_MAX_UART_FRAME_LENGTH];
        *p_data_type_out = DATAPOOL_DATATYPE_UINT8_T;
        *p_data_size_out = sizeof(uint8_t);
        return true;


    /* DP.EPS.EPS_REQUEST_LENGTH */
    case 0x8807:
        *pp_data_out = &DP.EPS.EPS_REQUEST_LENGTH;
        *p_data_type_out = DATAPOOL_DATATYPE_SIZE_T;
        *p_data_size_out = sizeof(size_t);
        return true;


    /* DP.EPS.EPS_REPLY[EPS_MAX_UART_FRAME_LENGTH] */
    case 0x8808:
        *pp_data_out = &DP.EPS.EPS_REPLY[EPS_MAX_UART_FRAME_LENGTH];
        *p_data_type_out = DATAPOOL_DATATYPE_UINT8_T;
        *p_data_size_out = sizeof(uint8_t);
        return true;


    /* DP.EPS.EPS_REPLY_LENGTH */
    case 0x8809:
        *pp_data_out = &DP.EPS.EPS_REPLY_LENGTH;
        *p_data_type_out = DATAPOOL_DATATYPE_SIZE_T;
        *p_data_size_out = sizeof(size_t);
        return true;


    /* DP.EPS.UART_FRAME_NUMBER */
    case 0x880a:
        *pp_data_out = &DP.EPS.UART_FRAME_NUMBER;
        *p_data_type_out = DATAPOOL_DATATYPE_UINT8_T;
        *p_data_size_out = sizeof(uint8_t);
        return true;


    /* DP.EPS.COMMAND_STATUS */
    case 0x880b:
        *pp_data_out = &DP.EPS.COMMAND_STATUS;
        *p_data_type_out = DATAPOOL_DATATYPE_EPS_COMMANDSTATUS;
        *p_data_size_out = sizeof(Eps_CommandStatus);
        return true;


    /* DP.EPS.HK_DATA */
    case 0x880c:
        *pp_data_out = &DP.EPS.HK_DATA;
        *p_data_type_out = DATAPOOL_DATATYPE_EPS_HKDATA;
        *p_data_size_out = sizeof(Eps_HkData);
        return true;


    /* DP.POWER.INITIALISED */
    case 0xd401:
        *pp_data_out = &DP.POWER.INITIALISED;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.POWER.ERROR_CODE */
    case 0xd402:
        *pp_data_out = &DP.POWER.ERROR_CODE;
        *p_data_type_out = DATAPOOL_DATATYPE_ERRORCODE;
        *p_data_size_out = sizeof(ErrorCode);
        return true;


    /* DP.POWER.TIMER_ERROR_CODE */
    case 0xd403:
        *pp_data_out = &DP.POWER.TIMER_ERROR_CODE;
        *p_data_type_out = DATAPOOL_DATATYPE_ERRORCODE;
        *p_data_size_out = sizeof(ErrorCode);
        return true;


    /* DP.POWER.LOW_POWER_MODE_REQUEST */
    case 0xd404:
        *pp_data_out = &DP.POWER.LOW_POWER_MODE_REQUEST;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.POWER.TASK_TIMER_EVENT */
    case 0xd405:
        *pp_data_out = &DP.POWER.TASK_TIMER_EVENT;
        *p_data_type_out = DATAPOOL_DATATYPE_EVENT;
        *p_data_size_out = sizeof(Event);
        return true;


    /* DP.POWER.REQUESTED_OCP_STATE */
    case 0xd406:
        *pp_data_out = &DP.POWER.REQUESTED_OCP_STATE;
        *p_data_type_out = DATAPOOL_DATATYPE_EPS_OCPSTATE;
        *p_data_size_out = sizeof(Eps_OcpState);
        return true;


    /* DP.POWER.UPDATE_EPS_HK */
    case 0xd407:
        *pp_data_out = &DP.POWER.UPDATE_EPS_HK;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.POWER.UPDATE_EPS_CFG */
    case 0xd408:
        *pp_data_out = &DP.POWER.UPDATE_EPS_CFG;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.POWER.UPDATE_EPS_OCP_STATE */
    case 0xd409:
        *pp_data_out = &DP.POWER.UPDATE_EPS_OCP_STATE;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.POWER.LAST_EPS_COMMAND */
    case 0xd40a:
        *pp_data_out = &DP.POWER.LAST_EPS_COMMAND;
        *p_data_type_out = DATAPOOL_DATATYPE_EPS_UARTDATATYPE;
        *p_data_size_out = sizeof(Eps_UartDataType);
        return true;


    /* DP.POWER.NUM_CONSEC_FAILED_EPS_COMMANDS */
    case 0xd40b:
        *pp_data_out = &DP.POWER.NUM_CONSEC_FAILED_EPS_COMMANDS;
        *p_data_type_out = DATAPOOL_DATATYPE_UINT8_T;
        *p_data_size_out = sizeof(uint8_t);
        return true;


    /* DP.POWER.EPS_OCP_STATE_CORRECT */
    case 0xd40c:
        *pp_data_out = &DP.POWER.EPS_OCP_STATE_CORRECT;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.POWER.OPMODE_CHANGE_IN_PROGRESS */
    case 0xd40d:
        *pp_data_out = &DP.POWER.OPMODE_CHANGE_IN_PROGRESS;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.OPMODEMANAGER.INITIALISED */
    case 0x2801:
        *pp_data_out = &DP.OPMODEMANAGER.INITIALISED;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.OPMODEMANAGER.ERROR_CODE */
    case 0x2802:
        *pp_data_out = &DP.OPMODEMANAGER.ERROR_CODE;
        *p_data_type_out = DATAPOOL_DATATYPE_ERRORCODE;
        *p_data_size_out = sizeof(ErrorCode);
        return true;


    /* DP.OPMODEMANAGER.STATE */
    case 0x2803:
        *pp_data_out = &DP.OPMODEMANAGER.STATE;
        *p_data_type_out = DATAPOOL_DATATYPE_OPMODEMANAGER_STATE;
        *p_data_size_out = sizeof(OpModeManager_State);
        return true;


    /* DP.OPMODEMANAGER.OPMODE */
    case 0x2804:
        *pp_data_out = &DP.OPMODEMANAGER.OPMODE;
        *p_data_type_out = DATAPOOL_DATATYPE_OPMODEMANAGER_OPMODE;
        *p_data_size_out = sizeof(OpModeManager_OpMode);
        return true;


    /* DP.OPMODEMANAGER.NEXT_OPMODE */
    case 0x2805:
        *pp_data_out = &DP.OPMODEMANAGER.NEXT_OPMODE;
        *p_data_type_out = DATAPOOL_DATATYPE_OPMODEMANAGER_OPMODE;
        *p_data_size_out = sizeof(OpModeManager_OpMode);
        return true;


    /* DP.OPMODEMANAGER.TC_REQUEST_NEW_OPMODE */
    case 0x2806:
        *pp_data_out = &DP.OPMODEMANAGER.TC_REQUEST_NEW_OPMODE;
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;


    /* DP.OPMODEMANAGER.GRACE_TRANS_STATE */
    case 0x2807:
        *pp_data_out = &DP.OPMODEMANAGER.GRACE_TRANS_STATE;
        *p_data_type_out = DATAPOOL_DATATYPE_OPMODEMANAGER_GRACETRANSSTATE;
        *p_data_size_out = sizeof(OpModeManager_GraceTransState);
        return true;


    /* DP.OPMODEMANAGER.GRACE_TRANS_TIMEOUT_EVENT */
    case 0x2808:
        *pp_data_out = &DP.OPMODEMANAGER.GRACE_TRANS_TIMEOUT_EVENT;
        *p_data_type_out = DATAPOOL_DATATYPE_EVENT;
        *p_data_size_out = sizeof(Event);
        return true;


    /* DP.OPMODEMANAGER.APP_IN_NEXT_MODE[OPMODEMANAGER_MAX_NUM_APPS_IN_MODE] */
    case 0x2809:
        *pp_data_out = &DP.OPMODEMANAGER.APP_IN_NEXT_MODE[OPMODEMANAGER_MAX_NUM_APPS_IN_MODE];
        *p_data_type_out = DATAPOOL_DATATYPE_BOOL;
        *p_data_size_out = sizeof(bool);
        return true;

    
    default:
        DEBUG_ERR("Invalid DP ID: %d", id_in);
        return false;
    }
}

bool DataPool_get_symbol_str(
    DataPool_Id id_in, 
    char **pp_symbol_str_out
) {

    switch (id_in) {
    
    
    /* DP.INITIALISED */
    case 0x0001:
        *pp_symbol_str_out = strdup("DP.INITIALISED");
        return true;


    /* DP.BOARD_INITIALISED */
    case 0x0002:
        *pp_symbol_str_out = strdup("DP.BOARD_INITIALISED");
        return true;


    /* DP.EVENTMANAGER.INITIALISED */
    case 0x0c01:
        *pp_symbol_str_out = strdup("DP.EVENTMANAGER.INITIALISED");
        return true;


    /* DP.EVENTMANAGER.ERROR_CODE */
    case 0x0c02:
        *pp_symbol_str_out = strdup("DP.EVENTMANAGER.ERROR_CODE");
        return true;


    /* DP.EVENTMANAGER.MAX_EVENTS_REACHED */
    case 0x0c03:
        *pp_symbol_str_out = strdup("DP.EVENTMANAGER.MAX_EVENTS_REACHED");
        return true;


    /* DP.EVENTMANAGER.NUM_RAISED_EVENTS */
    case 0x0c04:
        *pp_symbol_str_out = strdup("DP.EVENTMANAGER.NUM_RAISED_EVENTS");
        return true;


    /* DP.EVENTMANAGER.EVENT_LIST_SIZE */
    case 0x0c05:
        *pp_symbol_str_out = strdup("DP.EVENTMANAGER.EVENT_LIST_SIZE");
        return true;


    /* DP.IMU.INITIALISED */
    case 0x9401:
        *pp_symbol_str_out = strdup("DP.IMU.INITIALISED");
        return true;


    /* DP.IMU.ERROR_CODE */
    case 0x9402:
        *pp_symbol_str_out = strdup("DP.IMU.ERROR_CODE");
        return true;


    /* DP.IMU.I2C_ERROR_CODE */
    case 0x9403:
        *pp_symbol_str_out = strdup("DP.IMU.I2C_ERROR_CODE");
        return true;


    /* DP.IMU.STATE */
    case 0x9404:
        *pp_symbol_str_out = strdup("DP.IMU.STATE");
        return true;


    /* DP.IMU.SUBSTATE */
    case 0x9405:
        *pp_symbol_str_out = strdup("DP.IMU.SUBSTATE");
        return true;


    /* DP.IMU.COMMAND */
    case 0x9406:
        *pp_symbol_str_out = strdup("DP.IMU.COMMAND");
        return true;


    /* DP.IMU.GYROSCOPE_DATA */
    case 0x9407:
        *pp_symbol_str_out = strdup("DP.IMU.GYROSCOPE_DATA");
        return true;


    /* DP.IMU.GYROSCOPE_DATA_VALID */
    case 0x9408:
        *pp_symbol_str_out = strdup("DP.IMU.GYROSCOPE_DATA_VALID");
        return true;


    /* DP.IMU.MAGNETOMETER_DATA */
    case 0x9409:
        *pp_symbol_str_out = strdup("DP.IMU.MAGNETOMETER_DATA");
        return true;


    /* DP.IMU.MAGNE_SENSE_ADJUST_DATA */
    case 0x940a:
        *pp_symbol_str_out = strdup("DP.IMU.MAGNE_SENSE_ADJUST_DATA");
        return true;


    /* DP.IMU.MAGNETOMETER_DATA_VALID */
    case 0x940b:
        *pp_symbol_str_out = strdup("DP.IMU.MAGNETOMETER_DATA_VALID");
        return true;


    /* DP.IMU.TEMPERATURE_DATA */
    case 0x940c:
        *pp_symbol_str_out = strdup("DP.IMU.TEMPERATURE_DATA");
        return true;


    /* DP.IMU.TEMPERATURE_DATA_VALID */
    case 0x940d:
        *pp_symbol_str_out = strdup("DP.IMU.TEMPERATURE_DATA_VALID");
        return true;


    /* DP.MEMSTOREMANAGER.INITIALISED */
    case 0x1001:
        *pp_symbol_str_out = strdup("DP.MEMSTOREMANAGER.INITIALISED");
        return true;


    /* DP.MEMSTOREMANAGER.ERROR_CODE */
    case 0x1002:
        *pp_symbol_str_out = strdup("DP.MEMSTOREMANAGER.ERROR_CODE");
        return true;


    /* DP.MEMSTOREMANAGER.EEPROM_ERROR_CODE */
    case 0x1003:
        *pp_symbol_str_out = strdup("DP.MEMSTOREMANAGER.EEPROM_ERROR_CODE");
        return true;


    /* DP.MEMSTOREMANAGER.CFG_FILE_1_OK */
    case 0x1004:
        *pp_symbol_str_out = strdup("DP.MEMSTOREMANAGER.CFG_FILE_1_OK");
        return true;


    /* DP.MEMSTOREMANAGER.CFG_FILE_2_OK */
    case 0x1005:
        *pp_symbol_str_out = strdup("DP.MEMSTOREMANAGER.CFG_FILE_2_OK");
        return true;


    /* DP.MEMSTOREMANAGER.CFG_FILE_3_OK */
    case 0x1006:
        *pp_symbol_str_out = strdup("DP.MEMSTOREMANAGER.CFG_FILE_3_OK");
        return true;


    /* DP.MEMSTOREMANAGER.USE_BACKUP_CFG */
    case 0x1007:
        *pp_symbol_str_out = strdup("DP.MEMSTOREMANAGER.USE_BACKUP_CFG");
        return true;


    /* DP.MEMSTOREMANAGER.PERS_DATA_DIRTY */
    case 0x1008:
        *pp_symbol_str_out = strdup("DP.MEMSTOREMANAGER.PERS_DATA_DIRTY");
        return true;


    /* DP.MEMSTOREMANAGER.PERS_FILE_1_OK */
    case 0x1009:
        *pp_symbol_str_out = strdup("DP.MEMSTOREMANAGER.PERS_FILE_1_OK");
        return true;


    /* DP.MEMSTOREMANAGER.PERS_FILE_2_OK */
    case 0x100a:
        *pp_symbol_str_out = strdup("DP.MEMSTOREMANAGER.PERS_FILE_2_OK");
        return true;


    /* DP.MEMSTOREMANAGER.PERS_FILE_3_OK */
    case 0x100b:
        *pp_symbol_str_out = strdup("DP.MEMSTOREMANAGER.PERS_FILE_3_OK");
        return true;


    /* DP.EPS.INITIALISED */
    case 0x8801:
        *pp_symbol_str_out = strdup("DP.EPS.INITIALISED");
        return true;


    /* DP.EPS.ERROR_CODE */
    case 0x8802:
        *pp_symbol_str_out = strdup("DP.EPS.ERROR_CODE");
        return true;


    /* DP.EPS.STATE */
    case 0x8803:
        *pp_symbol_str_out = strdup("DP.EPS.STATE");
        return true;


    /* DP.EPS.CONFIG_SYNCED */
    case 0x8804:
        *pp_symbol_str_out = strdup("DP.EPS.CONFIG_SYNCED");
        return true;


    /* DP.EPS.NEW_REQUEST */
    case 0x8805:
        *pp_symbol_str_out = strdup("DP.EPS.NEW_REQUEST");
        return true;


    /* DP.EPS.EPS_REQUEST[EPS_MAX_UART_FRAME_LENGTH] */
    case 0x8806:
        *pp_symbol_str_out = strdup("DP.EPS.EPS_REQUEST[EPS_MAX_UART_FRAME_LENGTH]");
        return true;


    /* DP.EPS.EPS_REQUEST_LENGTH */
    case 0x8807:
        *pp_symbol_str_out = strdup("DP.EPS.EPS_REQUEST_LENGTH");
        return true;


    /* DP.EPS.EPS_REPLY[EPS_MAX_UART_FRAME_LENGTH] */
    case 0x8808:
        *pp_symbol_str_out = strdup("DP.EPS.EPS_REPLY[EPS_MAX_UART_FRAME_LENGTH]");
        return true;


    /* DP.EPS.EPS_REPLY_LENGTH */
    case 0x8809:
        *pp_symbol_str_out = strdup("DP.EPS.EPS_REPLY_LENGTH");
        return true;


    /* DP.EPS.UART_FRAME_NUMBER */
    case 0x880a:
        *pp_symbol_str_out = strdup("DP.EPS.UART_FRAME_NUMBER");
        return true;


    /* DP.EPS.COMMAND_STATUS */
    case 0x880b:
        *pp_symbol_str_out = strdup("DP.EPS.COMMAND_STATUS");
        return true;


    /* DP.EPS.HK_DATA */
    case 0x880c:
        *pp_symbol_str_out = strdup("DP.EPS.HK_DATA");
        return true;


    /* DP.POWER.INITIALISED */
    case 0xd401:
        *pp_symbol_str_out = strdup("DP.POWER.INITIALISED");
        return true;


    /* DP.POWER.ERROR_CODE */
    case 0xd402:
        *pp_symbol_str_out = strdup("DP.POWER.ERROR_CODE");
        return true;


    /* DP.POWER.TIMER_ERROR_CODE */
    case 0xd403:
        *pp_symbol_str_out = strdup("DP.POWER.TIMER_ERROR_CODE");
        return true;


    /* DP.POWER.LOW_POWER_MODE_REQUEST */
    case 0xd404:
        *pp_symbol_str_out = strdup("DP.POWER.LOW_POWER_MODE_REQUEST");
        return true;


    /* DP.POWER.TASK_TIMER_EVENT */
    case 0xd405:
        *pp_symbol_str_out = strdup("DP.POWER.TASK_TIMER_EVENT");
        return true;


    /* DP.POWER.REQUESTED_OCP_STATE */
    case 0xd406:
        *pp_symbol_str_out = strdup("DP.POWER.REQUESTED_OCP_STATE");
        return true;


    /* DP.POWER.UPDATE_EPS_HK */
    case 0xd407:
        *pp_symbol_str_out = strdup("DP.POWER.UPDATE_EPS_HK");
        return true;


    /* DP.POWER.UPDATE_EPS_CFG */
    case 0xd408:
        *pp_symbol_str_out = strdup("DP.POWER.UPDATE_EPS_CFG");
        return true;


    /* DP.POWER.UPDATE_EPS_OCP_STATE */
    case 0xd409:
        *pp_symbol_str_out = strdup("DP.POWER.UPDATE_EPS_OCP_STATE");
        return true;


    /* DP.POWER.LAST_EPS_COMMAND */
    case 0xd40a:
        *pp_symbol_str_out = strdup("DP.POWER.LAST_EPS_COMMAND");
        return true;


    /* DP.POWER.NUM_CONSEC_FAILED_EPS_COMMANDS */
    case 0xd40b:
        *pp_symbol_str_out = strdup("DP.POWER.NUM_CONSEC_FAILED_EPS_COMMANDS");
        return true;


    /* DP.POWER.EPS_OCP_STATE_CORRECT */
    case 0xd40c:
        *pp_symbol_str_out = strdup("DP.POWER.EPS_OCP_STATE_CORRECT");
        return true;


    /* DP.POWER.OPMODE_CHANGE_IN_PROGRESS */
    case 0xd40d:
        *pp_symbol_str_out = strdup("DP.POWER.OPMODE_CHANGE_IN_PROGRESS");
        return true;


    /* DP.OPMODEMANAGER.INITIALISED */
    case 0x2801:
        *pp_symbol_str_out = strdup("DP.OPMODEMANAGER.INITIALISED");
        return true;


    /* DP.OPMODEMANAGER.ERROR_CODE */
    case 0x2802:
        *pp_symbol_str_out = strdup("DP.OPMODEMANAGER.ERROR_CODE");
        return true;


    /* DP.OPMODEMANAGER.STATE */
    case 0x2803:
        *pp_symbol_str_out = strdup("DP.OPMODEMANAGER.STATE");
        return true;


    /* DP.OPMODEMANAGER.OPMODE */
    case 0x2804:
        *pp_symbol_str_out = strdup("DP.OPMODEMANAGER.OPMODE");
        return true;


    /* DP.OPMODEMANAGER.NEXT_OPMODE */
    case 0x2805:
        *pp_symbol_str_out = strdup("DP.OPMODEMANAGER.NEXT_OPMODE");
        return true;


    /* DP.OPMODEMANAGER.TC_REQUEST_NEW_OPMODE */
    case 0x2806:
        *pp_symbol_str_out = strdup("DP.OPMODEMANAGER.TC_REQUEST_NEW_OPMODE");
        return true;


    /* DP.OPMODEMANAGER.GRACE_TRANS_STATE */
    case 0x2807:
        *pp_symbol_str_out = strdup("DP.OPMODEMANAGER.GRACE_TRANS_STATE");
        return true;


    /* DP.OPMODEMANAGER.GRACE_TRANS_TIMEOUT_EVENT */
    case 0x2808:
        *pp_symbol_str_out = strdup("DP.OPMODEMANAGER.GRACE_TRANS_TIMEOUT_EVENT");
        return true;


    /* DP.OPMODEMANAGER.APP_IN_NEXT_MODE[OPMODEMANAGER_MAX_NUM_APPS_IN_MODE] */
    case 0x2809:
        *pp_symbol_str_out = strdup("DP.OPMODEMANAGER.APP_IN_NEXT_MODE[OPMODEMANAGER_MAX_NUM_APPS_IN_MODE]");
        return true;


    default:
        DEBUG_ERR("Invalid DP ID: %d", id_in);
        return false;
    }
}
