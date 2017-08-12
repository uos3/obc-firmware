#include "board.h"
#include "../spi.h"
#include "../gpio.h"

#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"

#include "driverlib/gpio.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"

/* Note that this module currently has no locking of the underlying physical port */

/* SPI physical port description struct */
typedef struct SPI_port {
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
  uint32_t clockrate;         // Clockrate in bps
  bool     initialised;
} SPI_port;

static SPI_port SPI_ports[1] = 
  {
    { 
      SYSCTL_PERIPH_GPIOF,
      SYSCTL_PERIPH_SSI1,
      GPIO_PORTF_BASE,
      SSI1_BASE,
      GPIO_PF2_SSI1CLK,
      GPIO_PF0_SSI1RX,
      GPIO_PF1_SSI1TX,
      GPIO_PIN_2,
      GPIO_PIN_0,
      GPIO_PIN_1,
      1000000,
      false
    }
  };

/* SPI virtual port description struct */
typedef struct SPI {
  SPI_port *port;
  uint8_t  gpio_cs;           // CS GPIO Reference
  bool     initialised;
} SPI;

/* Array of enabled SPIs */
static SPI SPI_spis[2] = 
  {
    { 
      &SPI_ports[0],
      GPIO_PA3,
      false
    },
    { 
      &SPI_ports[0],
      GPIO_PF3,
      false
    }
  };

#define NUMBER_OF_SPIS  ( sizeof(SPI_spis) / sizeof(SPI) )

#define check_spi_num(x, y)  if(x >= NUMBER_OF_SPIS) { return y; }

static void SPI_flush(uint32_t base_spi)
{
  uint32_t d;
  while(SSIDataGetNonBlocking(base_spi, &d));
}

/** Public Functions */

void SPI_init(uint8_t spi_num)
{
  check_spi_num(spi_num,);
  SPI *spi = &SPI_spis[spi_num];

  /* Check Virtual SPI is initialised */
  if(!spi->initialised)
  {
    /* Set CS[bar] pin high to disable virtual port for now */
    GPIO_write(spi->gpio_cs, true);

    /* Check Physical SPI is initialised */
    if(!spi->port->initialised)
    {
      /* Initialise SPI Peripheral if not already initialised */
      if(!SysCtlPeripheralReady(spi->port->peripheral_spi))
      {
        SysCtlPeripheralEnable(spi->port->peripheral_spi);
        while(!SysCtlPeripheralReady(spi->port->peripheral_spi)) { };
      }

      /* Initialise GPIO Peripheral if not already initialised */
      if(!SysCtlPeripheralReady(spi->port->peripheral_gpio))
      {
        SysCtlPeripheralEnable(spi->port->peripheral_gpio);
        while(!SysCtlPeripheralReady(spi->port->peripheral_gpio)) { };
      }

      if(spi->port->base_gpio == GPIO_PORTF_BASE)
      {
        // sort out PF0...
        HWREG(GPIO_PORTF_BASE+GPIO_O_LOCK) = GPIO_LOCK_KEY; //Unlock 
        HWREG(GPIO_PORTF_BASE+GPIO_O_CR) |= 0x01; // Enable PF0 AFS 
        HWREG(GPIO_PORTF_BASE+GPIO_O_LOCK) = 0; // Relock
      }

      /* Configure Pins for SPI function */
      GPIOPinConfigure(spi->port->pin_clk_function);
      GPIOPinConfigure(spi->port->pin_miso_function);
      GPIOPinConfigure(spi->port->pin_mosi_function);

      /* Assign pins to SPI peripheral */
      GPIOPinTypeSSI(spi->port->base_gpio, spi->port->pin_clk | spi->port->pin_miso | spi->port->pin_mosi);
      SSIClockSourceSet(spi->port->base_spi, SSI_CLOCK_SYSTEM);

      /* Set peripheral clockrate */
      SSIConfigSetExpClk(spi->port->base_spi, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
                         SSI_MODE_MASTER, spi->port->clockrate, 8);
  
      SSIEnable(spi->port->base_spi);

      /* Clear current pending data */
      SPI_flush(spi->port->base_spi);

      spi->port->initialised = true;
    }

    spi->initialised = true;
  }
}

uint8_t SPI_read8(uint8_t spi_num, uint8_t addrByte, uint8_t *pData)
{
  check_spi_num(spi_num, 0);
  SPI *spi = &SPI_spis[spi_num];

  uint32_t readValue, d;

  /* Pull CS_N low and wait for SO to go low before communication starts */
  GPIO_write(spi->gpio_cs, false);
  //while(TRXEM_PORT_IN & TRXEM_SPI_MISO_PIN);   <-- do we need this..?
  while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso){};
  // do we need to flush the rx buffer first?
  SPI_flush(spi->port->base_spi);
 
  while(SSIBusy(spi->port->base_spi));
  /* Transmit address with Read Bit Set */
  SSIDataPut(spi->port->base_spi, (uint32_t)(0x80|addrByte));
  while(SSIBusy(spi->port->base_spi));
  SSIDataGet(spi->port->base_spi, &readValue);

  SSIDataPut(spi->port->base_spi, 0);
  while(SSIBusy(spi->port->base_spi)){};
  SSIDataGet(spi->port->base_spi, &d);
  *pData = (uint8_t)d;

  GPIO_write(spi->gpio_cs, true);
  /* return the status byte value */
  return((uint8_t)readValue);
}

uint8_t SPI_burstread8(uint8_t spi_num, uint8_t addrByte, uint8_t *pData, uint16_t len)
{
  check_spi_num(spi_num, 0);
  SPI *spi = &SPI_spis[spi_num];

  uint32_t readValue, d;

  /* Pull CS_N low and wait for SO to go low before communication starts */
  GPIO_write(spi->gpio_cs, false);
  //while(TRXEM_PORT_IN & TRXEM_SPI_MISO_PIN);   <-- do we need this..?
  while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso){};
  // do we need to flush the rx buffer first?
  SPI_flush(spi->port->base_spi);
 
  while(SSIBusy(spi->port->base_spi));
  /* Transmit address with Read Bit Set */
  SSIDataPut(spi->port->base_spi, (uint32_t)(0x80|addrByte));
  while(SSIBusy(spi->port->base_spi));
  SSIDataGet(spi->port->base_spi, &readValue);

  for(uint32_t i = 0; i < len; i++)
  {
    SSIDataPut(spi->port->base_spi, 0);
    while(SSIBusy(spi->port->base_spi));
    SSIDataGet(spi->port->base_spi, &d); //(uint32_t *)pData);
    *pData++ = (uint8_t)d;
  }

  GPIO_write(spi->gpio_cs, true);
  /* return the status byte value */
  return((uint8_t)readValue);
}

uint8_t SPI_write8(uint8_t spi_num, uint8_t addrByte, uint8_t *pData)
{
  check_spi_num(spi_num, 0);
  SPI *spi = &SPI_spis[spi_num];

  uint32_t readValue, d;

  /* Pull CS_N low and wait for SO to go low before communication starts */
  GPIO_write(spi->gpio_cs, false);
  //while(TRXEM_PORT_IN & TRXEM_SPI_MISO_PIN);   <-- do we need this..?
  while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso){};
  // do we need to flush the rx buffer first?
  SPI_flush(spi->port->base_spi);
 
  while(SSIBusy(spi->port->base_spi));
  /* Transmit address without Read Bit */
  SSIDataPut(spi->port->base_spi, (uint32_t)(0x7F & addrByte));
  while(SSIBusy(spi->port->base_spi));
  SSIDataGet(spi->port->base_spi, &readValue);


  SSIDataPut(spi->port->base_spi, (uint32_t)*pData);
  while(SSIBusy(spi->port->base_spi)) {};
  SSIDataGet(spi->port->base_spi, &d);

  GPIO_write(spi->gpio_cs, true);
  /* return the status byte value */
  return((uint8_t)readValue);
}

uint8_t SPI_burstwrite8(uint8_t spi_num, uint8_t addrByte, uint8_t *pData, uint16_t len)
{
  check_spi_num(spi_num, 0);
  SPI *spi = &SPI_spis[spi_num];

  uint32_t readValue, d;

  /* Pull CS_N low and wait for SO to go low before communication starts */
  GPIO_write(spi->gpio_cs, false);
  //while(TRXEM_PORT_IN & TRXEM_SPI_MISO_PIN);   <-- do we need this..?
  while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso){};
  // do we need to flush the rx buffer first?
  SPI_flush(spi->port->base_spi);
 
  while(SSIBusy(spi->port->base_spi));
  /* Transmit address without Read Bit */
  SSIDataPut(spi->port->base_spi, (uint32_t)(0x7F & addrByte));
  while(SSIBusy(spi->port->base_spi));
  SSIDataGet(spi->port->base_spi, &readValue);

  /* Communicate len number of bytes: if TX - the procedure doesn't overwrite pData */
  for (uint32_t i = 0; i < len; i++)
  {
    SSIDataPut(spi->port->base_spi, (uint32_t)*pData);
    while(SSIBusy(spi->port->base_spi)) {};
    SSIDataGet(spi->port->base_spi, &d);
    pData++;
  }

  GPIO_write(spi->gpio_cs, true);
  /* return the status byte value */
  return((uint8_t)readValue);
}

uint8_t SPI_cmdstrobe(uint8_t spi_num, uint8_t cmd, uint32_t *rc)
{
  check_spi_num(spi_num, 0);
  SPI *spi = &SPI_spis[spi_num];

  uint32_t readValue, d;

  /* Pull CS_N low and wait for SO to go low before communication starts */
  GPIO_write(spi->gpio_cs, false);

  while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso){};
  // do we need to flush the rx buffer first?
 
  while(SSIBusy(spi->port->base_spi));
  SSIDataPut(spi->port->base_spi, cmd);
  while(SSIBusy(spi->port->base_spi));
  SSIDataGet(spi->port->base_spi, rc);

  GPIO_write(spi->gpio_cs, true);
  /* return the status byte value */
  return((uint8_t)readValue);
}