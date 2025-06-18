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
static SensorData_t OutputData[PWR_CHN_NUM];

static float FpgaVrmsFactor = 0;
static float FpgaIrmsFactor = 0;
static float FpgaPhaseFactor = 0;

/* FUNCTION PROTOTYPES */
static uint8_t GetSensorRegisterValue(void);
static void GetInputSensorValue(uint8_t Pwr_Chn);
static void SetOutputSensorValue(void);
static float GetVSWRReflectance(float vswr_r,float vswr_x);
/******************************************************/
static int32_t Sensor_2G_GetMcuRegister(uint16_t RegAddress);
static void Sensor_2G_SetMcuRegister(uint16_t RegAddress,int32_t value);
/******************************************************/
static void Fpga_SetDDSStateSwitch(int32_t WriteReg);
static void Fpga_SetDDSChannelNumber(int32_t WriteReg);
static void Fpga_SetDDSFrequency(int32_t WriteReg);
static void Fpga_SetDDSPhase(int32_t WriteReg);
/******************************************************/
static void Fpga_SetACDCStateSwitch(int32_t WriteReg);
static void Fpga_SetACDCVoltage(int32_t WriteReg);
static void Fpga_SetACDCurrent(int32_t WriteReg);
/******************************************************/
static void  Fpga_SetSensorVrmsFactor(int32_t WriteReg);
static void  Fpga_SetSensorIrmsFactor(int32_t WriteReg);
static void  Fpga_SetSensorPhaseFactor(int32_t WriteReg);
/******************************************************/
static void  Fpga_SetPulseModeSwitch(int32_t WriteReg);
static void  Fpga_SetPulseModeDutyCircle(int32_t WriteReg);
static void  Fpga_SetPulseModeFrequency(int32_t WriteReg);
/******************************************************/
static void  Fpga_SetPulseSyncSourceSwitch(int32_t WriteReg);
static void  Fpga_SetPulseSyncOutEnable(int32_t WriteReg);
static void  Fpga_SetPulseSyncOutDelay(int32_t WriteReg);
/******************************************************/
static void  Fpga_SetFeedbackCollectionMode(int32_t WriteReg);
static void  Fpga_SetFeedbackPreMask(int32_t WriteReg);
static void  Fpga_SetFeedbackPostMask(int32_t WriteReg);
/******************************************************/
static void  Fpga_SetPhaseLevelToLevel(int32_t WriteReg);
static void  Fpga_SetPhaseStepSpeed(int32_t WriteReg);
static void  Fpga_SetPhaseStepTimer(int32_t WriteReg);

/* FUNCTION *********************************************************************************
 * Function Name : IF_Sensor_2G_ParamInit
 * Description   : 得到二代VI传感器的数据初始化
 * Parameter     : 无
 *                                 
 * END ***************************************************************************************/
void IF_FpgaSensor_ParamInit(void)
{
	memset((void*)&FpgaReg,0,sizeof(SensorReg_t));
	memset((void*)&OutputData[LP_CHN],0,sizeof(SensorData_t));	
	memset((void*)&OutputData[HP_CHN],0,sizeof(SensorData_t));	
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
	    GetInputSensorValue(HP_CHN);
	    GetInputSensorValue(LP_CHN);
	    SetOutputSensorValue(); 
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
    uint8_t ret = OFF;
    /*************************读取FPGA寄存器数据到寄存器缓冲区*******************************/
    IF_HAL_FpgaReg_ReadStart();
		
	for(uint8_t i = 0; i < 8; i++)
	{
		FpgaReg.Version[i] = IF_HAL_FpgaReg_Read(i);
	}
	/***********************Read Register******************************/
	FpgaReg.temperture    = Sensor_2G_GetMcuRegister(8);
	FpgaReg.RFDrainVolt   = Sensor_2G_GetMcuRegister(12);
	/***********************Read Register******************************/
	FpgaReg.FeedbackFreq[HP_CHN]  = Sensor_2G_GetMcuRegister(16);
	FpgaReg.FeedbackVrms[HP_CHN]  = Sensor_2G_GetMcuRegister(20);
	FpgaReg.FeedbackIrms[HP_CHN]  = Sensor_2G_GetMcuRegister(24);
	FpgaReg.FeedbackPhase[HP_CHN] = Sensor_2G_GetMcuRegister(28);
	/**********************************************************/
	FpgaReg.Freq[HP_CHN]   = Sensor_2G_GetMcuRegister(32);
	FpgaReg.Vrms[HP_CHN]   = Sensor_2G_GetMcuRegister(36);
	FpgaReg.Irms[HP_CHN]   = Sensor_2G_GetMcuRegister(40);
	FpgaReg.Phase[HP_CHN]  = Sensor_2G_GetMcuRegister(44);
	FpgaReg.Pdlv[HP_CHN]   = Sensor_2G_GetMcuRegister(48);
	FpgaReg.Pfwd[HP_CHN]   = Sensor_2G_GetMcuRegister(52);
	FpgaReg.Pref[HP_CHN]   = Sensor_2G_GetMcuRegister(56);
	FpgaReg.R[HP_CHN]   	  = Sensor_2G_GetMcuRegister(60);
	FpgaReg.X[HP_CHN]      = Sensor_2G_GetMcuRegister(64);
	FpgaReg.VSWR[HP_CHN]   = Sensor_2G_GetMcuRegister(68);
	/**********************************************************/
	FpgaReg.SyncInDuty  = Sensor_2G_GetMcuRegister(72);
	FpgaReg.SyncInFreq  = Sensor_2G_GetMcuRegister(76);
	FpgaReg.SyncOutDutyMeasure = Sensor_2G_GetMcuRegister(80);
	FpgaReg.SyncOutFreqMeasure = Sensor_2G_GetMcuRegister(84);
	/***********************Read Register******************************/
	FpgaReg.FeedbackFreq[LP_CHN]  = Sensor_2G_GetMcuRegister(88);
	FpgaReg.FeedbackVrms[LP_CHN]  = Sensor_2G_GetMcuRegister(92);
	FpgaReg.FeedbackIrms[LP_CHN]  = Sensor_2G_GetMcuRegister(96);
	FpgaReg.FeedbackPhase[LP_CHN] = Sensor_2G_GetMcuRegister(100);
	/**********************************************************/
	FpgaReg.Freq[LP_CHN]   = Sensor_2G_GetMcuRegister(104);
	FpgaReg.Vrms[LP_CHN]   = Sensor_2G_GetMcuRegister(108);
	FpgaReg.Irms[LP_CHN]   = Sensor_2G_GetMcuRegister(112);
	FpgaReg.Phase[LP_CHN]  = Sensor_2G_GetMcuRegister(116);
	FpgaReg.Pdlv[LP_CHN]   = Sensor_2G_GetMcuRegister(120);
	FpgaReg.Pfwd[LP_CHN]   = Sensor_2G_GetMcuRegister(124);
	FpgaReg.Pref[LP_CHN]   = Sensor_2G_GetMcuRegister(128);
	FpgaReg.R[LP_CHN]   	  = Sensor_2G_GetMcuRegister(132);
	FpgaReg.X[LP_CHN]      = Sensor_2G_GetMcuRegister(136);
	FpgaReg.VSWR[LP_CHN]   = Sensor_2G_GetMcuRegister(140);
    IF_HAL_FpgaReg_ReadEnd();	
	ret = ON;
	return ret;		
}
/* FUNCTION *********************************************************************************
 * Function Name : GetInputSensorValue(void)(低频13.560MHz)
 * Description   : 得到二代VI传感器的InputSensor计算值
 * Parameter     : pSensorData VI传感器数据结构体
 *                                 
 * END ***************************************************************************************/
static void GetInputSensorValue(uint8_t Pwr_Chn)	
{
	float PhaseRad =0,cosRad =0,sinRad =0,Z =0,R =0, X = 0,vswr_t =0,P_dlv =0,P_fwd =0,P_ref =0,VSWR = 100;
	
	float Freq =  (1026 - FpgaReg.FeedbackFreq[Pwr_Chn])* FREQ_2M_RATE; 
	float V_rms = FpgaReg.FeedbackVrms[Pwr_Chn]*FpgaVrmsFactor/RMS_VALUE;
    float I_rms = FpgaReg.FeedbackIrms[Pwr_Chn]*FpgaIrmsFactor/RMS_VALUE;
	float Phase = FpgaReg.FeedbackPhase[Pwr_Chn]*ARC_RAD_VALUE + FpgaPhaseFactor;
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
    OutputData[Pwr_Chn].Freq = Freq;
    OutputData[Pwr_Chn].V_rms = V_rms;
    OutputData[Pwr_Chn].I_rms = I_rms;
    OutputData[Pwr_Chn].Phase = Phase;
    OutputData[Pwr_Chn].P_dlv = P_dlv;
    OutputData[Pwr_Chn].P_fwd = P_fwd;
    OutputData[Pwr_Chn].P_ref = P_ref;
    OutputData[Pwr_Chn].Z = Z;
    OutputData[Pwr_Chn].R = R;
    OutputData[Pwr_Chn].X = X;
	OutputData[Pwr_Chn].VSWR = VSWR;
}
float IF_Fpga_GetMcuAlgSensor(uint8_t ChnN,uint8_t Pwr_Chn)
{
	float fvalue = 0.0F;
	switch(ChnN)
	{
		case ChnN_Freq:  fvalue = OutputData[Pwr_Chn].Freq;		break; 
		case ChnN_Vrms:  fvalue = OutputData[Pwr_Chn].V_rms;	break; 
		case ChnN_Irms:  fvalue = OutputData[Pwr_Chn].I_rms; 	break;
		case ChnN_Phase: fvalue = OutputData[Pwr_Chn].Phase;	break;
		case ChnN_Pdlv:  fvalue = OutputData[Pwr_Chn].P_dlv;	break;
		case ChnN_Pfwd:  fvalue = OutputData[Pwr_Chn].P_fwd;	break;
		case ChnN_Pref:  fvalue = OutputData[Pwr_Chn].P_ref;	break;
		case ChnN_Z:  	 fvalue = OutputData[Pwr_Chn].Z;		break;
		case ChnN_R:	 fvalue = OutputData[Pwr_Chn].R;		break;
		case ChnN_X:	 fvalue = OutputData[Pwr_Chn].X;		break;
		case ChnN_VSWR:  fvalue = OutputData[Pwr_Chn].VSWR;		break;
	}
    return fvalue;
}
float IF_Fpga_GetRegAlgSensor(uint8_t ChnN,uint8_t Pwr_Chn)
{
	float fvalue = 0.0F;
	switch(ChnN)
	{
		case ChnN_Freq:  fvalue = FpgaReg.Freq[Pwr_Chn]/1000000.0F;		break; 
		case ChnN_Vrms:  fvalue = FpgaReg.Vrms[Pwr_Chn]/1000.0F;	 	break; 
		case ChnN_Irms:  fvalue = FpgaReg.Irms[Pwr_Chn]/1000.0F;  		break;
		case ChnN_Phase: fvalue = FpgaReg.Phase[Pwr_Chn]/1000.0F;		break;
		case ChnN_Pdlv:  fvalue = FpgaReg.Pdlv[Pwr_Chn]/1000.0F;		break;
		case ChnN_Pfwd:  fvalue = FpgaReg.Pfwd[Pwr_Chn]/1000.0F;		break;
		case ChnN_Pref:  fvalue = FpgaReg.Pref[Pwr_Chn]/1000.0F;		break;
		case ChnN_Z:  	 fvalue = FpgaReg.Vrms[Pwr_Chn]/FpgaReg.Irms[Pwr_Chn];break;
		case ChnN_R:	 fvalue = FpgaReg.R[Pwr_Chn]/1000.0F;			break;
		case ChnN_X:	 fvalue = FpgaReg.X[Pwr_Chn]/1000.0F;			break;
		case ChnN_VSWR:  fvalue = FpgaReg.VSWR[Pwr_Chn]/1000.0F;		break;
	}
    return fvalue;
}
uint32_t IF_Fpga_GetTemperture(void)
{
	uint32_t temp = (0.123041F*FpgaReg.temperture - 273.15F)*10;
	return temp;
}
uint32_t IF_Fpga_GetDrainVoltage(void)
{
	uint32_t DrainVot = g_FactorData.DrainGain*FpgaReg.RFDrainVolt + g_FactorData.DrainOffset;
	return DrainVot;
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
	int32_t value = 0;
	/******************DDS设置**************************/
	value = IF_InternalParam_GetDDSChannelNo();
	Fpga_SetDDSChannelNumber(value);
	
	value = IF_InternalParam_GetDDSCenterFreq();		
	Fpga_SetDDSFrequency(value);
	
	value = IF_CmdParam_GetDDSPhase();	
	Fpga_SetDDSPhase(value);
	
	value = IF_CmdParam_GetDDSSignSwitch();	
	Fpga_SetDDSStateSwitch(value);	
	
	/******************AC-DC设置**************************/
	value = IF_InternalParam_GetACDCCurrent();	
	Fpga_SetACDCurrent(value);
	
	value = IF_CmdParam_GetACDCVoltage();
	Fpga_SetACDCVoltage(value);
	
	value = IF_CmdParam_GetACDCStateSwitch();
	Fpga_SetACDCStateSwitch(value);

	/******************Sensor Factor设置**************************/
	value = (int32_t)(FpgaVrmsFactor*1000);
	Fpga_SetSensorVrmsFactor(value);
	
	value = (int32_t)(FpgaIrmsFactor*1000);
	Fpga_SetSensorIrmsFactor(value);
	
	value = (int32_t)(FpgaPhaseFactor*1000);
	Fpga_SetSensorPhaseFactor(value);
	
    /******************脉冲设置**************************/
	value = IF_CmdParam_GetPulseMode();
	Fpga_SetPulseModeSwitch(value);
	
	value = IF_CmdParam_GetPulseFrequency();
	Fpga_SetPulseModeFrequency(value);
	
	value = IF_CmdParam_GetPulseDutyCircle();
	Fpga_SetPulseModeDutyCircle(value);
	
	/******************同步设置**************************/
	value = IF_CmdParam_GetSyncSource();
	Fpga_SetPulseSyncSourceSwitch(value);
	
	value = IF_CmdParam_GetSyncOutEnable();
	Fpga_SetPulseSyncOutEnable(value);
	
	value = IF_CmdParam_GetSyncOutDelay();
	Fpga_SetPulseSyncOutDelay(value);
	
	/******************输出同步设置**************************/
	value = IF_InternalParam_GetFeedCollectionMode();
	Fpga_SetFeedbackCollectionMode(value);
	
	value = IF_InternalParam_GetFeedPreMask();
	Fpga_SetFeedbackPreMask(value);
	
	value = IF_InternalParam_GetFeedPostMask();
	Fpga_SetFeedbackPostMask(value);
	
	/******************相位设置**************************/
	value = IF_InternalParam_GetPhaseState2();
	Fpga_SetPhaseLevelToLevel(value);
	
	value = IF_InternalParam_GetPhaseStepSpeed();
	Fpga_SetPhaseStepSpeed(value);
	
	value = IF_InternalParam_GetPhaseStepTimer();
	Fpga_SetPhaseStepTimer(value);
		
}	

//////////////////////////////////////////////////////////////////////////////////////////
uint8_t IF_Sensor_GetVersion(uint8_t *pBuf)
{
	uint8_t j = 0;
	pBuf[j++] = FpgaReg.Version[3];
	pBuf[j++] = FpgaReg.Version[2];
	pBuf[j++] = FpgaReg.Version[1];
	pBuf[j++] = FpgaReg.Version[0];
	pBuf[j++] = FpgaReg.Version[7];	
	pBuf[j++] = FpgaReg.Version[6];	
	pBuf[j++] = FpgaReg.Version[5];	
	pBuf[j++] = FpgaReg.Version[4];
    return j;
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_SetDDSStateSwitch
 * Description   : 设置DDS信号开关
 * Parameter     : value -----开关值
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetDDSStateSwitch(int32_t WriteReg)
{
	if(FpgaReg.DDSStateSwitch != WriteReg)
	{
		Sensor_2G_SetMcuRegister(DDS_STATESWITCH_ADDR,WriteReg);
		FpgaReg.DDSStateSwitch = WriteReg;
	}
}

static void  Fpga_SetDDSChannelNumber(int32_t WriteReg)
{
	if(FpgaReg.DDSChannelNo != WriteReg)
	{
		Sensor_2G_SetMcuRegister(DDS_CHANNELNO_ADDR ,WriteReg);
		FpgaReg.DDSChannelNo = WriteReg;
	}
}

static void  Fpga_SetDDSFrequency(int32_t WriteReg)
{	
	float fvalue = 0;
	if(FpgaReg.DDSFrequency != WriteReg)
	{
		fvalue = WriteReg/1000.0F;	//(MHz)	
		FpgaVrmsFactor  = g_FactorData.VrmsFactor *fvalue + g_FactorData.VrmsOffset;
		FpgaIrmsFactor  = g_FactorData.IrmsFactor *fvalue + g_FactorData.IrmsOffset;
		FpgaPhaseFactor = g_FactorData.PhaseFactor*fvalue + g_FactorData.PhaseOffset;
		Sensor_2G_SetMcuRegister(DDS_FREQUENCY_ADDR ,WriteReg);
		FpgaReg.DDSFrequency = WriteReg;		
	}
}

static void  Fpga_SetDDSPhase(int32_t WriteReg)
{	
	WriteReg = WriteReg/10;
	if(FpgaReg.DDSPhase != WriteReg)
	{
		Sensor_2G_SetMcuRegister(DDS_PAHSE_ADDR ,WriteReg);
		FpgaReg.DDSPhase = WriteReg;
	}
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_SetACDCVoltage
 * Description   : 设置电压信号值
 * Parameter     : value -----电压值
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetACDCVoltage(int32_t WriteReg)
{	
	float fvalue = 0;
	if(WriteReg >= g_FactorData.ACDCVoltOffset)
	{
		fvalue = (WriteReg - g_FactorData.ACDCVoltOffset);
	}
	WriteReg =(uint32_t)(fvalue/g_FactorData.ACDCVoltGain + 8191.5F);
	if(FpgaReg.ACDCVoltage != WriteReg)
	{
		Sensor_2G_SetMcuRegister(ACDC_VOLTAGE_ADDR ,WriteReg);
		FpgaReg.ACDCVoltage = WriteReg;
	}
}

static void  Fpga_SetACDCurrent(int32_t WriteReg)
{
	WriteReg = g_FactorData.ACDCCurrentGain*WriteReg - g_FactorData.ACDCCurrentOffset;
	if(FpgaReg.ACDCCurrent != WriteReg)
	{
		Sensor_2G_SetMcuRegister(ACDC_CURRENT_ADDR ,WriteReg);
		FpgaReg.ACDCCurrent = WriteReg;
	}
}

static void  Fpga_SetACDCStateSwitch(int32_t WriteReg)
{	
	if(FpgaReg.ACDCStateSwitch != WriteReg)
	{
		Sensor_2G_SetMcuRegister(ACDC_STATESWITCH_ADDR ,WriteReg);
		FpgaReg.ACDCStateSwitch = WriteReg;
	}
}

/* FUNCTION *********************************************************************************
 * Function Name : Fpga_SetACDCStateSwitch
 * Description   : 设置ACDC开关信号值
 * Parameter     : value -----开关信号值
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetSensorVrmsFactor(int32_t WriteReg)
{	
	if(FpgaReg.VrmsFactor != WriteReg)
	{
		Sensor_2G_SetMcuRegister(FACTOR_VRMS_ADDR ,WriteReg);
		FpgaReg.VrmsFactor = WriteReg;
	}
}
static void  Fpga_SetSensorIrmsFactor(int32_t WriteReg)
{	
	if(FpgaReg.IrmsFactor != WriteReg)
	{
		Sensor_2G_SetMcuRegister(FACTOR_IRMS_ADDR ,WriteReg);
		FpgaReg.IrmsFactor = WriteReg;
	}
}
static void  Fpga_SetSensorPhaseFactor(int32_t WriteReg)
{	
	if(FpgaReg.PhaseFactor != WriteReg)
	{
		Sensor_2G_SetMcuRegister(FACTOR_PHASE_ADDR ,WriteReg);
		FpgaReg.PhaseFactor = WriteReg;
	}
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_SetPulseModeSwitch
 * Description   : 设置脉冲模式开关信号值
 * Parameter     : value -----开关信号值
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetPulseModeSwitch(int32_t WriteReg)
{	
	if(FpgaReg.PulseMode != WriteReg)
	{
		Sensor_2G_SetMcuRegister(PULSEMODE_SWITCH_ADDR ,WriteReg);
		FpgaReg.PulseMode = WriteReg;
	}
}
static void  Fpga_SetPulseModeFrequency(int32_t WriteReg)
{	
	if(WriteReg>0)WriteReg = 1000000*FPGA_SAMPLE_RATE/WriteReg;
	if(FpgaReg.PulsePeriod != WriteReg)
	{
		Sensor_2G_SetMcuRegister(PULSEMODE_FREQ_ADDR ,WriteReg);
		FpgaReg.PulsePeriod = WriteReg;
	}
}
static void  Fpga_SetPulseModeDutyCircle(int32_t WriteReg)
{		
	WriteReg = FpgaReg.PulsePeriod*WriteReg/100;
	if(FpgaReg.PulseDuty != WriteReg)
	{	
		Sensor_2G_SetMcuRegister(PULSEMODE_DUTY_ADDR ,WriteReg);
		FpgaReg.PulseDuty = WriteReg;
	}
}

/* FUNCTION *********************************************************************************
 * Function Name : Fpga_SetPulseSyncSourceSwitch
 * Description   : 设置脉冲模式开关信号值
 * Parameter     : value -----开关信号值
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetPulseSyncSourceSwitch(int32_t WriteReg)
{	
	if(FpgaReg.SyncSource != WriteReg)
	{
		Sensor_2G_SetMcuRegister(SYNCSOURCE_SWITCH_ADDR ,WriteReg);
		FpgaReg.SyncSource = WriteReg;
	}
}
static void  Fpga_SetPulseSyncOutEnable(int32_t WriteReg)
{	
	if(FpgaReg.SyncOutEnable != WriteReg)
	{
		Sensor_2G_SetMcuRegister(SYNCOUT_ENABLE_ADDR ,WriteReg);
		FpgaReg.SyncOutEnable = WriteReg;
	}
}
static void  Fpga_SetPulseSyncOutDelay(int32_t WriteReg)
{	
	WriteReg  = FPGA_SAMPLE_RATE * WriteReg;
	if(FpgaReg.SyncOutDelay != WriteReg)
	{
		Sensor_2G_SetMcuRegister(SYNCOUT_DELAY_ADDR ,WriteReg);
		FpgaReg.SyncOutDelay = WriteReg;
	}
}

/* FUNCTION *********************************************************************************
 * Function Name : Fpga_SetFeedbackCollectionMode
 * Description   : 设置脉冲模式开关信号值
 * Parameter     : value -----开关信号值
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetFeedbackCollectionMode(int32_t WriteReg)
{	
	if(FpgaReg.FeedCollectionMode != WriteReg)
	{
		Sensor_2G_SetMcuRegister(FEED_COLLECTIONMODE_ADDR ,WriteReg);
		FpgaReg.FeedCollectionMode = WriteReg;
	}
}
static void  Fpga_SetFeedbackPreMask(int32_t WriteReg)
{	
	WriteReg = FPGA_SAMPLE_RATE*WriteReg;
	if(FpgaReg.FeedPreMask != WriteReg)
	{
		Sensor_2G_SetMcuRegister(FEED_PREMASK_ADDR ,WriteReg);
		FpgaReg.FeedPreMask = WriteReg;
	}
}
static void  Fpga_SetFeedbackPostMask(int32_t WriteReg)
{	
    int32_t PostMaskTimer = FPGA_SAMPLE_RATE*WriteReg;
	if(FpgaReg.FeedPostMask > PostMaskTimer)
	{
		WriteReg = FpgaReg.PulseDuty - PostMaskTimer;
		if(FpgaReg.FeedPostMask != WriteReg)
		{
			Sensor_2G_SetMcuRegister(FEED_POSTMASK_ADDR ,WriteReg);
			FpgaReg.FeedPostMask = WriteReg;
		}
	}
}

/* FUNCTION *********************************************************************************
 * Function Name : Fpga_SetFeedSample
 * Description   : 设置脉冲模式开关信号值
 * Parameter     : value -----开关信号值
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetPhaseLevelToLevel(int32_t WriteReg)
{	
	WriteReg = WriteReg/10;
	if(FpgaReg.PhaseState2 != WriteReg)
	{
		Sensor_2G_SetMcuRegister(PHASE_LEVELTOLEVEL_ADDR ,WriteReg);
	}
}
static void  Fpga_SetPhaseStepSpeed(int32_t WriteReg)
{	
	WriteReg = WriteReg/10;
	if(FpgaReg.PhaseStepSpeed != WriteReg)
	{
		Sensor_2G_SetMcuRegister(PHASE_STEPSPEED_ADDR ,WriteReg);
	}
}
static void  Fpga_SetPhaseStepTimer(int32_t WriteReg)
{	
	WriteReg = FPGA_SAMPLE_RATE*WriteReg/1000;
	if(FpgaReg.PhaseStepTimer != WriteReg)
	{
		Sensor_2G_SetMcuRegister(PHASE_STEPTIMER_ADDR ,WriteReg);
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
static int32_t Sensor_2G_GetMcuRegister(uint16_t RegAddress)
{
	UINT32_VAL RegData;
	for(uint8_t i = 0; i <  + 4;i++)
	{
		RegData.v[i] = IF_HAL_FpgaReg_Read(RegAddress+i);
	}
	return RegData.Val;
}

/* FUNCTION *********************************************************************************
 * Function Name : Sensor_2G_GetMcuRegister
 * Description   : 得到MCU寄存器的参数
 * Parameter     : 
 *                                 
 * END ***************************************************************************************/
static void Sensor_2G_SetMcuRegister(uint16_t RegAddress,int32_t value)
{
	UINT32_VAL RegData;
	RegData.Val  = value; 
	for(uint8_t i = 0; i <  + 4;i++)
	{
		IF_HAL_FpgaReg_Write(RegAddress+i,RegData.v[i]);
	}	
}
//*****************************************************************************
//* END
//*****************************************************************************


