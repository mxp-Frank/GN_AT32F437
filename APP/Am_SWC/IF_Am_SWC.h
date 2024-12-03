/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_Am_SWC.h
 * @Brief :      
 * @Author :     Chenghanbing (iwanthaha@sina.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-20
 * ****************************************************************************
 */

#ifndef _IF_AM_SWC_H_
#define _IF_AM_SWC_H_
#include "stdint.h"
#include "configDevice.h"
#include "Common.h"

#define MAIN_TASK_PERIOD 				5
#define PROCESS_TASK_PERIOD				10
#define DATASTORAGE_TASK_PERIOD 		20
#define MAINTAINS_TASK_PERIOD			500
#define MATCHER_REGEASE_CNT             4000
#define DEVICE_CMD_LEN                  4
typedef enum
{
    STATE_INIT       = 0,    
    STATE_RUN        = 1,    
    STATE_MAINTAIN   = 2,    
    STATE_IDLE       = 3     
}StateEnum;

typedef enum
{
    MODE_MANUAL      = 0,			
    MODE_AUTO        = 1,			
    MODE_HOLD      	 = 2,			
}RunModeEnum;

typedef enum
{
    MODE_INDEX       = 0,
    MODE_REGREASE    = 1,
    MODE_NOMAINT     = 2
}MaintModeEnum;

typedef enum
{
    AM_RF_OFF        = 0,
    AM_RF_ON         = 1
}RFEnum;

typedef enum
{
    AM_NOT_TUNED 	= 0,
	AM_TUNING    	= 1,
    AM_TUNED     	= 2
}TUNEEnum;

typedef struct
{
    int8_t   SpeedDir;
    uint16_t SpeedMod;
    uint16_t MagOrPhaseErr;

    uint16_t HighSpeedThr;
    uint16_t Deadband;

}AM_AutoMatchParams_t;

typedef struct
{
    RunModeEnum   Now;
    RunModeEnum   Last;
}AM_RunMode_t;

typedef enum
{
    MAINT_INIT       = 0,
    MAINT_RUN        = 1,
    MAINT_SUCESSED   = 2,
    MAINT_FAILED     = 3
}MaintStsEnum;

typedef struct
{
    RFEnum   Now;
    RFEnum   Last;
}AM_RFPower_t;

typedef struct
{
    TUNEEnum   Now;
    TUNEEnum   Last;
}AM_Tune_t;


typedef enum
{
	AM_ACTION_NULL           = 0,    
    AM_ACTION_MOVELOAD       = 1,
    AM_ACTION_MOVETUNE       = 2,
	AM_ACTION_MOVEAUXI       = 3,
    AM_ACTION_CLEARFAULT     = 4,
	AM_ACTION_RESET			 = 5,
	AM_ACTION_PULSETHR		 = 6,
}AM_ActionEnum;
typedef struct
{
	volatile float Freq;
	volatile float Vrms;
	volatile float Irms; 
	volatile float R;
	volatile float X;
	volatile float Pfwd;
	volatile float VSWR;
}SensorInput_t;

typedef struct _FpgaPulse_t
{
	uint32_t RFPwrThr;
	uint8_t SyncMode;
	uint16_t SyncDelay;
	
}FpgaPulse_t;	
typedef struct
{
	
	StateEnum State;							
    AM_RunMode_t 	RunMode;						
    MaintModeEnum 	MaintMode;
	MaintStsEnum 	MaintStatus;						
    //automatch status and parameters
	AM_RFPower_t 	RFOnOff;						
    AM_Tune_t 	 	Tuned;	
	uint16_t 	 	IntlkState;	
	uint16_t 	 	AMFineTimer;        			
	uint16_t     	RegreaseCnt;
	SensorInput_t 	Sensor;
	FpgaPulse_t     Pulse;


}AM_Device_t;

extern SemaphoreHandle_t maintainSemaphore;

extern void IF_Am_SWCInit(void);
extern void IF_AM_Main_Task(void);
extern void IF_AM_DataStorage_Task(void);
extern void IF_AM_Maintains_Task(void);

#endif


//*****************************************************************************
//* END
//*****************************************************************************

