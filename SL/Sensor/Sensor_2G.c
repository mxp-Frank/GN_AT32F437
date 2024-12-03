/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Sensor_2G.c
 * @Brief :      
 * @Author :     Chenghanbing (iwanthaha@sina.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-21
 * ****************************************************************************
 */

//#include "board.h"
#include "IF_Sensor.h"
#include "params_impl.h"
#include "IF_HAL_Cfg.h"
#include "arm_math.h"
#include "IF_Interface.h"
#include "Sensor_2G.h"

/* LOCAL VARIABLES */
 volatile uint8_t Register[REGNUM_R] = {0}; 
 volatile SensorReg_t FpgaReg[CHN_NUM];		
 static SensorData_2G_t InputData[CHN_NUM];
 static SensorData_2G_t MainOutputData[CHN_NUM];
 static SensorData_2G_t AuxOutputData[CHN_NUM];

/* FUNCTION PROTOTYPES */
static uint8_t GetSensorRegisterValue_2G(void);
static void LF_CHN_GetInputValue_2G(void);
#if(config_LowFrequency ==1)
	#if(config_MainOutputSensor ==1)
		static void LF_CHN_GetMainOutputSensorValue_2G(void);
	#endif
	#if(config_AuxiOutputSensor ==1)
		static void LF_CHN_GetAuxiOutputSensorValue_2G(void);
	#endif
#endif

#if(config_HighFrequency ==1)
	static void HF_CHN_GetInputValue_2G(void);
	#if(config_MainOutputSensor ==1)
		static void HF_CHN_GetMainOutputSensorValue_2G(void);
	#endif
	#if(config_AuxiOutputSensor ==1)
		static void HF_CHN_GetAuxiOutputSensorValue_2G(void);
	#endif
#endif
/* FUNCTION *********************************************************************************
 * Function Name : Sensor_2G_Task
 * Description   : 得到二代VI传感器的数据任务
 * Parameter     : 无
 *                                 
 * END ***************************************************************************************/
void IF_Sensor_2G_ParamInit(void)
{
	for(uint8_t ChnNo=0;ChnNo<CHN_NUM;ChnNo++)
	{
		memset((void*)&InputData[ChnNo],0,sizeof(SensorData_2G_t)); 
		memset((void*)&MainOutputData[ChnNo],0,sizeof(SensorData_2G_t)); 
		memset((void*)&AuxOutputData[ChnNo],0,sizeof(SensorData_2G_t)); 
	}
}	

/* FUNCTION *********************************************************************************
 * Function Name : Sensor_2G_Task
 * Description   : 得到二代VI传感器的数据任务
 * Parameter     : 无
 *                                 
 * END ***************************************************************************************/
void Sensor_2G_Task(void)
{
   if( 1== GetSensorRegisterValue_2G())
   {
	    #if(config_LowFrequency == 1)
			taskENTER_CRITICAL();
			LF_CHN_GetInputValue_2G();
			taskEXIT_CRITICAL();
			#if(config_MainOutputSensor ==1)
				LF_CHN_GetMainOutputSensorValue_2G();
			#endif
			#if(config_AuxiOutputSensor ==1)
				LF_CHN_GetAuxiOutputSensorValue_2G();
			#endif	
	   #endif
	   
	   #if(config_HighFrequency == 1)
			taskENTER_CRITICAL();
			HF_CHN_GetInputValue_2G();
			taskEXIT_CRITICAL();
		   #if(config_MainOutputSensor ==1)
				HF_CHN_GetMainOutputSensorValue_2G();
		   #endif
			#if(config_AuxiOutputSensor ==1)
				HF_CHN_GetAuxiOutputSensorValue_2G();				
			#endif		 
	   #endif	  
   }	
}
/* FUNCTION *********************************************************************************
 * Function Name : GetSensorValue_2G(SensorData_2G_t *pSensorData)
 * Description   : 得到二代VI传感器的FPGA寄存器器值
 * Parameter     : pSensorData VI传感器数据结构体
 *                                 
 * END ***************************************************************************************/
static uint8_t  GetSensorRegisterValue_2G(void)
{
    uint8_t ret =OFF, j = 0; 
    /*************************读取FPGA寄存器数据到寄存器缓冲区*******************************/
    IF_HAL_FpgaReg_ReadStart();

    for(uint8_t i = 0; i < REGNUM_R; i++)
    {
        Register[i] = IF_HAL_FpgaReg_Read(i);
    }
	IF_HAL_FpgaReg_ReadEnd();	
	// FPGA版本号转换成ASCII码
	for(uint8_t i = 0;i< 4;i++)
	{
		uint8_t HighReg = (Register[3-i] & 0xF0)>>4;
		uint8_t LowReg =  (Register[3-i] & 0x0F);
		if((HighReg > 0)&&(HighReg < 10))
		{
			FpgaReg[LF_CHN].Version[j++] = HighReg + 0x30;
		}
		if(LowReg < 10)
		{
			FpgaReg[LF_CHN].Version[j++] = LowReg + 0x30;
		}
	}
	
#if(config_LowFrequency == 1)
		FpgaReg[LF_CHN].Freq     = MAKE_UINT32(Register[27], Register[26], Register[25], Register[24]);
		FpgaReg[LF_CHN].Voltage  = MAKE_UINT32(Register[31], Register[30], Register[29], Register[28]);
		FpgaReg[LF_CHN].Current  = MAKE_UINT32(Register[35], Register[34], Register[33], Register[32]);
		FpgaReg[LF_CHN].Phase    = MAKE_UINT32(Register[39], Register[38], Register[37], Register[36]);
		FpgaReg[LF_CHN].Vpp      = MAKE_UINT32(Register[43], Register[42], Register[41], Register[40]);
	#if(config_MainOutputSensor ==1)
		FpgaReg[LF_CHN].MainVolt = MAKE_UINT32(Register[47], Register[46], Register[45], Register[44]);	
		FpgaReg[LF_CHN].MainCur  = MAKE_UINT32(Register[51], Register[50], Register[49], Register[48]);
		FpgaReg[LF_CHN].MainPha  = MAKE_UINT32(Register[63], Register[62], Register[61], Register[60]);	
	#endif
		
	#if(config_AuxiOutputSensor ==1)
		FpgaReg[LF_CHN].AuxiVolt = MAKE_UINT32(Register[55], Register[54], Register[53], Register[52]);
		FpgaReg[LF_CHN].AuxiCur  = MAKE_UINT32(Register[59], Register[58], Register[57], Register[56]);
		FpgaReg[LF_CHN].AuxiPha  = MAKE_UINT32(Register[67], Register[66], Register[65], Register[64]);
	#endif
	FpgaReg[LF_CHN].PulseMode	 = MAKE_UINT32(Register[99], Register[98], Register[97], Register[96]);
	FpgaReg[LF_CHN].Duty  	 	 = MAKE_UINT32(Register[135], Register[134], Register[133], Register[132]);
	FpgaReg[LF_CHN].Period   	 = MAKE_UINT32(Register[139], Register[138], Register[137], Register[136]);
#endif

#if(config_HighFrequency == 1)
		FpgaReg[HF_CHN].Freq     = MAKE_UINT32(Register[7], Register[6], Register[5], Register[4]);
		FpgaReg[HF_CHN].Voltage  = MAKE_UINT32(Register[11], Register[10], Register[9], Register[8]);   
		FpgaReg[HF_CHN].Current  = MAKE_UINT32(Register[15], Register[14], Register[13], Register[12]);
		FpgaReg[HF_CHN].Phase    = MAKE_UINT32(Register[19], Register[18], Register[17], Register[16]);
		FpgaReg[HF_CHN].Vpp      = MAKE_UINT32(Register[23], Register[22], Register[21], Register[20]);	
	#if(config_MainOutputSensor ==1)
		FpgaReg[HF_CHN].MainVolt = MAKE_UINT32(Register[71], Register[70], Register[69], Register[68]);
		FpgaReg[HF_CHN].MainCur  = MAKE_UINT32(Register[75], Register[74], Register[73], Register[72]);
		FpgaReg[HF_CHN].MainPha  = MAKE_UINT32(Register[87], Register[86], Register[85], Register[84]);
	#endif
	
	#if(config_AuxiOutputSensor ==1)
		FpgaReg[HF_CHN].AuxiVolt = MAKE_UINT32(Register[79], Register[78], Register[77], Register[76]);
		FpgaReg[HF_CHN].AuxiCur  = MAKE_UINT32(Register[83], Register[82], Register[81], Register[80]);
		FpgaReg[HF_CHN].AuxiPha  = MAKE_UINT32(Register[91], Register[90], Register[89], Register[88]);
	#endif
	FpgaReg[HF_CHN].PulseMode	 = MAKE_UINT32(Register[103], Register[102], Register[101], Register[100]);
	FpgaReg[HF_CHN].Duty  	 	 = MAKE_UINT32(Register[143], Register[142], Register[141], Register[140]);
	FpgaReg[HF_CHN].Period   	 = MAKE_UINT32(Register[147], Register[146], Register[145], Register[144]);
#endif
	ret = ON;
   
	return ret;		
}

#if(config_LowFrequency == 1)
/* FUNCTION *********************************************************************************
 * Function Name : GetInputValue_2G(uint8_t ChnNo)(低频400KHz)
 * Description   : 得到二代VI传感器的InputSensor计算值
 * Parameter     : pSensorData VI传感器数据结构体
 *                                 
 * END ***************************************************************************************/

static void LF_CHN_GetInputValue_2G(void)	
{
    float Zreal,Zimag,vswr_r, vswr_x,vswr_tr, vswr_ti, vswr_t;
	float VrmsFactor,IrmsFactor,PhaseFactor;
	double Vpp,Freq,V_rms,I_rms,Phase,P_dlv,P_fwd,P_ref, Z,R,X,VSWR = 20;
	
	VrmsFactor =  0.001f*IF_NvmParam_GetInputVrmsFactor(1);
	IrmsFactor =  0.001f*IF_NvmParam_GetInputIrmsFactor(1);
	PhaseFactor = 0.001f*IF_NvmParam_GetInputPhaseFactor(1);

	Vpp =  ((double)FpgaReg[LF_CHN].Vpp/65536.0f/2048.0f); 
	Freq =  ((double)(1024 - (0x3FF & FpgaReg[LF_CHN].Freq) + 2)*64/1024.0f); 
	V_rms = ((double)FpgaReg[LF_CHN].Voltage/65536.0f)*VrmsFactor/2048.0f;
    I_rms = ((double)FpgaReg[LF_CHN].Current/65536.0f)*IrmsFactor/2048.0f;
    Phase = ((double)FpgaReg[LF_CHN].Phase*180/(1048576*(-1*M_PI)) +PhaseFactor);
    if(Phase < -180)
    {
        Phase = Phase + 360;
    }
    else if(Phase > 180)
    {
        Phase = Phase - 360;
    }
	double Phase_rad = (double)Phase*M_PI/180;   //angle-->radian
	Z = (float)(V_rms/I_rms);	
    R = Z*arm_cos_f32(Phase_rad);
    X = Z*arm_sin_f32(Phase_rad);
	
	if(FpgaReg[LF_CHN].PulseMode == 0xA0A0BBCC)  //脉冲模式下
	{
		Zreal  = 0.001F * IF_NvmParam_GetTargetReal(2); 
		Zimag  = 0.001F * IF_NvmParam_GetTargetImage(2);
	}else
	{
		Zreal  = 0.001F * IF_NvmParam_GetTargetReal(1); 
		Zimag  = 0.001F * IF_NvmParam_GetTargetImage(1);
	}
	vswr_r = (R*Zreal + X*Zimag)/(Zreal*Zreal + Zimag*Zimag);
	vswr_x = (X*Zreal - R*Zimag)/(Zreal*Zreal + Zimag*Zimag);
	
	vswr_r = (R*Zreal + X*Zimag)/(Zreal*Zreal + Zimag*Zimag);
	vswr_x = (X*Zreal - R*Zimag)/(Zreal*Zreal + Zimag*Zimag);
	
    vswr_tr = (vswr_r*vswr_r + vswr_x*vswr_x - 1)/((vswr_r + 1)*(vswr_r + 1) + vswr_x * vswr_x);
    vswr_ti = (2*vswr_x)/((vswr_r + 1)*(vswr_r + 1) + vswr_x*vswr_x);
    arm_sqrt_f32((vswr_tr*vswr_tr + vswr_ti*vswr_ti), &vswr_t);
   	if(vswr_t<1)VSWR = (1+vswr_t)/(1-vswr_t);
	
	P_dlv = V_rms*I_rms*arm_cos_f32(Phase_rad);
    P_fwd = P_dlv/(1-vswr_t*vswr_t);
    P_ref = P_fwd - P_dlv;
    /***************Input Sensor Data output***********/
	InputData[LF_CHN].Vpp = Vpp;
    InputData[LF_CHN].Freq = Freq;
    InputData[LF_CHN].V_rms = V_rms;
    InputData[LF_CHN].I_rms = I_rms;
    InputData[LF_CHN].Phase = Phase;
    InputData[LF_CHN].P_dlv = P_dlv;
    InputData[LF_CHN].P_fwd = P_fwd;
    InputData[LF_CHN].P_ref = P_ref;
    InputData[LF_CHN].Z = Z;
    InputData[LF_CHN].R = R;
    InputData[LF_CHN].X = X;
    InputData[LF_CHN].VSWR = VSWR;
}
#if(config_MainOutputSensor ==1)
	/* FUNCTION *********************************************************************************
	 * Function Name : GetMainOutputSensorValue_2G(uint8_t ChnNo)
	 * Description   : 得到二代VI传感器的InputSensor计算值
	 * Parameter     : pSensorData VI传感器数据结构体
	 *                                 
	 * END ***************************************************************************************/
	static void LF_CHN_GetMainOutputSensorValue_2G(void)	
	{
		float Zreal,Zimag,vswr_r, vswr_x,vswr_tr, vswr_ti, vswr_t;
		float VrmsFactor,IrmsFactor,PhaseFactor;
		double V_rms,I_rms,Phase,P_dlv,P_fwd,P_ref, Z,R,X,VSWR = 20;
		
		//Factor =a2*x^2+a1*x+a0
		VrmsFactor  = 0.001f * IF_NvmParam_GetMainOutputVrmsFactor(1);
		IrmsFactor  = 0.001f * IF_NvmParam_GetMainOutputIrmsFactor(1);
		PhaseFactor = 0.001f * IF_NvmParam_GetMainOutputPhaseFactor(1);
	 
		V_rms = ((double)FpgaReg[LF_CHN].MainVolt/65536.0f)*VrmsFactor/2048.0f;
		I_rms = ((double)FpgaReg[LF_CHN].MainCur/65536.0f)*IrmsFactor/2048.0f;
		Phase = ((double)FpgaReg[LF_CHN].MainPha*180/(1048576*(-1*M_PI)) + PhaseFactor);
		if(Phase < -180)
		{
			Phase = Phase + 360;
		}
		else if(Phase > 180)
		{
			Phase = Phase - 360;
		}
		if(Phase < -89.5f)
		{
			Phase = -89.5f;
		}
		else if(Phase > 89.5f)
		{
			Phase = 89.5f;
		}
		double Phase_rad = (double)Phase*M_PI/180;//angle-->radian
		Z = (float)(V_rms/I_rms);	
		R = Z*arm_cos_f32(Phase_rad);
		X = Z*arm_sin_f32(Phase_rad);
		
		if(FpgaReg[LF_CHN].PulseMode == 0xA0A0BBCC)  //脉冲模式下
		{
			Zreal  = 0.001F * IF_NvmParam_GetTargetReal(2); 
			Zimag  = 0.001F * IF_NvmParam_GetTargetImage(2);
		}else
		{
			Zreal  = 0.001F * IF_NvmParam_GetTargetReal(1); 
			Zimag  = 0.001F * IF_NvmParam_GetTargetImage(1);
		}
		vswr_r = (R*Zreal + X*Zimag)/(Zreal*Zreal + Zimag*Zimag);
		vswr_x = (X*Zreal - R*Zimag)/(Zreal*Zreal + Zimag*Zimag);
		
		vswr_tr = (vswr_r*vswr_r + vswr_x*vswr_x - 1)/((vswr_r + 1)*(vswr_r + 1) + vswr_x * vswr_x);
		vswr_ti = (2*vswr_x)/((vswr_r + 1)*(vswr_r + 1) + vswr_x*vswr_x);
		arm_sqrt_f32((vswr_tr*vswr_tr + vswr_ti*vswr_ti), &vswr_t);
		if(vswr_t<1)VSWR = (1+vswr_t)/(1-vswr_t);
		
		P_dlv = V_rms*I_rms*arm_cos_f32(Phase_rad);
		P_fwd = P_dlv/(1-vswr_t*vswr_t);
		P_ref = P_fwd - P_dlv;
		/***************Input Sensor Data output***********/
		MainOutputData[LF_CHN].V_rms = V_rms;
		MainOutputData[LF_CHN].I_rms = I_rms;
		MainOutputData[LF_CHN].Phase = Phase;
		MainOutputData[LF_CHN].P_dlv = P_dlv;
		MainOutputData[LF_CHN].P_fwd = P_fwd;
		MainOutputData[LF_CHN].P_ref = P_ref;
		MainOutputData[LF_CHN].Z = Z;
		MainOutputData[LF_CHN].R = R;
		MainOutputData[LF_CHN].X = X;
		MainOutputData[LF_CHN].VSWR = VSWR;
	}
#endif
	 
	
	/* FUNCTION *********************************************************************************
	 * Function Name : GetMainOutputSensorValue_2G(uint8_t ChnNo)
	 * Description   : 得到二代VI传感器的InputSensor计算值
	 * Parameter     : pSensorData VI传感器数据结构体
	 *                                 
	 * END ***************************************************************************************/
#if(config_AuxiOutputSensor == 1)
	static void LF_CHN_GetAuxiOutputSensorValue_2G(void)	
	{

		float Zreal,Zimag,vswr_r, vswr_x,vswr_tr, vswr_ti, vswr_t;
		float VrmsFactor,IrmsFactor,PhaseFactor;
		double V_rms,I_rms,Phase,P_dlv,P_fwd,P_ref, Z,R,X,VSWR = 20;
		//Factor =a2*x^2+a1*x+a0
		VrmsFactor  = 0.001f * IF_NvmParam_GetAuxiOutputVrmsFactor(1);
		IrmsFactor  = 0.001f * IF_NvmParam_GetAuxiOutputIrmsFactor(1);
		PhaseFactor = 0.001f * IF_NvmParam_GetAuxiOutputPhaseFactor(1);
		//Factor =a2*x^2+a1*x+a0
		V_rms = ((double)FpgaReg[LF_CHN].AuxiVolt/65536.0f)*VrmsFactor/2048.0f;
		I_rms = ((double)FpgaReg[LF_CHN].AuxiCur/65536.0f)*IrmsFactor/2048.0f;
		Phase = ((double)FpgaReg[LF_CHN].AuxiPha*180/(1048576*(-1*M_PI)) + PhaseFactor);
		
		if(Phase < -180)
		{
			Phase = Phase + 360;
		}
		else if(Phase > 180)
		{
			Phase = Phase - 360;
		}
		if(Phase < -89.5f)
		{
			Phase = -89.5f;
		}
		else if(Phase > 89.5f)
		{
			Phase = 89.5f;
		}
		double Phase_rad  = (double)Phase*M_PI/180;  //angle-->radian	
		Z = V_rms/I_rms;
		R = Z*arm_cos_f32(Phase_rad);
		X = Z*arm_sin_f32(Phase_rad);

		if(FpgaReg[LF_CHN].PulseMode == 0xA0A0BBCC)  //脉冲模式下
		{
			Zreal  = 0.001F * IF_NvmParam_GetTargetReal(2); 
			Zimag  = 0.001F * IF_NvmParam_GetTargetImage(2);
		}else
		{
			Zreal  = 0.001F * IF_NvmParam_GetTargetReal(1); 
			Zimag  = 0.001F * IF_NvmParam_GetTargetImage(1);
		}
		vswr_r = (R*Zreal + X*Zimag)/(Zreal*Zreal + Zimag*Zimag);
		vswr_x = (X*Zreal - R*Zimag)/(Zreal*Zreal + Zimag*Zimag);
		
		vswr_tr = (vswr_r*vswr_r + vswr_x*vswr_x - 1)/((vswr_r + 1)*(vswr_r + 1) + vswr_x * vswr_x);
		vswr_ti = (2*vswr_x)/((vswr_r + 1)*(vswr_r + 1) + vswr_x*vswr_x);
		arm_sqrt_f32((vswr_tr*vswr_tr + vswr_ti*vswr_ti), &vswr_t);
		if(vswr_t<1)VSWR = (1+vswr_t)/(1-vswr_t);
		
		P_dlv = V_rms*I_rms*arm_cos_f32(Phase_rad);
		P_fwd = P_dlv/(1-vswr_t*vswr_t);
		P_ref = P_fwd - P_dlv;
		/***************Input Sensor Data output***********/
		AuxOutputData[LF_CHN].V_rms = V_rms;
		AuxOutputData[LF_CHN].I_rms = I_rms;
		AuxOutputData[LF_CHN].Phase = Phase;
		AuxOutputData[LF_CHN].P_dlv = P_dlv;
		AuxOutputData[LF_CHN].P_fwd = P_fwd;
		AuxOutputData[LF_CHN].P_ref = P_ref;
		AuxOutputData[LF_CHN].Z = Z;
		AuxOutputData[LF_CHN].R = R;
		AuxOutputData[LF_CHN].X = X;
		AuxOutputData[LF_CHN].VSWR = VSWR;
	}
	#endif
#endif

#if(config_HighFrequency == 1)
/* FUNCTION *********************************************************************************
 * Function Name : GetInputValue_2G(uint8_t ChnNo)
 * Description   : 得到二代VI传感器的InputSensor计算值(高频40.68MHz)
 * Parameter     : pSensorData VI传感器数据结构体
 *                                 
 * END ***************************************************************************************/
static void HF_CHN_GetInputValue_2G(void)	
{
	float Zreal,Zimag,vswr_r, vswr_x,vswr_tr, vswr_ti, vswr_t;
	float VrmsFactor,IrmsFactor,PhaseFactor;
	double Vpp,Freq,V_rms,I_rms,Phase,P_dlv,P_fwd,P_ref, Z,R,X,VSWR = 20;
	//Factor =a2*x^2+a1*x+a0
	VrmsFactor = 0.001f*IF_NvmParam_GetInputVrmsFactor(HF_CHN,1);
	IrmsFactor = 0.001f*IF_NvmParam_GetInputIrmsFactor(HF_CHN,1);
	PhaseFactor  =  0.001f*IF_NvmParam_GetInputPhaseFactor(HF_CHN,1);
	
	Vpp =  FpgaReg[HF_CHN].Vpp/65536.0f/2048.0f; 
	Freq = ((double)(64-(1024 - (0x3FF & FpgaReg[HF_CHN].Freq) + 2)*64/1024.0f)); 
	V_rms = ((double)FpgaReg[HF_CHN].Voltage/65536.0f)*VrmsFactor/2048.0f;
    I_rms = ((double)FpgaReg[HF_CHN].Current/65536.0f)*IrmsFactor/2048.0f;
    Phase = ((double)FpgaReg[HF_CHN].Phase*180/(1048576*(1*M_PI)) +PhaseFactor);
    if(Phase < -180)
    {
        Phase = Phase + 360;
    }
    else if(Phase > 180)
    {
        Phase = Phase - 360;
    }
	double Phase_rad  = (double)Phase*M_PI/180;  //angle-->radian	
	Z = V_rms/I_rms;
	R = Z*arm_cos_f32(Phase_rad);
	X = Z*arm_sin_f32(Phase_rad);

	if(FpgaReg[HF_CHN].PulseMode == 0xA0A0BBCC)  //脉冲模式下
	{
		Zreal  = 0.001F * IF_NvmParam_GetTargetReal(HF_CHN,2); 
		Zimag  = 0.001F * IF_NvmParam_GetTargetImage(HF_CHN,2);
	}else
	{
		Zreal  = 0.001F * IF_NvmParam_GetTargetReal(HF_CHN,1); 
		Zimag  = 0.001F * IF_NvmParam_GetTargetImage(HF_CHN,1);
	}
	vswr_r = (R*Zreal + X*Zimag)/(Zreal*Zreal + Zimag*Zimag);
	vswr_x = (X*Zreal - R*Zimag)/(Zreal*Zreal + Zimag*Zimag);
	
	vswr_tr = (vswr_r*vswr_r + vswr_x*vswr_x - 1)/((vswr_r + 1)*(vswr_r + 1) + vswr_x * vswr_x);
	vswr_ti = (2*vswr_x)/((vswr_r + 1)*(vswr_r + 1) + vswr_x*vswr_x);
	arm_sqrt_f32((vswr_tr*vswr_tr + vswr_ti*vswr_ti), &vswr_t);
	if(vswr_t<1)VSWR = (1+vswr_t)/(1-vswr_t);
	
	P_dlv = V_rms*I_rms*arm_cos_f32(Phase_rad);
    P_fwd = P_dlv/(1-vswr_t*vswr_t);
    P_ref = P_fwd - P_dlv;
    /***************Input Sensor Data output***********/
	InputData[HF_CHN].Vpp = Vpp;
    InputData[HF_CHN].Freq = Freq;
    InputData[HF_CHN].V_rms = V_rms;
    InputData[HF_CHN].I_rms = I_rms;
    InputData[HF_CHN].Phase = Phase;
    InputData[HF_CHN].P_dlv = P_dlv;
    InputData[HF_CHN].P_fwd = P_fwd;
    InputData[HF_CHN].P_ref = P_ref;
    InputData[HF_CHN].Z = Z;
    InputData[HF_CHN].R = R;
    InputData[HF_CHN].X = X;
    InputData[HF_CHN].VSWR = VSWR;
}

/* FUNCTION *********************************************************************************
 * Function Name : GetMainOutputSensorValue_2G(uint8_t ChnNo)
 * Description   : 得到二代VI传感器的InputSensor计算值
 * Parameter     : pSensorData VI传感器数据结构体
 *                                 
 * END ***************************************************************************************/
#if(config_MainOutputSensor ==1)
	static void HF_CHN_GetMainOutputSensorValue_2G(void)	
	{

		float Zreal,Zimag,vswr_r, vswr_x,vswr_tr, vswr_ti, vswr_t;
		float VrmsFactor,IrmsFactor,PhaseFactor;
		double Vpp,Freq,V_rms,I_rms,Phase,P_dlv,P_fwd,P_ref, Z,R,X,VSWR = 20;
		//Factor =a2*x^2+a1*x+a0
		VrmsFactor  = 0.001f * IF_NvmParam_GetMainOutputSensorVrmsFactor(HF_CHN,1);
		IrmsFactor  = 0.001f * IF_NvmParam_GetMainOutputSensorIrmsFactor(HF_CHN,1);
		PhaseFactor    = 0.001f * IF_NvmParam_GetMainOutputSensorPhaseFactor(HF_CHN,1);
		
		V_rms = ((double)FpgaReg[HF_CHN].MainVolt/65536.0f)*VrmsFactor/2048.0f;
		I_rms = ((double)FpgaReg[HF_CHN].MainCur/65536.0f)*IrmsFactor/2048.0f;
		Phase = ((double)FpgaReg[HF_CHN].MainPha*180/(1048576*(1*M_PI)) + PhaseFactor);
		if(Phase < -180)
		{
			Phase = Phase + 360;
		}
		else if(Phase > 180)
		{
			Phase = Phase - 360;
		}
		if(Phase < -89.5f)
		{
			Phase = -89.5f;
		}
		else if(Phase > 89.5f)
		{
			Phase = 89.5f;
		}
		double Phase_rad  = (double)Phase*M_PI/180;  //angle-->radian	
		Z = V_rms/I_rms;
		R = Z*arm_cos_f32(Phase_rad);
		X = Z*arm_sin_f32(Phase_rad);
		
		if(FpgaReg[HF_CHN].PulseMode == 0xA0A0BBCC)  //脉冲模式下
		{
			Zreal  = 0.001F * IF_NvmParam_GetTargetReal(HF_CHN,2); 
			Zimag  = 0.001F * IF_NvmParam_GetTargetImage(HF_CHN,2);
		}else
		{
			Zreal  = 0.001F * IF_NvmParam_GetTargetReal(HF_CHN,1); 
			Zimag  = 0.001F * IF_NvmParam_GetTargetImage(HF_CHN,1);
		}
		vswr_r = (R*Zreal + X*Zimag)/(Zreal*Zreal + Zimag*Zimag);
		vswr_x = (X*Zreal - R*Zimag)/(Zreal*Zreal + Zimag*Zimag);
		
		vswr_tr = (vswr_r*vswr_r + vswr_x*vswr_x - 1)/((vswr_r + 1)*(vswr_r + 1) + vswr_x * vswr_x);
		vswr_ti = (2*vswr_x)/((vswr_r + 1)*(vswr_r + 1) + vswr_x*vswr_x);
		arm_sqrt_f32((vswr_tr*vswr_tr + vswr_ti*vswr_ti), &vswr_t);
		if(vswr_t<1)VSWR = (1+vswr_t)/(1-vswr_t);
		
		P_dlv = V_rms*I_rms*arm_cos_f32(Phase_rad);
		P_fwd = P_dlv/(1-vswr_t*vswr_t);
		P_ref = P_fwd - P_dlv;
		/***************Input Sensor Data output***********/
		MainOutputData[HF_CHN].V_rms = V_rms;
		MainOutputData[HF_CHN].I_rms = I_rms;
		MainOutputData[HF_CHN].Phase = Phase;
		MainOutputData[HF_CHN].P_dlv = P_dlv;
		MainOutputData[HF_CHN].P_fwd = P_fwd;
		MainOutputData[HF_CHN].P_ref = P_ref;
		MainOutputData[HF_CHN].Z = Z;
		MainOutputData[HF_CHN].R = R;
		MainOutputData[HF_CHN].X = X;
		MainOutputData[HF_CHN].VSWR = VSWR;

	}
#endif

/* FUNCTION *********************************************************************************
 * Function Name : GetMainOutputSensorValue_2G(uint8_t ChnNo)
 * Description   : 得到二代VI传感器的InputSensor计算值
 * Parameter     : pSensorData VI传感器数据结构体
 *                                 
 * END ***************************************************************************************/
#if(config_AuxiOutputSensor == 1)
	static void HF_CHN_GetAuxiOutputSensorValue_2G(void)	
	{

		float Zreal,Zimag,vswr_r, vswr_x,vswr_tr, vswr_ti, vswr_t;
		float VrmsFactor,IrmsFactor,PhaseFactor;
		double Vpp,Freq,V_rms,I_rms,Phase,P_dlv,P_fwd,P_ref, Z,R,X,VSWR = 20;
		//Factor =a2*x^2+a1*x+a0
		VrmsFactor  = 0.001f * IF_NvmParam_GetAuxiOutputSensorVrmsFactor(HF_CHN,1);
		IrmsFactor  = 0.001f * IF_NvmParam_GetAuxiOutputSensorIrmsFactor(HF_CHN,1);
		PhaseFactor    = 0.001f * IF_NvmParam_GetAuxiOutputSensorPhaseFactor(HF_CHN,1);
		
		V_rms = ((double)FpgaReg[HF_CHN].AuxiVolt/65536.0f)*VrmsFactor/2048.0f;
		I_rms = ((double)FpgaReg[HF_CHN].AuxiCur/65536.0f)*IrmsFactor/2048.0f;
		Phase =  ((double)FpgaReg[HF_CHN].AuxiPha*180/(1048576*(-1*M_PI)) + PhaseFactor);
		if(Phase < -180)
		{
			Phase = Phase + 360;
		}
		else if(Phase > 180)
		{
			Phase = Phase - 360;
		}
		if(Phase < -89.5f)
		{
			Phase = -89.5f;
		}
		else if(Phase > 89.5f)
		{
			Phase = 89.5f;
		}
		double Phase_rad  = (double)Phase*M_PI/180;  //angle-->radian	
		Z = V_rms/I_rms;
		R = Z*arm_cos_f32(Phase_rad);
		X = Z*arm_sin_f32(Phase_rad);
		
		if(FpgaReg[HF_CHN].PulseMode == 0xA0A0BBCC)  //脉冲模式下
		{
			Zreal  = 0.001F * IF_NvmParam_GetTargetReal(HF_CHN,2); 
			Zimag  = 0.001F * IF_NvmParam_GetTargetImage(HF_CHN,2);
		}else
		{
			Zreal  = 0.001F * IF_NvmParam_GetTargetReal(HF_CHN,1); 
			Zimag  = 0.001F * IF_NvmParam_GetTargetImage(HF_CHN,1);
		}
		vswr_r = (R*Zreal + X*Zimag)/(Zreal*Zreal + Zimag*Zimag);
		vswr_x = (X*Zreal - R*Zimag)/(Zreal*Zreal + Zimag*Zimag);
		
		vswr_tr = (vswr_r*vswr_r + vswr_x*vswr_x - 1)/((vswr_r + 1)*(vswr_r + 1) + vswr_x * vswr_x);
		vswr_ti = (2*vswr_x)/((vswr_r + 1)*(vswr_r + 1) + vswr_x*vswr_x);
		arm_sqrt_f32((vswr_tr*vswr_tr + vswr_ti*vswr_ti), &vswr_t);
		if(vswr_t<1)VSWR = (1+vswr_t)/(1-vswr_t);
		
		P_dlv = V_rms*I_rms*arm_cos_f32(Phase_rad);
		P_fwd = P_dlv/(1-vswr_t*vswr_t);
		P_ref = P_fwd - P_dlv;
		/***************Input Sensor Data output***********/
		AuxOutputData[HF_CHN].V_rms = V_rms;
		AuxOutputData[HF_CHN].I_rms = I_rms;
		AuxOutputData[HF_CHN].Phase = Phase;
		AuxOutputData[HF_CHN].P_dlv = P_dlv;
		AuxOutputData[HF_CHN].P_fwd = P_fwd;
		AuxOutputData[HF_CHN].P_ref = P_ref;
		AuxOutputData[HF_CHN].Z = Z;
		AuxOutputData[HF_CHN].R = R;
		AuxOutputData[HF_CHN].X = X;
		AuxOutputData[HF_CHN].VSWR = VSWR;
	}
	#endif
#endif
/* FUNCTION *********************************************************************************
 * Function Name : LF_CHN_SetPulse_RFPowerThr
 * Description   : 设置低频二代VI传感器的脉冲功率阈值
 * Parameter     : value -----设置脉冲功率阈值
 *                                 
 * END ***************************************************************************************/
void Set_Sensor_2G_PulseRFPowerThr(uint8_t ChnNo,int32_t value)
{
	uint8_t RegAddress,RegData[4];
	switch(ChnNo)
	{
		case LF_CHN:RegAddress = LF_PULSE_RFPOWERTHR_ADDR;break;
		case HF_CHN:RegAddress = HF_PULSE_RFPOWERTHR_ADDR;break;
	}
	RegData[0] =  Byte0_UINT32(value);
	RegData[1] =  Byte1_UINT32(value);
	RegData[2] =  Byte2_UINT32(value);
	RegData[3] =  Byte3_UINT32(value);
	for(uint8_t i = 0;i < 4;i++)
	{
		IF_HAL_FpgaReg_Write(RegAddress + i,RegData[i]);
	}
}
void Set_Sensor_2G_PulseSyncDelay(uint8_t ChnNo,int32_t value)
{
	uint8_t  RegAddress = 0,RegData[4]={0};
	uint32_t RegValue = 0;
	switch(ChnNo)
	{
		case LF_CHN: 
			RegAddress = LF_PULSE_SYNC_DELAY_ADDR;
			RegValue = value*64;
		break;
		case HF_CHN: 
			RegAddress = HF_PULSE_SYNC_DELAY_ADDR;
			RegValue = value*64;
		break;
	}
	RegData[0] =  Byte0_UINT32(RegValue);
	RegData[1] =  Byte1_UINT32(RegValue);
	RegData[2] =  Byte2_UINT32(RegValue);
	RegData[3] =  Byte3_UINT32(RegValue);
	for(uint8_t i = 0;i < 4;i++)
	{
		IF_HAL_FpgaReg_Write(RegAddress + i,RegData[i]);
	}	
}
void Set_Sensor_2G_PulseSyncMode(uint8_t ChnNo,int32_t value)
{
	uint8_t  RegAddress = 0,RegData[4]={0};
	switch(ChnNo)
	{
		case LF_CHN: RegAddress = LF_PULSE_SYNC_MODE_ADDR;break;
		case HF_CHN: RegAddress = HF_PULSE_SYNC_MODE_ADDR;break;
	}
	RegData[0] =  Byte0_UINT32(value);
	RegData[1] =  Byte1_UINT32(value);
	RegData[2] =  Byte2_UINT32(value);
	RegData[3] =  Byte3_UINT32(value);
	for(uint8_t i = 0;i < 4;i++)
	{
		IF_HAL_FpgaReg_Write(RegAddress + i,RegData[i]);
	}
}

void Set_Sensor_2G_FpgaRegister(uint8_t RegAddress,int32_t value)
{
	uint8_t RegData[4]={0};
	RegData[0] =  Byte0_UINT32(value);
	RegData[1] =  Byte1_UINT32(value);
	RegData[2] =  Byte2_UINT32(value);
	RegData[3] =  Byte3_UINT32(value);
	for(uint8_t i = 0;i < 4;i++)
	{
		IF_HAL_FpgaReg_Write(4*RegAddress + i,RegData[i]);
	}	
}
uint32_t Get_Sensor_2G_FpgaRegister(uint8_t RegAddress)
{
    uint32_t value=0;
	value = MAKE_UINT32(Register[4*RegAddress+3], Register[4*RegAddress+2], Register[4*RegAddress+1], Register[4*RegAddress+0]);
	return value;	
}
/*********************InputSensor*****************************/
float Get_Sensor_2G_InputFreq(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = InputData[ChnNo].Freq;
    return fvalue;
}

float Get_Sensor_2G_InputVrms(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = InputData[ChnNo].V_rms;
    return fvalue;
}

float Get_Sensor_2G_InputIrms(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = InputData[ChnNo].I_rms;
    return fvalue;
}

float Get_Sensor_2G_InputPhase(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = InputData[ChnNo].Phase;
    return fvalue;
}

float Get_Sensor_2G_InputPdlv(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = InputData[ChnNo].P_dlv;
    return fvalue;
}

float Get_Sensor_2G_InputPfwd(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = InputData[ChnNo].P_fwd;
    return fvalue;
}

float Get_Sensor_2G_InputPref(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = InputData[ChnNo].P_ref;
    return fvalue;
}

float Get_Sensor_2G_InputR(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = InputData[ChnNo].R;
    return fvalue;
}

float Get_Sensor_2G_InputX(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = InputData[ChnNo].X;
    return fvalue;
}

float Get_Sensor_2G_InputZ(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = InputData[ChnNo].Z;
    return fvalue;
}

float Get_Sensor_2G_InputVSWR(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = InputData[ChnNo].VSWR;
    return fvalue;
}
/*********************OutputSensor*****************************/
float Get_Sensor_2G_OutputFreq(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = MainOutputData[ChnNo].Freq;
    return fvalue;
}

float Get_Sensor_2G_OutputVpp(uint8_t ChnNo)
{
	float fvalue = 0;
	float InputFreq =1000*Get_Sensor_2G_InputFreq(ChnNo);
	float a2= 0.001f*IF_NvmParam_GetMainOutputVppAttenFactor(2);
	float a1= 0.001f*IF_NvmParam_GetMainOutputVppAttenFactor(1);
	float a0= 0.001f*IF_NvmParam_GetMainOutputVppAttenFactor(0);
	float b = 0.001f*IF_NvmParam_GetMainOutputVppAttenOffset();
	
	float Ka= a2*InputFreq*InputFreq+a1*InputFreq+a0;
	fvalue = InputData[ChnNo].Vpp*Ka+b;
    return fvalue;
}

float Get_Sensor_2G_OutputVrms(uint8_t ChnNo)
{
	float fvalue = 0;
	#if(config_MainOutputSensor == 1)
	{
		fvalue = MainOutputData[ChnNo].V_rms;
	}
	#else
	{
		float InputFreq =1000*Get_Sensor_2G_InputFreq(ChnNo);
		float a2= 0.001f*IF_NvmParam_GetMainOutputVppAttenFactor(2);
		float a1= 0.001f*IF_NvmParam_GetMainOutputVppAttenFactor(1);
		float a0= 0.001f*IF_NvmParam_GetMainOutputVppAttenFactor(0);
		float b = 0.001f*IF_NvmParam_GetMainOutputVppAttenOffset();
		
		float Ka= a2*InputFreq*InputFreq+a1*InputFreq+a0;
		fvalue = InputData[ChnNo].Vpp*Ka+b;
	}
	#endif
    return fvalue;
}

float Get_Sensor_2G_OutputIrms(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = MainOutputData[ChnNo].I_rms;
    return fvalue;
}

float Get_Sensor_2G_OutputPhase(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = MainOutputData[ChnNo].Phase;
    return fvalue;
}

float Get_Sensor_2G_OutputPdlv(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue =  MainOutputData[ChnNo].P_dlv;
	return fvalue;
}

float Get_Sensor_2G_OutputPfwd(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue =  MainOutputData[ChnNo].P_fwd;
    return fvalue;
}

float Get_Sensor_2G_OutputPref(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = MainOutputData[ChnNo].P_ref;
    return fvalue;
}

float Get_Sensor_2G_OutputR(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = MainOutputData[ChnNo].R;
    return fvalue;
}

float Get_Sensor_2G_OutputX(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = MainOutputData[ChnNo].X;
    return fvalue;
}

float Get_Sensor_2G_OutputZ(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = MainOutputData[ChnNo].Z;
    return fvalue;
}

float Get_Sensor_2G_OutputVSWR(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = MainOutputData[ChnNo].VSWR;
    return fvalue;
}

/*********************AuxOutputSensor*****************************/
float Get_Sensor_2G_AuxOutputFreq(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = AuxOutputData[ChnNo].Freq;
    return fvalue;
}

float Get_Sensor_2G_AuxOutputVrms(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = AuxOutputData[ChnNo].V_rms;
    return fvalue;
}

float Get_Sensor_2G_AuxOutputIrms(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = AuxOutputData[ChnNo].I_rms;
    return fvalue;
}

float Get_Sensor_2G_AuxOutputPhase(uint8_t ChnNo)
{
    float fvalue = 0;
	fvalue = AuxOutputData[ChnNo].Phase;
	return fvalue;
}

float Get_Sensor_2G_AuxOutputPdlv(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = AuxOutputData[ChnNo].P_dlv;
    return fvalue;
}

float Get_Sensor_2G_AuxOutputPfwd(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = AuxOutputData[ChnNo].P_fwd;
    return fvalue;
}

float Get_Sensor_2G_AuxOutputPref(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = AuxOutputData[ChnNo].P_ref;
    return fvalue;
}

float Get_Sensor_2G_AuxOutputR(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = AuxOutputData[ChnNo].R;
    return fvalue;
}

float Get_Sensor_2G_AuxOutputX(uint8_t ChnNo)
{
	float fvalue = 0;
    fvalue = AuxOutputData[ChnNo].X;
	return fvalue;
}

float Get_Sensor_2G_AuxOutputZ(uint8_t ChnNo)
{
	float fvalue = 0;
    fvalue = AuxOutputData[ChnNo].Z;
	return fvalue;
}

float Get_Sensor_2G_AuxOutputVSWR(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = AuxOutputData[ChnNo].VSWR;
    return fvalue;
}
uint8_t Get_Sensor_2G_PulseMode(uint8_t ChnNo)
{
	uint8_t value = 0;
	if(FpgaReg[ChnNo].PulseMode == 0xA0A0BBCC)
	{
		value = 1;
	}
	return value;
}

uint8_t Get_Sensor_2G_PulseDutyCircle(uint8_t ChnNo)
{
	uint8_t value = 0;
	value = 100*FpgaReg[ChnNo].Duty/FpgaReg[ChnNo].Period;
    return value;
}
uint32_t Get_Sensor_2G_PulseFreq(uint8_t ChnNo)
{
	int32_t value = 0;
	value =1000000*64/FpgaReg[ChnNo].Period;
    return value;
}
uint32_t Get_Sensor_2G_PulseDynamicSyncDelay(uint8_t ChnNo)
{
	int32_t value = 0,  RFPowerPulseSyncDelay =0;
	
	if(ON==IF_NvmParam_GetPulseSyncMode())  //外部脉冲同步模式
	{
		RFPowerPulseSyncDelay = IF_NvmParam_GetRFPowerPulseSyncDelay();	
	}
	value = (FpgaReg[ChnNo].Duty/64-20)/2 + RFPowerPulseSyncDelay;
    return value;
}
float Get_Sensor_2G_DCBias(void)
{
	float fvalue = 0;
	fvalue = (IF_HAL_DCBiasRead()-VDC_ZERO_VREF)/ADC_VRESFACTOR;
	return fvalue;
}
float Get_Sensor_2G_Vpp(uint8_t ChnNo)
{
	float fvalue = 0;
	fvalue = InputData[ChnNo].Vpp;
    return fvalue;
}
uint8_t Get_Sensor_2G_Version(uint8_t* pBuf)
{
	uint8_t i = 0;	
	for(i =0;i< FPGAWARE_VERSION_LEN;i++)
	{
		if(FpgaReg[LF_CHN].Version[i] ==0)
		{
			break;
		}
		pBuf[i] = FpgaReg[LF_CHN].Version[i];
	}
	return i;	
}
//*****************************************************************************
//* END
//*****************************************************************************


