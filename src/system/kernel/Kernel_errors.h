/**
 * @ingroup kernel
 * @file Kernel_errors.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Definition of the standard Event error code.
 * 
 * @version 0.1
 * @date 2020-11-30
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef H_KERNEL_ERRORS_H
#define H_KERNEL_ERRORS_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdint.h>

/* -------------------------------------------------------------------------   
 * TYPES
 * ------------------------------------------------------------------------- */

/**
 * @brief An error code that may be returned by any module.
 * 
 * An error code is a 16-bit unsigned value that can be returned by any module.
 * These codes respect the constant module ID codes contained in
 * Kernel_module_ids.h. Errors are defined at the module-level in a
 * Module_errors.h file. For an example of how to define errors see the
 * Kernel_errors.h file.
 * 
 * The convention is that driver functions, which don't access the DataPool, 
 * return an ErrorCode directly, while all other modules return a bool. If
 * false is returned an error occured and the module's DP.MODULE.ERROR_CODE
 * value is set accordingly.
 */
typedef uint16_t ErrorCode;

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief The no-error case.
 * 
 * This is a reserved value which shall never be occupied by any other error
 * code. It defines the case of no error - i.e. the success case.
 */
#define ERROR_NONE ((ErrorCode)0x00)

#endif /* H_KERNEL_ERRORS_H */