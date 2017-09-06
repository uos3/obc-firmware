#include "board.h"
#include "../uart.h"

#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"

#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"

/* UART description struct */
typedef struct UART {
  uint32_t peripheral_gpio; // TI Driver GPIO Peripheral Reference
  uint32_t peripheral_uart; // TI Driver UART Peripheral Reference
  uint32_t base_gpio;       // TI Driver GPIO Base Reference
  uint32_t base_uart;       // TI Driver UART Base Reference
  uint32_t pin_rx_function; // TI Driver UART RX Pin Reference
  uint32_t pin_tx_function; // TI Driver UART TX Pin Reference
  uint8_t  pin_rx;          // TI Driver RX Pin Reference
  uint8_t  pin_tx;          // TI Driver TX Pin Reference
  bool     initialised;
} UART;

/* Array of enabled UARTs */
static UART UART_uarts[2] = 
  {
    { 
      SYSCTL_PERIPH_GPIOA,
      SYSCTL_PERIPH_UART0,
      GPIO_PORTA_BASE,
      UART0_BASE,
      GPIO_PA0_U0RX,
      GPIO_PA1_U0TX,
      GPIO_PIN_0,
      GPIO_PIN_1,
      false
    },
    { 
      SYSCTL_PERIPH_GPIOC,
      SYSCTL_PERIPH_UART3,
      GPIO_PORTC_BASE,
      UART3_BASE,
      GPIO_PC6_U3RX,
      GPIO_PC7_U3TX,
      GPIO_PIN_6,
      GPIO_PIN_7,
      false
    }
  };
#define NUMBER_OF_UARTS  ( sizeof(UART_uarts) / sizeof(UART) )

#define check_uart_num(x, y)  if(x >= NUMBER_OF_UARTS) { return y; }

/** Public Functions */

void UART_init(uint8_t uart_num, uint32_t baudrate)
{
  check_uart_num(uart_num,);
  UART *uart = &UART_uarts[uart_num];

  /* Check UART is initialised */
  if(!uart->initialised)
  {
    /* Initialise UART Peripheral if not already initialised */
    if(!SysCtlPeripheralReady(uart->peripheral_uart))
    {
      SysCtlPeripheralEnable(uart->peripheral_uart);
      while(!SysCtlPeripheralReady(uart->peripheral_uart)) { };
    }

    /* Initialise GPIO Peripheral if not already initialised */
    if(!SysCtlPeripheralReady(uart->peripheral_gpio))
    {
      SysCtlPeripheralEnable(uart->peripheral_gpio);
      while(!SysCtlPeripheralReady(uart->peripheral_gpio)) { };
    }

    GPIOPinConfigure(uart->pin_rx_function);
    GPIOPinConfigure(uart->pin_tx_function);
    GPIOPinTypeUART(uart->base_gpio, uart->pin_rx | uart->pin_tx);

    UARTClockSourceSet(uart->base_uart, UART_CLOCK_SYSTEM);

    uart->initialised = true;
  }

  UARTConfigSetExpClk(uart->base_uart, SysCtlClockGet(), baudrate,
      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
}

char UART_getc(uint8_t uart_num)
{
  check_uart_num(uart_num, '\0');
  UART *uart = &UART_uarts[uart_num];

  return (char)UARTCharGet(uart->base_uart);
}

void UART_putc(uint8_t uart_num, char c)
{
  check_uart_num(uart_num,);
  UART *uart = &UART_uarts[uart_num];

  UARTCharPut(uart->base_uart, c);
}

void UART_puts(uint8_t uart_num, char *str)
{
  while(*str != '\0')
  {
    UART_putc(uart_num, *str++);
  }
}

void UART_putb(uint8_t uart_num, char *str, uint32_t len)
{
  while(len--)
  {
    UART_putc(uart_num, *str++);
  }
}

bool UART_getc_nonblocking(uint8_t uart_num, char *c)
{
  check_uart_num(uart_num, false);
  UART *uart = &UART_uarts[uart_num];

  int32_t value = UARTCharGetNonBlocking(uart->base_uart);

  if(value >= 0)
  {
    *c = (char)value;
    return true;
  }
  else
  {
    return false;
  }
}

bool UART_putc_nonblocking(uint8_t uart_num, char c)
{
  check_uart_num(uart_num, false);
  UART *uart = &UART_uarts[uart_num];

  return UARTCharPutNonBlocking(uart->base_uart, c);
}

bool UART_puts_nonblocking(uint8_t uart_num, char *str)
{
  while(*str != '\0')
  {
    if(!UART_putc_nonblocking(uart_num, *str++))
    {
      return false;
    }
  }
  return true;
}

bool UART_putb_nonblocking(uint8_t uart_num, char *str, uint32_t len)
{
  while(len--)
  {
    if(!UART_putc_nonblocking(uart_num, *str++))
    {
      return false;
    }
  }
  return true;
}

bool UART_busy(uint8_t uart_num)
{
  check_uart_num(uart_num, false);
  UART *uart = &UART_uarts[uart_num];

  return UARTBusy(uart->base_uart);
}

bool UART_charsAvail(uint8_t uart_num)
{
  check_uart_num(uart_num, false);
  UART *uart = &UART_uarts[uart_num];

  return UARTCharsAvail(uart->base_uart);
}