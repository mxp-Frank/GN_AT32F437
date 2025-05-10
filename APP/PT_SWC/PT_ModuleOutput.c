
#include "math.h"
#include "arm_math.h"
#include "IF_PT_SWC.h"
#include "IF_params_impl.h"
#include "IF_HAL_Cfg.h"
#include "IF_SL_Cfg.h"

static void Module_Port_Output(void);
/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/
void IF_Module_PortOutput_Task(void)
{
	if(GN_Device.InterlockOpen == 1) //运行状态下
	{
		Module_Port_Output();
	}	
}
/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/
/* FUNCTION *********************************************************************************
 * Function 	 : Module_Port_Output
 * Description   : 射频电源端口输出函数(DB25)
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void Module_Port_Output(void)
{	
	/*************功率输出时，监测电源功率*************************************/
	if(GN_Device.Sensor.Pfwd >= 5)
	{
		g_StatusWord.bits.RFOnOrOff = 1;
	}else
	{
		g_StatusWord.bits.RFOnOrOff = 0;
	}	
	/*******************如果反射功率大于反射功率保护阈值**************************/
	if(GN_Device.Sensor.Pref > IF_SL_UserParam_GetReflectPowerThreshold())
	{
		if(IF_SL_UserParam_GetReflectPowerDelayOff()>0)
		{
			g_StatusWord.bits.RFWillswitchoff = ON; 
			IF_SL_Timer_SetRFOffDelayFlag(ON);  			//设置等待延时关闭功率开启
		}
		if( ON == IF_SL_Timer_GetRFOffDelayTimeOutFlag()) 
		{
			IF_SL_CmdParam_SetDDSDriverState(RF_OFF);   //关闭射频驱动			
			IF_SL_Timer_ClearRFOffDelayTimeOutFlag();  //清除超时延时标志位
		}	
	}else
	{
		IF_SL_Timer_SetRFOffDelayFlag(OFF);  			//设置等待关闭功率关闭
		g_StatusWord.bits.RFWillswitchoff = OFF; 
	}
	
	/**********************脉冲时间(<15us)状态******************************************/
	if(10000*GN_Device.FpgaPulse.DutyCircle < 15*GN_Device.FpgaPulse.Frequency)  
	{
		g_StatusWord.bits.ExternalPulseshort = OFF;	
	}
	else if(GN_Device.FpgaPulse.Mode)
	{
		g_StatusWord.bits.ExternalPulseshort  =  ON;
	}
}

//*****************************************************************************
//* END
//*****************************************************************************
