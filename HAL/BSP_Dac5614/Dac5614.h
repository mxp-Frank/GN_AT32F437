/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Dac5614.h--->internal use
 * @Brief :      ref from bsp_dac_tlv5614.h(AMP/AME svn324) Author: Huxian
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */
#ifndef _DAC5614_H_
#define _DAC5614_H_

#include "IF_Board.h"
#include "IF_Gpio.h"

#define HIGH                 1
#define LOW                  0

#define FAST_MODE            0x1000
#define POWER_DOWN           0x2000

#define DAC_PD_GPIO			GPIOB	
#define DAC_PD_PIN			DAC_PD_Pin136_PB6

#define DAC_LDAC_GPIO		GPIOB	
#define DAC_LDAC_PIN		DAC_LDAC_Pin137_PB7

#define DAC_DIN_GPIO		GPIOB	
#define DAC_DIN_PIN			DAC_DIN_Pin139_PB8

#define DAC_SCLK_GPIO		GPIOB	
#define DAC_SCLK_PIN		DAC_SCK_Pin140_PB9

#define DAC_FS_GPIO         GPIOE
#define DAC_FS_PIN			DAC_FS_Pin142_PE1

#define PD_LOW   	         IF_OutPutGpio_Write(DAC_PD_GPIO, DAC_PD_PIN, 0, LOGIC_POUT)
#define PD_HIGH 	         IF_OutPutGpio_Write(DAC_PD_GPIO, DAC_PD_PIN, 1, LOGIC_POUT)

#define LDAC_LOW  	         IF_OutPutGpio_Write(DAC_LDAC_GPIO, DAC_LDAC_PIN, 0, LOGIC_POUT)
#define LDAC_HIGH 	         IF_OutPutGpio_Write(DAC_LDAC_GPIO, DAC_LDAC_PIN, 1, LOGIC_POUT)


#define DIN_LOW  	         IF_OutPutGpio_Write(DAC_DIN_GPIO, DAC_DIN_PIN, 0, LOGIC_POUT)
#define DIN_HIGH 	         IF_OutPutGpio_Write(DAC_DIN_GPIO, DAC_DIN_PIN, 1, LOGIC_POUT)

#define SCLK_LOW  	         IF_OutPutGpio_Write(DAC_SCLK_GPIO, DAC_SCLK_PIN, 0, LOGIC_POUT)
#define SCLK_HIGH 	         IF_OutPutGpio_Write(DAC_SCLK_GPIO, DAC_SCLK_PIN, 1, LOGIC_POUT)

#define FS_LOW  	         IF_OutPutGpio_Write(DAC_FS_GPIO, DAC_FS_PIN, 0, LOGIC_POUT)
#define FS_HIGH 	         IF_OutPutGpio_Write(DAC_FS_GPIO, DAC_FS_PIN, 1, LOGIC_POUT)
#endif 

//*****************************************************************************
//* END
//*****************************************************************************

