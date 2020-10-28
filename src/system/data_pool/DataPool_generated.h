/**
 * TODO
 */
#ifndef H_DATAPOOL_STRUCT_H
#define H_DATAPOOL_STRUCT_H

#include <stdint.h>
#include <stdbool.h>

typedef struct _DataPool {

    /**
     * @brief Indicates whether or not the DataPool has been initialised.
     *
     * 
     */
    bool INITIALISED;

    

    /**
     * @brief Contains the number of raised events in the EventManager.
     *
     * 
     */
    uint8_t EVENTMANAGER_NUM_RAISED_EVENTS;

    
} DataPool;

typedef uint16_t DataPoolId;

DataPoolId DataPool_get_id_from_symbol(char *p_symbol_in);


/**
 * @brief Get the value in the DataPool associated with the given ID.
 *
 * @param id_in The ID of the DataPool member.
 * @param p_value_out Pointer to the location to store the retrieved value.
 * @return true The retrieval was successful.
 * @return false Invalid ID.
 */
bool DataPool_get_bool(DataPoolId id_in, bool *p_value_out);


/**
 * @brief Get the value in the DataPool associated with the given ID.
 *
 * @param id_in The ID of the DataPool member.
 * @param p_value_out Pointer to the location to store the retrieved value.
 * @return true The retrieval was successful.
 * @return false Invalid ID.
 */
bool DataPool_get_uint8_t(DataPoolId id_in, uint8_t *p_value_out);



/**
 * @brief Set the value in the DataPool associated with the given ID.
 *
 * @param id_in The ID of the DataPool member.
 * @param value_in The value to store in the DataPool.
 * @return true The operation completed successfully.
 * @return false Invalid ID.
 */
bool DataPool_set_bool(DataPoolId id_in, bool value_in);


/**
 * @brief Set the value in the DataPool associated with the given ID.
 *
 * @param id_in The ID of the DataPool member.
 * @param value_in The value to store in the DataPool.
 * @return true The operation completed successfully.
 * @return false Invalid ID.
 */
bool DataPool_set_uint8_t(DataPoolId id_in, uint8_t value_in);


#endif
