/*
 * params.h
 *
 *  Created on: 2022年3月11日
 *      Author: Huxian
 */

#ifndef _PARAMS_IMPL_H_
#define _PARAMS_IMPL_H_
#include "stdint.h"
#include "fgipv2.h"
#include "stm32_crc32.h"
#include "if_comm.h"
#include "IF_HAL_cfg.h"
#include "IF_SL_cfg.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
#ifdef USE_GLOBE
#define EXTERN
#else
#define EXTERN extern
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
//params_interface.h
#ifdef FLASH_SECTOR_SIZE
#define FLASH_BUF_SIZE                           FLASH_SECTOR_SIZE
#else
#define FLASH_BUF_SIZE                           4096
#endif

#ifndef MAX_APP_FILE_SIZE
#define MAX_APP_FILE_SIZE                        0x32000   //200K
#endif

#define HARDWARE_VERSION_LEN           			32
#define PRODUCT_MODEL_LEN              			32
#define SERIAL_NUMBER_LEN              			32
#define TRACKING_NUMBER_LEN            			48

#define MAX_PD_RECORD_NUM               		1000
#define MAX_RFPWRVOL_NUM						1000
#pragma pack(push)
#pragma pack(1)

typedef struct _PartsParam_t
{
	uint8_t DeviceType;                              /* 1=generator, 2=match box */
	uint8_t VendorCode;                              /* 1=FG */
	uint8_t HardwareVersion[HARDWARE_VERSION_LEN];   /* less than max length with 0x00 appending */
	uint8_t ProductModel[PRODUCT_MODEL_LEN];         /* less than max length with 0x00 appending */
	uint8_t SerialNumber[SERIAL_NUMBER_LEN];         /* less than max length with 0x00 appending */
	uint8_t TrackingNumber[TRACKING_NUMBER_LEN];     /* less than max length with 0x00 appending */
} PartsParam_t;

typedef struct _CommonParam_t
{
	uint8_t  CommunicateType;				//Communication interface select   0:RS232 	1:RS485   2:LAN
	uint8_t  DeviceAddress;                  /* the range is [1, 254]*/
	uint8_t  DataRate;                       /* 0: 9600Bit/s, 1: 19200 Bit/s, 2: 38400 Bit/s, 3: 57600 Bit/s, 4: 115200Bit/s */
    uint8_t  Reserve1[6];
	
	uint8_t  MAC[6];                         /* MAC address */
	uint32_t IP;                             /* IP address */
	uint32_t Mask;                           /* subnet mask */
	uint32_t Gateway;                        /* gateway */
    uint16_t EtherCatAddress;                /* use 2 reserve1[..] for EtherCatAddress; */
	       
}CommonParam_t;

typedef struct _InternalParam_t
{              
    uint16_t AnalogVoltRange;				//AnaglogInterface Voltage Range 
	uint16_t AnalogRFPowerFactor;           //AnaglogInterface Power Factor	 100 (0~10V)--(0~1000W)
    int32_t VrmsFactor;						/* the InputSensor Vrms Probe Attenuation Factor */
    int32_t IrmsFactor;						/* the InputSensor Irms Probe Attenuation Factor */
    int32_t PhaseFactor;					/* the InputSensor Phase Probe Attenuation Factor */
	
	int32_t PIDProportion;
	int32_t PIDIntegral;
	int32_t PIDDerivatice;
	int32_t PIDErrorThr;
	
	int32_t  VSWRLimit;
	int32_t  StartPoint;
	uint32_t FpgaPulsePowerThr;
	uint16_t FpgaPulseSyncDelay;
	
	int32_t ACDCVoltGain;						/* the ACDCVolt Probe Attenuation Factor */
    int32_t ACDCVoltOffset;					/* the ACDCVolt Probe Attenuation Offset */
	int32_t ACDCCurrentGain;					/* the ACDCCurrent Probe Attenuation Factor */
    int32_t ACDCCurrentOffset;				/* the ACDCCurrent Probe Attenuation Offset */
    int32_t DrainVoltGain;					/* the DrainVolt Probe Attenuation Factor */
	int32_t DrainVoltOffset;					/* the DrainVolt Probe Attenuation Offset */
	
} InternalParam_t;


typedef struct _UserParam_t
{
	uint8_t  RegulationMode;				//Regulation Mode 0:Pfwd 1:Pdlv 2:Vdc
	uint16_t PfwdLimit;						//Forword Power Limit
	uint16_t PrefLimit;						//Reflect Power Limit
	uint16_t PvdcLimit;						//vdc Power Limit
	
	uint8_t  PrefMode;						//Reflect Power Protect Mode;
	uint16_t PrefThr;						//Reflect Power Threshold;
	uint8_t  PrefDelayOff;					//Delay time after reaching Reflect Power Threshould until RF is turn off.
	
	uint8_t  PulseMode;						//Pulse Mode 0:pulse off 1:internal pulsing 
	uint32_t PulseFreq;						//pulsing Frequency
	uint8_t  PulseDuty;						//pulsing Duty circle
	
	uint8_t  MatchMode;						//the match work mode
	uint8_t  SlowMode;						//Slow delay enable/disable
	uint16_t SlowRFOnDelay;					//Slow delay power on
	uint16_t SlowRFOffDelay;				//Slow delay power off
	int32_t  VDCFactor;		             	//the VDC Sense Probe attenaction Factor 
	uint16_t PowerOffsetFactor;				//output power Offset attenaction Factor
}UserParam_t;



typedef struct _DefaultParam_t
{
 uint8_t 	FactoryMode;         //工厂模式	
 uint8_t 	RFPowerState;		 //射频电源工作状态    	  0：RF_OFF        1：RF_ON
 uint8_t 	DDSSignState;	     //射频电源驱动DDS工作状态   0：RF_OFF        1：RF_ON
 uint8_t    SetACDCState;        //设置射频电源AC-DC工作状态   0：RF_OFF        1：RF_ON
 uint8_t 	PowerWorkMode;	     //设置电源功率工作模式   0：正常模式      1：调试模式(开环模式)
 uint8_t 	SetChannelNo;	     //设置
 uint16_t   SetPointValue;       //设置射频电源功率  0~1000
 uint32_t 	SetACDCVolt;	    //设置射频电源AC-DC电压值  0~160000
 uint32_t   SetACDCCurrent;     //设置射频电源AC-DC电流值，0-200
 uint32_t   SetWorkFreq;        //设置射频电源输出频率值，0-2000
 uint32_t   SetWorkPhase;       //设置射频电源输出相位值，0-2000
 uint16_t 	TargetPos[CAP_NUM];
 uint16_t   CurrentPos[CAP_NUM];
}DefaultParam_t;

//Running Status Info of the Core board...
typedef struct _RunningStatus_t
{

    int16_t  DcBiasVol;
    uint16_t LastTuningTime;
    uint32_t RfOnTime;
    uint16_t StatusWord;
	
    int32_t  InputSensorFreq;
    int32_t  InputSensorVrms;
    int32_t  InputSensorIrms;
    int32_t  InputSensorPhase;
	
    int32_t  OutputSensorFreq;
    int32_t  OutputSensorVrms;
    int32_t  OutputSensorIrms;
    int32_t  OutputSensorPhase;
	
    int32_t  AuxOutputSensorFreq;
    int32_t  AuxOutputSensorVrms;
    int32_t  AuxOutputSensorIrms;
    int32_t  AuxOutputSensorPhase;
	
    int32_t TunedLoad_R; 			//provided by the algorithm2G, LoadRo = LoadRi;
    int32_t TunedLoad_X; 			//provided by the algorithm2G, LoadXo = LoadXi - jw(RearL);
	
    uint32_t AliveTime;
	

	
	uint32_t FgpaPulseFreq;
	uint8_t  FpgaPulseDutyCircle;
	uint16_t FpgaPulseSyncDelay;

} RunningStatus_t;
typedef struct _ProcessDataRecord_t
{
    uint32_t  ACDCVolt;
    uint32_t  Pfwd;
	uint32_t  Pref;
	uint32_t  vswr;
} PDORecord_t;

typedef struct _ProcessData_t
{
    uint16_t RecordNum;
    PDORecord_t *Records;

} ProcessData_t;

#define PARTS_PARAM_LEN					  		  sizeof(PartsParam_t)
#define COMMUN_PARAM_LEN                   		  sizeof(CommonParam_t)
#define INTERNAL_PARAM_LEN                        sizeof(InternalParam_t)
#define USER_PARAM_LEN                            sizeof(UserParam_t)	
#define RUNNING_STATUS_LEN						  sizeof(RunningStatus_t)
	
#define PD_ONE_RECORD_LEN                         sizeof(PDORecord_t)
#define PROCESS_DATA_LEN                          sizeof(ProcessData_t)
#define PD_RECORD_NUM_EACH_FRAME                  (240/PD_ONE_RECORD_LEN)  //Data max len 248
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma pack(pop)





///////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum
{
	ID_SAVE_FS_PARTS_PARAM    ,
	ID_SAVE_FS_COMMON_PARAM		,
	ID_SAVE_FS_INTERNAL_PARAM	,
	ID_SAVE_FS_USER_PARAM	 	,
	
	ID_SAVE_PARTS_PARAM       ,
	ID_SAVE_COMMON_PARAM		,
	ID_SAVE_INTERNAL_PARAM		,
	ID_SAVE_USER_PARAM			,
	ID_SAVE_RFPWRVOLT_PARAM 	,
	
	ID_READ_FS_PARTS_PARAM		,
	ID_READ_FS_COMMON_PARAM		,
	ID_READ_FS_INTERNAL_PARAM 	,
	ID_READ_FS_USER_PARAM		,
	
	ID_READ_PARTS_PARAM			,
	ID_READ_COMMON_PARAM		,
	ID_READ_INTERNAL_PARAM		,
	ID_READ_USER_PARAM			,
	
	ID_RESUME_ALLPARAMS			,
	//设置匹配器控制
	ID_MOVE_LOADTOPOS			,
	ID_MOVE_TUNETOPOS			,
			
	//设置波特率
	ID_SET_BAUDRATE				,
	//设置设备重启	
	ID_SET_DEVICERESET			,			
	
	ID_SET_ALLNUM				,

} ActionsRSPEnum;

typedef union _StatusWord_t
{
	uint32_t Dword;

	struct
	{
		uint32_t RFOnOrOff		  		: 1;  // bit0,  射频电源开关状态						(1)
		uint32_t RFOnRequeseted   		: 1;  // bit1,  射频电源开关请求状态  					(1)
		uint32_t MatchConnected   		: 1;  // bit2,  匹配器状态连接状态    					(1)
		uint32_t MatchMode		  		: 1;  // bit3,  匹配器工作模式状态    					(1)  
		uint32_t bit4	  				: 1;  // bit4,  射频电源控制状态      					(1)
		uint32_t MotorMoving      		: 1;  // bit5,  匹配器电容移动状态        				(1) 
		
		uint32_t RFWillswitchoff  		: 1;  // bit6,  射频电源反射功率超过阈值，即将关闭状态	(1) 		
		uint32_t ExternalPulseshort     : 1;  // bit7,  脉冲值到达脉冲的上限或者下限. 			(1) 
		uint32_t PowerAtLimit     	    : 1;  // bit8,  电压或电流超过内部SMPS的极限.			(1) 
		
		uint32_t PforwMaximunReached    : 1;  // bit9,  达到真实位置所需的正向功率高于最大正向功率(1) 
		uint32_t PreflAtLimit      	    : 1;  // bit10, 反射功率超过设置反射功率上限			(1) 
		uint32_t PforwAtLimit     	    : 1;  // bit11, 正向功率超过设置正向功率上限			(1) 
		uint32_t FrequecyAtLimit        : 1;  // bit12, 电源频率值到底脉冲的上限或者下限		(1)
		uint32_t LocalOrRemote	  		: 2;  // bit13-14,  射频电源控制状态(0:UART_MODE 1:ANALOG_MODE 2:PANEL MODE)     (2)
		uint32_t bit13 					: 1;  // bit13, 保留									(1) 
		uint32_t bit14					: 1;  // bit14, 保留
		uint32_t bit15					: 1;  // bit15, 保留
		uint32_t ACDCStatusOK			: 1;  // 保留
		uint32_t ACDCEn_AC				: 1;  // 保留
		uint32_t ACDCEn_PA				: 1;  // 保留	
		uint32_t ACDCKout_Fb		    : 1;  // 保留
		uint32_t bit20				    : 1;  // 保留
		uint32_t bit21				    : 1;  // 保留	
		uint32_t bit22				    : 1;  // 保留
		uint32_t bit23				    : 1;  // 保留
		uint32_t bit24				    : 1;  // 保留	
		uint32_t bit25				    : 1;  // 保留
		uint32_t bit26				    : 1;  // 保留
		uint32_t bit27				    : 1;  // 保留	
		uint32_t bit28				    : 1;  // 保留
		uint32_t bit29				    : 1;  // 保留
		uint32_t bit30				    : 1;  // 保留	
		uint32_t bit31				    : 1;  // 保留
	} bits;

}StatusWord_t;

typedef union _FaultWord_t
{
	uint32_t Dword;
	struct
	{
		uint32_t InterlockOpen     	   		: 1;  // bit0, InterLock故障
		uint32_t OverPCBTempAlarm	   		: 1;  // bit1, 主控板温度超过上限故障
		uint32_t Power24VAlarm      		: 1;  // bit2, 24V电压低或高故障
		uint32_t ACDCOverCurrentAlarm   	: 1;  // bit3, AC-DC 输出电流上限故障
		uint32_t ACDCUnderVoltageAlarm      : 1;  // bit4, AC-DC 输出电压下限故障
		uint32_t ACDCOverVoltageAlarm       : 1;  // bit5, AC-DC 输出电压上限故障
		uint32_t ACDCOverDCTempAlarm   	    : 1;  // bit6, AC-DC 输出温度上限故障
		uint32_t OverFpgaTempAlarm      	: 1;  // bit7, Fpga  输出温度异常故障
		
		uint32_t OverRFTempAlarm       		: 1;  // bit8,  射频驱动板温度超过上限故障
		uint32_t ACDCFanWorkAlarm        	: 1;  // bit9,  AC-DC散热风扇故障
		uint32_t RFFanOrWaterAlarm     		: 1;  // bit10, 射频驱动板散热风扇故障
	
		uint32_t AV380_HAlarm	   			: 1;  // bit11, AC380 A相电压高故障
		uint32_t AV380_LAlarm				: 1;  // bit12, AC380 A相电压低故障
		uint32_t AI380_HAlarm				: 1;  // bit13, AC380 A相电流高故障
		
		uint32_t BV380_HAlarm	   			: 1;  // bit14, AC380 B相电压高故障
		uint32_t BV380_LAlarm				: 1;  // bit15, AC380 B相电压低故障
		uint32_t BI380_HAlarm				: 1;  // bit16, AC380 B相电流高故障
		
		uint32_t CV380_HAlarm	   			: 1;  // bit17, AC380 C相电压高故障
		uint32_t CV380_LAlarm				: 1;  // bit18, AC380 C相电压低故障
		uint32_t CI380_HAlarm				: 1;  // bit19, AC380 C相电流高故障
		
		uint32_t DCDC1V_HAlarm				: 1;  // bit20, DCDC1  电压高故障
		uint32_t DCDC1V_LAlarm				: 1;  // bit21, DCDC1  电压低故障
		uint32_t DCDC1I_HAlarm				: 1;  // bit22, DCDC1  电流高故障
		uint32_t DCDC1T_HAlarm				: 1;  // bit23, DCDC1  超时故障	
		
		uint32_t DCDC2V_HAlarm				: 1;  // bit24, DCDC2  电压高故障
		uint32_t DCDC2V_LAlarm				: 1;  // bit25, DCDC2  电压低故障
		uint32_t DCDC2_HAlarm				: 1;  // bit26, DCDC2  电流高故障
		uint32_t DCDC2T_HAlarm				: 1;  // bit27, DCDC2  超时故障
		
		uint32_t bit28				   		: 1;  // bit28, 		
		uint32_t bit29				   		: 1;  // bit29, 
		uint32_t bit30				   		: 1;  // bit30, 
		uint32_t bit31				   		: 1;  // bit31, 			
	}bits;

}FaultWord_t;

///////////////////////////////////////////////////////////////////////////////////////////////////
// 以下协议和应用中使用到的变量或参数

// 以下是应用中使用到的变量或参数
EXTERN FaultWord_t 		g_FaultWord;
EXTERN StatusWord_t 	g_StatusWord;
EXTERN ProcessData_t    g_ProcessData;
EXTERN RunningStatus_t	g_RunningStatus;
EXTERN uint8_t  FpgaVersion[FPGAWARE_VERSION_LEN];
#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _PARAMS_IMPL_H_ */
