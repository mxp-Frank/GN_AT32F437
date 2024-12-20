/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_SL_Cfg.h
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-06
 * ****************************************************************************
 */

#ifndef _IF_SL_CFG_H_
#define _IF_SL_CFG_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "arm_math.h"

#include "IF_PrjLib.h"
#include "IF_Boot.h"
#include "IF_Sensor.h"
#include "IF_Comm.h"
#include "IF_Timer.h"
#include "IF_Nvm.h"
#include "IF_HAL_Cfg.h"
#include "params_impl.h"
#include "IF_Params_impl.h"

#include "stm32_crc32.h"
#include "IF_Comm.h"

#include "IF_Modbus.h"

#include "bsip.h"
#include "bsip_layer2.h"
#include "bsip_layer7.h"

#include "fgipv2.h"
#include "pppfcs.h"
#include "fgipv2_layer2.h"
#include "fgipv2_layer7.h"

#include "flashdb.h"

#include "configDevice.h"
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

typedef enum
{
    IOSIG_DEBUGLED     			= 0,      
    IOSIG_PWR12VON             	= 1, 
	IOSIG_DC5VON        		= 2,   	
    IOSIG_INTLOCKEN        		= 3,   
    IOSIG_PWRONLED				= 4, 
	IOSIG_FAULTLED				= 5, 
	IOSIG_WARNINGLED			= 6, 
	IOSIG_INTLOCKLED		    = 7, 
	IOSIG_RFONLED		    	= 8, 
	//Record array len
    IOSIG_MAX            		= 9
}IOSWEnum;
/******************************************************************************
* Interface functions                                                         *
******************************************************************************/
extern void IF_SL_CfgInit(void);
extern void IF_SL_WDOG_FEED(void);
extern void IF_SL_Bootloader(void);

extern void IF_SL_Sensor_Device_Task(void);
/**********Uart Interface SL **************/
extern void IF_SL_UartTask1(void);
extern void IF_SL_UartTask2(void);
extern void IF_SL_UartTask3(void);
extern void IF_SL_UartReciveTask4(void);
extern void IF_SL_UartSendTask4(void);


/**********Interface DAC SL**************/
extern void IF_SL_EXDAC_SetReflectPowerOutput(float value);	
extern void IF_SL_EXDAC_SetForwardPowerOutput(float value);	
extern void IF_SL_EXDAC_SetACDCVoltageOutput(float value);

/**********Interface Output SL**************/
void IF_SL_SetInterfaceOutput(IOSWEnum IOSwitch,uint8_t value);

/**********Nvm Parameter SL**************/

/**********Uart Parameter SL**************/
extern void IF_SL_SetUartBaudRate(uint8_t baudrate);

/************Communication Parameters SL********/

/************Internal Parameters SL********/
extern float IF_SL_InternalParam_GetVSWRLimit(void);
extern uint16_t IF_SL_InternalParam_GetInitPoint(void);
/************User parameter SL************/
extern uint8_t IF_SL_UserParam_GetStartMode(void);
extern void IF_SL_UserParam_SetStartMode(uint8_t value);

extern uint16_t IF_SL_CmdParam_GetSetPoint(void);
extern void  IF_SL_CmdParam_SetSetPoint(uint16_t value);

extern uint8_t IF_SL_UserParam_GetRegulationMode(void);
extern void  IF_SL_UserParam_SetRegulationMode(uint8_t value);

extern void IF_SL_UserParam_SetPulseMode(uint8_t value);
extern uint8_t IF_SL_UserParam_GetPulseMode(void);

extern uint32_t IF_SL_UserParam_GetPulseFrequency(void);
extern void IF_SL_UserParam_SetPulseFrequency(uint32_t value);

extern uint16_t IF_SL_UserParam_GetPulseDutyCircle(void);
extern void IF_SL_UserParam_SetPulseDutyCircle(uint16_t value);

extern uint8_t IF_SL_UserParam_GetReflectPowerDelayOff(void);

extern uint32_t IF_SL_InternalParam_GetFpgaPulsePowerThr(void);
extern void IF_SL_InternalParam_SetFpgaPulsePowerThr(uint32_t value);

extern uint16_t IF_SL_InternalParam_GetFpgaPulseSyncDelay(void);
extern void IF_SL_InternalParam_SetFpgaPulseSyncDelay(uint16_t value);

extern uint16_t IF_SL_UserParam_GetForwardPowerLimit(void);
extern void IF_SL_UserParam_SetForwardPowerLimit(uint16_t);

extern uint16_t IF_SL_UserParam_GetReflectPowerLimit(void);
extern void IF_SL_UserParam_SetReflectPowerLimit(uint16_t);

extern uint16_t IF_SL_UserParam_GetDCBiasPowerLimit(void);
extern void IF_SL_UserParam_SetDCBiasPowerLimit(uint16_t);

extern uint16_t IF_SL_UserParam_GetReflectPowerThreshold(void);

extern int32_t IF_SL_NvmParam_GetRFPwrTo50Volt(uint16_t index);

extern uint8_t IF_SL_UserParam_GetMatchMode(void);	
extern void IF_SL_UserParam_SetMatchMode(uint8_t value);

extern uint32_t IF_SL_UserParam_GetVDCFactor(void);
extern void IF_SL_UserParam_SetVDCFactor(uint32_t value);

extern uint8_t IF_SL_UserParam_GetSlowMode(void);
extern void IF_SL_UserParam_SetSlowMode(uint8_t value);

extern uint16_t IF_SL_UserParam_GetSlowStartDelay(void);
extern void IF_SL_UserParam_SetSlowStartDelay(uint16_t value);

extern uint16_t IF_SL_UserParam_GetSlowStopDelay(void);
extern void IF_SL_UserParam_SetSlowStopDelay(uint16_t value);

/****************Modbus SL***********************/
void IF_SL_ACDC_SetParamsRW(ModBusTypeEnum ModbusType);


/***************Sensor SL****************/
extern void IF_SL_SensorTask(void);
extern uint16_t IF_SL_Sensor_GetDCBias(void);

extern float IF_SL_Fpga_GetSensor(uint8_t ChnNo);

extern int16_t IF_SL_Get_Sensor_DCBias(void);
/***************Timer SL****************/
extern void IF_SL_Timer_SetPulseModeTimerFlag(uint8_t OnorOff);

extern uint16_t IF_SL_Timer_GetRFSlowStartDelayTime(void);
extern void IF_SL_Timer_SetRFSlowStartDelayFlag(uint8_t OnorOff);

extern uint16_t IF_SL_Timer_GetRFSlowStopDelayTime(void);
extern void IF_SL_Timer_SetRFSlowStopDelayFlag(uint8_t OnorOff);


extern void IF_SL_Timer_SetRFOffDelayFlag(uint8_t OnorOff);
extern uint8_t IF_SL_Timer_GetRFOffDelayTimeOutFlag(void);
extern void IF_SL_Timer_ClearRFOffDelayTimeOutFlag(void);

/****************Nvm SL***********************/
extern void IF_SL_Nvm_WriteSystemResetTimes(void);
extern void IF_SL_Nvm_ParamsRW(NVMRWMask_Enum NVMRW_Mask);

extern void IF_SL_RFPower_UpdateProcessData(void);
extern void IF_SL_RFPower_ClearProcessData(void);
/****************Cmd SL***********************/
extern void IF_SL_ResetDevice(void);
extern uint8_t IF_SL_CmdParam_GetPowerWorkMode(void);

extern uint8_t IF_SL_CmdParam_GetRFPowerState(void);
extern void IF_SL_CmdParam_SetRFPowerState(uint8_t value);

extern void IF_SL_ExecuteAction(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

 #endif /*_IF_SL_CFG_H_*/
 /*
*******************************************************************************
* END
*******************************************************************************
*/

