/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  SL_Cfg.c
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-06
 * ****************************************************************************
 */


#include "SL_Cfg.h"
#include "IF_SL_Cfg.h"
#include "IF_HAL_Cfg.h"
 /* CONST & MACROS */

 /* DATA STRUCTURES */

 /* LOCAL VARIABLES */

 /* GLOBAL VARIABLES */

 /* FUNCTION PROTOTYPES */
#ifdef SL_CFG_DEBUG
static void _SL_CfgDebug(void);
#endif


/************************************************************************/
/* Global Functions Definitions                                         */
/************************************************************************/
/* FUNCTION *********************************************************************************
 * Function Name : IF_SL_CfgInit
 * Description   : SL层 Cfg函数初始化接口
 * Parameter     : 
 * Parameter     :                               
 * END ***************************************************************************************/
void IF_SL_CfgInit(void)
{   
    IF_NvmInit();
    IF_SensorInit();
    IF_TimerInit();
	IF_InterfaceInit();	
	IF_CommInit();
}
	
void IF_SL_WDOG_FEED(void)
{
    IF_HAL_WDOG_FEED();
}

//-----------------------------
//---SL Interface for APP

//Boot SL interface
void IF_SL_Bootloader(void)
{
    IF_Bootloader();
}

/**************Comm SL interface**********************/
/* FUNCTION *********************************************************************************
 * Function Name : IF_SL_UartTask1
 * Description   : User Uart1任务函数接口
 * Parameter     : 
 * Parameter     :                               
 * END ***************************************************************************************/
void IF_SL_UartTask1(void)
{
    IF_UartTask1();
}

/* FUNCTION *********************************************************************************
 * Function Name : IF_SL_UartTask2
 * Description   : Debug Uart2任务函数接口
 * Parameter     : 
 * Parameter     :                               
 * END ***************************************************************************************/
void IF_SL_UartTask2(void)
{
    IF_UartTask2();
}
/* FUNCTION *********************************************************************************
 * Function Name : IF_SL_UartTask3
 * Description   : Lcd Uart3任务函数接口
 * Parameter     : 
 * Parameter     :                               
 * END ***************************************************************************************/
void IF_SL_UartTask3(void)
{
   IF_UartTask3();
}
/* FUNCTION *********************************************************************************
 * Function Name : IF_SL_UartTask3
 * Description   : Lcd Uart3任务函数接口
 * Parameter     : 
 * Parameter     :                               
 * END ***************************************************************************************/
void IF_SL_UartReciveTask4(void)
{
   IF_UartReciveTask4();
}
void IF_SL_UartSendTask4(void)
{
   IF_UartSendTask4();
}
/**************Nvm SL interface*******************/

//Sensor SL interface
/* FUNCTION *********************************************************************************
 * Function Name : IF_SL_Sensor_Fpga_Task
 * Description   : Sensor任务函数接口
 * Parameter     : 
 * Parameter     :                               
 * END ***************************************************************************************/
void IF_SL_Sensor_Fpga_Task(void)
{
    IF_Sensor_Fpga_Task();
}

/* FUNCTION *********************************************************************************
 * Function Name : IF_SL_Sensor_Fpga_Task
 * Description   : Sensor任务函数接口
 * Parameter     : 
 * Parameter     :                               
 * END ***************************************************************************************/
void IF_SL_Sensor_Device_Task(void)
{
	 IF_Sensor_Device_Task();
}
/* FUNCTION *********************************************************************************
 * Function Name : IF_SL_InterfaceInput_Task
 * Description   : Interface任务函数接口
 * Parameter     : 
 * Parameter     :                               
 * END ***************************************************************************************/
void IF_SL_InterfaceInput_Task(void)
{
	IF_InterfaceInput();
}	
/**********************Fpga Sensor Layer****************************************/
float IF_SL_Fpga_GetSensor(uint8_t ChnNo,uint8_t Pwr_Chn)
{
	float fvalue;
	fvalue = IF_Fpga_GetRegAlgSensor(ChnNo,Pwr_Chn);
	return fvalue;
}
uint32_t IF_SL_Fpga_GetSyncOutMeasureFrequency(void)
{
	return IF_Fpga_GetSyncOutMeasureFrequency();
}
uint8_t IF_SL_Fpga_GetSyncOutMeasureDutyCircle(void)
{
	return IF_Fpga_GetSyncOutMeasureDutyCircle();
}
int16_t IF_SL_Get_Sensor_DCBias(void)
{
	int16_t value =0;
	value = IF_Sensor_GetDCBias();
	return value;
}	
/**********************Interface  Output Layer****************************************/
void IF_SL_SetInterfaceOutput(IOSignEnum IOSwitch,uint8_t OnorOff)
{
	 IF_SetNormalOutput_SIG(IOSwitch, OnorOff);
}
/*********************DAC SL **********************************/
void IF_SL_EXDAC_SetReflectPowerOutput(float value)
{
	IF_HAL_EXDAC_SET(CHANNEL_A,value);
}

void IF_SL_EXDAC_SetForwardPowerOutput(float value)
{
	IF_HAL_EXDAC_SET(CHANNEL_B,value);
}

void IF_SL_EXDAC_SetACDCVoltageOutput(float value)
{
	IF_HAL_EXDAC_SET(CHANNEL_C,value);
}
/**********************Internal Parameters*******************************/
//PhasePoint of parameter
uint16_t IF_SL_InternalParam_GetInitPoint(void)
{
	uint16_t Value = 0;
	Value = IF_InternalParam_GetInitPoint();
	return Value;
}
//PulseRFPowerThr of parameter 
uint32_t IF_SL_InternalParam_GetFpgaPulsePowerThr(void)
{
	uint32_t value = 0;
	value = IF_InternalParam_GetFpgaPulsePowerThr();
	return value;
}	
void IF_SL_InternalParam_SetFpgaPulsePowerThr(uint32_t value)
{
	IF_InternaleParam_SetFpgaPulsePowerThr(value);
}
//FpgaPulseSyncDelay of parameter 
uint16_t IF_SL_InternalParam_GetFpgaPulseSyncDelay(void)
{
	uint16_t value = 0;
	value = IF_InternalParam_GetFpgaPulseSyncDelay();
	return value;
}	
void IF_SL_InternalParam_SetFpgaPulseSyncDelay(uint16_t value)
{
	IF_InternalParam_SetFpgaPulseSyncDelay(value);
}
/*****************************User Parameters*****************************/
//RegulationMode of parameter
uint8_t IF_SL_UserParam_GetRegulationMode(void)
{
	uint8_t value = 0;
	value = IF_UserParam_GetRegulationMode();
	return value;
}
void IF_SL_UserParam_SetRegulationMode(uint8_t value)
{
	IF_UserParam_SetRegulationMode(value);
}
//ForwardPowerLimit of parameter 
uint16_t IF_SL_UserParam_GetForwardPowerLimit(void)
{
	uint16_t value = 0;
	value = IF_UserParam_GetForwardPowerLimit();
	return value;
}
void IF_SL_UserParam_SetForwardPowerLimit(uint16_t value)
{	
	IF_UserParam_SetForwardPowerLimit(value);
}
//ReflectPowerLimit of parameter 
uint16_t  IF_SL_UserParam_GetReflectPowerLimit(void)
{
	uint16_t value = 0;
	value = IF_UserParam_GetReflectPowerLimit();
	return value;
}
void  IF_SL_UserParam_SetReflectPowerLimit(uint16_t value)
{
	IF_UserParam_SetReflectPowerLimit(value);
}
//DCBiasPowerLimit of parameter
uint16_t  IF_SL_UserParam_GetDCBiasPowerLimit(void)
{
	uint16_t value = 0;
	value = IF_UserParam_GetDCBiasPowerLimit();
	return value;
}
void  IF_SL_UserParam_SetDCBiasPowerLimit(uint16_t value)
{	
	IF_UserParam_SetDCBiasPowerLimit(value);
}
//ReflectPowerDelayOff of parameter
uint8_t IF_SL_UserParam_GetReflectPowerDelayOff(void)
{
	return IF_UserParam_GetReflectPowerDelayOff();
}	
uint16_t IF_SL_UserParam_GetReflectPowerThreshold(void)
{
	uint16_t value = 0;
	value = IF_UserParam_GetReflectPowerThreshold();
	return value;
}
//MatchMode of parameter
uint8_t IF_SL_UserParam_GetMatchMode(void)
{
	uint8_t value = 0;
	value = IF_UserParam_GetMatchMode();
	return value;
}	
void IF_SL_UserParam_SetMatchMode(uint8_t value)
{
	IF_UserParam_SetMatchMode(value);
}

//SlowMode of parameter
uint8_t IF_SL_UserParam_GetSlowMode(void)
{
	uint8_t value = 0;
	value = IF_UserParam_GetSlowMode();
	return value;
}
void IF_SL_UserParam_SetSlowMode(uint8_t value)
{
	IF_UserParam_SetSlowMode(value);
}
//SlowRFOnDelay of parameter
uint16_t IF_SL_UserParam_GetSlowStartDelay(void)
{
	uint16_t value = 0;
	value = IF_UserParam_GetSlowStartDelay();
	return value;
}
void IF_SL_UserParam_SetSlowStartDelay(uint16_t value)
{
	IF_UserParam_SetSlowStartDelay(value);
}
//SlowRFOffDelay of parameter
uint16_t IF_SL_UserParam_GetSlowStopDelay(void)
{
	uint16_t value = 0;
	value = IF_UserParam_GetSlowStopDelay();
	return value;
}
void IF_SL_UserParam_SetSlowStopDelay(uint16_t value)
{
	IF_UserParam_SetSlowStopDelay(value);
}
//VDCFactor of parameter
uint32_t IF_SL_UserParam_GetVDCFactor(void)
{
	uint32_t value =0;
	value = IF_UserParam_GetVDCFactor();
	return value;
}
void IF_SL_UserParam_SetVDCFactor(uint32_t value)
{
	IF_UserParam_SetVDCFactor(value);
}

/**********************Nvm Layer****************************************/
void IF_SL_Nvm_WriteSystemResetTimes(void)
{
	static uint16_t SystemPowerTimer = 0;
	static uint8_t powerUp10sFlagCleared = OFF;
	/******上电后10s将系统复位次数清零********/
	if(powerUp10sFlagCleared == OFF)
	{
		if(IF_Timer_GetPowerUpTimer() >= 10) 
		{
			IF_NvmParam_WriteSystemResetTimes(0);
			powerUp10sFlagCleared = ON;	
		}
	}
	/******上电后1hour将系统将累计上电时间加1次数********/
	if(IF_Timer_GetPowerUpDurationFlag()== ON)
	{
		SystemPowerTimer= IF_NvmParam_ReadSystemTotalTimes()+1;
		IF_NvmParam_WriteSystemTotalTimes(SystemPowerTimer);
		IF_Timer_SetPowerUpDurationFlag(OFF);
	}	
}	
void IF_SL_Nvm_ParamsRW(NVMRWMask_Enum NVMRW_Mask)
{
   IF_NvmParamsRW(NVMRW_Mask);
}

/**********************Timer Layer*******************************************/
uint32_t IF_SL_Timer_GetPowerUpTimer(void)
{
	return IF_Timer_GetPowerUpTimer();
}

float IF_SL_Timer_GetRFSlowStartDelayTime(void)
{
	return IF_Timer_GetRFSlowStartDelayTime();
}	

void IF_SL_Timer_SetRFSlowStartDelayFlag(uint8_t OnorOff)
{
	IF_Timer_SetRFSlowStartDelayFlag(OnorOff);
}
float IF_SL_Timer_GetRFSlowStopDelayTime(void)
{
	return IF_Timer_GetRFSlowStopDelayTime();
}
void IF_SL_Timer_SetRFSlowStopDelayFlag(uint8_t OnorOff)
{
	IF_Timer_SetRFSlowStopDelayFlag(OnorOff);
}

void IF_SL_Timer_SetRFOffDelayFlag(uint8_t OnorOff)
{
	IF_Timer_SetRFOffDelayFlag(OnorOff);
}
uint8_t IF_SL_Timer_GetRFOffDelayTimeOutFlag(void)
{
	return IF_Timer_GetRFOffDelayTimeOutFlag();
}
void IF_SL_Timer_ClearRFOffDelayTimeOutFlag(void)
{
	IF_Timer_ClearRFOffDelayTimeOutFlag();
}
uint8_t IF_SL_Timer_GetSlowStartFlag(void)
{
	return IF_Timer_GetSlowStartFlag();
}	
/**********************control and other Layer****************************************/
void  IF_SL_SetUartBaudRate(uint8_t baudrateKey)
{
	IF_HAL_SetBaudRate(baudrateKey);
}

void IF_SL_ResetDevice(void)
{
	delay_ms(100);
	NVIC_SystemReset();
}
//**************************************
uint16_t  IF_SL_CmdParam_GetPwrPoint(void)
{
	return  IF_CmdParam_GetRFPwrPoint();   
}
void  IF_SL_CmdParam_SetPwrPoint(uint16_t value)
{
	IF_CmdParam_SetRFPwrPoint(value);   
}
//**************************************
void IF_SL_CmdParam_SetDDSDriverState(uint8_t ONorOFF)
{
	IF_CmdParam_SetDDSSignSwitch(ONorOFF);
}
//**************************************
void IF_SL_CmdParam_SetACDCDriverState(uint8_t ONorOFF)
{
	 IF_CmdParam_SetACDCStateSwitch(ONorOFF);
}
//**************************************
uint8_t IF_SL_CmdParam_GetRFPowerState(void)
{
	 return IF_CmdParam_GetRFPowerSwitch();
}
void IF_SL_CmdParam_SetRFPowerState(uint8_t ONorOFF)
{
	IF_CmdParam_SetRFPowerSwitch(ONorOFF);
}
//**************************************
uint8_t IF_SL_CmdParam_GetPowerWorkMode(void)
{
	return IF_CmdParam_GetPowerWorkMode();
}

void IF_SL_CmdParam_SetPowerWorkMode(uint8_t value)
{
	IF_CmdParam_SetPowerWorkMode(value);
}	
void IF_SL_CmdParam_SetDDSPhase(uint32_t value)
{
	 IF_CmdParam_SetDDSPhase(value);
}
void IF_SL_CmdParam_SetACDCVoltage(uint32_t value)
{
	IF_CmdParam_SetACDCVoltage(value);
}


/*****************************************/
void IF_SL_UpdateRFPwrPIDProcessData(void)
{
	if( 0 == IF_GetRFPwrPIDProcessDataFlag())
	{
		IF_UpdateRFPwrPIDProcessData();
	}
}
void IF_SL_ClearRFPwrPIDProcessData(void)
{
	if(1 == IF_GetRFPwrPIDProcessDataFlag())
	{
		IF_ClearRFPwrPIDProcessData();
	}
}
void IF_SL_ClearRFPwrPIDProcessDataFlag(void)
{
	IF_SetRFPwrPIDProcessDataFlag(0);
}
/*****************************************/
void IF_SL_ExecuteAction(void)
{
	IF_Param_ExecuteActionsAfterRsp();
}
/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/
#ifdef SL_CFG_DEBUG
/**
 * ************************************************************************
 * @Brief :      
 * ************************************************************************
 */
static void _SL_CfgDebug(void)
{

}
#endif



/*End:SL_CFG.c*/

