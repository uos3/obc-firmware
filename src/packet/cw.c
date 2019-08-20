/**
 * @ingroup cw
 *
 * @file cw.c
 *
 * @{
 */


#include "../firmware.h"

#define CW_PERIOD_MS	100

/* Each element: length, pattern (2-bit, pause: 00, dit: 01, dah: 11) */
/* Length Guide: dit: 1, dah: 3, symbol space: 1, letter space: 3, word space: 7 */
static const uint16_t Packet_cw_lookup[59][2] = 
{
	// 32	word space
	{ 4, GCC_BINARY(0b00000000) }, // (7 - 3)
	// 33 !	exclamation mark
	{ 6, GCC_BINARY(0b110111011111) },	// − · − · − − 
	// 34	"	double quote
	{ 6, GCC_BINARY(0b011101011101) },	// · − · · − · 
	// 35	#	hash
	{ 0, 0},
	// 36	$	dollar
	{ 7, GCC_BINARY(0b01010111010111) }, // · · · − · · − 
	// 37	%	percent
	{ 0, 0},
	// 38	&	ampersand
	{ 0, 0},
	// 39	'	apostrophe
	{ 6, GCC_BINARY(0b011111111101) }, // · − − − − · 
	// 40 (	left parenthesis
	{ 5, GCC_BINARY(0b1101111101) }, // − · − − · 
	// 41 )	right parenthesis
	{ 6, GCC_BINARY(0b110111110111) }, // − · − − · − 
	// 42 * asterisk
	{ 0, 0},
	// 43 + plus
	{ 5, GCC_BINARY(0b0111011101) }, // · − · − · 
	// 44 , comma
	{ 6, GCC_BINARY(0b111101011111) }, // − − · · − − 
	// 45 - minus
	{ 6, GCC_BINARY(0b110101010111) }, // − · · · · − 
	// 46 . period
	{ 6, GCC_BINARY(0b011101110111) }, // · − · − · − 
	// 47 / slash
	{ 5, GCC_BINARY(0b1101011101) }, // − · · − · 
	// 48 0	zero
	{ 5, GCC_BINARY(0b1111111111) }, // − − − − − 
	// 49 1 one
	{ 5, GCC_BINARY(0b0111111111) }, // · − − − − 
	// 50 2 two
	{ 5, GCC_BINARY(0b0101111111) }, // · · − − − 
	// 51 3 three
	{ 5, GCC_BINARY(0b0101011111) }, // · · · − −  
	// 52 4 four
	{ 5, GCC_BINARY(0b0101010111) }, // · · · · − 
	// 53 5 five
	{ 5, GCC_BINARY(0b0101010101) }, // · · · · ·  
	// 54 6 six
	{ 5, GCC_BINARY(0b1101010101) }, // − · · · · 
	// 55 7 seven
	{ 5, GCC_BINARY(0b1111010101) }, // − − · · · 
	// 56 8 eight
	{ 5, GCC_BINARY(0b1111110101) }, // − − − · · 
	// 57 9 nine
	{ 5, GCC_BINARY(0b1111111101) }, // − − − − · 
	// 58 : colon
	{ 6, GCC_BINARY(0b111111010101) }, // − − − · · ·
	// 59 ; semicolon
	{ 6, GCC_BINARY(0b110111011101) }, // − · − · − · 
	// 60 < less than
	{ 0, 0},
	// 61 = equals
	{ 5, GCC_BINARY(0b1101010111) }, // − · · · − 
	// 62 > greater than
	{ 0, 0},
	// 63 ? question mark
	{ 6, GCC_BINARY(0b010111110101) }, // · · − − · · 
	// 64 @ at sign
	{ 6, GCC_BINARY(0b011111011101) }, // · − − · − · 
	// 65 A
	{ 2, GCC_BINARY(0b0111) }, // · −
	// 66 B
	{ 4, GCC_BINARY(0b11010101) }, // − · · ·
	// 67 C
	{ 4, GCC_BINARY(0b11011101) }, // − · − · 
	// 68 D
	{ 3, GCC_BINARY(0b110101) }, // − · · 
	// 69 E
	{ 1, GCC_BINARY(0b01) }, // · 
	// 70 F
	{ 4, GCC_BINARY(0b01011101) }, // · · − · 
	// 71 G
	{ 3, GCC_BINARY(0b111101) }, // − − · 
	// 72 H
	{ 4, GCC_BINARY(0b01010101) }, // · · · · 
	// 73 I
	{ 2, GCC_BINARY(0b0101) }, // · · 
	// 74 J
	{ 4, GCC_BINARY(0b01111111) }, // · − − − 
	// 75 K
	{ 3, GCC_BINARY(0b110111) }, // − · − 
	// 76 L
	{ 4, GCC_BINARY(0b01110101) }, // · − · · 
	// 77 M
	{ 2, GCC_BINARY(0b1111) }, // − − 
	// 78 N
	{ 2, GCC_BINARY(0b1101) }, // − · 
	// 79 O
	{ 3, GCC_BINARY(0b111111) }, // − − − 
	// 80 P
	{ 4, GCC_BINARY(0b01111101) }, // · − − · 
	// 81 Q
	{ 4, GCC_BINARY(0b11110111) }, // − − · − 
	// 82 R
	{ 3, GCC_BINARY(0b011101) }, // · − · 
	// 83 S
	{ 3, GCC_BINARY(0b010101) }, // · · · 
	// 84 T
	{ 1, GCC_BINARY(0b11) }, // − 
	// 85 U
	{ 3, GCC_BINARY(0b010111) }, // · · − 
	// 86 V
	{ 4, GCC_BINARY(0b01010111) }, // · · · − 
	// 87 W
	{ 3, GCC_BINARY(0b011111) }, // · − − 
	// 88 X
	{ 4, GCC_BINARY(0b11010111) }, // − · · −  
	// 89 Y
	{ 4, GCC_BINARY(0b11011111) }, // − · − −   
	// 90 Z
	{ 4, GCC_BINARY(0b11110101) } // − − · · 
};


void Packet_cw_transmit_buffer(uint8_t *cw_buffer, uint32_t cw_length, radio_config_t *Radio_config, void _cw_on(radio_config_t *), void _cw_off(void))
{
	uint32_t i, j;
	uint8_t c;
  	char output[100];

	for(i=0; i<cw_length; i++)
	{
		c = cw_buffer[i];
		sprintf(output,"Sending Character :\"%c\"\r\n", c);
    	UART_puts(UART_INTERFACE, output);

		/* Move lower case to upper case */
		if((c >= 97) && (c <= 122))
		{
			c = (uint8_t)(c - 32);
		}

		/* Bounds check - move to next character if invalid */
		if((c < 32) || (c > 91))
		{
			UART_puts(UART_INTERFACE, "Bounds check failed");
			continue;
		}

		/* Prepare for table lookup */
		c = (uint8_t)(c - 32);

		for(
			j = (uint32_t)(2 * (Packet_cw_lookup[c][0]));
			j>0; 
			j -= 2
		)
		{

			switch((Packet_cw_lookup[c][1] >> (j-2)) & GCC_BINARY(0b11))
			{
				case GCC_BINARY(0b00):
					UART_puts(UART_INTERFACE, "Space\r\n");
					/* Pause (only used for space character) */
					_cw_off();
					Delay_ms(CW_PERIOD_MS);
					break;
				case GCC_BINARY(0b01):
					UART_puts(UART_INTERFACE, "Dit\r\n");
					/* Dit */
					_cw_on(Radio_config);
					Delay_ms(CW_PERIOD_MS);
					break;
				case GCC_BINARY(0b11):
					/* Dah */
					UART_puts(UART_INTERFACE, "Dah\r\n");
					_cw_on(Radio_config);
					Delay_ms(CW_PERIOD_MS);
					Delay_ms(CW_PERIOD_MS);
					Delay_ms(CW_PERIOD_MS);
					break;
				default:
					UART_puts(UART_INTERFACE, "Lookup failed\r\n");
					break;
			}
			UART_puts(UART_INTERFACE, "Switch Complete\r\n");

			_cw_off();
			UART_puts(UART_INTERFACE, "Dot/dash sent\r\n");

			/* Inter pulse pause */
			Delay_ms(CW_PERIOD_MS);
			// if(j==0)
			// {
			// 	break;
			// }
		}
		/* Additional inter letter pause */
		Delay_ms(CW_PERIOD_MS);
		Delay_ms(CW_PERIOD_MS);
	}
}