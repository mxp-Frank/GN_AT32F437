
#include "IF_PT_SWC.h"
#include "IF_params_impl.h"
#include "IF_HAL_Cfg.h"
#include "IF_SL_Cfg.h"
#include "IF_Comm.h"
#include "PT_ModuleInput.h"

static void PT_Module_Sensor_Input(void);
static void PT_Module_Cmd_Execute(void);
static void PT_Module_Port_Input(void);
/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/
/* FUNCTION *************************************************************
 * Function Name : IF_PT_SensorInput_Task
 * Description   : 设备所需sensor输入函数
 * Parameter     : 
 * return        :               
 * END *****************************************************************/
void IF_PT_SensorInput_Task(void)
{
	PT_Module_Sensor_Input();
}
/* FUNCTION *************************************************************
 * Function Name : IF_PT_CmdExecute_Task
 * Description   : cmd执行输入函数
 * Parameter     : 
 * return        :               
 * END *****************************************************************/
void IF_PT_CmdExecute_Task(void)
{
	PT_Module_Cmd_Execute();
}
/* FUNCTION *************************************************************
 * Function Name : IF_PT_InterLockInput_Task
 * Description   : 模拟接口输入函数
 * Parameter     : 
 * return        :               
 * END *****************************************************************/
void IF_PT_PortInput_Task(void)
{
	PT_Module_Port_Input();		
}

/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/
/* FUNCTION *********************************************************************************
 * Function 	 : PT_Module_Sensor_Input
 * Description   : 匹配器命令输入函数(Cmd)
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void PT_Module_Sensor_Input(void)
{
	
	Device.InputSensor.Freq   =  IF_SL_Fpga_GetSensor(ChnN_Freq);
	Device.InputSensor.Pfwd   =  IF_SL_Fpga_GetSensor(ChnN_Pfwd);
	Device.InputSensor.Pref   =  IF_SL_Fpga_GetSensor(ChnN_Pref);
	Device.InputSensor.VSWR   =  IF_SL_Fpga_GetSensor(ChnN_VSWR);
	Device.InputSensor.VDCBias = IF_SL_Get_Sensor_DCBias(); 
}

/* FUNCTION *********************************************************************************
 * Function 	 : PT_Module_Cmd_Execute
 * Description   : 匹配器命令输入函数(Cmd)
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void PT_Module_Cmd_Execute(void)
{	
  	DeviceCmdMsg_t DeviceCmd;
	if(pdTRUE ==xQueueReceive(CmdQueue, &DeviceCmd, (TickType_t)0))
	{  
		switch(DeviceCmd.cmdId)
		{
		/********************射频电源NVM读写设置*********************/
		case CMD_SAVE_FACTORYPARAM:  //保存内存工厂配置到NVM   
			IF_SL_Nvm_ParamsRW(Write_FS_IdentificationMask);
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

		case CMD_READ_FACTORYPARAM: //读取工厂NVM到内存
			IF_SL_Nvm_ParamsRW(Read_FS_IdentificationMask);
		break;

		case CMD_READ_FSCOMMONPARAM: //读取工厂NVM到内存
			IF_SL_Nvm_ParamsRW(Read_FS_CommonMask);
		break;

		case CMD_READ_FSINTERNALPARAM:  //读取工厂NVM到内存
			IF_SL_Nvm_ParamsRW(Read_FS_InternalMask);
		break;

		case CMD_READ_FSUSERPARAM: //读取工厂NVM到内存
			IF_SL_Nvm_ParamsRW(Read_UserMask);	
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

		case CMD_READ_COMMONPARAM://读取NVM到内存
			IF_SL_Nvm_ParamsRW(Read_CommonMask);		
		break;

		case CMD_READ_INTERNALPARAM: //读取NVM到内存
			IF_SL_Nvm_ParamsRW(Read_InternalMask);	
		break;

		case CMD_READ_USERPARAM:	//读取NVM到内存
			IF_SL_Nvm_ParamsRW(Read_UserMask);		
		break;

		case CMD_RESUME_ALLPARAM:	//读取NVM到内存
			IF_SL_Nvm_ParamsRW(Write_AllResumeMask);		
		break; 	
		case CMD_SETBAUDRATE: //设置通信的波特率
			IF_SL_SetUartBaudRate(DeviceCmd.cmdData.Val);
		break;

		case CMD_DEVICERESET: //设置电源软重启
			IF_SL_ResetDevice();
		break;
		default:
		break;
		}			
	}
}
/* FUNCTION *********************************************************************************
 * Function 	 : PT_Module_Port_Input
 * Description   : 电源输入端口函数(PORT)
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void PT_Module_Port_Input(void)
{
	Device.InterlockOpen = IF_HAL_InterLockDect_SigRead();
	/**********开始监测InterLock状态(默认InterLock SW 关闭)*********/
	if(Device.InterlockOpen)
	{
		g_FaultWord.bits.InterlockOpen = 0;		
	}else
	{
		Device.ACDCState = 0;
		Device.RFDrvState =0;
		IF_SL_CmdParam_SetSetPoint(0);
		g_FaultWord.bits.InterlockOpen = 1;
	}
}

//*****************************************************************************
//* END
//*****************************************************************************
