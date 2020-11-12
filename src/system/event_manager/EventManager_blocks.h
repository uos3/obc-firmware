/**
 * @ingroup event_manager
 * 
 * @file EventManager_blocks.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief This file defines the blocks for the events.
 * 
 * Event ID assignments are broken down into blocks in a similar manner to the
 * DataPool, that is a 7 bit block ID (128 possibilities) and a 9 bit event
 * index (512 possibilities). Each module is assigned one block to prevent
 * collisions in event IDs.
 * 
 * @version 0.1
 * @date 2020-11-12
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef H_EVENTMANAGER_BLOCKS_H
#define H_EVENTMANAGER_BLOCKS_H

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief Block assignment for the EventManager module.
 */
#define EVENTMANAGER_BLOCK_EVENTMANAGER (1)

/**
 * @brief Block assignment for the I2c module.
 */
#define EVENTMANAGER_BLOCK_I2C (2)

#endif /* H_EVENTMANAGER_BLOCKS_H */
