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

void Packet_cw_transmit_buffer(uint8_t *cw_buffer, uint32_t cw_length, void _cw_on(void), void _cw_off(void))
{
	uint32_t i, j;
	uint8_t c;

	for(i=0; i<cw_length; i++)
	{
		c = cw_buffer[i];

		/* Move lower case to upper case */
		if((c >= 97) && (c <= 122))
		{
			c = (uint8_t)(c - 32);
		}

		/* Bounds check - move to next character if invalid */
		if((c < 32) || (c > 91))
		{
			continue;
		}

		/* Prepare for table lookup */
		c = (uint8_t)(c - 32);

		for(
			j = (uint32_t)(2 * (Packet_cw_lookup[c][0] - 1));
			true; 
			j -= 2
		)
		{
			switch((Packet_cw_lookup[c][1] >> j) & GCC_BINARY(0b11))
			{
				case GCC_BINARY(0b00):
					/* Pause (only used for space character) */
					_cw_off();
					Delay_ms(CW_PERIOD_MS);
					break;
				case GCC_BINARY(0b01):
					/* Dit */
					_cw_on();
					Delay_ms(CW_PERIOD_MS);
					break;
				case GCC_BINARY(0b11):
					/* Dah */
					_cw_on();
					Delay_ms(CW_PERIOD_MS);
					Delay_ms(CW_PERIOD_MS);
					Delay_ms(CW_PERIOD_MS);
					break;
				default:
					break;
			}
			_cw_off();
			/* Inter pulse pause */
			Delay_ms(CW_PERIOD_MS);
			if(j==0)
			{
				break;
			}
		}
		/* Additional inter letter pause */
		Delay_ms(CW_PERIOD_MS);
		Delay_ms(CW_PERIOD_MS);
	}
}

static const registerSetting_t preferredSettings_cw[]= 
{
   {CC112X_IOCFG3,         0x59},
   {CC112X_IOCFG2,         0x13},
   {CC112X_IOCFG1,         0xB0},
   {CC112X_IOCFG0,            0x06},
   {CC112X_SYNC_CFG1,         0x08},
   {CC112X_DEVIATION_M,       0xB4},
   {CC112X_MODCFG_DEV_E,      0x0A},
   {CC112X_DCFILT_CFG,        0x1C},
   {CC112X_PREAMBLE_CFG1,     0x00},
   {CC112X_FREQ_IF_CFG,       0x35},
   {CC112X_IQIC,              0xC6},
   {CC112X_CHAN_BW,           0x10},
   {CC112X_MDMCFG1,           0x06},
   {CC112X_MDMCFG0,           0x05},
   {CC112X_SYMBOL_RATE2,      0x40},
   {CC112X_SYMBOL_RATE1,      0x62},
   {CC112X_SYMBOL_RATE0,      0x4E},
   {CC112X_AGC_REF,           0x20},
   {CC112X_AGC_CS_THR,        0x19},
   {CC112X_AGC_CFG1,          0xA9},
   {CC112X_AGC_CFG0,          0xCF},
   {CC112X_FIFO_CFG,          0x00},
   {CC112X_SETTLING_CFG,      0x03},
   {CC112X_FS_CFG,            0x1B},
   {CC112X_PKT_CFG2,          0x05},
   {CC112X_PKT_CFG1,          0x00},
   {CC112X_PKT_CFG0,          0x20},
   {CC112X_PA_CFG2,           0x34},
   {CC112X_PA_CFG0,           0x7E},
   {CC112X_IF_MIX_CFG,        0x00},
   {CC112X_FREQOFF_CFG,       0x22},
   {CC112X_CFM_DATA_CFG,      0x01},
   {CC112X_FREQ2,             0x5A},
   {CC112X_FREQ1,             0xEF},
   {CC112X_FREQ0,             0xFF},
   {CC112X_IF_ADC0,           0x05},
   {CC112X_FS_DIG1,           0x00},
   {CC112X_FS_DIG0,           0x5F},
   {CC112X_FS_CAL0,           0x0E},
   {CC112X_FS_DIVTWO,         0x03},
   {CC112X_FS_DSM0,           0x33},
   {CC112X_FS_DVC0,           0x17},
   {CC112X_FS_PFD,            0x50},
   {CC112X_FS_PRE,            0x6E},
   {CC112X_FS_REG_DIV_CML,    0x14},
   {CC112X_FS_SPARE,          0xAC},
   {CC112X_XOSC5,             0x0E},
   {CC112X_XOSC3,             0xC7},
   {CC112X_XOSC1,             0x07},
   {CC112X_SERIAL_STATUS,     0x08},
};


static double freq = 145.5;
static double pwr = 10.0;

static uint8_t buffer[17] = "UOS3 UOS3 UOS3 k\0";
static uint32_t buffer_length = 16;

static void cw_tone_on(void)
{
  uint8_t pwr_reg;

  radio_reset_config(SPI_RADIO_TX, preferredSettings_cw, sizeof(preferredSettings_cw)/sizeof(registerSetting_t));
  manualCalibration(SPI_RADIO_TX);

  radio_set_freq_f(SPI_RADIO_TX, &freq);

  radio_set_pwr_f(SPI_RADIO_TX, &pwr, &pwr_reg);

  SPI_cmd(SPI_RADIO_TX, CC112X_STX);

  LED_on(LED_B);
}

static void cw_tone_off(void)
{
  radio_reset_config(SPI_RADIO_TX, preferredSettings_cw, sizeof(preferredSettings_cw)/sizeof(registerSetting_t));

  LED_off(LED_B);
}