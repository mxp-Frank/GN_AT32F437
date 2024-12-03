/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_At24c256.h--->external use
 * @Brief :      ref from bsp_eeprom.h(AMP/AME svn324) Author: Huxian
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

 #ifndef _IF_AT24C256_H_
 #define _IF_AT24C256_H_

 #include "IF_PrjLib.h"

#define EEPROM_DEV_ADDR			   0xA0	    		/* 24xx256的设备地址 */
#define EEPROM_PAGE_SIZE		   64		  	  	/* 24xx256的页面大小 */
#define EEPROM_SIZE				   32768			/* 24xx256总容量 */

 /******************************************************************************
 * Interface functions                                                         *
 ******************************************************************************/
extern void IF_At24c256Init(void);
extern uint8_t IF_At24c256_CheckOk(void);
extern uint8_t IF_At24c256_ReadBytes(uint16_t address, uint8_t *readBuf, uint16_t size);
extern uint8_t IF_At24c256_WriteBytes(uint16_t address, uint8_t *writeBuf, uint16_t size);

 #endif

 /*
*******************************************************************************
* END
*******************************************************************************
*/

