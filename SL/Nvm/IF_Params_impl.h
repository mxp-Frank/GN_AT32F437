/*
 * params.h
 *
 *  Created on: 2022年3月11日
 *      Author: Huxian
 */

#ifndef _IF_PARAMS_IMPL_H_
#define _IF_PARAMS_IMPL_H_
#include "stdint.h"
#include "fgipv2.h"
#include "stm32_crc32.h"
#include "if_comm.h"
#include "params_impl.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#ifdef USE_GLOBE
#define EXTERN
#else
#define EXTERN extern
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
#define NVM_PARAM_MASK     						0x00000001

typedef enum 
{	
	
	Write_Fs_PartsMask  	     = 	(NVM_PARAM_MASK<<0x00),
	Write_FS_CommonMask			 = 	(NVM_PARAM_MASK<<0x01),
	Write_FS_InternalMask		 = 	(NVM_PARAM_MASK<<0x02),	
	Write_FS_UserMask			 =	(NVM_PARAM_MASK<<0x03),
		
	Write_PartsMask  	         = 	(NVM_PARAM_MASK<<0x05),
	Write_CommonMask			 =  (NVM_PARAM_MASK<<0x06),
	Write_InternalMask			 = 	(NVM_PARAM_MASK<<0x07),	
	Write_UserMask				 = 	(NVM_PARAM_MASK<<0x08),
	Write_ModbusMask			 = 	(NVM_PARAM_MASK<<0x09),
	Write_VoltMapMask          =  (NVM_PARAM_MASK<<0x0A),
	Write_PhaseMapMask         =  (NVM_PARAM_MASK<<0x0B),	
}NVMRWMask_Enum;

///////////////////////////////////////////////////////////////////////////////////////////////////
extern void Params_Init(void);

///////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------------------
// Identification parameter Operation
extern void IF_IdentParam_SetDeviceType(uint8_t value);
extern uint8_t IF_IdentParam_GetDeviceType(void);

extern void IF_IdentParam_SetVendorCode(uint8_t value);
extern uint8_t IF_IdentParam_GetVendorCode(void);

extern void IF_IdentParam_SetHardwareVersion(uint8_t* pBuf, uint8_t len);
extern uint8_t IF_IdentParam_GetHardwareVersion(uint8_t* pBuf);

extern uint8_t IF_IdentParam_GetSoftwareVersion(uint8_t* pBuf);

extern uint8_t IF_IdentParam_GetBootloaderVersion(uint8_t* pBuf);

extern void IF_IdentParam_SetProductModel(uint8_t* pBuf, uint8_t len);
extern uint8_t IF_IdentParam_GetProductModel(uint8_t* pBuf);

extern void IF_IdentParam_SetSerialNumber(uint8_t* pBuf, uint8_t len);
extern uint8_t IF_IdentParam_GetSerialNumber(uint8_t* pBuf);

extern void IF_IdentParam_SetTrackingNumber(uint8_t* pBuf, uint8_t len);
extern uint8_t IF_IdentParam_GetTrackingNumber(uint8_t* pBuf);

extern uint32_t IF_IdentParam_GetCheckSum(void);

//-------------------------------------------------------------------------------------------------
// Communication parameter Operation
extern void IF_CommParam_SetDeviceAddress(uint8_t value);
extern uint8_t IF_CommParam_GetDeviceAddress(void);

extern void IF_CommParam_SetNetMAC(uint8_t* pBuf);
extern uint8_t IF_CommParam_GetNetMAC(uint8_t* pBuf);

extern void IF_CommParam_SetNetIP(uint32_t value);
extern uint32_t IF_CommParam_GetNetIP(void);

extern void IF_CommParam_SetNetMask(uint32_t value);
extern uint32_t IF_CommParam_GetNetMask(void);

extern void IF_CommParam_SetNetGateway(uint32_t value);
extern uint32_t IF_CommParam_GetNetGateway(void);

extern void IF_CommParam_SetDataRate(uint8_t value);
extern uint8_t IF_CommParam_GetDataRate(void);

extern void IF_CommParam_SetEtherCatAddress(uint16_t value);
extern uint16_t IF_CommParam_GetEtherCatAddress(void);
//---------------------------------------------------------------
extern void IF_CommParam_SetCommunicateType(uint8_t value);
extern uint8_t IF_CommParam_GetCommunicateType(void);
//-------------------------------------------------------------------------------------------------
//PT Configurable Internal Parameters
extern void IF_InternalParam_SetSensorVrmsFactor(int32_t value);
extern int32_t IF_InternalParam_GetSensorVrmsFactor(void);

extern void IF_InternalParam_SetSensorIrmsFactor(int32_t value);
extern int32_t IF_InternalParam_GetSensorIrmsFactor(void);

extern void IF_InternalParam_SetSensorPhaseFactor(int32_t value);
extern int32_t IF_InternalParam_GetSensorPhaseFactor(void);

extern void IF_InternalParam_SetSensorVrmsOffset(int32_t value);
extern int32_t IF_InternalParam_GetSensorVrmsOffset(void);

extern void IF_InternalParam_SetSensorIrmsOffset(int32_t value);
extern int32_t IF_InternalParam_GetSensorIrmsOffset(void);

extern void IF_InternalParam_SetSensorPhaseOffset(int32_t value);
extern int32_t IF_InternalParam_GetSensorPhaseOffset(void);

extern void IF_InternalParam_SetAnalogVoltRange(uint16_t value);
extern uint16_t IF_InternalParam_GetAnalogVoltRange(void);

extern void IF_InternalParam_SetAnalogRFPowerFactor(uint16_t value);
extern uint16_t IF_InternalParam_GetAnalogRFPowerFactor(void);

extern void IF_InternalParam_SetPhasePIDProportion(int32_t value);
extern int32_t IF_InternalParam_GetPhasePIDProportion(void);
	
extern void IF_InternalParam_SetPhasePIDIntegral(int32_t value);
extern int32_t IF_InternalParam_GetPhasePIDIntegral(void);

extern void IF_InternalParam_SetPhasePIDDerivatice(int32_t value);
extern int32_t IF_InternalParam_GetPhasePIDDerivatice(void);

extern void IF_InternalParam_SetPhasePIDErrorThr(int32_t value);
extern int32_t IF_InternalParam_GetPhasePIDErrorThr(void);

extern void IF_InternalParam_SetVoltPIDProportion(int32_t value);
extern int32_t IF_InternalParam_GetVoltPIDProportion(void);
	
extern void IF_InternalParam_SetVoltPIDIntegral(int32_t value);
extern int32_t IF_InternalParam_GetVoltPIDIntegral(void);

extern void IF_InternalParam_SetVoltPIDDerivatice(int32_t value);
extern int32_t IF_InternalParam_GetVoltPIDDerivatice(void);

extern void IF_InternalParam_SetVoltPIDErrorThr(int32_t value);
extern int32_t IF_InternalParam_GetVoltPIDErrorThr(void);

extern void IF_InternalParam_SetVSWRLimit(int32_t value);
extern int32_t IF_InternalParam_GetVSWRLimit(void);

extern void IF_InternalParam_StartPoint(int32_t value);
extern int32_t IF_InternalParam_GetPhasePoint(void);

extern void IF_InternalParam_SetACDCVoltGain(int32_t value);
extern int32_t IF_InternalParam_GetACDCVoltGain(void);

extern void IF_InternalParam_SetACDCVoltOffset(int32_t value);
extern int32_t IF_InternalParam_GetACDCVoltOffset(void);

extern void IF_InternalParam_SetACDCCurrentGain(int32_t value);
extern int32_t IF_InternalParam_GetACDCCurrentGain(void);

extern void IF_InternalParam_SetACDCCurrentOffset(int32_t value);
extern int32_t IF_InternalParam_GetACDCCurrentOffset(void);

extern void IF_InternalParam_SetDrainVoltGain(int32_t value);
extern int32_t IF_InternalParam_GetDrainVoltGain(void);

extern void IF_InternalParam_SetDrainVoltOffset(int32_t value);
extern int32_t IF_InternalParam_GetDrainVoltOffset(void);
//---------------------------------------------------------------
extern uint8_t IF_InternalParam_GetDDSChannelNo(void);
extern void IF_InternalParam_SetDDSChannelNo(uint8_t value);
//---------------------------------------------------------------
extern uint32_t IF_InternalParam_GetWorkCenterFreq(void);
extern void IF_InternalParam_SetWorkCenterFreq(uint32_t value);
//---------------------------------------------------------------
extern uint32_t IF_InternalParam_GetACDCCurrent(void);
extern void IF_InternalParam_SetACDCCurrent(uint32_t value);
//---------------------------------------------------------------
extern void IF_InternaleParam_SetFpgaPulsePowerThr(uint32_t value);
extern uint32_t IF_InternalParam_GetFpgaPulsePowerThr(void);
//---------------------------------------------------------------
extern void IF_InternalParam_SetFpgaPulseSyncDelay(uint16_t value);
extern uint16_t IF_InternalParam_GetFpgaPulseSyncDelay(void);
//-------------------------------------------------------------------------------------------------
//AM Configurable User Parameters
extern void IF_UserParam_SetRegulationMode(uint8_t value);
extern uint8_t IF_UserParam_GetRegulationMode(void);
//---------------------------------------------------------------
extern void IF_UserParam_SetForwardPowerLimit(uint16_t value);
extern uint16_t IF_UserParam_GetForwardPowerLimit(void);
//---------------------------------------------------------------
extern void IF_UserParam_SetReflectPowerLimit(uint16_t value);
extern uint16_t IF_UserParam_GetReflectPowerLimit(void);
//---------------------------------------------------------------
extern void IF_UserParam_SetDCBiasPowerLimit(uint16_t value);
extern uint16_t IF_UserParam_GetDCBiasPowerLimit(void);
//---------------------------------------------------------------
extern void IF_UserParam_SetReflectPowerSwitchOff(uint8_t value);
extern uint8_t IF_UserParam_GetReflectPowerSwitchOff(void);
//---------------------------------------------------------------
extern void IF_UserParam_SetReflectPowerThreshold(uint16_t value);
extern uint16_t IF_UserParam_GetReflectPowerThreshold(void);
//---------------------------------------------------------------
extern void IF_UserParam_SetReflectPowerDelayOff(uint8_t value);
extern uint8_t IF_UserParam_GetReflectPowerDelayOff(void);
//---------------------------------------------------------------
extern void IF_UserParam_SetSlowStartDelay(uint16_t value);
extern uint16_t IF_UserParam_GetSlowStartDelay(void);
//---------------------------------------------------------------
extern void IF_UserParam_SetSlowStopDelay(uint16_t value);
extern uint16_t IF_UserParam_GetSlowStopDelay(void);
//---------------------------------------------------------------
extern void IF_UserParam_SetPulseMode(uint8_t value);
extern uint8_t IF_UserParam_GetPulseMode(void);
//---------------------------------------------------------------
extern void IF_UserParam_SetSlowMode(uint8_t value);
extern uint8_t IF_UserParam_GetSlowMode(void);
//---------------------------------------------------------------
extern void IF_UserParam_SetPulseFrequency(uint32_t value);
extern uint32_t IF_UserParam_GetPulseFrequency(void);
//---------------------------------------------------------------
extern void IF_UserParam_SetPulseDutyCircle(uint8_t value);
extern uint8_t IF_UserParam_GetPulseDutyCircle(void);

extern void IF_UserParam_SetAnalogVoltageRange(uint32_t value);
extern uint32_t IF_UserParam_GetAnalogVoltageRange(void);

extern void IF_UserParam_SetVDCFactor(uint32_t value);
extern uint32_t IF_UserParam_GetVDCFactor(void);

extern void IF_UserParam_SetMatchMode(uint8_t value);
extern uint8_t IF_UserParam_GetMatchMode(void);

extern void IF_UserParam_SetVDCOffset(int32_t value);
extern int32_t IF_UserParam_GetVDCOffset(void);
/***************PT Control and Other Parameters********************/
extern void IF_NvmParam_SetVoltMapMap(int32_t value,uint16_t index);
extern int32_t IF_NvmParam_GetVoltMapTable(uint16_t index);
extern void IF_NvmParam_SetPhaseMapMap(int32_t value,uint16_t index);
extern int32_t IF_NvmParam_GetPhaseMapTable(int32_t powermw);

/***************PT Control and Other Parameters********************/
extern void IF_CmdParam_SetPwrPoint(uint16_t value);
extern uint16_t IF_CmdParam_GetPwrPoint(void);
//---------------------------------------------------------------
extern uint8_t IF_CmdParam_GetDDSDriverState(void);
extern void IF_CmdParam_SetDDSDriverState(uint8_t value);
//---------------------------------------------------------------
extern uint32_t IF_CmdParam_GetACDCDriverState(void);
extern void IF_CmdParam_SetACDCDriverState(uint32_t value);
//---------------------------------------------------------------
extern void IF_CmdParam_SetPowerWorkMode(uint8_t value);
extern uint8_t IF_CmdParam_GetPowerWorkMode(void);
//---------------------------------------------------------------
extern uint32_t IF_CmdParam_GetACDCVoltage(void);
extern void IF_CmdParam_SetACDCVoltage(uint32_t value);
//---------------------------------------------------------------
extern uint32_t IF_CmdParam_GetDDSWorkPhase(void);
extern void IF_CmdParam_SetDDSWorkPhase(uint32_t value);

extern void IF_CmdParam_SetResetDevice(void);
extern void IF_CmdParam_ResumeFactorySettings(void);

extern void IF_CmdParam_SetRFPowerState(uint8_t value);
extern uint8_t IF_CmdParam_GetRFPowerState(void);

extern void IF_CmdParam_SetFactoryMode(uint8_t value);
extern uint8_t IF_CmdParam_GetFactoryMode(void);
//-------------------------------------------------------------------------------------------------
//Executable Action
extern uint16_t IF_CmdParam_GetMatchCurrentPos(uint8_t capIndex);
extern uint16_t IF_CmdParam_GetMatchMoveToPos(uint8_t capIndex);
extern void IF_CmdParam_SetMatchMoveToPos(uint8_t capIndex, uint16_t value);
//-------------------------------------------------------------------------------------------------

void IF_NvmParam_SetPartsParams(uint8_t  *pBuf, uint16_t len);
void IF_NvmParam_GetPartsParams(uint8_t  *pBuf, uint16_t len);
//-------------------------------------------------------------------------------------------------
void IF_NvmParam_SetInternalParams(uint8_t  *pBuf, uint16_t len);
void IF_NvmParam_GetInternalParams(uint8_t  *pBuf, uint16_t len);
//-------------------------------------------------------------------------------------------------
void IF_NvmParam_SetUserParams(uint8_t  *pBuf, uint16_t len);
void IF_NvmParam_GetUserParams(uint8_t  *pBuf, uint16_t len);
//-------------------------------------------------------------------------------------------------

extern uint32_t IF_GetTotaltRFPwrPIDProcessDataLength(void);
extern uint16_t IF_GetRFPwrPIDProcessData(uint16_t frameNo, uint8_t* pBuf);
extern void IF_UpdateRFPwrPIDProcessData(void);
extern void IF_ClearRFPwrPIDProcessData(void);
extern uint16_t IF_GetRFPwrPIDProcessDataRecordNum(void);



///////////////////////////////////////////////////////////////////////////////////////////////////
//软件升级使用的外部函数，函数须在外部实现
extern uint8_t IF_NvmParam_WriteDownloadFile(uint32_t destAddrOffset, uint16_t* data, uint32_t size);
extern uint32_t IF_NvmParam_ReadDownloadFileLen(void);
extern uint8_t IF_NvmParam_CheckDownloadFile(void);
extern uint8_t IF_NvmParam_WriteDownloadFlag(void);

extern void IF_Param_ExecuteActionsAfterRsp(void);
extern void IF_NvmParamsRW(NVMRWMask_Enum NVM_Mask);

#if defined(__cplusplus)
 }
#endif /* __cplusplus */

#endif /* _PARAMS_IMPL_H_ */
