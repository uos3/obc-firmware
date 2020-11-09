/**
 * @ingroup event_manager
 * 
 * @file EventManager_private.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Provides private functions for the EventManager
 * 
 * @version 0.1
 * @date 2020-11-09
 * 
 * @copyright Copyright (c) UoS3 2020
 */

#ifndef H_EVENTMANAGER_PRIVATE_H
#define H_EVENTMANAGER_PRIVATE_H

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief If the current number of raised events is less than 1/4 the allocated 
 * list size the lists shall be shrunk by 1/2.
 * 
 * @return true The lists were successfully shrunk.
 * @return false An error occured.
 */
bool EventManager_shrink_lists(void);

#endif /* H_EVENTMANAGER_PRIVATE_H */