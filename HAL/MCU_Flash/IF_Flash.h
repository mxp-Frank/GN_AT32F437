/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_Flash.h--->external use
 * @Brief :      ref from bsp_motor.c(AMP/AME svn324) Author: Huxian
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#ifndef _IF_FLASH_H_
#define _IF_FLASH_H_

#include "IF_PrjLib.h"

#include "at32f435_437_flash.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#define FLASH_SIZE                        0x100000

#define FLASH_SECTOR_SIZE                 2048

#define FLASH_SECTOR_ADDR(x)              ((uint32_t)(FLASH_BASE + FLASH_SECTOR_SIZE * (x)))  // x = 0 ~ 255


// /* @函数声明 *******************/
// status_t mflash_drv_init(void);
// status_t mflash_drv_erase(uint32_t destAddr, uint32_t size);
// status_t mflash_drv_write(uint32_t destAddr, uint8_t* data, uint32_t size);
// void  mflash_drv_read(uint32_t destAddr, uint8_t* data,uint32_t size);


#define SECTOR_SIZE     2048   /* this parameter depends on the specific model of the chip */


/******************************************************************************
* Interface functions                                                         *
******************************************************************************/
extern void IF_FlashInit(void);
extern void IF_Flash_ReadByte(uint32_t read_addr, uint8_t *p_buffer, uint16_t num_read);
extern error_status IF_Flash_SectorErase(uint32_t sectorAddr);
extern void IF_Flash_ReadWord(uint32_t read_addr, uint16_t *p_buffer, uint16_t num_read);
extern error_status IF_Flash_write(uint32_t write_addr,uint16_t *p_Buffer, uint16_t num_write);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif

 /*
*******************************************************************************
* END
*******************************************************************************
*/

