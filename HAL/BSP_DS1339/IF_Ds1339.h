/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_DS1339.h--->external use
 * @Brief :      ref from bsp_eeprom.h(AMP/AME svn324) Author: Huxian
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#ifndef _IF_DS1339_H_
#define _IF_DS1339_H_ 
#include "IF_PrjLib.h"
#include "time.h" 
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */


 /******************************************************************************
 * Interface functions                                                         *
 ******************************************************************************/
extern void IF_DS1339Init(void);
extern void IF_DS1339_SetTimer(time_t timer);
extern void IF_DS1339_GetTimer(struct tm *timer);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif

 /*
*******************************************************************************
* END
*******************************************************************************
*/

