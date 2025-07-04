/**
  **************************************************************************
  * @file     spi_flash.c
  * @brief    spi_flash source code
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

#include "IF_MX25L128.h"
#include "MX25L128.h"
#include "IF_GPIO.h"
#include "IF_Tmr.h"
/** @addtogroup AT32F437_periph_examples
  * @{
  */

/** @addtogroup 437_SPI_w25q_flash
  * @{
  */
/* CONST & MACROS */

/* DATA STRUCTURES */

/* LOCAL VARIABLES */
static uint8_t IsSpiFlash = 0;
/* GLOBAL VARIABLES */

/* FUNCTION PROTOTYPES */
uint8_t spiflash_sector_buf[SPIF_SECTOR_SIZE];
static void spiflash_init(void);
static void spiflash_write(uint8_t *pbuffer, uint32_t write_addr, uint32_t length);
static void spiflash_read(uint8_t *pbuffer, uint32_t read_addr, uint32_t length);
static void spiflash_sector_erase(uint32_t erase_addr);
static void spiflash_write_nocheck(uint8_t *pbuffer, uint32_t write_addr, uint32_t length);
static void spiflash_page_write(uint8_t *pbuffer, uint32_t write_addr, uint32_t length);
static void spi_bytes_write(uint8_t *pbuffer, uint32_t length);
static void spi_bytes_read(uint8_t *pbuffer, uint32_t length);
static void spiflash_wait_busy(void);
static uint8_t spiflash_read_sr1(void);
static void spiflash_write_enable(void);
static uint16_t spiflash_read_id(void);
static uint8_t spi_byte_write(uint8_t data);
static uint8_t spi_byte_read(void);

/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/
void IF_SpiflashInit(void)
{
	spiflash_init();
	IsSpiFlash = IF_SPIFlash_CheckOK();
}	

uint8_t IF_SPIFlash_CheckOK(void)
{
	uint8_t value = 0;
	uint32_t spiflash_id = 0;
	spiflash_id = spiflash_read_id();   //读取flash_id值
	value =(spiflash_id == W25Q01JV)? 1:0; 				
	return value;
}

void IF_Spiflash_write(uint8_t *pbuffer, uint32_t write_addr, uint32_t length)
{
	if(IsSpiFlash)
	{
		spiflash_write(pbuffer, write_addr, length);
	}
	
}
void IF_Spiflash_read(uint8_t *pbuffer, uint32_t read_addr, uint32_t length)
{
	if(IsSpiFlash)
	{
		spiflash_read(pbuffer,read_addr,length);
	}
}

/**
  * @brief  spi configuration.
  * @param  none
  * @retval none
  */
static void spiflash_init(void)
{
  gpio_init_type gpio_initstructure;
  spi_init_type spi_init_struct;

  /* software cs, pd0 as a general io to control flash cs */
  gpio_initstructure.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_pull           = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode           = GPIO_MODE_OUTPUT;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;

  gpio_initstructure.gpio_pins = 0;
  gpio_initstructure.gpio_pins = FLASH_CS_PIN;
  gpio_init(FLASH_CS_PORT, &gpio_initstructure);

  gpio_initstructure.gpio_pins = 0;
  gpio_initstructure.gpio_pins = FLASH_RESET_PIN;
  gpio_init(FLASH_RESET_PORT, &gpio_initstructure);

  gpio_initstructure.gpio_pins = 0;
  gpio_initstructure.gpio_pins = FLASH_WP_PIN;
  gpio_init(FLASH_WP_PORT, &gpio_initstructure);
  
  //设置默认值 CS =1 RST = 1 WP = 1
  IF_OutPutGpio_Write(FLASH_CS_PORT, FLASH_CS_PIN,   1, LOGIC_POUT);
  IF_OutPutGpio_Write(FLASH_RESET_PORT, FLASH_RESET_PIN, 1, LOGIC_POUT);
  IF_OutPutGpio_Write(FLASH_WP_PORT, FLASH_WP_PIN,   1, LOGIC_POUT);
  
  /* sck,miso,mosi */
  gpio_initstructure.gpio_pull  = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode  = GPIO_MODE_MUX;
  gpio_initstructure.gpio_pins  = FLASH_CLK_PIN;
  gpio_init(FLASH_CLK_PORT, &gpio_initstructure);
  gpio_pin_mux_config(FLASH_CLK_PORT, FLASH_CLK_SOURCE, FLASH_CLK_MUX);
  
  gpio_initstructure.gpio_pull  = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode  = GPIO_MODE_MUX;
  gpio_init(FLASH_CLK_PORT, &gpio_initstructure);
  gpio_initstructure.gpio_pins  = FLASH_MISO_PIN;
  gpio_init(FLASH_MISO_PORT, &gpio_initstructure);
  gpio_pin_mux_config(FLASH_MISO_PORT, FLASH_MISO_SOURCE, FLASH_MISO_MUX);
  
  gpio_initstructure.gpio_pull  = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode  = GPIO_MODE_MUX;
  gpio_init(FLASH_CLK_PORT, &gpio_initstructure);
  gpio_initstructure.gpio_pins  = FLASH_MOSI_PIN;
  gpio_init(FLASH_MOSI_PORT, &gpio_initstructure);
  gpio_pin_mux_config(FLASH_MOSI_PORT,FLASH_MOSI_SOURCE, FLASH_MOSI_MUX);

  crm_periph_clock_enable(CRM_SPI4_PERIPH_CLOCK, TRUE);
  
  spi_default_para_init(&spi_init_struct);
  spi_init_struct.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;
  spi_init_struct.master_slave_mode = SPI_MODE_MASTER;
  spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_32;
  spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
  spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
  spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_HIGH;
  spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE;
  spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
  spi_init(SPI4, &spi_init_struct);
  spi_enable(SPI4, TRUE);
  
  spi_byte_write(0xff);

}

/**
  * @brief  write data to flash
  * @param  pbuffer: the pointer for data buffer
  * @param  write_addr: the address where the data is written
  * @param  length: buffer length
  * @retval none
  */
static void spiflash_write(uint8_t *pbuffer, uint32_t write_addr, uint32_t length)
{
  uint32_t sector_pos;
  uint16_t sector_offset;
  uint16_t sector_remain;
  uint16_t index;
  uint8_t *spiflash_buf;
  spiflash_buf = spiflash_sector_buf;

  /* sector address */
  sector_pos = write_addr / SPIF_SECTOR_SIZE;

  /* address offset in a sector */
  sector_offset = write_addr % SPIF_SECTOR_SIZE;

  /* the remain in a sector */
  sector_remain = SPIF_SECTOR_SIZE - sector_offset;
  if(length <= sector_remain)
  {
    /* smaller than a sector size */
    sector_remain = length;
  }
  while(1)
  {
    /* read a sector */
    spiflash_read(spiflash_buf, sector_pos * SPIF_SECTOR_SIZE, SPIF_SECTOR_SIZE);

    /* validate the read erea */
    for(index = 0; index < sector_remain; index++)
    {
      if(spiflash_buf[sector_offset + index] != 0xFF)
      {
        /* there are some data not equal 0xff, so this secotr needs erased */
        break;
      }
    }
    if(index < sector_remain)
    {
      /* erase the sector */
      spiflash_sector_erase(sector_pos);

      /* copy the write data */
      for(index = 0; index < sector_remain; index++)
      {
        spiflash_buf[index + sector_offset] = pbuffer[index];
      }
      spiflash_write_nocheck(spiflash_buf, sector_pos * SPIF_SECTOR_SIZE, SPIF_SECTOR_SIZE); /* program the sector */
    }
    else
    {
      /* write directly in the erased area */
      spiflash_write_nocheck(pbuffer, write_addr, sector_remain);
    }
    if(length == sector_remain)
    {
      /* write end */
      break;
    }
    else
    {
      /* go on writing */
      sector_pos++;
      sector_offset = 0;

      pbuffer += sector_remain;
      write_addr += sector_remain;
      length -= sector_remain;
      if(length > SPIF_SECTOR_SIZE)
      {
        /* could not write the remain data in the next sector */
        sector_remain = SPIF_SECTOR_SIZE;
      }
      else
      {
        /* could write the remain data in the next sector */
        sector_remain = length;
      }
    }
  }
}

/**
  * @brief  read data from flash
  * @param  pbuffer: the pointer for data buffer
  * @param  read_addr: the address where the data is read
  * @param  length: buffer length
  * @retval none
  */
static void spiflash_read(uint8_t *pbuffer, uint32_t read_addr, uint32_t length)
{
  FLASH_CS_LOW();
  spi_byte_write(SPIF_READDATA); /* send instruction */
  spi_byte_write((uint8_t)((read_addr) >> 16)); /* send 24-bit address */
  spi_byte_write((uint8_t)((read_addr) >> 8));
  spi_byte_write((uint8_t)read_addr);
  spi_bytes_read(pbuffer, length);
  FLASH_CS_HIGH();
}

/**
  * @brief  erase a sector data
  * @param  erase_addr: sector address to erase
  * @retval none
  */
static void spiflash_sector_erase(uint32_t erase_addr)
{
  erase_addr *= SPIF_SECTOR_SIZE; /* translate sector address to byte address */
  spiflash_write_enable();
  spiflash_wait_busy();
  FLASH_CS_LOW();
  spi_byte_write(SPIF_SECTORERASE);
  spi_byte_write((uint8_t)((erase_addr) >> 16));
  spi_byte_write((uint8_t)((erase_addr) >> 8));
  spi_byte_write((uint8_t)erase_addr);
  FLASH_CS_HIGH();
  spiflash_wait_busy();
}

/**
  * @brief  write data without check
  * @param  pbuffer: the pointer for data buffer
  * @param  write_addr: the address where the data is written
  * @param  length: buffer length
  * @retval none
  */
static void spiflash_write_nocheck(uint8_t *pbuffer, uint32_t write_addr, uint32_t length)
{
  uint16_t page_remain;

  /* remain bytes in a page */
  page_remain = SPIF_PAGE_SIZE - write_addr % SPIF_PAGE_SIZE;
  if(length <= page_remain)
  {
    /* smaller than a page size */
    page_remain = length;
  }
  while(1)
  {
    spiflash_page_write(pbuffer, write_addr, page_remain);
    if(length == page_remain)
    {
      /* all data are programmed */
      break;
    }
    else
    {
      /* length > page_remain */
      pbuffer += page_remain;
      write_addr += page_remain;

      /* the remain bytes to be prorammed */
      length -= page_remain;
      if(length > SPIF_PAGE_SIZE)
      {
        /* can be progrmmed a page at a time */
        page_remain = SPIF_PAGE_SIZE;
      }
      else
      {
        /* smaller than a page size */
        page_remain = length;
      }
    }
  }
}

/**
  * @brief  write a page data
  * @param  pbuffer: the pointer for data buffer
  * @param  write_addr: the address where the data is written
  * @param  length: buffer length
  * @retval none
  */
static void spiflash_page_write(uint8_t *pbuffer, uint32_t write_addr, uint32_t length)
{
  if((0 < length) && (length <= SPIF_PAGE_SIZE))
  {
    /* set write enable */
    spiflash_write_enable();

    FLASH_CS_LOW();

    /* send instruction */
    spi_byte_write(SPIF_PAGEPROGRAM);

    /* send 24-bit address */
    spi_byte_write((uint8_t)((write_addr) >> 16));
    spi_byte_write((uint8_t)((write_addr) >> 8));
    spi_byte_write((uint8_t)write_addr);
    spi_bytes_write(pbuffer,length);

    FLASH_CS_HIGH();

    /* wait for program end */
    spiflash_wait_busy();
  }
}

/**
  * @brief  write data continuously
  * @param  pbuffer: the pointer for data buffer
  * @param  length: buffer length
  * @retval none
  */
static void spi_bytes_write(uint8_t *pbuffer, uint32_t length)
{
  volatile uint8_t dummy_data;
  while(length--)
  {
    while(spi_i2s_flag_get(SPI4, SPI_I2S_TDBE_FLAG) == RESET);
    spi_i2s_data_transmit(SPI4, *pbuffer);
    while(spi_i2s_flag_get(SPI4, SPI_I2S_RDBF_FLAG) == RESET);
    dummy_data = spi_i2s_data_receive(SPI4);
    pbuffer++;
  }
}

/**
  * @brief  read data continuously
  * @param  pbuffer: buffer to save data
  * @param  length: buffer length
  * @retval none
  */
static void spi_bytes_read(uint8_t *pbuffer, uint32_t length)
{
  uint8_t write_value = FLASH_SPI_DUMMY_BYTE;
  while(length--)
  {
    while(spi_i2s_flag_get(SPI4, SPI_I2S_TDBE_FLAG) == RESET);
    spi_i2s_data_transmit(SPI4, write_value);
    while(spi_i2s_flag_get(SPI4, SPI_I2S_RDBF_FLAG) == RESET);
    *pbuffer = spi_i2s_data_receive(SPI4);
    pbuffer++;
  }
}

/**
  * @brief  wait program done
  * @param  none
  * @retval none
  */
static void spiflash_wait_busy(void)
{
  while((spiflash_read_sr1() & 0x01) == 0x01);
}

/**
  * @brief  read sr1 register
  * @param  none
  * @retval none
  */
static uint8_t spiflash_read_sr1(void)
{
  uint8_t breadbyte = 0;
  FLASH_CS_LOW();
  spi_byte_write(SPIF_READSTATUSREG1);
  breadbyte = (uint8_t)spi_byte_read();
  FLASH_CS_HIGH();
  return (breadbyte);
}

/**
  * @brief  enable write operation
  * @param  none
  * @retval none
  */
static void spiflash_write_enable(void)
{
  FLASH_CS_LOW();
  spi_byte_write(SPIF_WRITEENABLE);
  FLASH_CS_HIGH();
}

/**
  * @brief  read device id
  * @param  none
  * @retval device id
  */
static uint16_t spiflash_read_id(void)
{
  uint16_t wreceivedata = 0;
  FLASH_CS_LOW();
  spi_byte_write(SPIF_MANUFACTDEVICEID);
  spi_byte_write(0x00);
  spi_byte_write(0x00);
  spi_byte_write(0x00);
  wreceivedata |= spi_byte_read() << 8;
  wreceivedata |= spi_byte_read();
  FLASH_CS_HIGH();
  return wreceivedata;
}

/**
  * @brief  write a byte to flash
  * @param  data: data to write
  * @retval flash return data
  */
static uint8_t spi_byte_write(uint8_t data)
{
  uint8_t brxbuff;
  spi_i2s_data_transmit(SPI4, data);
  while(spi_i2s_flag_get(SPI4, SPI_I2S_RDBF_FLAG) == RESET);
  brxbuff = spi_i2s_data_receive(SPI4);
  while(spi_i2s_flag_get(SPI4, SPI_I2S_BF_FLAG) != RESET);
  return brxbuff;
}

/**
  * @brief  read a byte to flash
  * @param  none
  * @retval flash return data
  */
static uint8_t spi_byte_read(void)
{
  return (spi_byte_write(FLASH_SPI_DUMMY_BYTE));
}

/**
  * @}
  */

/**
  * @}
  */

