/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_Tpc112S4.h--->external use
 * @Brief :      ref from bsp_dac_tlv5614.h(AMP/AME svn324) Author: Huxian
 * @Author :     Chenghanbing (iwanthaha@sina.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#ifndef _IF_Tpc112S4_H_
#define _IF_Tpc112S4_H_

#include "IF_PrjLib.h"
#include "configDevice.h"


#if(config_ExDAC_TLV5614 ==1)
	#define CHANNEL_A            0
	#define CHANNEL_B            1
	#define CHANNEL_C            2
	#define CHANNEL_D            3
#else
	#define CHANNEL_A            0x08
	#define CHANNEL_B            0x0A
	#define CHANNEL_C            0x0C
	#define CHANNEL_D            0x0E
#endif
 /******************************************************************************
 * Interface functions                                                         *
 ******************************************************************************/
extern void IF_ExDAC_Init(void);
extern void IF_ExDAC_WriteDA(uint16_t channel,uint16_t refVolt);

#endif

 /*
*******************************************************************************
* END
*******************************************************************************
*/

