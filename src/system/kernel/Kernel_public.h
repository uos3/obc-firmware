/**
 * @file Kernel_public.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Public definition for the Kernel.
 * 
 * TODO:
 * Task ref: []
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


#endif /* H_KERNEL_PUBLIC_H */