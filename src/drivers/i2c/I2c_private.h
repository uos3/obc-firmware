/**
 * @ingroup i2c
 * 
 * @file I2c_private.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Private header for the I2C driver.
 * 
 * @version 0.1
 * @date 2020-11-11
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef H_I2C_PRIVATE_H
#define H_I2C_PRIVATE_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Internal includes */
#include "drivers/i2c/I2c_public.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief Maximum number of actions that can occur in one cycle.
 * 
 * This value comes from the fact that there are only two I2C devices being
 * used by the software:
 *  - IMU
 *  - Temperature sensor
 * 
 * TODO: Check if the battery should also be an I2C device.
 */
#define I2C_MAX_NUM_ACTIONS (2)

/**
 * @brief The number of modules on the TM4C device. 
 */
#define I2C_NUM_MODULES (4)

/**
 * @brief The maximum number of times to check for the peripheral to be ready
 * before an error will be issued.
 * 
 * TODO: This value is arbitrary
 */
#define I2C_MAX_NUM_I2C_PERIPH_READY_CHECKS (10)

/**
 * @brief The maximum number of times to check for the peripheral to be ready
 * before an error will be issued.
 * 
 * TODO: This value is arbitrary
 */
#define I2C_MAX_NUM_GPIO_PERIPH_READY_CHECKS (10)

/* -------------------------------------------------------------------------   
 * ENUMS
 * ------------------------------------------------------------------------- */

/**
 * @brief Possible action types that can be performed by the I2C driver.
 */
typedef enum _I2c_ActionType {
    I2C_ACTION_TYPE_NONE,
    I2C_ACTION_TYPE_SINGLE_SEND,
    I2C_ACTION_TYPE_SINGLE_RECV,
    I2C_ACTION_TYPE_BURST_SEND,
    I2C_ACTION_TYPE_BURST_RECV
} I2c_ActionType;

/* -------------------------------------------------------------------------   
 * STRUCTS/UNIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief I2C module definition
 * 
 * TODO: Add module disable if peripherals not responding.
 */
typedef struct _I2c_Module {
    uint32_t peripheral_gpio;
    uint32_t peripheral_i2c;
    uint32_t base_gpio;
    uint32_t base_i2c;
    uint32_t pin_scl_function;
    uint32_t pin_sda_function;
    uint8_t  pin_scl;
    uint8_t  pin_sda;
    bool     full_speed;
    bool     initialised;
} I2c_Module;

typedef struct _I2c_ActionSingleSend {
    /**
     * @brief The current step of the action state machine.
     * 
     * The action is broken down into a number of steps that execute
     * sequentially. At each step transition the action can exit while it waits
     * for some event (i.e. an interrupt) to occur.
     */
    uint8_t step;

    /**
     * @brief The current status of this action.
     */
    I2c_ActionStatus status;

    /**
     * @brief Stores any error associated with this action.
     */
    I2c_ErrorCode error;

    /**
     * @brief The device to perform this action on.
     */
    I2c_Device device;

    /**
     * @brief The byte to send to the device.
     */
    uint8_t byte;
    
} I2c_ActionSingleSend;

typedef struct _I2c_ActionSingleRecv {
    /**
     * @brief The current step of the action state machine.
     * 
     * The action is broken down into a number of steps that execute
     * sequentially. At each step transition the action can exit while it waits
     * for some event (i.e. an interrupt) to occur.
     */
    uint8_t step;

    /**
     * @brief The current status of this action.
     */
    I2c_ActionStatus status;

    /**
     * @brief Stores any error associated with this action.
     */
    I2c_ErrorCode error;

    /**
     * @brief The device to perform this action on.
     */
    I2c_Device device;

    /**
     * @brief The register on the device to read from.
     */
    uint8_t reg;

    /**
     * @brief The retrieved byte from the device's register
     */
    uint8_t byte;

} I2c_ActionSingleRecv;

typedef struct _I2c_ActionBurstSend {
    /**
     * @brief The current step of the action state machine.
     * 
     * The action is broken down into a number of steps that execute
     * sequentially. At each step transition the action can exit while it waits
     * for some event (i.e. an interrupt) to occur.
     */
    uint8_t step;

    /**
     * @brief The current status of this action.
     */
    I2c_ActionStatus status;

    /**
     * @brief Stores any error associated with this action.
     */
    I2c_ErrorCode error;

    /**
     * @brief The device to perform this action on.
     */
    I2c_Device device;

    /**
     * @brief Pointer to an array of bytes to send to the device.
     */
    uint8_t *p_byte;

    /**
     * @brief The length of the byte array to send 
     */
    size_t length;
    
} I2c_ActionBurstSend;

typedef struct _I2c_ActionBurstRecv {
    /**
     * @brief The current step of the action state machine.
     * 
     * The action is broken down into a number of steps that execute
     * sequentially. At each step transition the action can exit while it waits
     * for some event (i.e. an interrupt) to occur.
     */
    uint8_t step;

    /**
     * @brief The current status of this action.
     */
    I2c_ActionStatus status;

    /**
     * @brief Stores any error associated with this action.
     */
    I2c_ErrorCode error;

    /**
     * @brief The device to perform this action on.
     */
    I2c_Device device;

    /**
     * @brief The register on the device to read from.
     */
    uint8_t reg;

    /**
     * @brief The expected length of the bytes to recieve.
     * 
     */
    size_t length;

    /**
     * @brief Pointer to the retrieved bytes from the device's register
     */
    uint8_t *p_bytes;

} I2c_ActionBurstRecv;

/**
 * @brief Union which can be any one of the possible actions.
 * 
 * It is important to only use enum members that are of the type given in the
 * I2c.p_action_types array.
 */
typedef union _I2c_Action {
    I2c_ActionSingleSend single_send;
    I2c_ActionSingleRecv single_recv;
    I2c_ActionBurstSend burst_send;
    I2c_ActionBurstRecv burst_recv;
} I2c_Action;

/**
 * @brief I2C driver state.
 */
typedef struct _I2c {
    /**
     * @brief Flag indicating whether or not the driver has been initialised.
     */
    bool initialised;

    /**
     * @brief Array of action types associated with each element in u_actions.
     */
    I2c_ActionType action_types[I2C_MAX_NUM_ACTIONS];

    /**
     * @brief Array of actions to be performed. 
     */
    I2c_Action u_actions[I2C_MAX_NUM_ACTIONS];
} I2c;

/* -------------------------------------------------------------------------   
 * GLOBALS
 * ------------------------------------------------------------------------- */

/**
 * @brief The global instance of the I2C driver state.
 */
extern I2c I2C;

/**
 * @brief The array of modules available on the TM4C. 
 */
extern I2c_Module I2C_MODULES[I2C_NUM_MODULES];

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Step a single send action.
 * 
 * @param p_action_in The action to perform
 * @return I2c_ErrorCode Return code.
 */
I2c_ErrorCode I2c_action_single_send(I2c_ActionSingleSend *p_action_in);

/**
 * @brief Step a single receive action.
 * 
 * @param p_action_in The action to perform
 * @return I2c_ErrorCode Return code.
 */
I2c_ErrorCode I2c_action_single_recv(I2c_ActionSingleRecv *p_action_in);

/**
 * @brief Step a burst send action.
 * 
 * @param p_action_in The action to perform
 * @return I2c_ErrorCode Return code.
 */
I2c_ErrorCode I2c_action_burst_send(I2c_ActionBurstSend *p_action_in);

/**
 * @brief Step a burst receive action.
 * 
 * @param p_action_in The action to perform
 * @return I2c_ErrorCode Return code.
 */
I2c_ErrorCode I2c_action_burst_recv(I2c_ActionBurstRecv *p_action_in);

/**
 * @brief Returns true if both devices have the same module and address.
 * 
 * @param p_a_in The first device
 * @param p_b_in The second device
 * @return bool True if the devices are the same, false otherwise.
 */
bool I2c_devices_equal(I2c_Device *p_a_in, I2c_Device *p_b_in);


#endif /* H_I2C_PRIVATE_H */