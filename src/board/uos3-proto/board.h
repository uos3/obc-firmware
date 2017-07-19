/* This file contains any board-specific constants that need to be passed into the driver functions */

#ifndef __BOARD_UOS3_PROTO_H__
#define __BOARD_UOS3_PROTO_H__


#define LED_A	0x0
#define LED_B	0x1


#define UART_PC104_HEADER	0x0
#define UART_CAM_HEADER		0x1

#define RADIO_SPI_PORT		GPIO_PORTF_BASE
#define RADIO_MISO_PIN		GPIO_PIN_0
#define RADIO_MOSI_PIN		GPIO_PIN_1
#define RADIO_CLK_PIN		GPIO_PIN_2


#define RADIO_TX_CS_PORT	GPIO_PORTF_BASE
#define RADIO_TX_CS_PIN		GPIO_PIN_3
#define RADIO_RX_CS_PORT	GPIO_PORTA_BASE
#define RADIO_RX_CS_PIN		GPIO_PIN_3
#define RADIO_RX_GP0_PORT	GPIO_PORTB_BASE
#define RADIO_RX_GP0_PIN	GPIO_PIN_7
#define RADIO_TX_GP0_PORT	GPIO_PORTA_BASE
#define RADIO_TX_GP0_PIN	GPIO_PIN_2

#define RADIO_TX 0
#define RADIO_RX 1

#define GPIO_PA0	0x00
#define GPIO_PA1	0x01
#define GPIO_PA2	0x02
#define GPIO_PA3	0x03
#define GPIO_PA4	0x04
#define GPIO_PA5	0x05
#define GPIO_PA6	0x06
#define GPIO_PA7	0x07

#define GPIO_PB0	0x08
#define GPIO_PB1	0x09
#define GPIO_PB2	0x0A
#define GPIO_PB3	0x0B
#define GPIO_PB4	0x0C
#define GPIO_PB5	0x0D
#define GPIO_PB6	0x0E
#define GPIO_PB7	0x0F

#define GPIO_PC0	0x10
#define GPIO_PC1	0x11
#define GPIO_PC2	0x12
#define GPIO_PC3	0x13
#define GPIO_PC4	0x14
#define GPIO_PC5	0x15
#define GPIO_PC6	0x16
#define GPIO_PC7	0x17

#define GPIO_PD0	0x18
#define GPIO_PD1	0x19
#define GPIO_PD2	0x1A
#define GPIO_PD3	0x1B
#define GPIO_PD4	0x1C
#define GPIO_PD5	0x1D
#define GPIO_PD6	0x1E
#define GPIO_PD7	0x1F

#define GPIO_PE0	0x20
#define GPIO_PE1	0x21
#define GPIO_PE2	0x22
#define GPIO_PE3	0x23
#define GPIO_PE4	0x24
#define GPIO_PE5	0x25
#define GPIO_PE6	0x26
#define GPIO_PE7	0x27

#define GPIO_PF0	0x28
#define GPIO_PF1	0x29
#define GPIO_PF2	0x2A
#define GPIO_PF3	0x2B
#define GPIO_PF4	0x2C
#define GPIO_PF5	0x2D
#define GPIO_PF6	0x2E
#define GPIO_PF7	0x2F

#endif /*  __BOARD_UOS3_PROTO_H__ */