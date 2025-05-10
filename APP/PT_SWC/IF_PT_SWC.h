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
#define ANALOG_VOLT								(10)				//模拟口增益值	
#define CAP_NUM									2


#define STANDBY_STATE						0
#define IDLE_STATE							1
#define RUN_STATE							2

typedef enum
{
    MODE_OPEN         = 0,			
    MODE_CLOSE        = 1,					
}LoopModeEnum;

typedef struct
{
    LoopModeEnum   Now;
    LoopModeEnum   Last;
}LoopMode_t;
typedef enum
{
    AM_RF_OFF        = 0,
    AM_RF_ON         = 1
}RFEnum;

typedef struct
{
    RFEnum   Now;
    RFEnum   Last;
}RFPower_t;
		
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
	uint8_t			 InterlockOpen;    //当前Interlock状态
	uint8_t          PDO_End;		   //过程数据结束标志位
	LoopMode_t   	 LoopMode;	       //工作模式状态
	RFPower_t        RFPwrState;       //射频工作状态
	
	Sensor_t         Sensor;	       //设备sensor值	
	FpgaPulse_t		 FpgaPulse;        //设备输出脉冲值
	
	uint32_t 		 LastPower;       	   //设备输出功率值	
	uint32_t         Volt_PID_Cnt;  	//PID过程计数
	float 	 	 	 SetPIDVolt;	    //设备PID驱动电压值	
	
	uint32_t         Phase_PID_Cnt;    //PID过程计数
	float 	 	     SetPIDPhase;	   //设备PID驱动相位值	
			
	uint16_t         PowerThrDown; 	   //阈值下降斜率
}GN_Device_t;

typedef struct 
{
	__IO uint32_t  	AdjCnt;				//循环次数
	__IO float  	iError;      		//定义偏差
	__IO float  	LastError;			//前一次误差
	__IO float  	PrevError;			//前两次误差
	__IO float      ErrorThr;           //偏差阈值
}PID_Typedef;

extern uint8_t NetIP_Addr[4];
extern uint8_t SystemTimerClearFlag;
extern uint8_t ResumeSystemParamFlag;
extern GN_Device_t GN_Device;
extern SemaphoreHandle_t maintainSemaphore;

extern void IF_Module_SWCInit(void);
extern void IF_Module_Main_Task(void);
extern void IF_Module_Output_Task(void);
extern void IF_Module_PortOutput_Task(void);
extern void IF_Module_SensorInput_Task(void);
extern void IF_PT_CmdExecute_Task(void);
extern void IF_PT_PortInput_Task(void);
extern void IF_Module_Output_Task(void);

#if defined(__cplusplus)
extern "C" }
#endif /* __cplusplus */
#endif


//*****************************************************************************
//* END
//*****************************************************************************

