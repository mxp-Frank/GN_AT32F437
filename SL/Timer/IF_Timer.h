/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_Timer.h
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-09
 * ****************************************************************************
 */
 
#ifndef _IF_TIMER_H_
#define _IF_TIMER_H_

#include "IF_Board.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
/////////////////////////////////////////////////////////////////////////////////////////////////// 		   
#define RATE_5_HZ		               5
#define RATE_10_HZ		               10
#define RATE_20_HZ		               20
#define RATE_25_HZ		               25
#define RATE_50_HZ		               50
#define RATE_100_HZ		               100
#define RATE_200_HZ 	               200
#define RATE_250_HZ 	               250
#define RATE_500_HZ 	               500
#define RATE_1000_HZ 	               1000

#define MAIN_LOOP_RATE 	               configTICK_RATE_HZ
#define MAIN_LOOP_DT	               (uint32_t)(1000/MAIN_LOOP_RATE)	   /*单位ms*/

#define RATE_DO_EXECUTE(RATE_HZ, TICK) ((TICK % (MAIN_LOOP_RATE / RATE_HZ)) == 0)

typedef struct _SystemTimer_t
{
	uint16_t PowerUpTimer;
	uint32_t PowerUpDuration;
	uint8_t PowerUpDurationFlag;
	
	uint16_t RFOnTimer;
	uint32_t RFOnDuration;
		
	uint32_t SlowStartDelayTimer;
	uint8_t  SlowStartDelayFlag;
	
	uint32_t SlowStopDelayTimer;
	uint8_t  SlowStopDelayFlag;

	uint32_t RFOffDelayTimer;
	uint8_t  RFOffDelayFlag;
    uint8_t  RFOffDelayTimeOutFlag;
	
	uint32_t FanTimer;
	uint32_t FanSpeedVal[4];
} SystemTimer_t;

/******************************************************************************
* Interface functions                                                         *
******************************************************************************/
extern void IF_TimerInit(void);
extern uint32_t Now(void);

extern uint32_t IF_Timer_GetPowerUpTimer(void);
extern uint32_t IF_Timer_GetRFOnUpTimer(void);
extern uint8_t IF_Timer_GetPowerUpDurationFlag(void);
extern void IF_Timer_SetPowerUpDurationFlag(uint8_t OnorOff);

extern uint16_t IF_Timer_GetRFSlowStartDelayTime(void);
extern void IF_Timer_SetRFSlowStartDelayFlag(uint8_t OnorOff);

extern uint16_t IF_Timer_GetRFSlowStopDelayTime(void);
extern void IF_Timer_SetRFSlowStopDelayFlag(uint8_t OnorOff);
extern uint8_t IF_Timer_GetRFSlowStopDelayTimeOutFlag(void);
extern void IF_Timer_ClearRFSlowOffDelayTimeOutFlag(void);

extern void IF_Timer_SetRFOffDelayFlag(uint8_t OnorOff);
extern uint8_t IF_Timer_GetRFOffDelayTimeOutFlag(void);
extern void IF_Timer_ClearRFOffDelayTimeOutFlag(void);

extern uint32_t  IF_Timer_GetFanSpeedValue(uint8_t FanChnNo);

#if defined(__cplusplus)
extern "C" }
#endif /* __cplusplus */

#endif

 /*
*******************************************************************************
* END
*******************************************************************************
*/

