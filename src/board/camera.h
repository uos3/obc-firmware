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

typedef enum {
	image_acquisition_profile_1600x1200,
	image_acquisition_profile_1280x960,
	image_acquisition_profile_800x600,
	image_acquisition_profile_640x480,
	//image_acquisition_profile_320x240,
	//image_acquisition_profile_160x120
} image_acquisition_profile_t;

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
bool Camera_capture(uint32_t *picture_size, uint16_t* no_of_pages, image_acquisition_profile_t resolution /*, uint8_t* slot_counter*/);

#endif /*  __CAMERA_H__ */
