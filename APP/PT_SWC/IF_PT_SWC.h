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
#define MAIN_TASK_PERIOD       					(2)  
#define PID_INIT_TIMER						    (2)
#define PID_LOOP_TIMER							(10)
#define THR_RATE								(1.06F)
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

typedef struct 
{
	uint32_t         Volt_Cnt;  	//PID过程计数
	uint32_t         VoltLimit;    //设备PID驱动电压的限制
	
	uint32_t         Phase_Cnt;     //PID过程计数
	uint32_t         PhaseLimit;    //设备PID驱动相位的限制		
}SetPID_t;

typedef struct
{
	uint32_t         Phase;			//设备阈值相位值
	uint32_t         Volt;			//设备阈值电压值
	uint32_t         Power;			//设备阈值功率值
}AjustThr_t;	

typedef struct 
{
	uint32_t Point;
	uint32_t Phase;
	uint32_t Volt;
}PwrState_t;


typedef struct _GN_Device_t
{ 	
	RunState_t		 RunState;    		//I监测状态
	RFPower_t        RFPwrState;        //射频工作状态
	Sensor_t         Sensor;	        //设备sensor值	
	
	PwrState_t       TargetPower;       //设备目标值
	PwrState_t 		 LastPower;       	//上次设备输出值
	
	AjustThr_t		 AjustThr;          //设备阈值调整
	AjustThr_t		 LastAjustThr;      //上次设备阈值调整
	
	SetPID_t	     SetPID;            //设备PID调整
	uint32_t		 SetPower;          //设备上位机设置功率	
	uint32_t 		 PowerLimit;        //设备设置功率上限
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
extern void IF_Module_Output_Task(void);
extern void IF_Module_Input_Task(void);
extern void IF_Module_CmdExecute_Task(void);

#if defined(__cplusplus)
extern "C" }
#endif /* __cplusplus */
#endif


//*****************************************************************************
//* END
//*****************************************************************************

