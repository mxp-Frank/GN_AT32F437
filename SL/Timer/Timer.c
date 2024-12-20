/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Timer.c
 * @Brief :      ref from bsp_motor.c(AMP/AME svn324) Author: Huxian
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-09
 * ****************************************************************************
 */

#include "IF_Timer.h"
#include "Timer.h"
#include "IF_Comm.h"
#include "Common.h"

#include "params_impl.h"
#include "IF_HAL_Cfg.h"

/* CONST & MACROS */

/* DATA STRUCTURES */

/* LOCAL VARIABLES */
static uint32_t sysTickCnt=0;

static SystemTimer_t SystemTimer;


/* GLOBAL VARIABLES */


/* FUNCTION PROTOTYPES */
static void Check_RFSlowStartDelay(void);
static void Check_RFSlowStopDelay(void);
static void Check_RFReflectOffDelay(void);
static void Check_PowerUpTimer(void);
static void Check_RFOnTimer(void);
static void Check_FanSpeedTimer(void);

static void Timer_ISR_Callback(void);

/************************************************************************/
/*1ms ISR Callback Functions Definitions                                   */
/************************************************************************/

void TMR_1ms_Callback(void)
{
    Timer_ISR_Callback();
}

/************************************************************************/
/* Global Functions Definitions                                         */
/************************************************************************/

/**
 * ************************************************************************
 * @Brief :      
 * ************************************************************************
 */
void IF_TimerInit(void)
{
	memset((void*)&SystemTimer,0,sizeof(SystemTimer_t));	
}
/* FUNCTION *********************************************************************************
 * Function  : IF_Timer_GetPowerUpTimer
 * Description   : 得到射频电源系统时间函数
 * Parameter     : OnorOff  ---->状态
 * return        :                
 * END ***************************************************************************************/
uint32_t IF_Timer_GetPowerUpTimer(void)
{
    return SystemTimer.PowerUpDuration;
}

/* FUNCTION *********************************************************************************
 * Function  : IF_Timer_GetRFOnUpTimer
 * Description   : 得到射频电源开启时间函数
 * Parameter     : OnorOff  ---->状态
 * return        :                
 * END ***************************************************************************************/
uint32_t IF_Timer_GetRFOnUpTimer(void)
{
    return SystemTimer.RFOnDuration;
}
uint8_t IF_Timer_GetPowerUpDurationFlag(void)
{
	return SystemTimer.PowerUpDurationFlag;
}
void IF_Timer_SetPowerUpDurationFlag(uint8_t OnorOff)
{
	SystemTimer.PowerUpDurationFlag=OnorOff;
}
/* FUNCTION *********************************************************************************
 * Function  : IF_Timer_SetRFOffDelayFlag
 * Description   : 设置射频电源开启延时函数
 * Parameter     : OnorOff  ---->状态
 * return        :                
 * END ***************************************************************************************/
void IF_Timer_SetRFSlowStartDelayFlag(uint8_t OnorOff)
{
    if(OnorOff == ON)
    {
        SystemTimer.SlowStartDelayFlag = 1;
    }
    else
    {
        SystemTimer.SlowStartDelayFlag = 0;
    }
}

uint16_t  IF_Timer_GetRFSlowStartDelayTime(void)
{
	return SystemTimer.SlowStartDelayTimer;
}	

/* FUNCTION *********************************************************************************
 * Function  : IF_Timer_SetRFOffDelayFlag
 * Description   : 设置射频电源开启延时函数
 * Parameter     : OnorOff  ---->状态
 * return        :                
 * END ***************************************************************************************/
void IF_Timer_SetRFSlowStopDelayFlag(uint8_t OnorOff)
{
    if(OnorOff == ON)
    {
        SystemTimer.SlowStopDelayFlag = 1;
    }
    else
    {
        SystemTimer.SlowStopDelayFlag = 0;
    }
}

/* FUNCTION *********************************************************************************
 * Function  	 : IF_Timer_GetRFOnDelayTimeOutFlag
 * Description   : 得到射频电源开启超时标志位函数
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
uint16_t IF_Timer_GetRFSlowStopDelayTime(void)
{
    return SystemTimer.SlowStopDelayTimer;
}

/* FUNCTION *********************************************************************************
 * Function  : IF_Timer_SetRFOffDelayFlag
 * Description   : 设置射频电源关闭延时函数
 * Parameter     : OnorOff  ---->状态
 * return        :                
 * END ***************************************************************************************/
void IF_Timer_SetRFOffDelayFlag(uint8_t OnorOff)
{
    if(OnorOff == ON)
    {
        SystemTimer.RFOffDelayFlag = 1;
    }
    else
    {
        SystemTimer.RFOffDelayFlag = 0;
    }
}

/* FUNCTION *********************************************************************************
 * Function  : IF_Timer_GetRFOffDelayTimeOutFlag
 * Description   : 得到射频电源关闭超时标志位函数
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
uint8_t IF_Timer_GetRFOffDelayTimeOutFlag(void)
{
    return SystemTimer.RFOffDelayTimeOutFlag;
}

/* FUNCTION *********************************************************************************
 * Function 	 : IF_Timer_ClearRFOffDelayTimeOutFlag
 * Description   : 设置射频电源关闭超时标志位函数
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
void IF_Timer_ClearRFOffDelayTimeOutFlag(void)
{
    SystemTimer.RFOffDelayTimeOutFlag = 0;
}


uint32_t  IF_Timer_GetFanSpeedValue(uint8_t FanChnNo)
{
	return SystemTimer.FanSpeedVal[FanChnNo];
}
/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/

/* FUNCTION *********************************************************************************
 * Function Name : Check_RFSlowStartDelay
 * Description   : 射频电源缓启动定时器回调函数
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void Check_RFSlowStartDelay(void)
{
	if (SystemTimer.SlowStartDelayFlag)
	{
		SystemTimer.SlowStartDelayTimer++;
		if (SystemTimer.SlowStartDelayTimer >= IF_UserParam_GetSlowStartDelay())
		{
			SystemTimer.SlowStartDelayTimer = IF_UserParam_GetSlowStartDelay();
		}
	}
	else
	{
		SystemTimer.SlowStartDelayTimer = 0;
	}
}
/* FUNCTION *********************************************************************************
 * Function Name : Check_RFSlowStopDelay
 * Description   : 射频电源缓启动关闭定时器回调函数
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void Check_RFSlowStopDelay(void)
{
	if (SystemTimer.SlowStopDelayFlag)
	{
		SystemTimer.SlowStopDelayTimer--;
		if (SystemTimer.SlowStopDelayTimer <= 0)
		{
			SystemTimer.SlowStopDelayTimer = 0;
		}
	}
	else
	{
		SystemTimer.SlowStopDelayTimer = IF_UserParam_GetSlowStopDelay();
	}
}

/* FUNCTION *********************************************************************************
 * Function Name : Check_RFReflectOffDelay
 * Description   : 射频电源开启关闭定时器回调函数
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void Check_RFReflectOffDelay(void)
{

	if (SystemTimer.RFOffDelayFlag)
	{
		SystemTimer.RFOffDelayTimer++;
		if (SystemTimer.RFOffDelayTimer >= 1000*IF_UserParam_GetReflectPowerDelayOff())
		{
			SystemTimer.RFOffDelayFlag = 0;
            SystemTimer.RFOffDelayTimeOutFlag = 1;  //time out
		}
	}
	else
	{
		SystemTimer.RFOffDelayTimer = 0;
	}
}
/* FUNCTION *********************************************************************************
 * Function Name : Check_PowerUpTimer
 * Description   : 匹配器电源开启定时器回调函数(系统时间使用LF_UNIT时间)
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void Check_PowerUpTimer(void)
{
	SystemTimer.PowerUpTimer++;

	if (SystemTimer.PowerUpTimer >= 1000)       // 1s时间到
	{
		SystemTimer.PowerUpTimer = 0;
		SystemTimer.PowerUpDuration++;
		if(SystemTimer.PowerUpDuration%3600==0)
		{
			SystemTimer.PowerUpDurationFlag =1;
		}
	}
}
/* FUNCTION *********************************************************************************
 * Function Name : Check_RFOnTimer
 * Description   : 射频电源开启定时器回调函数
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void Check_RFOnTimer(void)
{
	if (g_StatusWord.bits.RFOnOrOff == ON)
	{
		SystemTimer.RFOnTimer++;
		if (SystemTimer.RFOnTimer >= 1000)  // 1s时间到
		{
			SystemTimer.RFOnTimer = 0;
			SystemTimer.RFOnDuration++;
		}
	}
	else
	{
		SystemTimer.RFOnDuration = 0;
	}
}
static void Check_FanSpeedTimer(void)
{
	SystemTimer.FanTimer++;
	if(SystemTimer.FanTimer>25)
	{
		SystemTimer.FanTimer=0;
		for(uint8_t i=0;i<4;i++)
		{
			if(FANS_Time_OVERcnt[i] >= 5)
			{
				FANS_Time_OVERcnt[i] = 0;
				FANS_Period_Val[i]  = 0;
				FANS_Curr_CCR_Val[i] = 0;
				FANS_Last_CCR_Val[i]= 0;
				SystemTimer.FanSpeedVal[i] =0;
			}else
			{
				SystemTimer.FanSpeedVal[i]=IF_Fan_FG_Speed(i,50);
			}
		}
	}	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* FUNCTION *********************************************************************************
 * Function Name : Timer_ISR_Callback
 * Description   : 定时器回调函数
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void Timer_ISR_Callback(void)
{
	Check_PowerUpTimer();
	Check_RFOnTimer();
	Check_RFReflectOffDelay();
	Check_RFSlowStartDelay();
	Check_RFSlowStopDelay();
	Check_FanSpeedTimer();
	
}


//timing related...

/* FUNCTION *********************************************************************************
 * Function Name : SysTick_Handler
 * Description   : 滴答定时器中断服务函数
 * Parameter     :
 * Return        :
 * END ***************************************************************************************/

//Unique function(port.c has defined this function) call for SysTick_Handler!
extern void xPortSysTickHandler(void);

void SysTick_Handler(void)
{
	if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)	/*系统已经运行*/
    {
        xPortSysTickHandler();
    }
	else
	{
		sysTickCnt++;	/*调度开启之前计数*/
	}
}

/* FUNCTION *********************************************************************************
 *
 * Function Name : GetSysTickCnt
 * Description   : 调度开启之前返回sysTickCnt,调度开启之后返回 xTaskGetTickCount()
 * Parameter     :
 * Return        :
 * END ***************************************************************************************/
uint32_t GetSysTickCnt(void)
{
	if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)	/*系统已经运行*/
	{
		return xTaskGetTickCount();
	}
	else
	{
		return sysTickCnt;
	}
}

///* FUNCTION *********************************************************************************
// *
// * Function Name : timestamp_to_localtime
// * Description   : 时间戳转换成localtimer函数
// * Parameter     : time -----时间戳，Time_t -----当地时间（年月日时分秒）
// * Return        :
// * END ***************************************************************************************/
//static void timestamp_to_localtime(int32_t time,Time_t *t)
//{
//    unsigned int Pass4year;
//    int hours_per_year;
// 
//    if(time < 0)
//    {
//        time = 0;
//    }
//    //取秒时间
//    t->second=(int)(time % 60);
//    time /= 60;
//    //取分钟时间
//    t->minute=(int)(time % 60);
//    time /= 60;
//    //取过去多少个四年，每四年有 1461*24 小时
//    Pass4year=((unsigned int)time / (1461L * 24L));
//    //计算年份
//    t->year=(Pass4year << 2) + 1970;
//    //四年中剩下的小时数
//    time %= 1461L * 24L;
//    //校正闰年影响的年份，计算一年中剩下的小时数
//    for (;;)
//    {
//        //一年的小时数
//        hours_per_year = 365 * 24;
//        //判断闰年
//        if((t->year & 3) == 0)
//        {
//            //是闰年，一年则多24小时，即一天
//            hours_per_year += 24;
//        }
//        if(time < hours_per_year)
//        {
//            break;
//        }
//        t->year++;
//        time -= hours_per_year;
//    }
//    //小时数
//    t->hour=(int)(time % 24);
//    //一年中剩下的天数
//    time /= 24;
//    //假定为闰年
//    time++;
//    //校正闰年的误差，计算月份，日期
//    if((t->year & 3) == 0)
//    {
//        if(time > 60)
//        {
//            time--;
//        }
//        else
//        {
//            if(time == 60)
//            {
//                t->month = 1;
//                t->day = 29;
//                return ;
//            }
//        }
//    }
//    //计算月日
//    for(t->month = 0; Days[t->month] < time; t->month++)
//    {
//        time -= Days[t->month];
//    }
// 
//    t->day = (int)(time);
//	
//    /*时间校正*/
//	t->month+=1;
//	t->weekday+=1;
//    return;
//}
///* FUNCTION *********************************************************************************
// *
// * Function Name : isleap
// * Description   : 闰年计算
// * Parameter     : year ---年
// * Return        : 
// * END ***************************************************************************************/ 
//static int isleap(int year)
//{
//    return (year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0);
//}
///* FUNCTION *********************************************************************************
// *
// * Function Name : mktimestamp
// * Description   : localtimer转换成时间戳函数
// * Parameter     : Time_t -----当地时间（年月日时分秒）
// * Return        : 时间戳
// * END ***************************************************************************************/ 
//static int32_t mktimestamp( Time_t dt)
//{
//    int32_t result;
//    int i =0;
//    // 以平年时间计算的秒数
//    result = (dt.year - 1970) * 365 * 24 * 3600 +
//    (mon_yday[isleap(dt.year)][dt.month-1] + dt.day - 1) * 24 * 3600 +
//    dt.hour * 3600 + dt.minute * 60 + dt.second;
//    // 加上闰年的秒数
//    for(i=1970; i < dt.year; i++)
//    {
//        if(isleap(i))
//        {
//            result += 24 * 3600;
//        }
//    }
//    return(result);
//}

//*****************************************************************************
//* END
//*****************************************************************************

