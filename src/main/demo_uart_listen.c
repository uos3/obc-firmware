#include "../utility/debug.h"
#include "../driver/uart.h"

#define output_size 128

int main(){
	debug_init();
	char got_char = 0;
	char output[output_size];
	debug_print("=== uart read demo ====\r\npress a key");

	// got_char = UART_getc(UART_INTERFACE);
	// debug_printf("got char: %c: %d", got_char, (uint8_t) got_char);

	debug_get_command(output, output_size);

	debug_printf("successfully got command: %s", output);

	debug_end();
}

