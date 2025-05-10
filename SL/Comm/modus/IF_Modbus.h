/******************************************************************
 * 文件：IF_Modbus.h
 * 功能：声明modbus相关函数
 * 日期：2023-05-15
 * 作者：xingping.meng
 * 版本：Ver.1.0 
 * 
 * Copyright (C) 2019 chengyuan-tech. All rights reserved.
*******************************************************************/
#ifndef _IF_MODBUS_H_
#define _IF_MODBUS_H_

#include "board.h"
#include "modbus.h"
#include "fgipv2_layer2.h"

#define MODBUS_TX_PERIOD						100

typedef enum
{	//uinit: ms
	DEVICE_READ_VERSIONREG                      = 0,       //读版本输入寄存器
	DEVICE_READ_HOLDREG                         = 1,	   //读保持寄存器
	DEVICE_READ_PROCSSREG                       = 2,       //读过程输入寄存器	
	
	DEVICE_READ_COILS                           = 3,	   //读线圈状态
	DEVICE_WRITE_SINGLECOIL                     = 4,	   //写单个线圈状态	
	DEVICE_WRITE_MULTCOILS                      = 5,	   //写多个线圈状态
	
	DEVICE_WRITE_SETTINGEREG                    = 6,       //写设置保持寄存器
	DEVICE_WRITE_PARAMREG                       = 7,	   //写参数保持寄存器
	DEVICE_WRITE_HARDWAREREG                    = 8,       //写硬件版本保持寄存器
	DEVICE_WRITE_SOFTWAREREG                    = 9,       //写软件版本保持寄存器

	DEVICE_NUM								    = 11
} ModBusTypeEnum;

extern void Dealwith_Modbus_Service(pCommMsg_t pRxMsg);
extern void Modbus_ReceiveChar(uint8_t port, uint8_t ch);
extern void Modbus_TransmitChar(uint8_t port);
extern void Modbus_TxBufManagment(uint8_t port);

void IF_ACDC_SetParamsRWType(ModBusTypeEnum ModbusType);

//ACDC Sensor output value
extern uint8_t IF_Sensor_GetInputKout_feedback(void);
extern uint8_t IF_Sensor_GetInputEn_AC(void);
extern uint8_t IF_Sensor_GetInputEn_PA(void);
extern int32_t IF_Sensor_GetACInputVoltage(uint8_t channel);
extern int32_t IF_Sensor_GetACInputCurrent(uint8_t channel);
extern int32_t IF_Sensor_GetPAVoltage(uint8_t channel);
extern int32_t IF_Sensor_GetPACurrent(uint8_t channel);
extern int32_t IF_Sensor_GetACDCTemperature(uint8_t channel);
extern uint8_t IF_Sensor_GetACDCAppVersion(uint8_t *pBuf);
extern uint8_t IF_Sensor_GetACDCBootVersion(uint8_t *pBuf);
extern uint8_t IF_Sensor_GetACDCHardWareVersion(uint8_t *pBuf);
extern void  IF_Sensor_SetACDCHardWareVersion(uint8_t *pBuf,uint8_t len);
extern uint32_t IF_Sensor_GetACDCAppChecksum(void);

//ACDC parameters value
extern void IF_ModbusSetting_SetV_Set(int32_t value);
extern int32_t IF_ModbusSetting_GetV_Set(void);

extern void IF_ModbusSetting_SetI_Set(int32_t value);
extern int32_t IF_ModbusSetting_GetI_Set(void);

extern void IF_ModbusParam_SetFactor_Ac380_AV(int32_t value);
extern int32_t IF_ModbusParam_GetFactor_Ac380_AV(void);

extern void IF_ModbusParam_SetFactor_Ac380_BV(int32_t value);
extern int32_t IF_ModbusParam_GetFactor_Ac380_BV(void);

extern void IF_ModbusParam_SetFactor_Ac380_CV(int32_t value);
extern int32_t IF_ModbusParam_GetFactor_Ac380_CV(void);

extern void IF_ModbusParam_SetFactor_Ac380_AI(int32_t value);
extern int32_t IF_ModbusParam_GetFactor_Ac380_AI(void);

extern void IF_ModbusParam_SetFactor_Ac380_BI(int32_t value);
extern int32_t IF_ModbusParam_GetFactor_Ac380_BI(void);

extern void IF_ModbusParam_SetFactor_Ac380_CI(int32_t value);
extern int32_t IF_ModbusParam_GetFactor_Ac380_CI(void);

extern void IF_ModbusParam_SetFactor_Way3_I(int32_t value);
extern int32_t IF_ModbusParam_GetFactor_Way3_I(void);

extern void IF_ModbusParam_SetFactor_Way4_I(int32_t value);
extern int32_t IF_ModbusParam_GetFactor_Way4_I(void);
	
extern void IF_ModbusParam_SetFactor_DcDc1_I(int32_t value);
extern int32_t IF_ModbusParam_GetFactor_DcDc1_I(void);

extern void IF_ModbusParam_SetFactor_DcDc1_V(int32_t value);
extern int32_t IF_ModbusParam_GetFactor_DcDc1_V(void);

extern void IF_ModbusParam_SetFactor_DcDc2_I(int32_t value);
extern int32_t IF_ModbusParam_GetFactor_DcDc2_I(void);

extern void IF_ModbusParam_SetFactor_DcDc2_V(int32_t value);
extern int32_t IF_ModbusParam_GetFactor_DcDc2_V(void);

extern void IF_ModbusParam_SetHardverion(uint8_t *pBuf, uint8_t len);
extern uint8_t IF_ModbusParam_GetHardverion(uint8_t *pBuf);

///////////////////////////////////////////////////////////////////////
extern void IF_NvmParam_SetModbusSettings(uint8_t *pBuf, uint16_t len);
extern void IF_NvmParam_GetModbusSettings(uint8_t *pBuf, uint16_t len);

extern void IF_NvmParam_SetModbusParams(uint8_t *pBuf, uint16_t len);
extern void IF_NvmParam_GetModbusParams(uint8_t *pBuf, uint16_t len);

extern void IF_NvmParam_SetModbusVersons(uint8_t *pBuf, uint16_t len);
extern void IF_NvmParam_GetModbusVersons(uint8_t *pBuf, uint16_t len);
#endif  //_MODBUS_H_


/***************************************************************************
End
****************************************************************************/
