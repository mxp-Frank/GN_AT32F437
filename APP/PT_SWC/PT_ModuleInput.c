
#include "IF_PT_SWC.h"
#include "IF_params_impl.h"
#include "IF_HAL_Cfg.h"
#include "IF_SL_Cfg.h"
#include "IF_Comm.h"


static void Module_Port_Input(void);
static void Module_Sensor_Input(void);
/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/
/* FUNCTION *************************************************************
 * Function Name : IF_Module_Input_Task
 * Description   : 设备所需sensor输入函数
 * Parameter     : 
 * return        :               
 * END *****************************************************************/
void IF_Module_Input_Task(void)
{
	Module_Port_Input();
	Module_Sensor_Input();
}
/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/
static void Module_Sensor_Input(void)
{
	if(GN_Device.RunState.Now == RUN_STATE)
	{
		
		GN_Device.SetPower 	=  IF_SL_CmdParam_GetPwrPoint();
		GN_Device.Sensor.Now.RFState = (RFEnum)IF_SL_CmdParam_GetRFPowerState();
		GN_Device.Sensor.Now.Freq   =  IF_SL_Fpga_GetSensor(ChnN_Freq,HP_CHN);
		GN_Device.Sensor.Now.Pfwd   =  IF_SL_Fpga_GetSensor(ChnN_Pfwd,HP_CHN);
		GN_Device.Sensor.Now.Pdlv   =  IF_SL_Fpga_GetSensor(ChnN_Pdlv,HP_CHN);
		GN_Device.Sensor.Now.Pref   =  IF_SL_Fpga_GetSensor(ChnN_Pref,HP_CHN);
		GN_Device.Sensor.Now.VSWR   =  IF_SL_Fpga_GetSensor(ChnN_VSWR,HP_CHN);
		GN_Device.Sensor.Now.Gamma  =  IF_SL_Fpga_GetSensor(ChnN_Gamma,HP_CHN);		
	}else
	{
		GN_Device.SetPower 	= 0;
		memset(&GN_Device.Sensor,0,sizeof(Sensor_t));
	}
}
/* FUNCTION *********************************************************************************
 * Function 	 : Module_Port_Input
 * Description   : 电源输入端口函数(PORT)
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void Module_Port_Input(void)
{
	/**********开始监测InterLock状态*********/
	if(1 == IF_Get_NormalInput_SW(IOSWITCH_INTLKDECT))
	{
		GN_Device.RunState.Now = INTLK_STATE;
	}else
	{
		GN_Device.RunState.Now = STANDBY_STATE;
	}
	/***********开始监测ACDC InterLock Enable状态*********************/
	if(GN_Device.RunState.Now == INTLK_STATE)
	{
		if(1 == IF_Get_NormalInput_SW(IOSWITCH_INTLKEBABLE))
		{
			GN_Device.RunState.Now = IDLE_STATE;
		}else
		{
			GN_Device.RunState.Now = INTLK_STATE;
		}
	}
	/***********开始监测缓启动完成状态******************/
	if(GN_Device.RunState.Now == IDLE_STATE) //
	{
		if(1 == IF_SL_Timer_GetSlowStartFlag()) 
		{
			GN_Device.RunState.Now = OPEN_STATE;
		}else
		{
			GN_Device.RunState.Now = IDLE_STATE;
		}
	}
	/**************开始监测Loop Mode 状态*********************/
	if(GN_Device.RunState.Now == OPEN_STATE)	
	{
		if(1 == IF_SL_CmdParam_GetPowerWorkMode())	
		{
			GN_Device.RunState.Now = RUN_STATE;
		}else
		{
			GN_Device.RunState.Now = OPEN_STATE;		
		}
	}
	if(GN_Device.RunState.Now != GN_Device.RunState.Last)
	{
		IF_SL_CmdParam_SetPwrPoint(0);              //设置功率设置值为零
		IF_SL_CmdParam_SetDDSPhase(0);          	//关闭功放驱动相位	
		IF_SL_CmdParam_SetACDCVoltage(0);			//设置AC-DC电压为零	
		IF_SL_CmdParam_SetRFPowerState(OFF);	    //关闭功率使能		
		IF_SL_CmdParam_SetDDSDriverState(OFF); 		//关闭功放驱动电源使能	
		IF_SL_CmdParam_SetACDCDriverState(OFF);     //关闭AC-DC电源使能			
	}		
	GN_Device.RunState.Last = GN_Device.RunState.Now;
}
	
//*****************************************************************************
//* END
//*****************************************************************************
