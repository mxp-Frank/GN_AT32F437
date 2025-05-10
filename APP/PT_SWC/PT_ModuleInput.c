
#include "IF_PT_SWC.h"
#include "IF_params_impl.h"
#include "IF_HAL_Cfg.h"
#include "IF_SL_Cfg.h"
#include "IF_Comm.h"

static void Module_Cmd_Execute(void);
static void Module_Port_Input(void);
static void Module_Sensor_Input(void);
/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/
/* FUNCTION *************************************************************
 * Function Name : IF_PT_CmdExecute_Task
 * Description   : cmd执行输入函数
 * Parameter     : 
 * return        :               
 * END *****************************************************************/
void IF_PT_CmdExecute_Task(void)
{
	Module_Cmd_Execute();
}
/* FUNCTION *************************************************************
 * Function Name : IF_PT_InterLockInput_Task
 * Description   : 模拟接口输入函数
 * Parameter     : 
 * return        :               
 * END *****************************************************************/
void IF_PT_PortInput_Task(void)
{
	Module_Port_Input();		
}
/* FUNCTION *************************************************************
 * Function Name : IF_Module_SensorInput_Task
 * Description   : 设备所需sensor输入函数
 * Parameter     : 
 * return        :               
 * END *****************************************************************/
void IF_Module_SensorInput_Task(void)
{
	//if(GN_Device.InterlockOpen)
	{
		Module_Sensor_Input();
	}
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

static void Module_Sensor_Input(void)
{
	GN_Device.Sensor.Freq   =  IF_SL_Fpga_GetSensor(ChnN_Freq);
	GN_Device.Sensor.Pfwd   =  IF_SL_Fpga_GetSensor(ChnN_Pfwd);
	GN_Device.Sensor.Pref   =  IF_SL_Fpga_GetSensor(ChnN_Pref);
	GN_Device.Sensor.VSWR   =  IF_SL_Fpga_GetSensor(ChnN_VSWR);
}
/* FUNCTION *********************************************************************************
 * Function 	 : Module_Port_Input
 * Description   : 电源输入端口函数(PORT)
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void Module_Port_Input(void)
{
	GN_Device.InterlockOpen = IF_HAL_InterLockDect_SigRead();
	/**********开始监测InterLock状态(默认InterLock SW 关闭)*********/
	if(GN_Device.InterlockOpen)
	{
		g_FaultWord.bits.InterlockOpen = 0;		
	}else
	{
		g_FaultWord.bits.InterlockOpen = 1;
	}
}

//*****************************************************************************
//* END
//*****************************************************************************
