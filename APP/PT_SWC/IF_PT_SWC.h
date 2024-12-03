/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_PT_SWC.h
 * @Brief :      
 * @Author :     mengxingping (mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2023-05-20
 * ****************************************************************************
 */

#ifndef _IF_PT_SWC_H_
#define _IF_PT_SWC_H_
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "arm_math.h"
#include "IF_SL_Cfg.h"
#include "IF_HAL_cfg.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "configDevice.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#define DEVICE_CMD_LEN                  		(4)
#define DACTO50VSCALE							(16.23F)			//DAC与AC_DC50直流源之间的转换率
#define ANALOG_VOLT								(10)				//模拟口增益值	
#define CAP_NUM									2


#define STANDBY_STATE						0
#define IDLE_STATE							1
#define RUN_STATE							2

typedef struct 
{
	float			Freq;		    //频率
	float         	Pfwd;       	//正向功率
	float         	Pref;       	//反射功率
	float			VSWR;			//驻波比
	float 			Tsqure;			//反射系数	
	uint16_t        VDCBias;        //匹配器VDC电压
}Sensor_t;	
typedef struct _FpgaPulse_t
{
	uint8_t  Mode;
	uint32_t Frequency;
	uint8_t  DutyCircle; 
	uint32_t RFPowerThr;
	uint16_t SyncDelay;
}FpgaPulse_t;

typedef struct _GN_Device_t
{ 	
	uint8_t			 InterlockOpen;  //当前Interlock状态	
	Sensor_t         InputSensor;	 //当前输入sensor值	
	FpgaPulse_t		 FpgaPulse;      //设备输出脉冲值	
	uint8_t        	 ACDCState;		 //设备输出状态
	uint8_t 		 RFDrvState;     //设备射频驱动状态
	float 	 	 	 SetACDCVolt;	 	//设备AC-DC驱动电压值	
	float 	 	 	 SetACDCCurrent;	 //设备AC-DC驱动电压值	
	float 	 	 	 SetFrequency;	 	//设备AC-DC驱动电压值	
	float 	 	 	 SetPhase;	 		//设备AC-DC驱动电压值	
	float 	 	 	 MaxACDC50Volt;  //设备AC-DC驱动电压值
	uint16_t 		 OuputPower;     //设备输出功率值
	uint16_t 		 LastOuputPower;     //设备输出功率值	
	uint32_t         PID_Count;  	 //PID过程计数
	uint8_t          PDO_End;		//过程数据结束标志位。
}GN_Device_t;

typedef struct 
{
	__IO uint64_t  	AdjCnt;				//循环次数
	__IO float  	SetPower;   		//设置功率目标值
	__IO float  	ActualPower; 		//功率实际值
	__IO float  	iError;      		//定义偏差
	__IO float  	LastError;			//前一次误差
	__IO float  	PrevError;			//前两次误差
	__IO float  	Kp;					//Kp系数
	__IO float  	Ki;					//Ki系数
	__IO float 	 	Kd;					//Kd系数
}PID_Typedef;


extern float SetVoltage;
extern uint8_t NetIP_Addr[4];
extern uint8_t SystemTimerClearFlag;
extern uint8_t ResumeSystemParamFlag;
extern GN_Device_t Device;
extern SemaphoreHandle_t maintainSemaphore;

extern void IF_PT_SWCInit(void);
extern void IF_PT_Main_Task(void);
extern void IF_PT_Module_Output_Task(void);
extern void IF_PT_PortOutput_Task(void);

#if defined(__cplusplus)
extern "C" }
#endif /* __cplusplus */
#endif


//*****************************************************************************
//* END
//*****************************************************************************

