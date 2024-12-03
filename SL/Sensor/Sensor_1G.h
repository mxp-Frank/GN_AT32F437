/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Sensor_1G.h
 * @Brief :      
 * @Author :     Chenghanbing (iwanthaha@sina.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-21
 * ****************************************************************************
 */

/*
 * sample_app.h
 *
 *  Created on: 2022年4月19日
 *      Author: Huxian
 */

#ifndef _SENSOR_1G_H_
#define _SENSOR_1G_H_
#include "stdint.h"

typedef struct _Sample_t
{
	int16_t  MagErr;
	int16_t  PhaseErr;
	uint16_t MagErrAbs;
	uint16_t PhaseErrAbs;

	uint16_t InputRFPower;
	uint16_t InputRFVoltage;
	uint16_t InputRFCurrent;

	uint16_t DCBias;
	uint16_t Vpp;

//	int16_t  Temperature;
//	uint16_t FanCurrent;
//	uint16_t PowerSupply;

} Sample_t;


extern void Sensor_1G_Task(void);

extern uint8_t Sensor_1G_RFOnChk(void);
extern int16_t Get_Sensor_1G_PhaseErr(void);
extern uint16_t Get_Sensor_1G_PhaseErrAbs(void);
extern int16_t Get_Sensor_1G_MagErr(void);
extern uint16_t Get_Sensor_1G_MagErrAbs(void);
extern uint16_t Get_Sensor_1G_InputRFPower(void);
extern uint16_t Get_Sensor_1G_InputRFVoltage(void);
extern uint16_t Get_Sensor_1G_InputRFCurrent(void);
extern uint16_t Get_Sensor_1G_DCBias(void);
extern uint16_t Get_Sensor_1G_Vpp(void);

#endif /* _SENSOR_1G_H_ */
