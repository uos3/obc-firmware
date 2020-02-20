#include <stdio.h>

#include "../driver/watchdog_ext.h"
#include "../driver/wdt.h"
#include "../utility/debug.h"
#include "../driver/board.h"

#include "../mission/packet_base.h"

char output[256];

int main(){
    Board_init();
    enable_watchdog_kick();
    watchdog_update = 0xFF;

    debug_init();
    debug_print("=== Packet base demo ===");

    Packet mypacket;
    mypacket.length=0x01;

    sprintf(output, "packet length %u", mypacket.length);
    debug_print(output);

    while(1){
        watchdog_update = 0xFF;
    }
}