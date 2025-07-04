/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  At24c256.h--->internal use
 * @Brief :      ref from bsp_eeprom.h(AMP/AME svn324) Author: Huxian
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#ifndef _AT24C256_H_
#define _AT24C256_H_

#include "IF_Board.h"
#include "IF_Gpio.h"

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
#define SDA_INPUT    0
#define SDA_OUTPUT   1
#define SCL_INPUT    SDA_INPUT
#define SCL_OUTPUT   SDA_OUTPUT

#define I2C_WR	     0		/* 写控制bit */
#define I2C_RD	     1		/* 读控制bit */
#define IIC2_SCL_GPIO		GPIOF
#define IIC2_SCL_PIN		IIC2_SCL_Pin11_PF1

#define IIC2_SDA_GPIO		GPIOF
#define IIC2_SDA_PIN		IIC2_SDA_Pin10_PF0

#define SCL_HIGH()   		IF_OutPutGpio_Write(IIC2_SCL_GPIO, IIC2_SCL_PIN, 1, LOGIC_POUT)
#define SCL_LOW()    		IF_OutPutGpio_Write(IIC2_SCL_GPIO, IIC2_SCL_PIN, 0, LOGIC_POUT)

#define SDA_HIGH()   		IF_OutPutGpio_Write(IIC2_SDA_GPIO, IIC2_SDA_PIN, 1, LOGIC_POUT)
#define SDA_LOW()    		IF_OutPutGpio_Write(IIC2_SDA_GPIO, IIC2_SDA_PIN, 0, LOGIC_POUT)

#define SDA_READ()    		IF_InPutGpio_Read(IIC2_SDA_GPIO, IIC2_SDA_PIN, ACTIVE_HIGH)

/* 
 * AT24C256C 256kb = 256/8 KB = 32KB
 * 512 pages of 64-bytes each
 *
 * Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0xA0
 * 1 0 1 0 0  0  0  1 = 0xA1 
 */

/**************************************************************
 * AT24C01/02每页有8个字节
 * AT24C04/08A/16A每页有16个字节
 * AT24C04/08A/16A每页有16个字节
 * AT24C32/64每页有32个字节
 * AT24C128/256每页有64个字节
 * AT24C512每页有128个字节
 ************************************************************/


#endif 
//*****************************************************************************
//* END
//*****************************************************************************

