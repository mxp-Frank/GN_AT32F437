/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File NPTe :  PT_SWC.c
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2023-05-10
 * ****************************************************************************
 */

#include "IF_HAL_Cfg.h"
#include "IF_SL_Cfg.h"
#include "IF_PT_SWC.h"
#include "configDevice.h"
/* CONST & MACROS */
	
/* DATA STRUCTURES */
/* LOCAL VARIABLES */
static PID_Typedef PfwdPID;
static PID_Typedef PrefPID;
/* GLOBAL VARIABLES */

GN_Device_t GN_Device;
/* FUNCTION PROTOTYPES */

static uint32_t Module_PowerDriverCtrl(void);
static void Module_TargetPowerOutput(uint32_t SetPower);

static void Module_PowerAdjustInit(uint32_t SetPower);
static float Module_PhasePIDAdjust(uint32_t SetPower);
static void Module_CheckPIDProcessData(void);
static void Module_ACDCOutputVoltage(uint32_t Voltage);
static void Module_ACDCOutputPhase(float SetPIDPhase);
static void Module_PhasePID_Init(void);
static float Pfwd_PID_realize(float OutputPower,float Pforward);
static float Pref_PID_realize(float OutputPower,float Pforward);
/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/
 /* FUNCTION *********************************************************************************
 * Function  : IF_Module_SWCInit
 * Description   : 匹配器设置初始化接口
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
void IF_Module_SWCInit(void)
{
    //Global Init...
	memset((void*)&GN_Device, 0, sizeof(GN_Device_t));
	IF_SL_CmdParam_SetPowerWorkMode(ON);
}
/* FUNCTION *********************************************************************************
 * Function  : IF_Module_Main_Task
 * Description   : 匹配器输入任务函数接口
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
void IF_Module_Main_Task(void)
{	
	uint32_t SetPower = 0;
	if(GN_Device.RunState.Now == RUN_STATE) 
	{
		SetPower = Module_PowerDriverCtrl();
		Module_TargetPowerOutput(SetPower);				
	}
	Module_ACDCOutputVoltage(INIT_ACDC_VOLTAGE);	
}
/* FUNCTION *********************************************************************************
 * Function  : Module_PowerDriverCtrl
 * Description   : 功率监测限制，判射频输出限制状态
 * Parameter     : SettingPower ---->设置功率
 * return        :                
 * END ***************************************************************************************/
static uint32_t Module_PowerDriverCtrl(void)
{
	float PowerSlowRate = 0.0F;
	/**********************射频功耗驱动开启状态*********************************/	
	if (RF_ON == GN_Device.Sensor.Now.State)
	{	
		if(OFF == IF_SL_UserParam_GetSlowMode())  				//缓启动模式关闭
		{
			IF_SL_Timer_SetRFSlowStartDelayFlag(OFF);
			IF_SL_Timer_SetRFSlowStopDelayFlag(OFF);		
		}else
		{
			IF_SL_Timer_SetRFSlowStartDelayFlag(ON);
			IF_SL_Timer_SetRFSlowStopDelayFlag(OFF);
		}
		PowerSlowRate = IF_SL_Timer_GetRFSlowStartDelayTime();					
	}else
	{
		if(OFF == IF_SL_UserParam_GetSlowMode())    			    //缓启动模式关闭
		{
			IF_SL_Timer_SetRFSlowStartDelayFlag(OFF);				//关闭缓启动定时器	
			IF_SL_Timer_SetRFSlowStopDelayFlag(OFF);				//关闭缓关闭定时器	
		}else 
		{	
			IF_SL_Timer_SetRFSlowStartDelayFlag(OFF);	    		//关闭缓启动定时器
			IF_SL_Timer_SetRFSlowStopDelayFlag(ON);					//开启缓关闭定时器
		}	
		PowerSlowRate = IF_SL_Timer_GetRFSlowStopDelayTime();				
	}
	uint32_t SetPower = GN_Device.SetPower*PowerSlowRate;
	return SetPower;	
}
/* FUNCTION *********************************************************************************
 * Function  : Module_TargetPowerOutput
 * Description   : 电源功率调整处理过程函数
 * Parameter     : TargetPower 设置的功率
 * return        :                
 * END ***************************************************************************************/
static void Module_TargetPowerOutput(uint32_t SetPower)
{	
	/**************监控功率开启************************************/
	if (RF_ON == GN_Device.Sensor.Now.State && SetPower >= POWER_RATIO) 		
	{	
		/************************功率和相位限制值***************************/		
		if(GN_Device.SetPID.PointLimit != GN_Device.SetPower)
		{
			
			GN_Device.SetPID.PointLimit = PWR_UP_LIMIT(GN_Device.SetPower);
			GN_Device.SetPID.PhaseLimit = IF_NvmParam_GetPhaseMapTable(GN_Device.SetPID.PointLimit);
		}	
		if(GN_Device.Sensor.Now.State != GN_Device.Sensor.Last.State) //初始化功率状态
		{
			Module_PhasePID_Init();
			Module_PowerAdjustInit(SetPower);
			IF_SL_ClearRFPwrPIDProcessData();
		}else if(GN_Device.Sensor.Now.Pfwd >= POWER_RATIO) //功率监测及PID过程
		{
			GN_Device.TargetPower.Phase += Module_PhasePIDAdjust(SetPower);
			Module_CheckPIDProcessData();
		}
		/*************************功率输出******************************/
		if(GN_Device.TargetPower.Phase <= GN_Device.SetPID.PhaseLimit)
		{	
			Module_ACDCOutputPhase(GN_Device.TargetPower.Phase);
		}
	}else
	{		
		IF_SL_CmdParam_SetDDSPhase(0);              	  //设置射频驱动相位为0
		IF_SL_CmdParam_SetDDSDriverState(OFF); 			  //关功放驱动电源使能
		memset((void*)&GN_Device.SetPID,0,sizeof(SetPID_t));
		memset((void*)&GN_Device.Sensor.Now,0,sizeof(Sensor_t));
		memset((void*)&GN_Device.TargetPower,0,sizeof(PwrState_t));			
	}		
	memcpy((void*)&GN_Device.Sensor.Last,(void*)&GN_Device.Sensor.Now,sizeof(Sensor_t));
}
/* FUNCTION *********************************************************************************
 * Function  	 :	Module_PowerAdjustInit
 * Description   : 电源功率调整初始化函数
 * Parameter     : SetPower 设置的功率
 * return        :                
 * END ***************************************************************************************/
static void Module_PowerAdjustInit(uint32_t SetPower)
{
	GN_Device.InitPower.Point = SetPower;
	if(SetPower >= IF_SL_InternalParam_GetInitPoint())
	{
		GN_Device.InitPower.Point = IF_SL_InternalParam_GetInitPoint();	
	}	
	GN_Device.InitPower.Phase = IF_NvmParam_GetPhaseMapTable(GN_Device.InitPower.Point);
	GN_Device.TargetPower.Point = GN_Device.InitPower.Point;
	GN_Device.TargetPower.Phase = GN_Device.InitPower.Phase;	
}	
	
/* FUNCTION *********************************************************************************
 * Function  	 :	Module_PhasePIDAdjust
 * Description   : 电源功率调整处理过程函数
 * Parameter     : TargetPower 设置的功率
 * return        :                
 * END ***************************************************************************************/
static  float Module_PhasePIDAdjust(uint32_t SetPower)
{
	float PrefPhase = 0,PfwdPhase=0,Increment  = 0;
	uint32_t TargetPrefPoint = IF_SL_UserParam_GetReflectPowerThreshold();	
	GN_Device.TargetPower.Point = SetPower;
	/****************调整差值阈值*************************/
	PfwdPID.iError = GN_Device.TargetPower.Point - GN_Device.Sensor.Now.Pfwd; 
	PrefPID.iError = TargetPrefPoint - GN_Device.Sensor.Now.Pref; 
	if(fabs(PfwdPID.iError) > PfwdPID.ErrorThr||PrefPID.iError > PrefPID.ErrorThr)
	{	
//		uint32_t PfmPoint = TargetPrefPoint/GN_Device.Sensor.Now.Gamma;  
//		uint32_t PfxPoint = GN_Device.Sensor.Now.Pfwd/GN_Device.Sensor.Now.Gamma;  
//		if(PfmPoint < PfxPoint && PfmPoint < SetPower)
//		{
//			GN_Device.TargetPower.Point = PfmPoint;
//		}else if(PfxPoint < PfmPoint&&PfxPoint< SetPower)
//		{
//			GN_Device.TargetPower.Point = PfxPoint;	
//		}else
//		{
//			GN_Device.TargetPower.Point = SetPower;
//		}
//		if(PfmPoint < SetPower)
//		{
//			GN_Device.TargetPower.Point = PfmPoint;
//		}else
//		{
//			GN_Device.TargetPower.Point = SetPower;
//		}
		PrefPhase = Pref_PID_realize(TargetPrefPoint,GN_Device.Sensor.Now.Pref);	
		PfwdPhase = Pfwd_PID_realize(GN_Device.TargetPower.Point,GN_Device.Sensor.Now.Pfwd);
	}
	Increment = (PrefPhase < PfwdPhase)? PrefPhase:PfwdPhase;
	return Increment;
}
		
/* FUNCTION *********************************************************************************
 * Function  : Module_OutputPhase
 * Description   : 设置Fpga输出相位接口
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void Module_ACDCOutputPhase(float SetPIDPhase)
{
	/*********************设置射频输出功率值*********************/	
	if(SetPIDPhase >= MAX_FPGA_PHASE)  //限制PID最大输出
	{
		SetPIDPhase = MAX_FPGA_PHASE;
		g_StatusWord.bits.PowerAtLimit = 1;			//电压超出极限值
	}else
	{
		g_StatusWord.bits.PowerAtLimit = 0;
	}	
	IF_SL_CmdParam_SetDDSDriverState(ON); 			  //关功放驱动电源使能
	IF_SL_CmdParam_SetDDSPhase(SetPIDPhase);	
}

/* FUNCTION *********************************************************************************
 * Function  	 :	Module_ACDCOutputVoltage
 * Description   : 电源ACDC初始化函数
 * Parameter     :  Voltage 设置的电压
 * return        :                
 * END ***************************************************************************************/
static void Module_ACDCOutputVoltage(uint32_t Voltage)
{
	if(GN_Device.RunState.Now == RUN_STATE) 
	{
		GN_Device.ACDCSlow_Cnt++;
		if(GN_Device.ACDCSlow_Cnt >= 1000)	
		{
			GN_Device.ACDCSlow_Cnt = 1000;
			IF_SL_CmdParam_SetACDCDriverState(ON);	//设置AC-DC状态	
		}
		IF_SL_CmdParam_SetACDCVoltage(Voltage);		//设置AC-DC电压	
	}else
	{
		GN_Device.ACDCSlow_Cnt = 0;
	}
}
/* FUNCTION *********************************************************************************
 * Function  : Module_CheckPIDProcessData
 * Description   : 电源电压输出阈值处理过程函数
 * Parameter     : OutputPower 输出的功率值
 * return        :                
 * END ***************************************************************************************/
static void Module_CheckPIDProcessData(void)
{	
	/************************过程数据采集******************************/
	if(GN_Device.Sensor.Now.VSWR >= TUNED_VSWR) 
	{
		IF_SL_UpdateRFPwrPIDProcessData();
	}	
}
 /* FUNCTION *********************************************************************************
 * Function      : Module_PhasePID_Init
 * Description   : 匹配器设置初始化接口
 * Parameter     : Power功率查询
 * return        : FpgaPhase值               
 * END ***************************************************************************************/
static void Module_PhasePID_Init(void)
{
	memset(&PfwdPID,0,sizeof(PID_Typedef));
	memset(&PrefPID,0,sizeof(PID_Typedef));
	PfwdPID.ErrorThr =  1 * g_FactorData.PhaseEThr;
	PrefPID.ErrorThr = -1 * g_FactorData.VoltEThr;
}
/* FUNCTION *********************************************************************************
 * Function  : incPIDCalc
 * Description   : 增量式PID计算
 * Parmeter      : SetPower 当前设置的功率值 
 * Parmeter      : Pforward 当前响应的功率值
 * return        : IncrementVol 经过PID运算得到的电压增量值               
 * END ***************************************************************************************/
static float Pfwd_PID_realize(float SetPower,float Pforward)
{
	float Increment = 0;
	//设置功率与Sensor采样功率的差值
	PfwdPID.iError = SetPower - Pforward; 	
	float Kp_Value =  g_FactorData.PhaseKp * (PfwdPID.iError - PfwdPID.LastError);
	float Ki_Value =  g_FactorData.PhaseKi * PfwdPID.iError;
	float Kd_Value =  g_FactorData.PhaseKd * (PfwdPID.iError -2*PfwdPID.LastError + PfwdPID.PrevError);
	Increment = Kp_Value + Ki_Value + Kd_Value;			
	
	PfwdPID.LastError = PfwdPID.iError;     //存储前一次误差，用于下次计算	
	PfwdPID.PrevError = PfwdPID.LastError;	//存储前两次误差，用于下次计算	
    return Increment; 						//返回增量值	
}
	
/* FUNCTION *********************************************************************************
 * Function  : incPIDCalc
 * Description   : 增量式PID计算
 * Parmeter      : SetPower 当前设置的功率值 
 * Parmeter      : Pforward 当前响应的功率值
 * return        : IncrementVol 经过PID运算得到的电压增量值               
 * END ***************************************************************************************/
static float Pref_PID_realize(float SetPower,float ActualPower)
{
	float Increment = 0;
	PrefPID.iError = SetPower - ActualPower; 		
	float Kp_Value = g_FactorData.VoltKp *(PrefPID.iError - PrefPID.LastError);
	float Ki_Value = g_FactorData.VoltKi * PrefPID.iError;
	float Kd_Value = g_FactorData.VoltKd *(PrefPID.iError - 2*PrefPID.LastError + PrefPID.PrevError);
	Increment = Kp_Value + Ki_Value + Kd_Value;			
	
	PrefPID.LastError = PrefPID.iError;     	//存储前一次误差，用于下次计算				
	PrefPID.PrevError = PrefPID.LastError;		//存储前两次误差，用于下次计算		
    return Increment; 							//返回增量值	
}

//*****************************************************************************
//* END
//*****************************************************************************

