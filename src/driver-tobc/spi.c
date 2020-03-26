/**
 * @ingroup uos3-proto
 * @ingroup spi
 *
 * @file uos3-proto/spi.c
 * @brief SPI Driver - uos3-proto board
 *
 * @{
 */

#include "../driver/board.h"
#include "../driver/spi.h"
#include "../driver/gpio.h"

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
      5000000,
      false
    }
  };

/* SPI virtual port description struct */
typedef struct SPI_t {
  SPI_port *port;
  uint8_t  gpio_cs;           // CS GPIO Reference
  bool     wait_miso;         // Whether the SPI peripheral waits for SO low (required for cc1120)
} SPI_t;

/* Array of enabled SPIs */
static SPI_t SPI_spis[3] = 
  {
    { 
      &SPI_ports[0],
      GPIO_PA3,
      true
    },
    { 
      &SPI_ports[0],
      GPIO_PF3,
      true
    },
    { 
      &SPI_ports[0],
      GPIO_PD6,
      false
    }
  };

#define NUMBER_OF_SPIS  ( sizeof(SPI_spis) / sizeof(SPI_t) )

#define check_spi_num(x, y)  if(x >= NUMBER_OF_SPIS) { return y; }

static void SPI_init(const SPI_t* spi)
{
  /* Check Physical SPI is initialised */
  if(!spi->port->initialised)
  {
    /* Set CS[bar] high for all virtual ports */
    for(uint8_t i=0; i<NUMBER_OF_SPIS; i++)
    {
      GPIO_write(SPI_spis[i].gpio_cs, true);
    }

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
    uint32_t d;
    while(SSIDataGetNonBlocking(spi->port->base_spi, &d));

    spi->port->initialised = true;
  }
}

/** Public Functions */

uint8_t SPI_cmd(const uint8_t spi_num, const uint8_t cmd)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d;

  /* Pull CS_N low and wait for SO to go low before communication starts */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }

  /* Send Command */
  SSIDataPut(spi->port->base_spi, (uint32_t)cmd);
  SSIDataGet(spi->port->base_spi, &r);

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

uint8_t SPI_read8(const uint8_t spi_num, const uint8_t addr, uint8_t *data)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d;

  /* Pull CS_N low */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }

  /* Write address, read status byte */
  SSIDataPut(spi->port->base_spi, (uint32_t)(addr));
  SSIDataGet(spi->port->base_spi, &r);

  /* Read Data */
  SSIDataPut(spi->port->base_spi, 0);
  SSIDataGet(spi->port->base_spi, &d);
  *data = (uint8_t)d;

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

uint8_t SPI_burstread8(const uint8_t spi_num, const uint8_t addr, uint8_t *data, uint32_t len)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d;

  /* Pull CS_N low */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }

  /* Transmit address with Read Bit Set */
  SSIDataPut(spi->port->base_spi, (uint32_t)(addr));
  SSIDataGet(spi->port->base_spi, &r);

  while(len--)
  {
    SSIDataPut(spi->port->base_spi, 0);
    SSIDataGet(spi->port->base_spi, &d);
    *data++ = (uint8_t)d;
  }

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

uint8_t SPI_write8(const uint8_t spi_num, const uint8_t addr, const uint8_t *data)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d, w;

  /* Pull CS_N low */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }

  /* Write Address, read status byte */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr);
  SSIDataGet(spi->port->base_spi, &r);

  if(data!=NULL)
  {
    /* Write Data */
    w = *data;
    SSIDataPut(spi->port->base_spi, w);
    SSIDataGet(spi->port->base_spi, &d);
  }

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

uint8_t SPI_burstwrite8(const uint8_t spi_num, const uint8_t addr, uint8_t *data, uint32_t len)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d, w;

  /* Pull CS_N low */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }

  /* Write Address, read status byte */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr);
  SSIDataGet(spi->port->base_spi, &r);

  /* Write data */
  while(len--)
  {
    w = *data;
    SSIDataPut(spi->port->base_spi, w);
    SSIDataGet(spi->port->base_spi, &d);
    data++;
  }

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

uint8_t SPI_read16(const uint8_t spi_num, const uint16_t addr, uint8_t *data)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d;

  uint8_t addr_msb  = (uint8_t)(addr >> 8);
  uint8_t addr_lsb = (uint8_t)(addr & 0x00FF);

  /* Pull CS_N low */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }
 
  /* Transmit MSB address */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_msb);
  SSIDataGet(spi->port->base_spi, &r);

  /* Transmit LSB address */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_lsb);
  SSIDataGet(spi->port->base_spi, &d);

  /* Read Result */
  SSIDataPut(spi->port->base_spi, 0);
  SSIDataGet(spi->port->base_spi, &d);
  *data = (uint8_t)d;

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

uint8_t SPI_burstread16(const uint8_t spi_num, const uint16_t addr, uint8_t *data, uint32_t len)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d;

  uint8_t addr_msb  = (uint8_t)(addr >> 8);
  uint8_t addr_lsb = (uint8_t)(addr & 0x00FF);

  /* Pull CS_N low */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }

  /* Transmit MSB address */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_msb);
  SSIDataGet(spi->port->base_spi, &r);

  /* Transmit LSB address */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_lsb);
  SSIDataGet(spi->port->base_spi, &d);

  while(len--)
  {
    SSIDataPut(spi->port->base_spi, 0);
    SSIDataGet(spi->port->base_spi, &d);
    *data++ = (uint8_t)d;
  }

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

uint8_t SPI_write16(const uint8_t spi_num, const uint16_t addr, const uint8_t *data)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d, w;

  uint8_t addr_msb  = (uint8_t)(addr >> 8);
  uint8_t addr_lsb = (uint8_t)(addr & 0x00FF);

  /* Pull CS_N low */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }

  /* Transmit MSB address */
  SSIDataPut(spi->port->base_spi, (uint32_t)(0x7F & addr_msb));
  SSIDataGet(spi->port->base_spi, &r);

  /* Transmit LSB address */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_lsb);
  SSIDataGet(spi->port->base_spi, &d);

  w = *data;
  SSIDataPut(spi->port->base_spi, w);
  SSIDataGet(spi->port->base_spi, &d);

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

uint8_t SPI_burstwrite16(const uint8_t spi_num, const uint16_t addr, uint8_t *data, uint32_t len)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d, w;

  uint8_t addr_msb  = (uint8_t)(addr >> 8);
  uint8_t addr_lsb = (uint8_t)(addr & 0x00FF);

  /* Pull CS_N low */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }

  /* Transmit MSB address */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_msb);
  SSIDataGet(spi->port->base_spi, &r);

  /* Transmit LSB address */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_lsb);
  SSIDataGet(spi->port->base_spi, &d);

  while(len--)
  {
    w = *(data++);
    SSIDataPut(spi->port->base_spi, w);
    SSIDataGet(spi->port->base_spi, &d);
  }

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

uint8_t SPI_read32(const uint8_t spi_num, const uint32_t addr, uint8_t *data)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d;

  uint8_t addr_mmsb  = (uint8_t)(addr >> 24);
  uint8_t addr_msb  = (uint8_t)(addr >> 16);
  uint8_t addr_lsb  = (uint8_t)(addr >> 8);
  uint8_t addr_llsb = (uint8_t)(addr & 0xFF);

  /* Pull CS_N low */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }
 
  /* Transmit MSB address, reading back status byte */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_mmsb);
  SSIDataGet(spi->port->base_spi, &r);

  /* Send less significant address bytes */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_msb);
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_lsb);
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_llsb);

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* Flush RX FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* Read Data */
  SSIDataPut(spi->port->base_spi, 0);
  SSIDataGet(spi->port->base_spi, &d);
  *data = (uint8_t)d;

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

uint8_t SPI_burstread32(const uint8_t spi_num, const uint32_t addr, uint8_t *data, uint32_t len)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d;

  uint8_t addr_mmsb  = (uint8_t)(addr >> 24);
  uint8_t addr_msb  = (uint8_t)(addr >> 16);
  uint8_t addr_lsb  = (uint8_t)(addr >> 8);
  uint8_t addr_llsb = (uint8_t)(addr & 0xFF);

  /* Pull CS_N low */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }

  /* Transmit MSB address, reading back status byte */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_mmsb);
  SSIDataGet(spi->port->base_spi, &r);

  /* Send less significant address bytes */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_msb);
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_lsb);
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_llsb);

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* Flush RX FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  while(len--)
  {
    SSIDataPut(spi->port->base_spi, 0);
    SSIDataGet(spi->port->base_spi, &d);
    *data++ = (uint8_t)d;
  }

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

uint8_t SPI_write32(const uint8_t spi_num, const uint32_t addr, const uint8_t *data)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d, w;

  uint8_t addr_mmsb  = (uint8_t)(addr >> 24);
  uint8_t addr_msb  = (uint8_t)(addr >> 16);
  uint8_t addr_lsb  = (uint8_t)(addr >> 8);
  uint8_t addr_llsb = (uint8_t)(addr & 0xFF);

  /* Pull CS_N low */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }

  /* Transmit MSB address, reading back status byte */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_mmsb);
  SSIDataGet(spi->port->base_spi, &r);

  /* Send less significant address bytes */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_msb);
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_lsb);
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_llsb);

  w = *data;
  SSIDataPut(spi->port->base_spi, w);

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

uint8_t SPI_burstwrite32(const uint8_t spi_num, const uint32_t addr, uint8_t *data, uint32_t len)
{
  check_spi_num(spi_num, 0);
  SPI_t *spi = &SPI_spis[spi_num];

  SPI_init(spi);

  uint32_t r, d, w;

  uint8_t addr_mmsb  = (uint8_t)(addr >> 24);
  uint8_t addr_msb  = (uint8_t)(addr >> 16);
  uint8_t addr_lsb  = (uint8_t)(addr >> 8);
  uint8_t addr_llsb = (uint8_t)(addr & 0xFF);

  /* Pull CS_N low */
  GPIO_write(spi->gpio_cs, false);

  /* Flush Input FIFO */
  while(SSIDataGetNonBlocking(spi->port->base_spi, &d)) {};

  /* If configured, wait for MISO to go low (indicates clock stability for cc1120) */
  if(spi->wait_miso)
  {
    while(GPIOPinRead(spi->port->base_gpio, spi->port->pin_miso) & spi->port->pin_miso) {};
  }

  /* Transmit MSB address, reading back status byte */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_mmsb);
  SSIDataGet(spi->port->base_spi, &r);

  /* Send less significant address bytes */
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_msb);
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_lsb);
  SSIDataPut(spi->port->base_spi, (uint32_t)addr_llsb);

  /* Put bytes into TX FIFO */
  while(len--)
  {
    w = *(data++);
    SSIDataPut(spi->port->base_spi, w);
  }

  /* Wait for TX FIFO to be exhausted */
  while(SSIBusy(spi->port->base_spi)) {};

  /* De-assert CS (high) */
  GPIO_write(spi->gpio_cs, true);

  /* return the status byte value */
  return (uint8_t)r;
}

/**
 * @}
 */