/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

/* Internal Includes */
#include "util/debug/Debug_public.h"
#include "system/event_manager/EventManager_public.h"

typedef struct _Spi_Module {
    uint32_t peripheral_gpio;
    uint32_t peripheral_ssi;
} Spi_Module;
