/**
 * @defgroup camera
 *
 * @brief Camera Hardware Driver
 *
 * @details Camera Hardware Driver.
 *
 * @ingroup drivers
 */

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <stdint.h>
#include <stdbool.h>
#include "../configuration/configuration.h"
/**
 * @function
 * @ingroup camera
 *
 * Captures an image from the camera, saving it one page at a time,
 * using the supplied function.
 *
 * @parameter uint32_t size of each page
 * @parameter Pointer to function to save each page
 * @returns Whether or not the image capture was successful
 */
enum print_to_debug{no, yes};           //enum for estimation whether print the picture data to debug uart or not

bool Camera_capture(uint32_t *picture_size, uint16_t* no_of_pages, uint8_t deb_uart_num,enum print_to_debug if_print, image_acquisition_profile_t resolution);

#endif /*  __CAMERA_H__ */