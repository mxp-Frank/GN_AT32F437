/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_Tpc112S4.h--->external use
 * @Brief :      ref from bsp_dac_tlv5614.h(AMP/AME svn324) Author: Huxian
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#ifndef _IF_MX25L128_H_
#define _IF_MX25L128_H_

#include "IF_PrjLib.h"
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
	
#define SPIF_BASE                      	 0
#define SPIF_PAGE_SIZE                   256
#define SPIF_SECTOR_SIZE                 4096
#define SPIF_CHIP_SIZE                   0x8000000

#define SPIF_SECTOR_ADDR(x)              ((uint32_t)(SPIF_BASE + SPIF_SECTOR_SIZE * (x)))  // x = 0 ~ 65536
 /******************************************************************************
 * Interface functions                                                         *
 ******************************************************************************/
extern void IF_SpiflashInit(void);
extern uint8_t IF_SPIFlash_CheckOK(void);
extern void IF_Spiflash_write(uint8_t *pbuffer, uint32_t write_addr, uint32_t length);
extern void IF_Spiflash_read(uint8_t *pbuffer, uint32_t read_addr, uint32_t length);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif

 /*
*******************************************************************************
* END
*******************************************************************************
*/

