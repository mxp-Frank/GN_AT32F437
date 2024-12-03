/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Sensor_2G.h
 * @Brief :      
 * @Author :     Chenghanbing (iwanthaha@sina.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-21
 * ****************************************************************************
 */

/************************************************************************
 * sample_app.h
 *
 *  Created on: 2022年4月19日
 *      Author: Huxian
 ************************************************************************/

#ifndef _SENSOR_2G_H_
#define _SENSOR_2G_H_
#include "stdint.h"
#define REGNUM_R 						 255
#define REGNUM_W 						 255

#define LF_PULSE_RFPOWERTHR_ADDR         108
#define HF_PULSE_RFPOWERTHR_ADDR		 112

#define LF_PULSE_SYNC_DELAY_ADDR         116
#define HF_PULSE_SYNC_DELAY_ADDR		 120

#define LF_PULSE_SYNC_MODE_ADDR          124
#define HF_PULSE_SYNC_MODE_ADDR		 	 128

#define FPGAWARE_VERSION_LEN			 20

#define M_PI		(3.1416)
#define ARC			(-180/M_PI)

typedef struct _SensorReg_2G_t
{
	uint8_t Version[FPGAWARE_VERSION_LEN];
	int32_t Freq;
    int32_t Voltage;
    int32_t Current;
    int32_t Phase;
    int32_t Vpp;
	int32_t MainVolt;
	int32_t MainCur;
	int32_t MainPha;
	int32_t AuxiVolt;
	int32_t AuxiCur;
	int32_t AuxiPha;
	int32_t PulseMode;
	int32_t Duty;
	int32_t Period;
}SensorReg_t;



typedef struct _SensorData_2G_t
{
	float Freq;  //unit: MHz
	float V_rms;
	float I_rms;
	float Phase;  //unit: angle degree, not radiums
	float P_dlv;
	float P_fwd;
	float P_ref;
	float Z;
	float R;
	float X;
	float VSWR;
	float Vpp;  //unit: Vpp
}SensorData_2G_t, *PSensorData_2G_t;

typedef struct _SensorOutData_2G_t
{
	float V_rms;
	float I_rms;
	float Phase;  //unit: angle degree, not radiums
	float P_dlv;
	float P_fwd;
	float P_ref;
	float Z;
	float R;
	float X;
	float VSWR;
}SensorOutputData_2G_t, *PSensorOutputData_2G_t;

extern void IF_Sensor_2G_ParamInit(void);
extern void Sensor_2G_Task(void);
extern uint8_t Sensor_2G_RFOnChk(uint8_t ChnNo);

extern float Get_Sensor_2G_InputFreq(uint8_t ChnNo);
extern float Get_Sensor_2G_InputVrms(uint8_t ChnNo);
extern float Get_Sensor_2G_InputIrms(uint8_t ChnNo);
extern float Get_Sensor_2G_InputPhase(uint8_t ChnNo);
extern float Get_Sensor_2G_InputPdlv(uint8_t ChnNo);
extern float Get_Sensor_2G_InputPfwd(uint8_t ChnNo);
extern float Get_Sensor_2G_InputPref(uint8_t ChnNo);
extern float Get_Sensor_2G_InputR(uint8_t ChnNo);
extern float Get_Sensor_2G_InputX(uint8_t ChnNo);
extern float Get_Sensor_2G_InputZ(uint8_t ChnNo);
extern float Get_Sensor_2G_InputVSWR(uint8_t ChnNo);

extern float Get_Sensor_2G_OutputFreq(uint8_t ChnNo);
extern float Get_Sensor_2G_OutputVpp(uint8_t ChnNo);
extern float Get_Sensor_2G_OutputVrms(uint8_t ChnNo);
extern float Get_Sensor_2G_OutputIrms(uint8_t ChnNo);
extern float Get_Sensor_2G_OutputPhase(uint8_t ChnNo);
extern float Get_Sensor_2G_OutputPdlv(uint8_t ChnNo);
extern float Get_Sensor_2G_OutputPfwd(uint8_t ChnNo);
extern float Get_Sensor_2G_OutputPref(uint8_t ChnNo);
extern float Get_Sensor_2G_OutputR(uint8_t ChnNo);
extern float Get_Sensor_2G_OutputX(uint8_t ChnNo);
extern float Get_Sensor_2G_OutputZ(uint8_t ChnNo);
extern float Get_Sensor_2G_OutputVSWR(uint8_t ChnNo);

extern float Get_Sensor_2G_AuxOutputFreq(uint8_t ChnNo);
extern float Get_Sensor_2G_AuxOutputVpp(uint8_t ChnNo);
extern float Get_Sensor_2G_AuxOutputVrms(uint8_t ChnNo);
extern float Get_Sensor_2G_AuxOutputIrms(uint8_t ChnNo);
extern float Get_Sensor_2G_AuxOutputPhase(uint8_t ChnNo);
extern float Get_Sensor_2G_AuxOutputPdlv(uint8_t ChnNo);
extern float Get_Sensor_2G_AuxOutputPfwd(uint8_t ChnNo);
extern float Get_Sensor_2G_AuxOutputPref(uint8_t ChnNo);
extern float Get_Sensor_2G_AuxOutputR(uint8_t ChnNo);
extern float Get_Sensor_2G_AuxOutputX(uint8_t ChnNo);
extern float Get_Sensor_2G_AuxOutputZ(uint8_t ChnNo);
extern float Get_Sensor_2G_AuxOutputVSWR(uint8_t ChnNo);

extern float Get_Sensor_2G_DCBias(void);
extern float Get_Sensor_2G_Vpp(uint8_t ChnNo);

extern uint8_t Get_Sensor_2G_PulseDutyCircle(uint8_t ChnNo);

extern uint8_t Get_Sensor_2G_PulseMode(uint8_t ChnNo);
extern uint32_t Get_Sensor_2G_PulseFreq(uint8_t ChnNo);


extern void Set_Sensor_2G_PulseRFPowerThr(uint8_t ChnNo,int32_t value);
extern void Set_Sensor_2G_PulseSyncDelay(uint8_t ChnNo,int32_t value);
extern uint32_t Get_Sensor_2G_PulseDynamicSyncDelay(uint8_t ChnNo);

extern void Set_Sensor_2G_PulseSyncMode(uint8_t ChnNo,int32_t value);

extern void Set_Sensor_2G_FpgaRegister(uint8_t RegAddress,int32_t value);
extern uint32_t Get_Sensor_2G_FpgaRegister(uint8_t RegAddress);

extern uint8_t Get_Sensor_2G_Version(uint8_t* pBuf);
#endif /* _SENSOR_2G_H_ */
