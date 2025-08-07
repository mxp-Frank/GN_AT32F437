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

#define CAP_NUM									(2)
#define MAIN_TASK_PERIOD       					(1)  
#define PID_LOOP_TIMER							(10)
#define PWR_UP_LIMIT(x)							(x*2)
typedef enum
{
    STANDBY_STATE     = 0,
    INTLK_STATE       = 1,	
    IDLE_STATE        = 2,
    OPEN_STATE		  = 3,
	RUN_STATE		  = 4,
}RunStateEnum;

typedef struct
{
    RunStateEnum   Now;
    RunStateEnum   Last;
}RunState_t;
typedef enum
{
    RF_OFF        = 0,
    RF_ON         = 1
}RFEnum;

typedef struct 
{
	uint8_t         State;          //状态	
	float			Freq;		    //频率
	float         	Pfwd;       	//正向功率
	float			Pdlv;			//传输功率
	float         	Pref;       	//反射功率
	float			VSWR;			//驻波比
	float 			Gamma;			//反射系数	
	uint16_t        VDCBias;        //匹配器VDC电压
}Sensor_t;	

typedef struct 
{
	Sensor_t    Now;
	Sensor_t	Last;
}OutSensor_t;	

typedef struct 
{
	uint32_t      	 PointLimit;
	uint32_t         PID_Cnt;  		//PID过程计数
	uint32_t         PhaseLimit;    //设备PID驱动相位的限制		
}SetPID_t;

typedef struct 
{
	uint32_t Point;
	uint32_t Phase;
}PwrState_t;


typedef struct _GN_Device_t
{ 	
	RunState_t		 RunState;    		//监测状态
	OutSensor_t      Sensor;	        //设备sensor值
	uint16_t		 SetPower;          //设备上位机设置功率
	PwrState_t       InitPower;         //初始化值
	PwrState_t       TargetPower;       //设备目标值
	SetPID_t	     SetPID;            //设备PID调整
	uint16_t         ACDCSlow_Cnt;		//ACDC缓启动计数值
}GN_Device_t;

typedef struct 
{
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
extern void IF_Module_Input_Task(void);
extern void IF_Module_Main_Task(void);
extern void IF_Module_Sensor_Task(void);
extern void IF_Module_CmdExecute_Task(void);
extern void IF_Module_Output_Task(void);

#if defined(__cplusplus)
extern "C" }
#endif /* __cplusplus */
#endif


//*****************************************************************************
//* END
//*****************************************************************************

