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

#define FPGAWARE_VERSION_LEN        	 10
#define M_PI							(double)(3.1415926F)
#define ARC								(double)(57.29578F)			  //57.29578=(180.0F/M_PI)
#define RAD_VALUE						(double)(-1048576.0F)
	
#define RMS_VALUE						(double)(134217728.0F)		 //134217728= 65536*2048	
#define ARC_RAD_VALUE                   (double)(-0.0000546415143F)   //=(180.0F/M_PI)/1048576

#define FREQ_2M_RATE					(double)(0.015625F)		     //0.015625=16/1024

#define FPGA_SAMPLE_RATE				100

//寄存器输出地址
#define DDS_STATESWITCH_ADDR         	164
#define DDS_CHANNELNO_ADDR				168
#define DDS_FREQUENCY_ADDR         		172	
#define DDS_PAHSE_ADDR                  176

#define ACDC_STATESWITCH_ADDR           188
#define ACDC_VOLTAGE_ADDR         		180
#define ACDC_CURRENT_ADDR         		184

#define FACTOR_VRMS_ADDR				192
#define FACTOR_IRMS_ADDR				196
#define FACTOR_PHASE_ADDR				200

#define PULSEMODE_SWITCH_ADDR			204
#define PULSEMODE_DUTY_ADDR				220
#define PULSEMODE_FREQ_ADDR				224

#define SYNCSOURCE_SWITCH_ADDR			208
#define SYNCOUT_ENABLE_ADDR				212 
#define SYNCOUT_DELAY_ADDR				216

#define FEED_COLLECTIONMODE_ADDR		228
#define FEED_PREMASK_ADDR		    	232
#define FEED_POSTMASK_ADDR				236

#define PHASE_LEVELTOLEVEL_ADDR			240
#define PHASE_STEPSPEED_ADDR			244
#define PHASE_STEPTIMER_ADDR			248

typedef struct _SensorReg_t
{
	uint8_t Version[FPGAWARE_VERSION_LEN];
	int32_t temperture;
    int32_t RFDrainVolt;
    int32_t FeedbackFreq;
    int32_t FeedbackVrms;
	int32_t FeedbackIrms;
	int32_t FeedbackPhase;
	
	int32_t AlgFreq;  
	int32_t AlgVrms;
	int32_t AlgIrms;
	int32_t AlgPhase;  //unit: angle degree, not radiums
	int32_t AlgPdlv;
	int32_t AlgPfwd;
	int32_t AlgPref;
	int32_t AlgR;
	int32_t AlgX;
	int32_t AlgVSWR;
	
	int32_t SyncInDuty;
	int32_t SyncInFreq;
	
	int32_t SyncOutDutyMeasure;
	int32_t SyncOutFreqMeasure;
	
	int32_t DDSStateSwitch;
	int32_t DDSChannelNo;
	int32_t DDSFreqSet;
	int32_t DDSPhaseSet;
	int32_t ACDCVoltageSet;
	int32_t ACDCCurrentSet;
	int32_t ACDCStateSwitch;
	
	int32_t VrmsFactor;
	int32_t IrmsFactor;
	int32_t PhaseFactor;
	
	int32_t PulseMode;
	int32_t PulseDuty;
	int32_t PulseFreq;
	
	int32_t SyncSource;
	int32_t SyncOutEnable;
	int32_t SyncOutDelay;
	
	int32_t FeedCollectionMode;
	int32_t FeedPreMask;
	int32_t FeedPostMask;
	
	int32_t PhaseState2;
	int32_t PhaseStepSpeed;
	int32_t PhaseStepTimer;
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

extern void IF_FpgaSensor_ParamInit(void);
extern void Sensor_Fpga_Sample(void);
#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SENSOR_H_ */
