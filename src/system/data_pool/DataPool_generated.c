/**
 * TODO
 */

#include <stdint.h>
#include <stdbool.h>

#include "system/data_pool/DataPool_public.h"

unsigned long hash_djb2(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash & 0xFFFFFFFF;
}

DataPoolId DataPool_get_id_from_symbol(char *p_symbol_in) {
    printf("Symbol hash: %ld\n", hash_djb2(p_symbol_in));
    switch (hash_djb2(p_symbol_in)) {
    
    /* INITIALISED */
    case 2074812084:
        return 1;
    

    /* EVENTMANAGER_NUM_RAISED_EVENTS */
    case 1953791708:
        return 2;
    
    default:
        return 0;
    }
}


bool DataPool_get_bool(DataPoolId id_in, bool *p_value_out) {
    switch (id_in) {
    
    /* INITIALISED */
    case 1:
        *p_value_out = DP.INITIALISED;
        break;
    
    default:
        return false;
    }

    return true;
}


bool DataPool_get_uint8_t(DataPoolId id_in, uint8_t *p_value_out) {
    switch (id_in) {
    
    /* EVENTMANAGER_NUM_RAISED_EVENTS */
    case 2:
        *p_value_out = DP.EVENTMANAGER_NUM_RAISED_EVENTS;
        break;
    
    default:
        return false;
    }

    return true;
}



bool DataPool_set_bool(DataPoolId id_in, bool value_in) {
    switch (id_in) {
    
    case 1:
        DP.INITIALISED = value_in;
        break;
    
    default:
        return false;
    }

    return true;
}


bool DataPool_set_uint8_t(DataPoolId id_in, uint8_t value_in) {
    switch (id_in) {
    
    case 2:
        DP.EVENTMANAGER_NUM_RAISED_EVENTS = value_in;
        break;
    
    default:
        return false;
    }

    return true;
}

