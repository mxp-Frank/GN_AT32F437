/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_Tmr.h--->external use
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#ifndef _IF_TMR_H_
#define _IF_TMR_H_

#include "IF_PrjLib.h"
#include "IF_Board.h"
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

typedef struct
{
  uint32_t tmrDiv;
  uint32_t tmrPeriod;
} tmr_cfg_type;

		
/******************************************************************************
 * Interface functions                                                         *
 ******************************************************************************/
extern void IF_TmrInit(void);

extern void IF_FAN_PWM_Switch(uint8_t para);
extern void delay_SuspendTick(void);
extern void delay_ResumeTick(void);
extern void delay_us(uint32_t nus);
extern void delay_xms(uint32_t nms);
extern void delay_ms(uint32_t nms);
extern void delay_xns(uint32_t ns);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif

/*
*******************************************************************************
* END
*******************************************************************************
*/

