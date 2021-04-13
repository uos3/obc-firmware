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

/**
 * @brief Represents an error that can exist as a part of an error chain.
 * 
 * This self-referential struct allows errors to be chained together to
 * represent the entire flow of errors that can occur. For instance, an error
 * in the UART driver may cause an error in the Eps component, which may cause
 * an error in the Power application. This chain would be represented by three
 * instances of `Error`, all of which are kept in the datapool:
 * ```
 *  // Note that since UART doesn't have a datapool the component itself must 
 *  // keep an error instance. The code of this should be set to the return 
 *  // code of the Uart_x functions.
 *  DP.EPS.UART_ERROR = Error {
 *      code = UART_ERROR_UDMA_FAULT // Dummy example error
 *      p_cause = NULL // This is the root cause, so there's no cause for it
 *  }
 *  DP.EPS.ERROR = Error {
 *      code = EPS_ERROR_TC_SEND_ERROR // Tells us what part of EPS broke
 *      p_cause = &DP.EPS.UART_ERROR // Points to the uart error
 *  }
 *  DP.POWER.ERROR = Error {
 *      code = POWER_ERROR_LOW_POWER_CHECK_FAULT // Tells us what high level 
 *                                               // process broke
 *      p_cause = &DP.EPS.ERROR // Points to the component's error
 *  }
 * ```
 * 
 * This chain can be serialised down into a simple ordered list of error codes
 * (and therefore bytes) using the Kernel_error_to_bytes() function.
 */
typedef struct _Error {
    /**
     * @brief The descriptive error code associated with this error.
     */
    ErrorCode code;

    /**
     * @brief Pointer to the cause of this error.
     * 
     * If this value is NULL it is considered the root cause.
     */
    struct _Error *p_cause;
} Error;

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