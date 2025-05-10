/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Sensor.h
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
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

#ifndef _FPGASENSOR_H_
#define _FPGASENSOR_H_
#include "stdint.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#define REGNUM_R 						512
#define REGNUM_W 						512

#define DDS_SWITCHSTATE_ADDR         	164
#define CHANNEL_SETTING_ADDR			168
#define FREQUENCY_SETTING_ADDR         	172	
#define PAHSE_SETTING_ADDR              176
#define ACDC_VOLTAGE_ADDR         		180
#define ACDC_CURRENT_ADDR         		184
#define ACDC_SWITCHSTATE_ADDR           188

#define FPGAWARE_VERSION_LEN        	 10

#define M_PI							(double)(3.1415926F)
#define ARC								(double)(57.29578F)			  //57.29578=(180.0F/M_PI)
#define RAD_VALUE						(double)(-1048576.0F)
	
#define RMS_VALUE						(double)(134217728.0F)		 //134217728= 65536*2048	
#define ARC_RAD_VALUE                   (double)(-0.0000546415143F)   //=(180.0F/M_PI)/1048576

#define FREQ_2M_RATE					(double)(0.015625F)		     //0.015625=16/1024

typedef struct _SensorReg_t
{
	uint8_t Version[FPGAWARE_VERSION_LEN];
	int32_t temperture;
    int32_t RFDrainVolt;
    int32_t FeedbackFreq;
    int32_t FeedbackVrms;
	int32_t FeedbackIrms;
	int32_t FeedbackPhase;
	int32_t DDSSignSwitch;
	int32_t ChannelSetting;
	int32_t FreqSetting;
	int32_t PhaseSetting;
	int32_t ACDCVoltSetting;
	int32_t ACDCCurrentSetting;
	int32_t ACDCSwitchSetting;
}SensorReg_t;

typedef struct _SensorData_t
{
	float Temp;  
	float DrainVolt;  
	float Freq;  
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
	float Vpp; 	 //Vpp	
}SensorData_t, *PSensorData_t;

typedef enum Sensor_ChnN_t
{
	 ChnN_Freq   	= 0x01, 
	 ChnN_Vrms  	= 0x02,
	 ChnN_Irms  	= 0x03,
	 ChnN_Phase 	= 0x04,  
	 ChnN_Pdlv  	= 0x05,
	 ChnN_Pfwd  	= 0x06,
	 ChnN_Pref  	= 0x07,
	 ChnN_Z      	= 0x08,
	 ChnN_R      	= 0x09,
	 ChnN_X      	= 0x0A,
	 ChnN_VSWR   	= 0x0B,
	 ChnN_Temp    	= 0x0C,
	 ChnN_Drain    	= 0x0D,
}Sensor_ChnN_t;


typedef union
{
	uint32_t Val;
	uint8_t v[4];
	uint16_t w[2];
	struct
	{
		uint8_t LB;
		uint8_t HB;	
		uint8_t UB;
		uint8_t MB;
	}byte;
}UINT32_Value;

extern void IF_FpgaSensor_ParamInit(void);
extern float IF_Get_Sensor(uint8_t ChnN);


#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SENSOR_H_ */
