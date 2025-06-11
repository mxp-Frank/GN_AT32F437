/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Sensor.c
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-21
 * ****************************************************************************
 */

#include "board.h"
#include "arm_math.h"
#include "IF_HAL_Cfg.h"
#include "IF_SL_Cfg.h"
#include "FpgaSensor.h"


/* LOCAL VARIABLES */

static SensorReg_t FpgaReg;					
static SensorData_t OutputData;

static uint32_t ReadVersion_Tick = 0;
static float FpgaVrmsFactor = 0;
static float FpgaIrmsFactor = 0;
static float FpgaPhaseFactor = 0;

static uint32_t FpgaPulseDutyTimer = 0;
static uint32_t FpgaPulsePeriod = 0;

/* FUNCTION PROTOTYPES */
static uint8_t GetSensorRegisterValue(void);
static void GetInputSensorValue(void);
static void SetOutputSensorValue(void);
static float GetVSWRReflectance(float vswr_r,float vswr_x);
static uint32_t Sensor_2G_GetMcuRegister(uint8_t RegAddress);

static void Sensor_2G_GetSoftVersion(uint8_t RegAddress);

static void Fpga_SetDDSStateSwitch(UINT32_VAL WriteReg);
static void Fpga_SetDDSChannelNumber(UINT32_VAL WriteReg);
static void Fpga_SetWorkDDSFrequency(UINT32_VAL WriteReg);
static void Fpga_SetDDSWorkPhase(UINT32_VAL WriteReg);

static void Fpga_SetACDCDriverSwitch(UINT32_VAL WriteReg);
static void Fpga_SetACDCVoltageSet(UINT32_VAL WriteReg);
static void Fpga_SetACDCurrentSetting(UINT32_VAL WriteReg);

static void  Fpga_SetFpgaSensorVrmsFactor(UINT32_VAL WriteReg);
static void  Fpga_SetFpgaSensorIrmsFactor(UINT32_VAL WriteReg);
static void  Fpga_SetFpgaSensorPhaseFactor(UINT32_VAL WriteReg);

static void  Fpga_SetPulseModeSwitch(UINT32_VAL WriteReg);
static void  Fpga_SetPulseModeDutyCircle(UINT32_VAL WriteReg);
static void  Fpga_SetPulseModeFrequency(UINT32_VAL WriteReg);


static void  Fpga_SetSyncSourceSwitch(UINT32_VAL WriteReg);
static void  Fpga_SetSyncOutEnableSwitch(UINT32_VAL WriteReg);
static void  Fpga_SetSyncOutDelay(UINT32_VAL WriteReg);

static void  Fpga_SetFeedbackCollectionMode(UINT32_VAL WriteReg);
static void  Fpga_SetFeedbackPreMask(UINT32_VAL WriteReg);
static void  Fpga_SetFeedbackPostMask(UINT32_VAL WriteReg);

static void  Fpga_SetPhaseLevelToLevel(UINT32_VAL WriteReg);
static void  Fpga_SetPhaseStepSpeed(UINT32_VAL WriteReg);
static void  Fpga_SetPhaseStepTimer(UINT32_VAL WriteReg);

/* FUNCTION *********************************************************************************
 * Function Name : IF_Sensor_2G_ParamInit
 * Description   : 得到二代VI传感器的数据初始化
 * Parameter     : 无
 *                                 
 * END ***************************************************************************************/
void IF_FpgaSensor_ParamInit(void)
{
	memset((void*)&FpgaReg,0,sizeof(SensorReg_t));
	memset((void*)&OutputData,0,sizeof(SensorData_t));	
}
	
/* FUNCTION *********************************************************************************
 * Function Name : Sensor_Fpga_Sample
 * Description   : 得到Fpga VI传感器的数据任务
 * Parameter     : 无
 *                                 
 * END ***************************************************************************************/
void Sensor_Fpga_Sample(void)
{
   if( 1== GetSensorRegisterValue())
   { 
	    taskENTER_CRITICAL();
	    GetInputSensorValue();
	    SetOutputSensorValue(); 
	    taskEXIT_CRITICAL();
   }		
}

/* FUNCTION *********************************************************************************
 * Function Name : GetSensorValue(SensorData_t *pSensorData)
 * Description   : 得到二代VI传感器的FPGA寄存器器值
 * Parameter     : pSensorData VI传感器数据结构体
 *                                 
 * END ***************************************************************************************/
static uint8_t  GetSensorRegisterValue(void)
{
	
    uint8_t ret =OFF;
    /*************************读取FPGA寄存器数据到寄存器缓冲区*******************************/
    if(0 == IF_HAL_FpgaReg_ReadStart())
	{
		ReadVersion_Tick++;
		if(ReadVersion_Tick %1000 == 0)
		{
			Sensor_2G_GetSoftVersion(0);
		}
		/***********************Read Register******************************/
		FpgaReg.temperture    = Sensor_2G_GetMcuRegister(8);
		FpgaReg.RFDrainVolt   = Sensor_2G_GetMcuRegister(12);
		FpgaReg.FeedbackFreq  = Sensor_2G_GetMcuRegister(16);
		FpgaReg.FeedbackVrms  = Sensor_2G_GetMcuRegister(20);
		FpgaReg.FeedbackIrms  = Sensor_2G_GetMcuRegister(24);
		FpgaReg.FeedbackPhase = Sensor_2G_GetMcuRegister(28);
		/**********************************************************/
		FpgaReg.AlgFreq   = Sensor_2G_GetMcuRegister(32);
		FpgaReg.AlgVrms   = Sensor_2G_GetMcuRegister(36);
		FpgaReg.AlgIrms   = Sensor_2G_GetMcuRegister(40);
		FpgaReg.AlgPhase  = Sensor_2G_GetMcuRegister(44);
		FpgaReg.AlgPdlv   = Sensor_2G_GetMcuRegister(48);
		FpgaReg.AlgPfwd   = Sensor_2G_GetMcuRegister(52);
		FpgaReg.AlgPref   = Sensor_2G_GetMcuRegister(56);
		FpgaReg.AlgR   	  = Sensor_2G_GetMcuRegister(60);
		FpgaReg.AlgX      = Sensor_2G_GetMcuRegister(64);
		FpgaReg.AlgVSWR   = Sensor_2G_GetMcuRegister(68);
		/**********************************************************/
		FpgaReg.SyncInDuty  = Sensor_2G_GetMcuRegister(72);
		FpgaReg.SyncInFreq  = Sensor_2G_GetMcuRegister(76);
		FpgaReg.SyncOutDutyMeasure = Sensor_2G_GetMcuRegister(80);
		FpgaReg.SyncOutFreqMeasure = Sensor_2G_GetMcuRegister(84);
		
		/***********************Write Register***********************************/
		FpgaReg.DDSStateSwitch  = Sensor_2G_GetMcuRegister(DDS_STATESWITCH_ADDR);
		FpgaReg.DDSChannelNo 	= Sensor_2G_GetMcuRegister(DDS_CHANNELNO_ADDR);
		FpgaReg.DDSFreqSet  	= Sensor_2G_GetMcuRegister(DDS_FREQUENCY_ADDR);
		FpgaReg.DDSPhaseSet  	= Sensor_2G_GetMcuRegister(DDS_PAHSE_ADDR);
		/**********************************************************/
		FpgaReg.ACDCVoltageSet  = Sensor_2G_GetMcuRegister(ACDC_VOLTAGE_ADDR);
		FpgaReg.ACDCCurrentSet  = Sensor_2G_GetMcuRegister(ACDC_CURRENT_ADDR);
		FpgaReg.ACDCStateSwitch = Sensor_2G_GetMcuRegister(ACDC_STATESWITCH_ADDR);
		/**********************************************************/
		FpgaReg.VrmsFactor   = Sensor_2G_GetMcuRegister(FACTOR_VRMS_ADDR);
		FpgaReg.IrmsFactor   = Sensor_2G_GetMcuRegister(FACTOR_IRMS_ADDR);
		FpgaReg.PhaseFactor  = Sensor_2G_GetMcuRegister(FACTOR_PHASE_ADDR);
		/**********************************************************/
		FpgaReg.PulseMode    = Sensor_2G_GetMcuRegister(PULSEMODE_SWITCH_ADDR);
		FpgaReg.PulseDuty    = Sensor_2G_GetMcuRegister(PULSEMODE_DUTY_ADDR);
		FpgaReg.PulseFreq    = Sensor_2G_GetMcuRegister(PULSEMODE_FREQ_ADDR);
		/**********************************************************/
		FpgaReg.SyncSource     = Sensor_2G_GetMcuRegister(SYNCSOURCE_SWITCH_ADDR);
		FpgaReg.SyncOutEnable  = Sensor_2G_GetMcuRegister(SYNCOUT_ENABLE_ADDR);
		FpgaReg.SyncOutDelay   = Sensor_2G_GetMcuRegister(SYNCOUT_DELAY_ADDR);
		/**********************************************************/
		FpgaReg.FeedCollectionMode   = Sensor_2G_GetMcuRegister(FEED_COLLECTIONMODE_ADDR);
		FpgaReg.FeedPreMask   = Sensor_2G_GetMcuRegister(FEED_PREMASK_ADDR);
		FpgaReg.FeedPostMask  = Sensor_2G_GetMcuRegister(FEED_POSTMASK_ADDR);
		/**********************************************************/
		FpgaReg.PhaseState2   = Sensor_2G_GetMcuRegister(PHASE_LEVELTOLEVEL_ADDR);
		FpgaReg.PhaseStepSpeed  = Sensor_2G_GetMcuRegister(PHASE_STEPSPEED_ADDR);
		FpgaReg.PhaseStepTimer  = Sensor_2G_GetMcuRegister(PHASE_STEPTIMER_ADDR);
		
		ret = ON;
	}
    IF_HAL_FpgaReg_ReadEnd();	
	
	return ret;		
}
/* FUNCTION *********************************************************************************
 * Function Name : GetInputSensorValue(void)(低频13.560MHz)
 * Description   : 得到二代VI传感器的InputSensor计算值
 * Parameter     : pSensorData VI传感器数据结构体
 *                                 
 * END ***************************************************************************************/
static void GetInputSensorValue(void)	
{
	float PhaseRad =0,cosRad =0,sinRad =0,Z =0,R =0, X = 0,vswr_t =0,P_dlv =0,P_fwd =0,P_ref =0,VSWR = 100;
	
	float Freq =  (1026 - FpgaReg.FeedbackFreq)* FREQ_2M_RATE; 
	float V_rms = FpgaReg.FeedbackVrms*FpgaVrmsFactor/RMS_VALUE;
    float I_rms = FpgaReg.FeedbackIrms*FpgaIrmsFactor/RMS_VALUE;
	float Phase = FpgaReg.FeedbackPhase*ARC_RAD_VALUE + FpgaPhaseFactor;

	float temp = (0.123041F*FpgaReg.temperture - 273.15F);
	float DrainVot = g_FactorData.DrainGain*FpgaReg.RFDrainVolt + g_FactorData.DrainOffset;
	float P_abs = V_rms*I_rms;
	
    if(Phase < -180.0F)
    {
        Phase += 360.0F;
    }
    else if(Phase > 180.0F)
    {
        Phase -= 360.0F;
    }	
	if(V_rms >= POWER_RATIO||I_rms >= POWER_RATIO||P_abs >= POWER_RATIO)
	{
		 PhaseRad = Phase/ARC;
		 cosRad = arm_cos_f32(PhaseRad);
		 sinRad = arm_sin_f32(PhaseRad);
		 Z = V_rms/I_rms;
		 R = Z*cosRad;
		 X = Z*sinRad;
		 vswr_t = GetVSWRReflectance(R/50.0F,X/50.0F);	
		 if(vswr_t < 1)
		 {
			 VSWR =(1+vswr_t)/(1-vswr_t);
			 P_dlv = P_abs*cosRad;			 
			 P_fwd = P_dlv/(1-vswr_t*vswr_t);
			 P_ref = P_fwd - P_dlv;
		 }
	}
    /***************Input Sensor Data output***********/
	OutputData.Temp = temp;
	OutputData.DrainVolt = DrainVot;
    OutputData.Freq = Freq;
    OutputData.V_rms = V_rms;
    OutputData.I_rms = I_rms;
    OutputData.Phase = Phase;
    OutputData.P_dlv = P_dlv;
    OutputData.P_fwd = P_fwd;
    OutputData.P_ref = P_ref;
    OutputData.Z = Z;
    OutputData.R = R;
    OutputData.X = X;
	OutputData.VSWR = VSWR;
}
float IF_Fpga_GetMcuAlgSensor(uint8_t ChnN)
{
	float fvalue = 0.0F;
	switch(ChnN)
	{
		case ChnN_Temp:  fvalue = OutputData.Temp;  break;
		case ChnN_Freq:  fvalue = OutputData.Freq;	break; 
		case ChnN_Vrms:  fvalue = OutputData.V_rms;	break; 
		case ChnN_Irms:  fvalue = OutputData.I_rms; break;
		case ChnN_Phase: fvalue = OutputData.Phase;	break;
		case ChnN_Pdlv:  fvalue = OutputData.P_dlv;	break;
		case ChnN_Pfwd:  fvalue = OutputData.P_fwd;	break;
		case ChnN_Pref:  fvalue = OutputData.P_ref;	break;
		case ChnN_Z:  	 fvalue = OutputData.Z;		break;
		case ChnN_R:	 fvalue = OutputData.R;		break;
		case ChnN_X:	 fvalue = OutputData.X;		break;
		case ChnN_VSWR:  fvalue = OutputData.VSWR;	break;
		case ChnN_Drain: fvalue = OutputData.DrainVolt;	break;
	}
    return fvalue;
}
float IF_Fpga_GetRegAlgSensor(uint8_t ChnN)
{
	float fvalue = 0.0F;
	switch(ChnN)
	{
		case ChnN_Freq:  fvalue = FpgaReg.AlgFreq/1000000.0F;	break; 
		case ChnN_Vrms:  fvalue = FpgaReg.AlgVrms/1000.0F;	 	break; 
		case ChnN_Irms:  fvalue = FpgaReg.AlgIrms/1000.0F;  	break;
		case ChnN_Phase: fvalue = FpgaReg.AlgPhase/1000.0F;		break;
		case ChnN_Pdlv:  fvalue = FpgaReg.AlgPdlv/1000.0F;		break;
		case ChnN_Pfwd:  fvalue = FpgaReg.AlgPfwd/1000.0F;		break;
		case ChnN_Pref:  fvalue = FpgaReg.AlgPref/1000.0F;		break;
		case ChnN_Z:  	 fvalue = FpgaReg.AlgVrms/FpgaReg.AlgIrms;break;
		case ChnN_R:	 fvalue = FpgaReg.AlgR/1000.0F;			break;
		case ChnN_X:	 fvalue = FpgaReg.AlgX/1000.0F;			break;
		case ChnN_VSWR:  fvalue = FpgaReg.AlgVSWR/1000.0F;		break;
	}
    return fvalue;
}
uint32_t IF_Fpga_GetSyncInFrequency(void)
{
	uint32_t value = 0;
	uint32_t SyncPeriod = FpgaReg.SyncInFreq/FPGA_SAMPLE_RATE;
	value = 1000000/SyncPeriod;
	return value;
}
uint8_t IF_Fpga_GetSyncInDutyCircle(void)
{
	uint8_t value = 0;
	value = 100* FpgaReg.SyncInDuty/FpgaReg.SyncInFreq;
	return value;
}
uint32_t IF_Fpga_GetSyncOutMeasureFrequency(void)
{
	uint32_t value = 0;
	uint32_t SyncPeriod = FpgaReg.SyncOutFreqMeasure/FPGA_SAMPLE_RATE;
	value = 1000000/SyncPeriod;
	return value;
}
uint8_t IF_Fpga_GetSyncOutMeasureDutyCircle(void)
{
	uint8_t value = 0;
	value = 100* FpgaReg.SyncOutDutyMeasure/FpgaReg.SyncOutFreqMeasure;
	return value;  
}
	

static void SetOutputSensorValue(void)
{
	UINT32_VAL WriteReg;
	/******************DDS设置**************************/
	WriteReg.Val = IF_InternalParam_GetDDSChannelNo();
	Fpga_SetDDSChannelNumber(WriteReg);
	
	WriteReg.Val = IF_InternalParam_GetDDSCenterFreq();	
	Fpga_SetWorkDDSFrequency(WriteReg);
	
	WriteReg.Val = IF_CmdParam_GetDDSWorkPhase();	
	Fpga_SetDDSWorkPhase(WriteReg);
	
	WriteReg.Val = IF_CmdParam_GetDDSDriverSwitch();	
	Fpga_SetDDSStateSwitch(WriteReg);	
	
	/******************AC-DC设置**************************/
	WriteReg.Val = IF_InternalParam_GetACDCCurrent();	
	Fpga_SetACDCurrentSetting(WriteReg);
	
	WriteReg.Val = IF_CmdParam_GetACDCVoltage();
	Fpga_SetACDCVoltageSet(WriteReg);
	
	WriteReg.Val = IF_CmdParam_GetACDCDriverSwitch();
	Fpga_SetACDCDriverSwitch(WriteReg);

	/******************Sensor Factor设置**************************/
	WriteReg.Val = FpgaVrmsFactor*1000;
	Fpga_SetFpgaSensorVrmsFactor(WriteReg);
	
	WriteReg.Val = FpgaIrmsFactor*1000;
	Fpga_SetFpgaSensorIrmsFactor(WriteReg);
	
	WriteReg.Val = FpgaPhaseFactor*1000;
	Fpga_SetFpgaSensorPhaseFactor(WriteReg);
	
    /******************脉冲设置**************************/
	WriteReg.Val = IF_CmdParam_GetPulseMode();
	Fpga_SetPulseModeSwitch(WriteReg);
	
	WriteReg.Val = IF_CmdParam_GetPulseFrequency();
	Fpga_SetPulseModeFrequency(WriteReg);
	
	WriteReg.Val = IF_CmdParam_GetPulseDutyCircle();
	Fpga_SetPulseModeDutyCircle(WriteReg);
	
	/******************同步设置**************************/
	WriteReg.Val = IF_CmdParam_GetSyncSource();
	Fpga_SetSyncSourceSwitch(WriteReg);
	
	WriteReg.Val = IF_CmdParam_GetSyncOutEnable();
	Fpga_SetSyncOutEnableSwitch(WriteReg);
	
	WriteReg.Val = IF_CmdParam_GetSyncOutDelay();
	Fpga_SetSyncOutDelay(WriteReg);
	
	/******************输出同步设置**************************/
	WriteReg.Val = IF_InternalParam_GetFeedCollectionMode();
	Fpga_SetFeedbackCollectionMode( WriteReg);
	
	WriteReg.Val = IF_InternalParam_GetFeedPreMask();
	Fpga_SetFeedbackPreMask( WriteReg);
	
	WriteReg.Val = IF_InternalParam_GetFeedPostMask();
	Fpga_SetFeedbackPostMask( WriteReg);
	
	/******************相位设置**************************/
	WriteReg.Val = IF_InternalParam_GetPhaseState2();
	Fpga_SetPhaseLevelToLevel( WriteReg);
	
	WriteReg.Val = IF_InternalParam_GetPhaseStepSpeed();
	Fpga_SetPhaseStepSpeed( WriteReg);
	
	WriteReg.Val = IF_InternalParam_GetPhaseStepTimer();
	Fpga_SetPhaseStepTimer( WriteReg);
		
}	


//////////////////////////////////////////////////////////////////////////////////////////
uint8_t IF_Sensor_GetVersion(uint8_t *pBuf)
{
	uint8_t j = 0;
	for(uint8_t i = 0;i < 8;i++)
	{
		pBuf[j++] = FpgaReg.Version[i];
	}
    return j;
}
/* FUNCTION *********************************************************************************
 * Function Name : Sensor_2G_GetSoftVersion
 * Description   : 读取Fpga软件版本
 * Parameter     : 
 *                                 
 * END ***************************************************************************************/
static void Sensor_2G_GetSoftVersion(uint8_t RegAddress)
{
	uint8_t j=0, regData[8] = {0};
	for(uint16_t i = 0; i < 8; i++)
	{
		regData[i] = IF_HAL_FpgaReg_Read(RegAddress+i);
	}
	FpgaReg.Version[j++] = regData[3];
	FpgaReg.Version[j++] = regData[2];
	FpgaReg.Version[j++] = regData[1];
	FpgaReg.Version[j++] = regData[0];
	FpgaReg.Version[j++] = regData[7];	
	FpgaReg.Version[j++] = regData[6];	
	FpgaReg.Version[j++] = regData[5];	
	FpgaReg.Version[j++] = regData[4];
}	


/* FUNCTION *********************************************************************************
 * Function Name : Fpga_SetDDSStateSwitch
 * Description   : 设置DDS信号开关
 * Parameter     : value -----开关值
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetDDSStateSwitch(UINT32_VAL WriteReg)
{
	if(FpgaReg.DDSStateSwitch != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(DDS_STATESWITCH_ADDR + i,WriteReg.v[i]);
		}
	}
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_SetDDSChannelNumber
 * Description   : 设置通道信号量
 * Parameter     : value -----通道信号值
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetDDSChannelNumber(UINT32_VAL WriteReg)
{
	if(FpgaReg.DDSChannelNo != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(DDS_CHANNELNO_ADDR + i,WriteReg.v[i]);
		}
	}
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_SetWorkDDSFrequency
 * Description   : 设置频率信号值
 * Parameter     : value -----频率值(kHz)
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetWorkDDSFrequency(UINT32_VAL WriteReg)
{	
	float fvalue = 0;
	if(FpgaReg.DDSFreqSet != WriteReg.Val)
	{
		fvalue = WriteReg.Val/1000.0F;	//(MHz)
		FpgaVrmsFactor = g_FactorData.VrmsFactor *fvalue + g_FactorData.VrmsOffset;
		FpgaIrmsFactor = g_FactorData.IrmsFactor *fvalue + g_FactorData.IrmsOffset;
		FpgaPhaseFactor= g_FactorData.PhaseFactor*fvalue + g_FactorData.PhaseOffset;	
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(DDS_FREQUENCY_ADDR + i,WriteReg.v[i]);
		}
	}
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_SetDDSStateSwitchState
 * Description   : 设置相位信号值
 * Parameter     : value -----相位值
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetDDSWorkPhase(UINT32_VAL WriteReg)
{	
	WriteReg.Val = WriteReg.Val/10;
	if(FpgaReg.DDSPhaseSet != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(DDS_PAHSE_ADDR + i,WriteReg.v[i]);
		}
	}
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_SetACDCVoltageSet
 * Description   : 设置电压信号值
 * Parameter     : value -----电压值
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetACDCVoltageSet(UINT32_VAL WriteReg)
{	
	double fvalue = 0;
//	double Gain = IF_InternalParam_GetACDCVoltGain()/1000.0F;
//	if(WriteReg.Val >= IF_InternalParam_GetACDCVoltOffset())
//	{
//		fvalue = (WriteReg.Val-IF_InternalParam_GetACDCVoltOffset())/1000.0F;
//	}else 
//	{
//		fvalue = 0.0F;
//	}
//	double fvalue1 = (fvalue)/Gain;
//	double fvalue2 = (fvalue1/0.9294F)+0.0005F;
//	double fvalue3 = (fvalue2*560.0F)/1000.0F;
//	double fvalue4 = fvalue3/1.2F;
//	double fvalue5 = fvalue4/(50.0F/2700.0F);
//	double fvalue6 = (fvalue5/32.0F)*16384.0F;
//	double fvalue7 = fvalue6 + 16383.0F;
//	WriteReg.Val =(uint32_t)(fvalue7/2.0F);	
	if(WriteReg.Val >= g_FactorData.ACDCVoltOffset)
	{
		fvalue = (WriteReg.Val- g_FactorData.ACDCVoltOffset);
	}
	WriteReg.Val =(uint32_t)(fvalue/g_FactorData.ACDCVoltGain + 8191.5F);
	if(FpgaReg.ACDCVoltageSet != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(ACDC_VOLTAGE_ADDR + i,WriteReg.v[i]);
		}
	}
}

/* FUNCTION *********************************************************************************
 * Function Name : Fpga_SetACDCurrentSetting
 * Description   : 设置电流信号值
 * Parameter     : value -----电流值
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetACDCurrentSetting(UINT32_VAL WriteReg)
{
//	double fvalue = 0,fvalue1 = 0,fvalue2 = 0,fvalue3 = 0,fvalue4 = 0,fvalue5 = 0,fvalue6 = 0;
//	double Gain   = IF_InternalParam_GetACDCCurrentGain()/1000.0F;
//	double Offset = IF_InternalParam_GetACDCCurrentOffset()/1000.0F;
//	
//	fvalue = WriteReg.Val/1000.0F;
//	fvalue1 = fvalue/Gain;
//	fvalue2 = fvalue1/1.0238F;
//	fvalue3 =fvalue2/(50.0F/2700.0F);
//	fvalue4 =fvalue3/1.2F;
//	fvalue5 =fvalue4/32.0F;
//	fvalue6 =fvalue5/2.0F;
//	WriteReg.Val =(uint32_t)(fvalue6*16384.0F);
	
	WriteReg.Val =(uint32_t)(g_FactorData.ACDCCurrentGain*WriteReg.Val - g_FactorData.ACDCCurrentOffset);
	if(FpgaReg.ACDCCurrentSet != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(ACDC_CURRENT_ADDR + i,WriteReg.v[i]);
		}
	}
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_SetACDCDriverSwitch
 * Description   : 设置开关信号值
 * Parameter     : value -----开关信号值
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetACDCDriverSwitch(UINT32_VAL WriteReg)
{	
	if(FpgaReg.ACDCStateSwitch != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(ACDC_STATESWITCH_ADDR + i,WriteReg.v[i]);
		}
	}
}


/* FUNCTION *********************************************************************************
 * Function Name : Fpga_SetACDCDriverSwitch
 * Description   : 设置ACDC开关信号值
 * Parameter     : value -----开关信号值
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetFpgaSensorVrmsFactor(UINT32_VAL WriteReg)
{	

	if(FpgaReg.VrmsFactor != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(FACTOR_VRMS_ADDR + i,WriteReg.v[i]);
		}
	}
}
static void  Fpga_SetFpgaSensorIrmsFactor(UINT32_VAL WriteReg)
{	
	if(FpgaReg.IrmsFactor != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(FACTOR_IRMS_ADDR + i,WriteReg.v[i]);
		}
	}
}
static void  Fpga_SetFpgaSensorPhaseFactor(UINT32_VAL WriteReg)
{	
	if(FpgaReg.PhaseFactor != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(FACTOR_PHASE_ADDR + i,WriteReg.v[i]);
		}
	}
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_SetPulseModeSwitch
 * Description   : 设置脉冲模式开关信号值
 * Parameter     : value -----开关信号值
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetPulseModeSwitch(UINT32_VAL WriteReg)
{	
	if(FpgaReg.PulseMode != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(PULSEMODE_SWITCH_ADDR + i,WriteReg.v[i]);
		}
	}
}
static void  Fpga_SetPulseModeFrequency(UINT32_VAL WriteReg)
{	
	if(WriteReg.Val > 0)FpgaPulsePeriod = 1000000*FPGA_SAMPLE_RATE/WriteReg.Val;
	if(FpgaReg.PulseFreq != FpgaPulsePeriod)
	{
		WriteReg.Val = FpgaPulsePeriod;
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(PULSEMODE_FREQ_ADDR + i,WriteReg.v[i]);
		}
	}
}
static void  Fpga_SetPulseModeDutyCircle(UINT32_VAL WriteReg)
{	
	FpgaPulseDutyTimer = FpgaPulsePeriod*WriteReg.Val/100;
	if(FpgaReg.PulseDuty != FpgaPulseDutyTimer)
	{
		WriteReg.Val = FpgaPulseDutyTimer;
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(PULSEMODE_DUTY_ADDR + i,WriteReg.v[i]);
		}
	}
}

/* FUNCTION *********************************************************************************
 * Function Name : Fpga_SetSyncSource
 * Description   : 设置脉冲模式开关信号值
 * Parameter     : value -----开关信号值
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetSyncSourceSwitch(UINT32_VAL WriteReg)
{	
	if(FpgaReg.SyncSource != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(SYNCSOURCE_SWITCH_ADDR + i,WriteReg.v[i]);
		}
	}
}
static void  Fpga_SetSyncOutEnableSwitch(UINT32_VAL WriteReg)
{	
	if(FpgaReg.SyncOutEnable != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(SYNCOUT_ENABLE_ADDR + i,WriteReg.v[i]);
		}
	}
}
static void  Fpga_SetSyncOutDelay(UINT32_VAL WriteReg)
{	
	WriteReg.Val  = FPGA_SAMPLE_RATE * WriteReg.Val;
	if(FpgaReg.SyncOutDelay != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(SYNCOUT_DELAY_ADDR + i,WriteReg.v[i]);
		}
	}
}

/* FUNCTION *********************************************************************************
 * Function Name : Fpga_SetFeedbackCollectionMode
 * Description   : 设置脉冲模式开关信号值
 * Parameter     : value -----开关信号值
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetFeedbackCollectionMode(UINT32_VAL WriteReg)
{	
	if(FpgaReg.FeedCollectionMode != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(FEED_COLLECTIONMODE_ADDR + i,WriteReg.v[i]);
		}
	}
}
static void  Fpga_SetFeedbackPreMask(UINT32_VAL WriteReg)
{	
	WriteReg.Val = FPGA_SAMPLE_RATE*WriteReg.Val;
	if(FpgaReg.FeedPreMask != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(FEED_PREMASK_ADDR + i,WriteReg.v[i]);
		}
	}
}
static void  Fpga_SetFeedbackPostMask(UINT32_VAL WriteReg)
{	
	WriteReg.Val = FPGA_SAMPLE_RATE*WriteReg.Val;
	if(FpgaReg.FeedPostMask != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(FEED_POSTMASK_ADDR + i,WriteReg.v[i]);
		}
	}
}

/* FUNCTION *********************************************************************************
 * Function Name : Fpga_SetFeedSample
 * Description   : 设置脉冲模式开关信号值
 * Parameter     : value -----开关信号值
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetPhaseLevelToLevel(UINT32_VAL WriteReg)
{	
	WriteReg.Val = WriteReg.Val/10;
	if(FpgaReg.PhaseState2 != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(PHASE_LEVELTOLEVEL_ADDR + i,WriteReg.v[i]);
		}
	}
}
static void  Fpga_SetPhaseStepSpeed(UINT32_VAL WriteReg)
{	
	WriteReg.Val = WriteReg.Val/10;
	if(FpgaReg.PhaseStepSpeed != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(PHASE_STEPSPEED_ADDR + i,WriteReg.v[i]);
		}
	}
}
static void  Fpga_SetPhaseStepTimer(UINT32_VAL WriteReg)
{	
	WriteReg.Val = FPGA_SAMPLE_RATE*WriteReg.Val/1000;
	if(FpgaReg.PhaseStepTimer != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(PHASE_STEPTIMER_ADDR + i,WriteReg.v[i]);
		}
	}
}
/* FUNCTION *********************************************************************************
 * Function Name : static float GetVSWRReflectance(float vswr_r,float vswr_x)
 * Description   : 得到反射系数
 * Parameter     : 传感器数据结构体
 *                                 
 * END ***************************************************************************************/
static float GetVSWRReflectance(float vswr_r,float vswr_x)
{
	float vswr_rsq,vswr_xsq,vswr_r1sq,vswr_trsq,vswr_tisq,vswr_t;
	
	arm_power_f32(&vswr_r,1,&vswr_rsq);   			//vswr_r*vswr_r
	arm_power_f32(&vswr_x,1,&vswr_xsq);   			//vswr_x*vswr_x
	
	float vswr_r1= vswr_r + 1;
	arm_power_f32(&vswr_r1,1,&vswr_r1sq);   		//(vswr_r+1)*(vswr_r+1)	
	
	float vswr_square=vswr_r1sq+vswr_xsq;	
    float vswr_tr = (vswr_rsq + vswr_xsq - 1)/vswr_square;
    float vswr_ti = (vswr_x+vswr_x)/vswr_square;
	
	arm_power_f32(&vswr_tr,1,&vswr_trsq);       //vswr_tr*vswr_tr
	arm_power_f32(&vswr_ti,1,&vswr_tisq);		//vswr_ti*vswr_ti
	
	float vswr_sqrt =vswr_trsq +vswr_tisq;
    arm_sqrt_f32(vswr_sqrt, &vswr_t);
	return vswr_t;
}
/* FUNCTION *********************************************************************************
 * Function Name : Sensor_2G_GetMcuRegister
 * Description   : 得到MCU寄存器的参数
 * Parameter     : 
 *                                 
 * END ***************************************************************************************/
static uint32_t Sensor_2G_GetMcuRegister(uint8_t RegAddress)
{
	UINT32_VAL RegData;
	for(uint8_t i = 0; i <  + 4;i++)
	{
		RegData.v[i] = IF_HAL_FpgaReg_Read(RegAddress+i);
	}
	return RegData.Val;
}


//*****************************************************************************
//* END
//*****************************************************************************


