 /**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_Dac.h--->external use
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */
 
#ifndef _IF_DAC_H_
#define _IF_DAC_H_

#include "IF_PrjLib.h"
#include "IF_Board.h"

#define DAC1 DAC1_SELECT
#define DAC2 DAC2_SELECT

/******************************************************************************
* Interface functions                                                         *
******************************************************************************/
extern void IF_DacInit(void);
extern void IF_Dac_SetValue(dac_select_type dacChn, uint16_t dacValue);

#endif

 /*
*******************************************************************************
* END
*******************************************************************************
*/

