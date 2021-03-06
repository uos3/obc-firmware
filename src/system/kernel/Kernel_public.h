/**
 * @file Kernel_public.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Public definition for the Kernel.
 * 
 * Task ref: [UT_2.9.12]
 * 
 * The kernel provides simple functions related to the general system, for
 * example disabling and enabling interrupts.
 * 
 * @version 0.1
 * @date 2021-02-10
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_KERNEL_PUBLIC_H
#define H_KERNEL_PUBLIC_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard includes */
#include <stdint.h>

/* Internal includes */
#include "system/kernel/Kernel_app_ids.h"
#include "system/kernel/Kernel_module_ids.h"
#include "system/kernel/Kernel_errors.h"

/* -------------------------------------------------------------------------   
 * TYPES
 * ------------------------------------------------------------------------- */

/**
 * @brief The type used for Application IDs.
 *
 * This type must be large enough to store the highest value of any module ID. 
 */
typedef uint8_t Kernel_AppId;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Disable interrupts on the system.
 * 
 * This can be used in combination with Kernel_enable_interrupts() to create a
 * critical section, i.e. one in which interrupts will not occur. 
 * 
 * CAUTION: Using this function without subsequently calling
 * Kernel_enable_interrupts() will result in failure of the system!
 */
void Kernel_disable_interrupts(void);

/**
 * @brief Enable interrupts on the system.
 * 
 * This can be used in combination with Kernel_disable_interrupts() to create a
 * critical section, i.e. one in which interrupts will not occur. 
 * 
 * CAUTION: This function must be called after calling
 * Kernel_disable_interrupts()!. 
 */
void Kernel_enable_interrupts(void);

/**
 * @brief Initialise the critical modules of the system.
 * 
 * This function is infallible as any failure in this initialisation process
 * prevents ANY activity of the software, therefore dealing with these failures
 * is reserved for this function itself.
 */
void Kernel_init_critical_modules(void);

/**
 * @brief Reboot the MCU.
 * 
 * DANGER: USE THIS ONLY WHEN YOU KNOW WHAT YOU'RE DOING!!!
 * 
 * Nominally whether or not to reboot is determined by the FDIR module. This
 * function should never be called outside of either FDIR or Kernel itself.
 */
void Kernel_reboot(void);

/**
 * @brief Serialises the given error into a series of bytes.
 * 
 * This will produce a big endian byte array for transmission as a part of the
 * telemetry system.
 * 
 * @param p_error_in The highest level error in the chain to serialise
 * @param p_bytes_out Pointer to an array of bytes to store the serialized
 * data. 
 * @param p_length_out Pointer to the length of the data filled into 
 * p_bytes_out.
 */
void Kernel_error_to_bytes(
    Error *p_error_in, 
    uint8_t *p_bytes_out, 
    uint8_t *p_length_out
);

/**
 * @brief Clear the error chain described by the given error.
 * 
 * This will reset all errors in the chain to ERROR_NONE and clear their cause
 * pointers. 
 * 
 * @param p_error_in The highest level error in the chain to clear.
 */
void Kernel_clear_error_chain(
    Error *p_error_in
);

#ifdef DEBUG_MODE
/**
 * @brief Writes the error chain to the given character string.
 * 
 * The user must allocate adequate (i.e. will be >64 bytes) space for the
 * string. 
 * 
 * @param p_error_in The highest level error in the chain to serialise
 * @param p_str_out Pointer to an array of characters to write into
 */
void Kernel_error_to_string(Error *p_error_in, char *p_str_out);
#endif

#endif /* H_KERNEL_PUBLIC_H */