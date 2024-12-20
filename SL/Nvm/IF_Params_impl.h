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
	Read_Fs_PartsMask	 	     = 	(NVM_PARAM_MASK<<0),
	Read_FS_CommonMask			 = 	(NVM_PARAM_MASK<<1),
	Read_FS_InternalMask		 = 	(NVM_PARAM_MASK<<2),	
	Read_FS_UserMask			 = 	(NVM_PARAM_MASK<<3),
	
	Write_Fs_PartsMask  	     = 	(NVM_PARAM_MASK<<5),
	Write_FS_CommonMask			 = 	(NVM_PARAM_MASK<<6),
	Write_FS_InternalMask		 = 	(NVM_PARAM_MASK<<7),	
	Write_FS_UserMask			 =	(NVM_PARAM_MASK<<8),
	
	Read_PartsMask	 		     = 	(NVM_PARAM_MASK<<10),
	Read_CommonMask				 = 	(NVM_PARAM_MASK<<11),
	Read_InternalMask			 = 	(NVM_PARAM_MASK<<12),	
	Read_UserMask				 = 	(NVM_PARAM_MASK<<13),
	
	Write_PartsMask  	         = 	(NVM_PARAM_MASK<<15),
	Write_CommonMask			 =  (NVM_PARAM_MASK<<16),
	Write_InternalMask			 = 	(NVM_PARAM_MASK<<17),	
	Write_UserMask				 = 	(NVM_PARAM_MASK<<18),
	Write_ModbusMask			 = 	(NVM_PARAM_MASK<<19),
	
	Write_AllResumeMask			 =  (NVM_PARAM_MASK<<20), 	
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
//-------------------------------------------------------------------------------------------------
//Executable Action

//-------------------------------------------------------------------------------------------------
//PT Configurable Internal Parameters
extern void IF_InternalParam_SetSensorVrmsFactor(int32_t value);
extern int32_t IF_InternalParam_GetSensorVrmsFactor(void);

extern void IF_InternalParam_SetSensorIrmsFactor(int32_t value);
extern int32_t IF_InternalParam_GetSensorIrmsFactor(void);

extern void IF_InternalParam_SetSensorPhaseFactor(int32_t value);
extern int32_t IF_InternalParam_GetSensorPhaseFactor(void);

extern void IF_InternalParam_SetAnalogVoltRange(uint16_t value);
extern uint16_t IF_InternalParam_GetAnalogVoltRange(void);

extern void IF_InternalParam_SetAnalogRFPowerFactor(uint16_t value);
extern uint16_t IF_InternalParam_GetAnalogRFPowerFactor(void);

extern void IF_InternalParam_SetProportion(int32_t value);
extern int32_t IF_InternalParam_GetProportion(void);
	
extern void IF_InternalParam_SetIntegral(int32_t value);
extern int32_t IF_InternalParam_GetIntegral(void);

extern void IF_InternalParam_SetDerivatice(int32_t value);
extern int32_t IF_InternalParam_GetDerivatice(void);

extern void IF_InternalParam_SetPIDErrorThr(int32_t value);
extern int32_t IF_InternalParam_GetPIDErrorThr(void);

extern void IF_InternalParam_SetVSWRLimit(int32_t value);
extern int32_t IF_InternalParam_GetVSWRLimit(void);

extern void IF_InternalParam_StartPoint(int32_t value);
extern int32_t IF_InternalParam_GetInitPoint(void);

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

//-------------------------------------------------------------------------------------------------
//AM Configurable User Parameters
extern void IF_UserParam_SetRegulationMode(uint8_t value);
extern uint8_t IF_UserParam_GetRegulationMode(void);

extern void IF_UserParam_SetForwardPowerLimit(uint16_t value);
extern uint16_t IF_UserParam_GetForwardPowerLimit(void);

extern void IF_UserParam_SetReflectPowerLimit(uint16_t value);
extern uint16_t IF_UserParam_GetReflectPowerLimit(void);

extern void IF_UserParam_SetDCBiasPowerLimit(uint16_t value);
extern uint16_t IF_UserParam_GetDCBiasPowerLimit(void);

extern void IF_UserParam_SetReflectPowerSwitchOff(uint8_t value);
extern uint8_t IF_UserParam_GetReflectPowerSwitchOff(void);

extern void IF_UserParam_SetReflectPowerThreshold(uint16_t value);
extern uint16_t IF_UserParam_GetReflectPowerThreshold(void);

extern void IF_UserParam_SetReflectPowerDelayOff(uint8_t value);
extern uint8_t IF_UserParam_GetReflectPowerDelayOff(void);

extern void IF_UserParam_SetSlowStartDelay(uint16_t value);
extern uint16_t IF_UserParam_GetSlowStartDelay(void);

extern void IF_UserParam_SetSlowStopDelay(uint16_t value);
extern uint16_t IF_UserParam_GetSlowStopDelay(void);

extern void IF_CommParam_SetCommunicateType(uint8_t value);
extern uint8_t IF_CommParam_GetCommunicateType(void);

extern void IF_UserParam_SetPulseMode(uint8_t value);
extern uint8_t IF_UserParam_GetPulseMode(void);

extern void IF_UserParam_SetSlowMode(uint8_t value);
extern uint8_t IF_UserParam_GetSlowMode(void);

extern void IF_UserParam_SetPulseFrequency(uint32_t value);
extern uint32_t IF_UserParam_GetPulseFrequency(void);

extern void IF_UserParam_SetPulseDutyCircle(uint8_t value);
extern uint8_t IF_UserParam_GetPulseDutyCircle(void);

extern void IF_InternaleParam_SetFpgaPulsePowerThr(uint32_t value);
extern uint32_t IF_InternalParam_GetFpgaPulsePowerThr(void);

extern void IF_InternalParam_SetFpgaPulseSyncDelay(uint16_t value);
extern uint16_t IF_InternalParam_GetFpgaPulseSyncDelay(void);

extern void IF_UserParam_SetAnalogVoltageRange(uint32_t value);
extern uint32_t IF_UserParam_GetAnalogVoltageRange(void);

extern void IF_UserParam_SetVDCFactor(uint32_t value);
extern uint32_t IF_UserParam_GetVDCFactor(void);

extern void IF_UserParam_SetMatchMode(uint8_t value);
extern uint8_t IF_UserParam_GetMatchMode(void);


extern void IF_UserParam_SetPowerOffsetFactor(int32_t value);
extern int32_t IF_UserParam_GetPowerOffsetFactor(void);
/***************PT Control and Other Parameters********************/
extern void IF_CmdParam_SetSetPoint(uint16_t value);
extern uint16_t IF_CmdParam_GetSetPoint(void);
//---------------------------------------------------------------
extern uint8_t IF_CmdParam_GetDDSDriverState(void);
extern void IF_CmdParam_SetDDSDriverState(uint8_t value);
//---------------------------------------------------------------
extern uint32_t IF_CmdParam_GetACDCDriverState(void);
extern void IF_CmdParam_SetACDCDriverState(uint32_t value);
//---------------------------------------------------------------
extern uint8_t IF_CmdParam_GetSensorChannelNo(void);
extern void IF_CmdParam_SetSensorChannelNo(uint8_t value);
//---------------------------------------------------------------
extern void IF_CmdParam_SetPowerWorkMode(uint8_t value);
extern uint8_t IF_CmdParam_GetPowerWorkMode(void);
//---------------------------------------------------------------
extern uint32_t IF_CmdParam_GetACDCVoltage(void);
extern void IF_CmdParam_SetACDCVoltage(uint32_t value);
//---------------------------------------------------------------
extern uint32_t IF_CmdParam_GetACDCCurrent(void);
extern void IF_CmdParam_SetACDCCurrent(uint32_t value);
//---------------------------------------------------------------
extern uint32_t IF_CmdParam_GetWorkFrequency(void);
extern void IF_CmdParam_SetWorkFrequency(uint32_t value);
//---------------------------------------------------------------
extern uint32_t IF_CmdParam_GetWorkPhase(void);
extern void IF_CmdParam_SetWorkPhase(uint32_t value);

extern void IF_CmdParam_SetPowerWorkMode(uint8_t value);
extern uint8_t IF_CmdParam_GetPowerWorkMode(void);

extern void IF_CmdParam_SetResetDevice(void);
extern void IF_CmdParam_ResumeFactorySettings(void);

extern void IF_CmdParam_SetRFPowerState(uint8_t value);
extern uint8_t IF_CmdParam_GetRFPowerState(void);

extern void IF_CmdParam_SetFactoryMode(uint8_t value);
extern uint8_t IF_CmdParam_GetFactoryMode(void);


extern uint16_t IF_CmdParam_GetMatchCurrentPos(uint8_t capIndex);
extern uint16_t IF_CmdParam_GetMatchMoveToPos(uint8_t capIndex);
extern void IF_Cmd_SetMatchMoveToPos(uint8_t capIndex, uint16_t value);
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


extern uint32_t IF_GetTotalAMPDLength(void);
extern uint16_t IF_GetAMPD(uint16_t frameNo, uint8_t* pBuf);
extern void IF_UpdateAMPD(void);
extern void IF_ClearAMPD(void);
extern uint16_t IF_GetAMPDRecordNum(void);
extern uint16_t IF_GetAMPDRecordNum(void);



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
