/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_Dac5614.h--->external use
 * @Brief :      ref from bsp_dac_tlv5614.h(AMP/AME svn324) Author: Huxian
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#ifndef _IF_DAC5614_H_
#define _IF_DAC5614_H_

#include "IF_PrjLib.h"
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
#define CHANNEL_A            0
#define CHANNEL_B            1
#define CHANNEL_C            2
#define CHANNEL_D            3
 /******************************************************************************
 * Interface functions                                                         *
 ******************************************************************************/
extern void IF_Dac5614Init(void);
extern void IF_Dac5614WriteDAC(uint16_t channel, uint16_t codeValue);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif

 /*
*******************************************************************************
* END
*******************************************************************************
*/

