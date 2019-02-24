/**
 * @ingroup uos3-proto
 * @ingroup camera
 *
 * @file uos3-proto/camera.c
 * @brief Camera Hardware Driver - uos3-proto board
 *
 * @{
 */

#include "board.h"
#include "../uart.h"
#include "../delay.h"

#include "../camera.h"

bool Camera_capture(uint32_t page_size, void (*page_store)(uint8_t*,uint32_t))
{
  uint32_t i=0;
  uint8_t page_buffer[page_size];

  for(i=0; i<page_size; i++)
    page_store(page_buffer, i);

  return true;
}
