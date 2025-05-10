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


/* FUNCTION PROTOTYPES */
static uint8_t GetSensorRegisterValue(void);
static void GetInputSensorValue(void);
static void SetOutputSensorValue(void);
static float GetVSWRReflectance(float vswr_r,float vswr_x);
static void Fpga_SetDDSDriverSwitch(UINT32_Value WriteReg);
static void Fpga_SetDDSChannelSetting(UINT32_Value WriteReg);
static void Fpga_SetWorkFreqSetting(UINT32_Value WriteReg);
static void Fpga_SetDDSWorkPhaseSetting(UINT32_Value WriteReg);
static void Fpga_SetACDCVoltSetting(UINT32_Value WriteReg);
static void Fpga_SetACDCurrentSetting(UINT32_Value WriteReg);
static void Fpga_SetACDCDriverSwitch(UINT32_Value WriteReg);
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
 * Function Name : Sensor_Fpga_Task
 * Description   : 得到Fpga VI传感器的数据任务
 * Parameter     : 无
 *                                 
 * END ***************************************************************************************/
void Sensor_Fpga_Task(void)
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
    uint8_t ret =OFF,j=0;
    uint8_t regData[REGNUM_R] = {0};
	
    /*************************读取FPGA寄存器数据到寄存器缓冲区*******************************/
    if(0 == IF_HAL_FpgaReg_ReadStart())
	{
		for(uint16_t i = 0; i < REGNUM_R; i++)
		{
			regData[i] = IF_HAL_FpgaReg_Read(i);
		}
		// FPGA版本号转换成ASCII码
		FpgaReg.Version[j++] = regData[3];
		FpgaReg.Version[j++] = regData[2];
		FpgaReg.Version[j++] = regData[1];
		FpgaReg.Version[j++] = regData[0];
		FpgaReg.Version[j++] = regData[7];	
		FpgaReg.Version[j++] = regData[6];	
		FpgaReg.Version[j++] = regData[5];	
		FpgaReg.Version[j++] = regData[4];	
		
		FpgaReg.temperture     	  = MAKE_UINT32(regData[11], regData[10], regData[9], regData[8]);
		FpgaReg.RFDrainVolt  	  = MAKE_UINT32(regData[15], regData[14], regData[13], regData[12]);
		FpgaReg.FeedbackFreq  	  = MAKE_UINT32(regData[19], regData[18], regData[17], regData[16]);
		FpgaReg.FeedbackVrms      = MAKE_UINT32(regData[23], regData[22], regData[21], regData[20]);
		FpgaReg.FeedbackIrms      = MAKE_UINT32(regData[27], regData[26], regData[25], regData[24]);
		FpgaReg.FeedbackPhase 	  = MAKE_UINT32(regData[31], regData[30], regData[29], regData[28]);
		
		
		FpgaReg.DDSSignSwitch  	  = MAKE_UINT32(regData[167], regData[166], regData[165], regData[164]);
		FpgaReg.ChannelSetting 	  = MAKE_UINT32(regData[171], regData[170], regData[169], regData[168]);
		FpgaReg.FreqSetting  	  = MAKE_UINT32(regData[175], regData[174], regData[173], regData[172]);
		FpgaReg.PhaseSetting  	  = MAKE_UINT32(regData[179], regData[178], regData[177], regData[176]);
		FpgaReg.ACDCVoltSetting   = MAKE_UINT32(regData[183], regData[182], regData[181], regData[180]);
		FpgaReg.ACDCCurrentSetting= MAKE_UINT32(regData[187], regData[186], regData[185], regData[184]);
		FpgaReg.ACDCSwitchSetting = MAKE_UINT32(regData[191], regData[190], regData[189], regData[188]);
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
	float cosRad =0,sinRad =0,Z =0,R =0, X = 0,vswr_t =0,P_dlv =0,P_fwd =0,P_ref =0,VSWR = 100;
	
	float Freq =  (1026 - FpgaReg.FeedbackFreq)* FREQ_2M_RATE; 
	float V_rms = FpgaReg.FeedbackVrms*(g_FactorData.VrmsFactor * Freq + g_FactorData.VrmsOffset);
    float I_rms = FpgaReg.FeedbackIrms*(g_FactorData.IrmsFactor * Freq + g_FactorData.IrmsOffset);
	float Phase = FpgaReg.FeedbackPhase*ARC_RAD_VALUE + (g_FactorData.PhaseFactor * Freq + g_FactorData.PhaseOffset);
	
//	float temp = (FpgaReg.temperture*503.975F)/4096.0F-273.15F;
//	float DrainVot = DrainGain*(FpgaReg.RFDrainVolt/4096.0F)*2.048F*0.9877F+DrainOffset;
	
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
		 double PhaseRad = Phase/ARC;
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
float IF_Fpga_GetSensor(uint8_t ChnN)
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
static void SetOutputSensorValue(void)
{
	UINT32_Value WriteReg;
	WriteReg.Val = IF_InternalParam_GetDDSChannelNo();
	Fpga_SetDDSChannelSetting(WriteReg);	
	
	WriteReg.Val = IF_InternalParam_GetWorkCenterFreq();	
	Fpga_SetWorkFreqSetting(WriteReg);	
	
	WriteReg.Val = IF_InternalParam_GetACDCCurrent();	
	Fpga_SetACDCurrentSetting(WriteReg);
	
	WriteReg.Val = IF_CmdParam_GetDDSDriverState();
	Fpga_SetDDSDriverSwitch(WriteReg);
	
	WriteReg.Val = IF_CmdParam_GetACDCDriverState();
	Fpga_SetACDCDriverSwitch(WriteReg);
	
	WriteReg.Val = IF_CmdParam_GetACDCVoltage();
	Fpga_SetACDCVoltSetting(WriteReg);
		
	WriteReg.Val = IF_CmdParam_GetDDSWorkPhase();	
	Fpga_SetDDSWorkPhaseSetting(WriteReg);		   
}	


//////////////////////////////////////////////////////////////////////////////////////////
uint8_t IF_Sensor_GetVersion(uint8_t *pBuf)
{
	uint8_t j = 0;
	for(uint8_t i = 0;i < 8;i++)
	{
		pBuf[j++]= FpgaReg.Version[i];
	}
    return j;
}

/* FUNCTION *********************************************************************************
 * Function Name : IF_Fpga_SetDDSDriverSwitch
 * Description   : 设置DDS信号开关
 * Parameter     : value -----开关值
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetDDSDriverSwitch(UINT32_Value WriteReg)
{
	if(FpgaReg.DDSSignSwitch != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(DDS_SWITCHSTATE_ADDR + i,WriteReg.v[i]);
		}
	}
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_SetDDSChannelSetting
 * Description   : 设置通道信号量
 * Parameter     : value -----通道信号值
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetDDSChannelSetting(UINT32_Value WriteReg)
{
	if(FpgaReg.ChannelSetting != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(CHANNEL_SETTING_ADDR + i,WriteReg.v[i]);
		}
	}
}
/* FUNCTION *********************************************************************************
 * Function Name : IF_Fpga_SetWorkFreqSetting
 * Description   : 设置频率信号值
 * Parameter     : value -----频率值
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetWorkFreqSetting(UINT32_Value WriteReg)
{	
	if(FpgaReg.FreqSetting != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(FREQUENCY_SETTING_ADDR + i,WriteReg.v[i]);
		}
	}
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_SetDDSWorkPhaseSetting
 * Description   : 设置相位信号值
 * Parameter     : value -----相位值
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetDDSWorkPhaseSetting(UINT32_Value WriteReg)
{	
	WriteReg.Val = WriteReg.Val/10;
	if(FpgaReg.PhaseSetting != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(PAHSE_SETTING_ADDR + i,WriteReg.v[i]);
		}
	}
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_SetACDCVoltSetting
 * Description   : 设置电压信号值
 * Parameter     : value -----电压值
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetACDCVoltSetting(UINT32_Value WriteReg)
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
	if(FpgaReg.ACDCVoltSetting != WriteReg.Val)
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
static void  Fpga_SetACDCurrentSetting(UINT32_Value WriteReg)
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
	if(FpgaReg.ACDCCurrentSetting != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(ACDC_CURRENT_ADDR + i,WriteReg.v[i]);
		}
	}
}
/* FUNCTION *********************************************************************************
 * Function Name : Fpga_SetACDCDriverSwitch
 * Description   : 设置ACDC开关信号值
 * Parameter     : value -----开关信号值
 *                                 
 * END ***************************************************************************************/
static void  Fpga_SetACDCDriverSwitch(UINT32_Value WriteReg)
{	
	if(FpgaReg.ACDCSwitchSetting != WriteReg.Val)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			IF_HAL_FpgaReg_Write(ACDC_SWITCHSTATE_ADDR + i,WriteReg.v[i]);
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
//*****************************************************************************
//* END
//*****************************************************************************


