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

static ReadReg_t FpgaReg;	
static WriteReg_t  FpgaWriteReg,lastFpgaWriteReg;
static SensorData_t OutputData[PWR_CHN_NUM];

static float FpgaVrmsFactor = 0;
static float FpgaIrmsFactor = 0;
static float FpgaPhaseFactor = 0;

/* FUNCTION PROTOTYPES */
static void GetInputSensorValue(void);
static void Fgpa_WriteRegisterValue(void);
static void Fgpa_ReadRegisterValue(void);

static void GetOutputSensorValue(uint8_t Pwr_Chn);
static float GetVSWRReflectance(float vswr_r,float vswr_x);
/******************************************************/
static int32_t Sensor_2G_GetMcuRegister(uint16_t RegAddress);
static void Sensor_2G_SetMcuRegister(uint16_t RegAddress,int32_t value);
/******************************************************/
static int32_t Fpga_GetDDSChannelNumber(void);
static int32_t Fpga_GetDDSFrequency(void);
static int32_t Fpga_GetDDSPhase(void);
static int32_t Fpga_GetDDSState(void);
/******************************************************/
static int32_t Fpga_GetACDCState(void);
static int32_t Fpga_GetACDCVoltage(void);
static int32_t Fpga_GetACDCurrent(void);
/******************************************************/
static int32_t  Fpga_GetSensorVrmsFactor(void);
static int32_t  Fpga_GetSensorIrmsFactor(void);
static int32_t  Fpga_GetSensorPhaseFactor(void);
static int32_t  Fpga_GetSensorVISampleSmooth(void);
/******************************************************/
static int32_t  Fpga_GetPulseModeSwitch(void);
static int32_t  Fpga_GetPulseModeDutyCircle(void);
static int32_t  Fpga_GetPulseModePeriod(void);
/******************************************************/
static int32_t  Fpga_GetPulseSyncOutSource(void);
static int32_t  Fpga_GetPulseSyncOutEnable(void);
static int32_t  Fpga_GetPulseSyncOutDelay(void);
/******************************************************/
static int32_t  Fpga_GetPulseFeedbackMode(void);
static int32_t  Fpga_GetPulseFeedbackPreMask(void);
static int32_t  Fpga_GetPulseFeedbackPostMask(void);
/******************************************************/
static int32_t  Fpga_GetPhaseLevelToLevel(void);
static int32_t  Fpga_GetPhaseStepSpeed(void);
static int32_t  Fpga_GetPhaseStepTimer(void);

/* FUNCTION *********************************************************************************
 * Function Name : IF_Sensor_2G_ParamInit
 * Description   : 得到二代VI传感器的数据初始化
 * Parameter     : 无
 *                                 
 * END ***************************************************************************************/
void IF_FpgaSensor_ParamInit(void)
{
	uint8_t FpgaInit = 0;
	memset((void*)&FpgaReg,0,sizeof(ReadReg_t));
	memset((void*)&FpgaWriteReg,0,sizeof(WriteReg_t));
	memset((void*)&lastFpgaWriteReg,0,sizeof(WriteReg_t));
	memset((void*)&OutputData[LP_CHN],0,sizeof(SensorData_t));	
	memset((void*)&OutputData[HP_CHN],0,sizeof(SensorData_t));
	
	while(FpgaReg.Version[0] == 0&&FpgaReg.Version[1] == 0)
	{
		FpgaInit++;
		if(FpgaInit > 100)break;
		IF_HAL_FpgaReg_ReadStart();	
		for(uint8_t i = 0; i < 8; i++)
		{
			FpgaReg.Version[i] = IF_HAL_FpgaReg_Read(i);
		}
		IF_HAL_FpgaReg_ReadEnd();	
	}
}
void IF_Sensor_Fpga_Write(void)
{
	GetInputSensorValue();
	Fgpa_WriteRegisterValue();	
}	
/* FUNCTION *********************************************************************************
 * Function Name : IF_Sensor_Fpga_Read
 * Description   : 得到Fpga VI传感器的数据任务
 * Parameter     : 无
 *                                 
 * END ***************************************************************************************/
void IF_Sensor_Fpga_Read(void)
{	
	Fgpa_ReadRegisterValue();  
	if(0 == IF_CmdParam_GetPowerWorkMode())
	{
		GetOutputSensorValue(HP_CHN);
		GetOutputSensorValue(LP_CHN);
	}else
	{
		memset(&OutputData[LP_CHN],0,sizeof(SensorData_t));	
		memset(&OutputData[HP_CHN],0,sizeof(SensorData_t));	
	}
		
}

/* FUNCTION *********************************************************************************
 * Function Name : Fgpa_WriteRegisterValue
 * Description   : 写二代VI传感器的FPGA寄存器器值
 * Parameter     : 
 *                                 
 * END ***************************************************************************************/
static void Fgpa_WriteRegisterValue(void)
{
	/*********************ACDC Register*********************************/
	if(lastFpgaWriteReg.ACDCCurrent != FpgaWriteReg.ACDCCurrent)
	{
		Sensor_2G_SetMcuRegister(ACDC_CURRENT_ADDR ,FpgaWriteReg.ACDCCurrent);
		lastFpgaWriteReg.ACDCCurrent = FpgaWriteReg.ACDCCurrent;
	}
	if(lastFpgaWriteReg.ACDCVoltage != FpgaWriteReg.ACDCVoltage)
	{
		Sensor_2G_SetMcuRegister(ACDC_VOLTAGE_ADDR ,FpgaWriteReg.ACDCVoltage);
		lastFpgaWriteReg.ACDCVoltage = FpgaWriteReg.ACDCVoltage;
	}
	if(lastFpgaWriteReg.ACDCState != FpgaWriteReg.ACDCState)	
	{
		Sensor_2G_SetMcuRegister(ACDC_STATESWITCH_ADDR ,FpgaWriteReg.ACDCState);
		lastFpgaWriteReg.ACDCState = FpgaWriteReg.ACDCState;
	}
	/********************DDS Register**********************/	
	if(lastFpgaWriteReg.DDSFrequency != FpgaWriteReg.DDSFrequency)
	{
		Sensor_2G_SetMcuRegister(DDS_FREQUENCY_ADDR ,FpgaWriteReg.DDSFrequency);
		lastFpgaWriteReg.DDSFrequency = FpgaWriteReg.DDSFrequency;
	}
	if(lastFpgaWriteReg.DDSPhase != FpgaWriteReg.DDSPhase)
	{
		Sensor_2G_SetMcuRegister(DDS_PAHSE_ADDR ,FpgaWriteReg.DDSPhase);
		lastFpgaWriteReg.DDSPhase = FpgaWriteReg.DDSPhase;
	}
	if(lastFpgaWriteReg.DDSChannelNo != FpgaWriteReg.DDSChannelNo)
	{
		Sensor_2G_SetMcuRegister(DDS_CHANNELNO_ADDR ,FpgaWriteReg.DDSChannelNo);
		lastFpgaWriteReg.DDSChannelNo = FpgaWriteReg.DDSChannelNo;
	}
	if(lastFpgaWriteReg.DDSState != FpgaWriteReg.DDSState)	
	{
		Sensor_2G_SetMcuRegister(DDS_STATESWITCH_ADDR,FpgaWriteReg.DDSState);
		lastFpgaWriteReg.DDSState = FpgaWriteReg.DDSState;
	}
	/**********************Factor Register********************/	
	if(lastFpgaWriteReg.VrmsFactor != FpgaWriteReg.VrmsFactor)	
	{
		Sensor_2G_SetMcuRegister(FACTOR_VRMS_ADDR ,FpgaWriteReg.VrmsFactor);
		lastFpgaWriteReg.VrmsFactor = FpgaWriteReg.VrmsFactor;
	}
	if(lastFpgaWriteReg.IrmsFactor != FpgaWriteReg.IrmsFactor)	
	{
		Sensor_2G_SetMcuRegister(FACTOR_IRMS_ADDR ,FpgaWriteReg.IrmsFactor);
		lastFpgaWriteReg.IrmsFactor = FpgaWriteReg.IrmsFactor;
	}
	if(lastFpgaWriteReg.PhaseFactor != FpgaWriteReg.PhaseFactor)	
	{
		Sensor_2G_SetMcuRegister(FACTOR_PHASE_ADDR ,FpgaWriteReg.PhaseFactor);
		lastFpgaWriteReg.PhaseFactor = FpgaWriteReg.PhaseFactor;
	}
	if(lastFpgaWriteReg.VISampleSmooth != FpgaWriteReg.VISampleSmooth)	
	{
		Sensor_2G_SetMcuRegister(FACTOR_SAMPLE_ADDR ,FpgaWriteReg.VISampleSmooth);
		lastFpgaWriteReg.VISampleSmooth = FpgaWriteReg.VISampleSmooth;
	}
	/*********************Pulse Register*********************/	
	if(lastFpgaWriteReg.PulseMode != FpgaWriteReg.PulseMode)	
	{
		Sensor_2G_SetMcuRegister(PULSEMODE_SWITCH_ADDR ,FpgaWriteReg.PulseMode);
		lastFpgaWriteReg.PulseMode = FpgaWriteReg.PulseMode;
	}
	if(lastFpgaWriteReg.PulsePeriod != FpgaWriteReg.PulsePeriod)	
	{
		Sensor_2G_SetMcuRegister(PULSEMODE_FREQ_ADDR ,FpgaWriteReg.PulsePeriod);
		lastFpgaWriteReg.PulsePeriod = FpgaWriteReg.PulsePeriod;
	}
	if(lastFpgaWriteReg.PulseDuty != FpgaWriteReg.PulseDuty)		
	{
		Sensor_2G_SetMcuRegister(PULSEMODE_DUTY_ADDR ,FpgaWriteReg.PulseDuty);
		lastFpgaWriteReg.PulseDuty = FpgaWriteReg.PulseDuty;
	}
	/********************SyncOut Register**********************/	
	if(lastFpgaWriteReg.SyncOutSource != FpgaWriteReg.SyncOutSource)	
	{
		Sensor_2G_SetMcuRegister(SYNCSOURCE_SWITCH_ADDR ,FpgaWriteReg.SyncOutSource);
		lastFpgaWriteReg.SyncOutSource = FpgaWriteReg.SyncOutSource;
	}
	if(lastFpgaWriteReg.SyncOutEnable != FpgaWriteReg.SyncOutEnable)	
	{
		Sensor_2G_SetMcuRegister(SYNCOUT_ENABLE_ADDR ,FpgaWriteReg.SyncOutEnable);
		lastFpgaWriteReg.SyncOutEnable = FpgaWriteReg.SyncOutEnable;
	}
	if(lastFpgaWriteReg.SyncOutDelay != FpgaWriteReg.SyncOutDelay)	
	{
		Sensor_2G_SetMcuRegister(SYNCOUT_DELAY_ADDR ,FpgaWriteReg.SyncOutDelay);
		lastFpgaWriteReg.SyncOutDelay = FpgaWriteReg.SyncOutDelay;
	}
	/************************FeedBack Register******************/	
	if(lastFpgaWriteReg.FeedbackMode != FpgaWriteReg.FeedbackMode)	
	{
		Sensor_2G_SetMcuRegister(FEED_BACKMODE_ADDR ,FpgaWriteReg.FeedbackMode);
		lastFpgaWriteReg.FeedbackMode = FpgaWriteReg.FeedbackMode;
	}
	if(lastFpgaWriteReg.FeedPreMask != FpgaWriteReg.FeedPreMask)	
	{
		Sensor_2G_SetMcuRegister(FEED_PREMASK_ADDR ,FpgaWriteReg.FeedPreMask);
		lastFpgaWriteReg.FeedPreMask = FpgaWriteReg.FeedPreMask;
	}
	if(lastFpgaWriteReg.FeedPostMask != FpgaWriteReg.FeedPostMask)	
	{
		Sensor_2G_SetMcuRegister(FEED_POSTMASK_ADDR ,FpgaWriteReg.FeedPostMask);
		lastFpgaWriteReg.FeedPostMask = FpgaWriteReg.FeedPostMask;
	}
	/************************PhaseStep Register******************/	
	if(lastFpgaWriteReg.PhaseState2 != FpgaWriteReg.PhaseState2)	
	{
		Sensor_2G_SetMcuRegister(PHASE_LEVELTOLEVEL_ADDR ,FpgaWriteReg.PhaseState2);
		lastFpgaWriteReg.PhaseState2 = FpgaWriteReg.PhaseState2;
	}
	if(lastFpgaWriteReg.PhaseStepSpeed != FpgaWriteReg.PhaseStepSpeed)	
	{
		Sensor_2G_SetMcuRegister(PHASE_STEPSPEED_ADDR,FpgaWriteReg.PhaseStepSpeed);
		lastFpgaWriteReg.PhaseStepSpeed = FpgaWriteReg.PhaseStepSpeed;
	}
	if(lastFpgaWriteReg.PhaseStepTimer != FpgaWriteReg.PhaseStepTimer)	
	{
		Sensor_2G_SetMcuRegister(PHASE_STEPTIMER_ADDR,FpgaWriteReg.PhaseStepTimer);
		lastFpgaWriteReg.PhaseStepTimer = FpgaWriteReg.PhaseStepTimer;
	}
}

/* FUNCTION *********************************************************************************
 * Function Name : Fgpa_ReadRegisterValue
 * Description   : 得到二代VI传感器的FPGA寄存器器值
 * Parameter     : 
 *                                 
 * END ***************************************************************************************/
static void  Fgpa_ReadRegisterValue(void)
{
    /*************************读取FPGA寄存器数据到寄存器缓冲区*******************************/
    IF_HAL_FpgaReg_ReadStart();
	/***********************Read Register******************************/
	FpgaReg.temperture    = Sensor_2G_GetMcuRegister(8);
	FpgaReg.RFDrainVolt   = Sensor_2G_GetMcuRegister(12);
	/***********************Read Register******************************/
	if(0 == IF_CmdParam_GetPowerWorkMode())
	{
		FpgaReg.FeedbackFreq[HP_CHN]  = Sensor_2G_GetMcuRegister(16);
		FpgaReg.FeedbackVrms[HP_CHN]  = Sensor_2G_GetMcuRegister(20);
		FpgaReg.FeedbackIrms[HP_CHN]  = Sensor_2G_GetMcuRegister(24);
		FpgaReg.FeedbackPhase[HP_CHN] = Sensor_2G_GetMcuRegister(28);
		
		FpgaReg.FeedbackFreq[LP_CHN]  = Sensor_2G_GetMcuRegister(88);
		FpgaReg.FeedbackVrms[LP_CHN]  = Sensor_2G_GetMcuRegister(92);
		FpgaReg.FeedbackIrms[LP_CHN]  = Sensor_2G_GetMcuRegister(96);
		FpgaReg.FeedbackPhase[LP_CHN] = Sensor_2G_GetMcuRegister(100);
	}
	/**********************************************************/
	FpgaReg.Freq[HP_CHN]   = Sensor_2G_GetMcuRegister(32);
	FpgaReg.Vrms[HP_CHN]   = Sensor_2G_GetMcuRegister(36);
	FpgaReg.Irms[HP_CHN]   = Sensor_2G_GetMcuRegister(40);
	FpgaReg.Phase[HP_CHN]  = Sensor_2G_GetMcuRegister(44);
	FpgaReg.Pdlv[HP_CHN]   = Sensor_2G_GetMcuRegister(48);
	FpgaReg.Pfwd[HP_CHN]   = Sensor_2G_GetMcuRegister(52);
	FpgaReg.Pref[HP_CHN]   = Sensor_2G_GetMcuRegister(56);
	FpgaReg.R[HP_CHN]      = Sensor_2G_GetMcuRegister(60);
	FpgaReg.X[HP_CHN]      = Sensor_2G_GetMcuRegister(64);
	FpgaReg.VSWR[HP_CHN]   = Sensor_2G_GetMcuRegister(68);
	
	FpgaReg.Freq[LP_CHN]   = Sensor_2G_GetMcuRegister(104);
	FpgaReg.Vrms[LP_CHN]   = Sensor_2G_GetMcuRegister(108);
	FpgaReg.Irms[LP_CHN]   = Sensor_2G_GetMcuRegister(112);
	FpgaReg.Phase[LP_CHN]  = Sensor_2G_GetMcuRegister(116);
	FpgaReg.Pdlv[LP_CHN]   = Sensor_2G_GetMcuRegister(120);
	FpgaReg.Pfwd[LP_CHN]   = Sensor_2G_GetMcuRegister(124);
	FpgaReg.Pref[LP_CHN]   = Sensor_2G_GetMcuRegister(128);
	FpgaReg.R[LP_CHN]      = Sensor_2G_GetMcuRegister(132);
	FpgaReg.X[LP_CHN]      = Sensor_2G_GetMcuRegister(136);
	FpgaReg.VSWR[LP_CHN]   = Sensor_2G_GetMcuRegister(140);
	/**********************************************************/
	FpgaReg.SyncInDuty  = Sensor_2G_GetMcuRegister(72);
	FpgaReg.SyncInFreq  = Sensor_2G_GetMcuRegister(76);
	FpgaReg.SyncOutDutyMeasure = Sensor_2G_GetMcuRegister(80);
	FpgaReg.SyncOutFreqMeasure = Sensor_2G_GetMcuRegister(84);	
	IF_HAL_FpgaReg_ReadEnd();		
}
/* FUNCTION *********************************************************************************
 * Function Name : GetOutputSensorValue(void)(低频13.560MHz)
 * Description   : 得到二代VI传感器的InputSensor计算值
 * Parameter     : pSensorData VI传感器数据结构体
 *                                 
 * END ***************************************************************************************/
void GetOutputSensorValue(uint8_t Pwr_Chn)	
{
	float PhaseRad =0,cosRad =0,sinRad =0,Z =0,R =0, X = 0,vswr_t =0,P_dlv =0,P_fwd =0,P_ref =0,gamma =100,VSWR = 100;
	
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
			 gamma = vswr_t*vswr_t;
			 VSWR =(1+vswr_t)/(1-vswr_t);
			 P_dlv = P_abs*cosRad;			 
			 P_fwd = P_dlv/(1-gamma);
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
	OutputData[Pwr_Chn].Gamma = gamma;
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
		case ChnN_Gamma:  fvalue = OutputData[Pwr_Chn].Gamma;	break;
	}
    return fvalue;
}
float IF_Fpga_GetRegAlgSensor(uint8_t ChnN,uint8_t Pwr_Chn)
{
	float fvalue = 20.0F;
	switch(ChnN)
	{
		case ChnN_Freq:  
			fvalue = FpgaReg.Freq[Pwr_Chn]/1000000.0F;		
		break; 
		case ChnN_Vrms:  
			fvalue = FpgaReg.Vrms[Pwr_Chn]/1000.0F;	 	
		break; 
		case ChnN_Irms:  
			fvalue = FpgaReg.Irms[Pwr_Chn]/1000.0F;  		
		break;
		case ChnN_Phase: 
			fvalue = FpgaReg.Phase[Pwr_Chn]/1000.0F;		
		break;
		case ChnN_Pdlv:  
			fvalue = FpgaReg.Pdlv[Pwr_Chn]/1000.0F;		
		break;
		case ChnN_Pfwd:  
			fvalue = FpgaReg.Pfwd[Pwr_Chn]/1000.0F;		
		break;
		case ChnN_Pref:  
			fvalue = FpgaReg.Pref[Pwr_Chn]/1000.0F;		
		break;
		case ChnN_Z:  	 
			fvalue = FpgaReg.Vrms[Pwr_Chn]/FpgaReg.Irms[Pwr_Chn];
		break;
		case ChnN_R:	 
			fvalue = FpgaReg.R[Pwr_Chn]/1000.0F;			
		break;
		case ChnN_X:	 
			fvalue = FpgaReg.X[Pwr_Chn]/1000.0F;			
		break;
		case ChnN_VSWR:  
			if(FpgaReg.VSWR[Pwr_Chn] >= 1000)
			{
				fvalue = FpgaReg.VSWR[Pwr_Chn]/1000.0F;	
			}
		break;
		case ChnN_Gamma:  
			if(FpgaReg.VSWR[Pwr_Chn] >= 1000)
			{
				fvalue = (FpgaReg.VSWR[Pwr_Chn]-1000)/(FpgaReg.VSWR[Pwr_Chn]+1000);	
			}
		break;				
		default:break;
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
	float fvalue  = g_FactorData.DrainGain*FpgaReg.RFDrainVolt + g_FactorData.DrainOffset;
	uint32_t value = fvalue;
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
	

static void GetInputSensorValue(void)
{
	/***********DDS设置**********************/
	FpgaWriteReg.DDSChannelNo = Fpga_GetDDSChannelNumber();	
	FpgaWriteReg.DDSFrequency = Fpga_GetDDSFrequency();
	FpgaWriteReg.DDSPhase = Fpga_GetDDSPhase();
	FpgaWriteReg.DDSState = Fpga_GetDDSState();	
	/*************AC-DC设置**************************/
	FpgaWriteReg.ACDCCurrent = Fpga_GetACDCurrent();
	FpgaWriteReg.ACDCVoltage = Fpga_GetACDCVoltage();
	FpgaWriteReg.ACDCState = Fpga_GetACDCState();	
	/**********Sensor Factor设置******************/
	FpgaWriteReg.VrmsFactor  = Fpga_GetSensorVrmsFactor();
	FpgaWriteReg.IrmsFactor  = Fpga_GetSensorIrmsFactor();
	FpgaWriteReg.PhaseFactor = Fpga_GetSensorPhaseFactor();	
	FpgaWriteReg.VISampleSmooth= Fpga_GetSensorVISampleSmooth();	
    /*************脉冲设置*******************/
	FpgaWriteReg.PulseMode = Fpga_GetPulseModeSwitch();
	FpgaWriteReg.PulsePeriod = Fpga_GetPulseModePeriod();
	FpgaWriteReg.PulseDuty = Fpga_GetPulseModeDutyCircle();
	/************同步设置****************************/
	FpgaWriteReg.SyncOutSource = Fpga_GetPulseSyncOutSource();
	FpgaWriteReg.SyncOutEnable = Fpga_GetPulseSyncOutEnable();
	FpgaWriteReg.SyncOutDelay = Fpga_GetPulseSyncOutDelay();	
	/******************输出同步设置**************************/
	FpgaWriteReg.FeedbackMode = Fpga_GetPulseFeedbackMode();
	FpgaWriteReg.FeedPreMask = Fpga_GetPulseFeedbackPreMask();
	FpgaWriteReg.FeedPostMask = Fpga_GetPulseFeedbackPostMask();
	/******************相位设置**************************/
	FpgaWriteReg.PhaseState2 = Fpga_GetPhaseLevelToLevel();
	FpgaWriteReg.PhaseStepSpeed = Fpga_GetPhaseStepSpeed();
	FpgaWriteReg.PhaseStepTimer = Fpga_GetPhaseStepTimer();
		
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
 * Function Name : Fpga_GetDDSState
 * Description   : 设置DDS信号开关
 * Parameter     : value -----开关值
 *                                 
 * END ***************************************************************************************/
static int32_t  Fpga_GetDDSState(void)
{
	int32_t WriteReg = IF_CmdParam_GetDDSSignSwitch();	
	return WriteReg;		
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_GetDDSChannelNumber
 * Description   : 设置DDS通道信号值
 * Parameter     : WriteReg -----寄存器值
 *                                 
 * END ***************************************************************************************/
static int32_t  Fpga_GetDDSChannelNumber(void)
{
	int32_t WriteReg = IF_InternalParam_GetDDSChannelNo();
	return WriteReg;		
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_GetDDSFrequency
 * Description   : 设置DDS频率信号值(精度转换 1/1000)
 * Parameter     : WriteReg -----寄存器值
 *                                 
 * END ***************************************************************************************/
static int32_t  Fpga_GetDDSFrequency(void)
{	
	int32_t WriteReg = IF_InternalParam_GetDDSCenterFreq();					
	return WriteReg;	
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_GetDDSPhase
 * Description   : 设置DDS相位信号值(精度转换 1/10)
 * Parameter     : WriteReg -----寄存器值
 *                                 
 * END ***************************************************************************************/
static int32_t  Fpga_GetDDSPhase(void)
{	
	int32_t WriteReg = 0;
	int32_t DDSPhaseOffset = IF_InternalParam_GetDDSPhaseOffset();
	int32_t value = (IF_CmdParam_GetDDSPhase() + DDSPhaseOffset)/10;	
	if(value <= MAX_FPGA_PHASE)
	{
		WriteReg = value;
	}
	return  WriteReg;
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_GetACDCVoltage
 * Description   : 设置电压信号值
 * Parameter     : WriteReg -----寄存器值
 *                                 
 * END ***************************************************************************************/
static int32_t  Fpga_GetACDCVoltage(void)
{	
	int32_t WriteReg = 0;
	float fvalue = IF_CmdParam_GetACDCVoltage(); 
	if(fvalue > g_FactorData.ACDCVoltOffset)
	{
		fvalue = (fvalue - g_FactorData.ACDCVoltOffset)/g_FactorData.ACDCVoltGain;
	}
	WriteReg =(uint32_t)(fvalue + 8191.5F);
	return WriteReg;
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_GetACDCurrent
 * Description   : 设置电流信号值
 * Parameter     : WriteReg -----寄存器值
 *                                 
 * END ***************************************************************************************/
static int32_t  Fpga_GetACDCurrent(void)
{
	int32_t WriteReg = 0;
	float value = IF_InternalParam_GetACDCCurrent()*g_FactorData.ACDCCurrentGain;	
	if(value > g_FactorData.ACDCCurrentOffset)
	{
		WriteReg = value - g_FactorData.ACDCCurrentOffset;
	}
	return WriteReg;
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_GetACDCState
 * Description   : 设置ACDC开关信号值
 * Parameter     : WriteReg -----寄存器值
 *                                 
 * END ***************************************************************************************/
static int32_t  Fpga_GetACDCState(void)
{	
	int32_t WriteReg = IF_CmdParam_GetACDCState();
	return WriteReg;		
}

/* FUNCTION *********************************************************************************
 * Function Name : Fpga_GetSensorVrmsFactor
 * Description   : 设置Sensor电压因子
 * Parameter     : WriteReg -----寄存器值
 *                                 
 * END ***************************************************************************************/
static int32_t  Fpga_GetSensorVrmsFactor(void)
{	
	float fvalue = FpgaWriteReg.DDSFrequency/1000.0F;	//(MHz)	
	FpgaVrmsFactor  = g_FactorData.VrmsFactor *fvalue + g_FactorData.VrmsOffset;
	int32_t WriteReg = FpgaVrmsFactor*1000;
	return WriteReg;	
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_GetSensorIrmsFactor
 * Description   : 设置Sensor电流因子
 * Parameter     : WriteReg -----寄存器值
 *                                 
 * END ***************************************************************************************/
static int32_t  Fpga_GetSensorIrmsFactor(void)
{	
	float fvalue = FpgaWriteReg.DDSFrequency/1000.0F;	//(MHz)	
	FpgaIrmsFactor  = g_FactorData.IrmsFactor *fvalue + g_FactorData.IrmsOffset;
	int32_t WriteReg = FpgaIrmsFactor*1000;	
	return WriteReg;		
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_GetSensorPhaseFactor
 * Description   : 设置Sensor相位因子
 * Parameter     : WriteReg -----寄存器值
 *                                 
 * END ***************************************************************************************/
static int32_t  Fpga_GetSensorPhaseFactor(void)
{	
	float fvalue = FpgaWriteReg.DDSFrequency/1000.0F;	//(MHz)	
	FpgaPhaseFactor = g_FactorData.PhaseFactor*fvalue + g_FactorData.PhaseOffset;
	int32_t WriteReg = FpgaPhaseFactor*1000;
	return WriteReg;	
}
static int32_t  Fpga_GetSensorVISampleSmooth(void)
{
	int32_t WriteReg = IF_InternalParam_GetVISampleSmooth();
	return WriteReg;	
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_GetPulseModeSwitch
 * Description   : 设置脉冲模式开关信号值
 * Parameter     : WriteReg -----寄存器值
 *                                 
 * END ***************************************************************************************/
static int32_t  Fpga_GetPulseModeSwitch(void)
{	
	int32_t WriteReg  = IF_CmdParam_GetPulseMode();
	return WriteReg;		
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_GetPulseModePeriod
 * Description   : 设置脉冲模式周期
 * Parameter     : WriteReg -----寄存器值
 *                                 
 * END ***************************************************************************************/
static int32_t  Fpga_GetPulseModePeriod(void)
{	
	int32_t WriteReg = 0;
	int32_t value = IF_CmdParam_GetPulseFrequency();
	if(value > 0) WriteReg = 1000000*FPGA_SAMPLE_RATE/value;
	return WriteReg;
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_GetPulseModeDutyCircle
 * Description   : 设置脉冲模式占空比时间
 * Parameter     : WriteReg -----寄存器值
 *                                 
 * END ***************************************************************************************/
static int32_t  Fpga_GetPulseModeDutyCircle(void)
{	
	int32_t WriteReg = 0;	
	int32_t value  = IF_CmdParam_GetPulseDutyCircle();	
	if(value > 0&&value < 100)WriteReg = FpgaWriteReg.PulsePeriod*value/100;
	return WriteReg;
}

/* FUNCTION *********************************************************************************
 * Function Name : Fpga_GetPulseSyncOutSource
 * Description   : 设置脉冲同步输出信号源
 * Parameter     : WriteReg -----寄存器值
 *                                 
 * END ***************************************************************************************/
static int32_t  Fpga_GetPulseSyncOutSource(void)
{	
	int32_t WriteReg = IF_CmdParam_GetSyncOutSource();
	return WriteReg;
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_GetPulseSyncOutEnable
 * Description   : 设置脉冲同步输出模式使能
 * Parameter     : WriteReg -----寄存器值
 *                                 
 * END ***************************************************************************************/
static int32_t  Fpga_GetPulseSyncOutEnable(void)
{	
	int32_t WriteReg = IF_CmdParam_GetSyncOutEnable();
	return WriteReg;
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_GetPulseSyncOutDelay
 * Description   : 设置脉冲同步输出输出延时
 * Parameter     : WriteReg -----寄存器值
 *                                 
 * END ***************************************************************************************/
static int32_t  Fpga_GetPulseSyncOutDelay(void)
{	
	int32_t value = IF_CmdParam_GetSyncOutDelay();
	int32_t WriteReg  = FPGA_SAMPLE_RATE * value;
	return WriteReg;			
}

/* FUNCTION *********************************************************************************
 * Function Name : Fpga_GetPulseFeedbackMode
 * Description   : 设置脉冲反馈模式
 * Parameter     : WriteReg -----寄存器值
 *                                 
 * END ***************************************************************************************/
static int32_t  Fpga_GetPulseFeedbackMode(void)
{	
	int32_t WriteReg = IF_InternalParam_GetFeedbackMode();
	return WriteReg;	
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_GetPulseFeedbackPreMask
 * Description   : 设置脉冲反馈模式前沿延时值
 * Parameter     : WriteReg -----寄存器值
 *                                 
 * END ***************************************************************************************/
static int32_t  Fpga_GetPulseFeedbackPreMask(void)
{	
	int32_t value = IF_InternalParam_GetFeedPreMask();
	int32_t WriteReg = FPGA_SAMPLE_RATE*value;
	return WriteReg;	
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_GetPulseFeedbackPostMask
 * Description   : 设置脉冲反馈模式后沿延时值
 * Parameter     : WriteReg -----寄存器值
 *                                 
 * END ***************************************************************************************/
static int32_t  Fpga_GetPulseFeedbackPostMask(void)
{	
	
	int32_t WriteReg = 0;
	int32_t value = IF_InternalParam_GetFeedPostMask();
    int32_t PostMaskTimer = FPGA_SAMPLE_RATE*value;
	if(FpgaWriteReg.PulseDuty > PostMaskTimer)
	{
		WriteReg = FpgaWriteReg.PulseDuty - PostMaskTimer;	
	}
	return WriteReg;
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_GetPhaseLevelToLevel
 * Description   : 设置levelToLevel相位值(精度转换 1/10)
 * Parameter     : WriteReg -----寄存器值
 *                                 
 * END ***************************************************************************************/
static int32_t  Fpga_GetPhaseLevelToLevel(void)
{	
	int32_t  WriteReg = IF_InternalParam_GetPhaseState2()/10;  
	return WriteReg;	
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_GetPhaseStepSpeed
 * Description   : 设置levelToLevel相位值步进速度(精度转换 1/10)
 * Parameter     : WriteReg -----寄存器值
 *                                 
 * END ***************************************************************************************/
static int32_t  Fpga_GetPhaseStepSpeed(void)
{	
	int32_t WriteReg  = IF_InternalParam_GetPhaseStepSpeed()/10;
	return WriteReg;
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_GetPhaseStepTimer
 * Description   : 设置levelToLevel相位值步进时间(精度转换 1/1000)
 * Parameter     : WriteReg -----寄存器值
 *                                 
 * END ***************************************************************************************/
static int32_t  Fpga_GetPhaseStepTimer(void)
{	
	int32_t value = IF_InternalParam_GetPhaseStepTimer(); 
	int32_t WriteReg = FPGA_SAMPLE_RATE*value/1000;
	return WriteReg;		
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
	uint8_t RegData[4];
	for(uint8_t i = 0; i <4;i++)
	{
		RegData[i] = IF_HAL_FpgaReg_Read(RegAddress+i);
	}
	int32_t value = MAKE_UINT32(RegData[3],RegData[2],RegData[1],RegData[0]);
	return value;
}

/* FUNCTION *********************************************************************************
 * Function Name : Sensor_2G_GetMcuRegister
 * Description   : 得到MCU寄存器的参数
 * Parameter     : 
 *                                 
 * END ***************************************************************************************/
static void Sensor_2G_SetMcuRegister(uint16_t RegAddress,int32_t value)
{
	uint8_t RegData[4]={0};
	RegData[0]= Byte0_UINT32(value);
	RegData[1]= Byte1_UINT32(value);
	RegData[2]= Byte2_UINT32(value);
	RegData[3]= Byte3_UINT32(value);
	for(uint8_t i = 0; i < 4;i++)
	{
		IF_HAL_FpgaReg_Write(RegAddress+i,RegData[i]);
	}	
}
//*****************************************************************************
//* END
//*****************************************************************************


