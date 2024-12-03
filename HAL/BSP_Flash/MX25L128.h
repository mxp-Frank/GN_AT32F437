/**
  **************************************************************************
  * @file     spi_flash.h
  * @brief    header file of spi_flash
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

#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H
#include "configDevice.h"
#include "at32f435_437.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#define  FLASH_CS_PORT				GPIOG
#define  FLASH_CS_PIN				SPI4_CS_Pin129_PG14

#define  FLASH_RESET_PORT			GPIOD
#define  FLASH_RESET_PIN			Flash_Reset_Pin117_PD3

#define  FLASH_WP_PORT				GPIOG
#define  FLASH_WP_PIN				Flash_WP_Pin125_PG10

#define  FLASH_CLK_PORT				GPIOG
#define  FLASH_CLK_PIN				SPI4_SCK_Pin126_PG11
#define  FLASH_CLK_SOURCE			GPIO_PINS_SOURCE11
#define  FLASH_CLK_MUX				GPIO_MUX_6

#define  FLASH_MISO_PORT			GPIOG
#define  FLASH_MISO_PIN				SPI4_MISO_Pin127_PG12
#define  FLASH_MISO_SOURCE			GPIO_PINS_SOURCE12								 
#define  FLASH_MISO_MUX				GPIO_MUX_6

#define  FLASH_MOSI_PORT			GPIOG
#define  FLASH_MOSI_PIN				SPI4_MOSI_Pin128_PG13
#define  FLASH_MOSI_SOURCE			GPIO_PINS_SOURCE13
#define  FLASH_MOSI_MUX				GPIO_MUX_6	
	
#define FLASH_CS_HIGH()             gpio_bits_set(FLASH_CS_PORT,   FLASH_CS_PIN)
#define FLASH_CS_LOW()              gpio_bits_reset(FLASH_CS_PORT, FLASH_CS_PIN)
#define SPIF_CHIP_SIZE                   0x8000000
#define SPIF_SECTOR_SIZE                 4096
#define SPIF_PAGE_SIZE                   256
//MX25X系列/L系列芯片列表
#define MX25L80 						0XC213
#define MX25L16 						0XC214
#define MX25L32 						0XC215
#define MX25L64 						0XC216
#define MX25L128						0XC217
#define MX25L256 						0XC218


//W25X系列/Q系列芯片列表
#define W25Q80 							0XEF13
#define W25Q16 							0XEF14
#define W25Q32 							0XEF15
#define W25Q64 							0XEF16
#define W25Q128							0XEF17
#define W25Q256 						0XEF18
#define W25Q01JV                        0XEF20
//指令表
#define SPIF_WRITEENABLE                 0x06
#define SPIF_WRITEDISABLE                0x04
/* s7-s0 */
#define SPIF_READSTATUSREG1              0x05
#define SPIF_WRITESTATUSREG1             0x01
/* s15-s8 */
#define SPIF_READSTATUSREG2              0x35
#define SPIF_WRITESTATUSREG2             0x31
/* s23-s16 */
#define SPIF_READSTATUSREG3              0x15
#define SPIF_WRITESTATUSREG3             0x11
#define SPIF_READDATA                    0x03
#define SPIF_FASTREADDATA                0x0B
#define SPIF_FASTREADDUAL                0x3B
#define SPIF_PAGEPROGRAM                 0x02
/* block size:64kb */
#define SPIF_BLOCKERASE                  0xD8
#define SPIF_SECTORERASE                 0x20
#define SPIF_CHIPERASE                   0xC7
#define SPIF_POWERDOWN                   0xB9
#define SPIF_RELEASEPOWERDOWN            0xAB
#define SPIF_DEVICEID                    0xAB
#define SPIF_MANUFACTDEVICEID            0x90
#define SPIF_JEDECDEVICEID               0x9F
#define FLASH_SPI_DUMMY_BYTE             0xFF

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif

