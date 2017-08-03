#include "../spi.h"

#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"

#include "driverlib/gpio.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"

/* SPI description struct */
typedef struct SPI {
  uint32_t peripheral_gpio;   // TI Driver GPIO Peripheral Reference
  uint32_t peripheral_spi;    // TI Driver SPI Peripheral Reference
  uint32_t base_gpio;         // TI Driver GPIO Base Reference
  uint32_t base_spi;          // TI Driver SPI Base Reference
  uint32_t pin_clk_function;  // TI Driver SPI CLK Pin Reference
  uint32_t pin_miso_function; // TI Driver SPI RX Pin Reference
  uint32_t pin_mosi_function; // TI Driver SPI TX Pin Reference
  uint8_t  pin_clk;           // TI Driver CLK Pin Reference
  uint8_t  pin_miso;          // TI Driver RX Pin Reference
  uint8_t  pin_mosi;          // TI Driver TX Pin Reference
  uint8_t  gpio_cs;           // CS GPIO Reference
  uint8_t  spi_id;            // number X in SPIX
  bool     initialised;
} SPI;

/* Array of enabled SPIs */
static SPI SPI_spis[2] = 
  {
    { 
      SYSCTL_PERIPH_GPIOF,
      SYSCTL_PERIPH_SSI1,
      GPIO_PORTF_BASE,
      SSI1_BASE,
      GPIO_PF2_S1CLK, /* name guessed */
      GPIO_PF0_S1MISO,
      GPIO_PF1_S1MOSI,
      GPIO_PIN_2,
      GPIO_PIN_0,
      GPIO_PIN_1,
      GPIO_PA3,
      0,
      false
    },
    { 
      SYSCTL_PERIPH_GPIOF,
      SYSCTL_PERIPH_SSI1,
      GPIO_PORTF_BASE,
      SSI1_BASE,
      GPIO_PF2_S1CLK, /* name guessed */
      GPIO_PF0_S1MISO,
      GPIO_PF1_S1MOSI,
      GPIO_PIN_2,
      GPIO_PIN_0,
      GPIO_PIN_1,
      GPIO_PF3,
      1,
      false
    }
  };

#define NUMBER_OF_SPIS  ( sizeof(SPI_spis) / sizeof(SPI) )

#define check_spi_num(x, y)  if(x >= NUMBER_OF_SPIS) { return y; }

/** Public Functions */

void SPI_init(uint8_t spi_num, uint32_t clockrate)
{
  check_uart_num(uart_num,);
  UART *uart = &UART_uarts[uart_num];

  /* Check SPI is initialised */
  if(!spi->initialised)
  {
    /* Initialise SPI Peripheral if not already initialised */
    if(!SysCtlPeripheralReady(spi->peripheral_spi))
    {
      SysCtlPeripheralEnable(spi->peripheral_spi);
      while(!SysCtlPeripheralReady(spi->peripheral_spi)) { };
    }

    /* Initialise GPIO Peripheral if not already initialised */
    if(!SysCtlPeripheralReady(spi->peripheral_gpio))
    {
      SysCtlPeripheralEnable(spi->peripheral_gpio);
      while(!SysCtlPeripheralReady(spi->peripheral_gpio)) { };
    }

    if(spi->base_gpio == GPIO_PORTF_BASE)
    {
      // sort out PF0...
      HWREG(GPIO_PORTF_BASE+GPIO_O_LOCK) = GPIO_LOCK_KEY; //Unlock 
      HWREG(GPIO_PORTF_BASE+GPIO_O_CR) |= 0x01; // Enable PF0 AFS 
      HWREG(GPIO_PORTF_BASE+GPIO_O_LOCK) = 0; // Relock
    }

    /* Configure Pins for SPI function */
    GPIOPinConfigure(spi->pin_clk_function);
    GPIOPinConfigure(spi->pin_rx_function);
    GPIOPinConfigure(spi->pin_tx_function);

    /* Assign pins to SPI peripheral */
    GPIOPinTypeSSI(spi->base_gpio, spi->pin_clk | spi->pin_rx | spi->pin_tx);
    SSIClockSourceSet(spi->base_spi, SSI_CLOCK_SYSTEM);

    /* Set CS[bar] pin high to disable bus for now */
    GPIO_write(spi->gpio_cs, true);
  
    SSIEnable(SSI1_BASE);

    spi->initialised = true;
  }

  /* Set peripheral clockrate */
  SSIConfigSetExpClk(spi->base_spi, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER, clockrate, 8);

  /* Clear current pending data */
  uint32_t d;
  while(SSIDataGetNonBlocking(spi->base_spi, &d));
}

uint8_t SPI_read8(uint8_t spi_num, uint8_t addrByte, uint8_t *pData, uint16_t len)
{
  check_spi_num(spi_num, 0);
  SPI *spi = &SPI_spis[spi_num];

  uint32_t readValue;

  /* Pull CS_N low and wait for SO to go low before communication starts */
  GPIO_write(spi->gpio_cs, false);
  //while(TRXEM_PORT_IN & TRXEM_SPI_MISO_PIN);   <-- do we need this..?
  while(GPIOPinRead(spi->base_gpio, spi->pin_miso) & spi->pin_miso){};
  // do we need to flush the rx buffer first?
  ssi_flush();
 
  while(SSIBusy(spi->base_spi));
  /* Transmit address with Read Bit Set */
  SSIDataPut(spi->base_spi, (uint32_t)(0x80|addrByte));
  while(SSIBusy(spi->base_spi));
  SSIDataGet(spi->base_spi, &readValue);
  trxReadWriteBurstSingle(accessType|addrByte,pData,len); 

  GPIO_write(spi->gpio_cs, true);
  /* return the status byte value */
  return((uint8_t)readValue);
}

uint8_t SPI_write8(uint8_t spi_num, uint8_t addrByte, uint8_t *pData, uint16_t len)
{
  check_spi_num(spi_num, 0);
  SPI *spi = &SPI_spis[spi_num];

  uint32_t readValue;

  /* Pull CS_N low and wait for SO to go low before communication starts */
  GPIO_write(spi->gpio_cs, false);
  //while(TRXEM_PORT_IN & TRXEM_SPI_MISO_PIN);   <-- do we need this..?
  while(GPIOPinRead(spi->base_gpio, spi->pin_miso) & spi->pin_miso){};
  // do we need to flush the rx buffer first?
  ssi_flush();
 
  while(SSIBusy(spi->base_spi));
  /* Transmit address without Read Bit */
  SSIDataPut(spi->base_spi, (uint32_t)(0x7F & addrByte));
  while(SSIBusy(spi->base_spi));
  SSIDataGet(spi->base_spi, &readValue);
  trxReadWriteBurstSingle(accessType|addrByte,pData,len); 

  GPIO_write(spi->gpio_cs, true);
  /* return the status byte value */
  return((uint8_t)readValue);
}