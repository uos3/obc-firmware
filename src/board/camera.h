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

bool Camera_capture(uint32_t page_size, uint32_t *image_length, uint32_t *number_of_pages, uint8_t *total_data);
void camera_data_print(uint32_t number_of_pages, uint32_t page_size);

#endif /*  __CAMERA_H__ */