
#include "math.h"
#include "arm_math.h"
#include "IF_PT_SWC.h"
#include "IF_params_impl.h"
#include "IF_HAL_Cfg.h"
#include "IF_SL_Cfg.h"

static void Module_Cmd_Execute(void);
static void Module_Port_Output(void);
/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/
/* FUNCTION *************************************************************
 * Function Name : IF_Module_CmdExecute_Task
 * Description   : cmd执行输出函数
 * Parameter     : 
 * return        :               
 * END *****************************************************************/
void IF_Module_CmdExecute_Task(void)
{
	Module_Cmd_Execute();
}
	
/* FUNCTION *************************************************************
 * Function Name : IF_Module_Output_Task
 * Description   : Port output执行函数
 * Parameter     : 
 * return        :               
 * END *****************************************************************/
void IF_Module_Output_Task(void)
{
	Module_Port_Output();	
}
/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/
/* FUNCTION *********************************************************************************
 * Function 	 : Module_Cmd_Execute
 * Description   : 匹配器命令输入函数(Cmd)
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void Module_Cmd_Execute(void)
{	
  	DeviceCmdMsg_t DeviceCmd;
	if(pdTRUE ==xQueueReceive(CmdQueue, &DeviceCmd, (TickType_t)0))
	{  
		switch(DeviceCmd.cmdId)
		{
		/*******************工厂模式下写参数**************************/
		case CMD_SAVE_FSPARTSPARAM:  //保存内存工厂配置到NVM   
			IF_SL_Nvm_ParamsRW(Write_Fs_PartsMask);
		break;

		case CMD_SAVE_FSCOMMONPARAM:	//保存内存工厂配置到NVM
			IF_SL_Nvm_ParamsRW(Write_FS_CommonMask);   
		break;

		case CMD_SAVE_FSINTERNALPARAM: //保存内存工厂配置到NVM
			IF_SL_Nvm_ParamsRW(Write_FS_InternalMask);
		break;

		case CMD_SAVE_FSUSERPARAM: //保存内存工厂配置到NVM
			IF_SL_Nvm_ParamsRW(Write_FS_UserMask);
		break;
		/*******************正常模式下写参数**************************/
		case CMD_SAVE_PARTSPARAM: //保存内存配置到NVM
			IF_SL_Nvm_ParamsRW(Write_PartsMask);
		break;

		case CMD_SAVE_COMMONPARAM: //保存内存配置到NVM
			IF_SL_Nvm_ParamsRW(Write_CommonMask);
		break;

		case CMD_SAVE_INTERNALPARAM: //保存内存配置到NVM
			IF_SL_Nvm_ParamsRW(Write_InternalMask); 
		break;

		case CMD_SAVE_USERPARAM:	 //保存内存配置到NVM
			IF_SL_Nvm_ParamsRW(Write_UserMask);
		break;
		case CMD_SAVE_VOLTMAPPARAM:
			IF_SL_Nvm_ParamsRW(Write_VoltMapMask);
		break;
		case CMD_SAVE_PHASEMAPPARAM:
			IF_SL_Nvm_ParamsRW(Write_PhaseMapMask);	
		break;
		case CMD_SETBAUDRATE: //设置通信的波特率
			IF_SL_SetUartBaudRate(DeviceCmd.cmdData.Val);
		break;
		case CMD_DEVICERESET: //设置电源软重启
			IF_SL_ResetDevice();
		break;	 
		default:break;			
		}
	}
}

/* FUNCTION *********************************************************************************
 * Function 	 : Module_Port_Output
 * Description   : 射频电源端口输出函数(DB25)
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void Module_Port_Output(void)
{	
	if(GN_Device.RunState.Now == STANDBY_STATE)
	{	
		IF_SetNormalOutput_SIG(IOSIGN_INTLOCKENABLE,OFF);	  	
	}else
	{
		IF_SetNormalOutput_SIG(IOSIGN_INTLOCKENABLE,ON);		
	}
	if(GN_Device.RunState.Now < OPEN_STATE)
	{
		g_FaultWord.bits.InterlockOpen = 1;		
	}else
	{
		g_FaultWord.bits.InterlockOpen = 0;		
	}	
	/*****************InterLock Led****************************/
	if(g_FaultWord.bits.InterlockOpen == 1)
	{
		IF_SetNormalOutput_SIG(IOSIGN_INTLKLEDONOFF,ON);		
	}else
	{
		IF_SetNormalOutput_SIG(IOSIGN_INTLKLEDONOFF,OFF);
	}
	
	/**************反射功率大于反射功率限制***************************/
	if(GN_Device.Sensor.Now.Pref > IF_SL_UserParam_GetReflectPowerLimit())
	{
		if(IF_SL_UserParam_GetReflectPowerDelayOff() > 0)
		{
			g_StatusWord.bits.RFWillswitchoff = 1; 
			IF_SL_Timer_SetRFOffDelayFlag(ON);  	     //设置等待延时关闭功率开启
		}
		if( ON == IF_SL_Timer_GetRFOffDelayTimeOutFlag()) 
		{
			IF_SL_CmdParam_SetRFPowerState(RF_OFF);    	 //关闭射频功率			
			IF_SL_Timer_ClearRFOffDelayTimeOutFlag();    //清除超时延时标志位
		}	
	}else
	{
		IF_SL_Timer_SetRFOffDelayFlag(OFF);  			 //设置等待关闭功率关闭
		g_StatusWord.bits.RFWillswitchoff = 0; 
		g_StatusWord.bits.PreflAtLimit = 0;
	}	
	
	/*************功率输出时，监测电源功率**************************/
	if(GN_Device.Sensor.Now.Pfwd >= POWER_RATIO)
	{
		g_StatusWord.bits.RFOnOrOff = 1;
		IF_SetNormalOutput_SIG(IOSIGN_RFONLEDONOFF,ON);
	}else
	{
		g_StatusWord.bits.RFOnOrOff = 0;
		IF_SetNormalOutput_SIG(IOSIGN_RFONLEDONOFF,OFF);
	}
	if(g_StatusWord.bits.RFWillswitchoff)
	{
		IF_SetNormalOutput_SIG(IOSIGN_WARNLEDONOFF,ON);		
	}else
	{
		IF_SetNormalOutput_SIG(IOSIGN_WARNLEDONOFF,OFF);
	}
	/*************************故障字*******************************/
	if(g_FaultWord.Dword > 0)
	{
		IF_SetNormalOutput_SIG(IOSIGN_FAULTLEDONOFF,ON);	
	}else
	{
		IF_SetNormalOutput_SIG(IOSIGN_FAULTLEDONOFF,ON);	
	}	
}

//*****************************************************************************
//* END
//*****************************************************************************
