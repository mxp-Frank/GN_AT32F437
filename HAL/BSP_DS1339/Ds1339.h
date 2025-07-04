/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  At24c256.h--->internal use
 * @Brief :      ref from bsp_eeprom.h(AMP/AME svn324) Author: Huxian
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#ifndef _DS1339_H_
#define _DS1339_H_

#include "IF_Board.h"
#include "IF_Gpio.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
#define IIC3_SDA_INPUT    	0
#define IIC3_SDA_OUTPUT   	1
#define IIC3_WR	     		0		/* 写控制bit */
#define IIC3_RD	     		1		/* 读控制bit */


#define IIC3_SCL_GPIO			GPIOD
#define IIC3_SCL_PIN			IIC3_SCL_Pin85_PD14

#define IIC3_SDA_GPIO			GPIOD
#define IIC3_SDA_PIN			IIC3_SDA_Pin86_PD15

#define IIC3_SCL_HIGH()   		IF_OutPutGpio_Write(IIC3_SCL_GPIO, IIC3_SCL_Pin85_PD14, 1, LOGIC_POUT)
#define IIC3_SCL_LOW()    		IF_OutPutGpio_Write(IIC3_SCL_GPIO, IIC3_SCL_Pin85_PD14, 0, LOGIC_POUT)

#define IIC3_SDA_HIGH()   		IF_OutPutGpio_Write(IIC3_SDA_GPIO, IIC3_SDA_Pin86_PD15, 1, LOGIC_POUT)
#define IIC3_SDA_LOW()    		IF_OutPutGpio_Write(IIC3_SDA_GPIO, IIC3_SDA_Pin86_PD15, 0, LOGIC_POUT)

#define IIC3_SDA_READ()    		IF_InPutGpio_Read(IIC3_SDA_GPIO,  IIC3_SDA_PIN, ACTIVE_HIGH)

#define DS1339_ADDR				0XD0

/*************初始化必须配置寄存器*****************************/
#define REGADDR_SECONDS			0X00 //秒  
#define REGADDR_MINUTES			0X01 //分  
#define REGADDR_HOURS			0X02 //时  
#define REGADDR_DAY				0X03 //周日  
#define REGADDR_DATA			0X04 //日
#define REGADDR_MONTH_CENTURY	0X05 //月  
#define REGADDR_YEAR			0X06 //年 
/*************闹钟配置寄存器*****************************/
#define REGADDR_ALARM1SECONDS			0X07 //闹钟1——秒  
#define REGADDR_ALARM1MINUTES			0X08 //闹钟1——分  
#define REGADDR_ALARM1HOURS				0X09 //闹钟1——时
#define REGADDR_ALARM1DAY_DATA			0X0A //闹钟1——周日  
 
#define REGADDR_ALARM2MINUTES			0X0B //闹钟2——分   
#define REGADDR_ALARM2HOURS				0X0C //闹钟2——时  
#define REGADDR_ALARM2DAY_DATA			0X0D //闹钟2——周日  
/*************默认配置寄存器*****************************/
#define REGADDR_CONTROL					0X0E  //控制寄存器，用与设置晶振频率
#define REGADDR_STATUS					0X0F //状态寄存器，
#define REGADDR_TRICKLENCHARGER			0X10

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif 
//*****************************************************************************
//* END
//*****************************************************************************

