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
#define FAN_PWM_PERIOD    40
#define FAN_PWM_DUTY(x)  (x*FAN_PWM_PERIOD/100)
/* DATA STRUCTURES */
static uint16_t Fan_Pwm_TimeOut = 0;
/* LOCAL VARIABLES */
static uint32_t sysTickCnt=0;

static SystemTimer_t SystemTimer;


/* GLOBAL VARIABLES */


/* FUNCTION PROTOTYPES */
static void Check_RFSlowStartDelay(void);
static void Check_RFSlowStopDelay(void);
static void Check_RFReflectOffDelay(void);
static void Check_RFOnTimer(void);
static void Check_FanSlowOnDelay(void);

/************************************************************************/
/*1ms ISR Callback Functions Definitions                                */
/************************************************************************/
void TMR_1ms_Callback(void)
{
    Check_RFOnTimer();
	Check_RFReflectOffDelay();
	Check_RFSlowStartDelay();
	Check_RFSlowStopDelay();
	Check_FanSlowOnDelay();	
}

/************************************************************************/
/*Fan ISR Callback Functions Definitions                                */
/************************************************************************/
void Fan_PWM_ISR_Callback(void)
{
	Fan_Pwm_TimeOut++;
	if(Fan_Pwm_TimeOut == FAN_PWM_DUTY(SystemTimer.Pwm_duty))
	{
		IF_HAL_Fan_PWM_Switch(IO_HIGH);
	}
	else if(Fan_Pwm_TimeOut > FAN_PWM_PERIOD)
	{
		IF_HAL_Fan_PWM_Switch(IO_LOW);
		Fan_Pwm_TimeOut = 0;
	}		
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
    return GetSysTickCnt()/1000;
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

float  IF_Timer_GetRFSlowStartDelayTime(void)
{
	float fvalue = 0.0F;
	if(SystemTimer.SlowStartDelayFlag)
	{
		fvalue = SystemTimer.SlowStartDelayTimer/IF_UserParam_GetSlowStartDelay();
	}else
	{
		fvalue = 1.0F;
	}
	return  fvalue;
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
float IF_Timer_GetRFSlowStopDelayTime(void)
{
	float fvalue = 0.0F;
	if(SystemTimer.SlowStopDelayFlag)
	{
		 fvalue = SystemTimer.SlowStopDelayTimer/IF_UserParam_GetSlowStopDelay();
	}else
	{
		fvalue = 0.0F;
	}	
    return fvalue;
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

uint8_t IF_Timer_GetSlowStartFlag(void)
{
	return SystemTimer.FanSlowOnFlag;
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
	}else
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
/* FUNCTION *********************************************************************************
 * Function Name : Check_FanSlowOnDelay
 * Description   : 风扇缓开启定时器回调函数
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void Check_FanSlowOnDelay(void)
{
	SystemTimer.FanSlowOnTimer++;
	if (SystemTimer.FanSlowOnTimer >= 50)       // 50ms时间到
	{
		SystemTimer.FanSlowOnTimer = 0;
		if(SystemTimer.Pwm_duty >= 90)
		{
			SystemTimer.FanSlowOnFlag = 1;
			SystemTimer.Pwm_duty = 90;
		}
		SystemTimer.Pwm_duty++;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////
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
//*****************************************************************************
//* END
//*****************************************************************************

