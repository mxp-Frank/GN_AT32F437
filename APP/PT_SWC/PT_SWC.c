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
static PID_Typedef PhasePID;
static PID_Typedef VoltPID;
/* GLOBAL VARIABLES */

GN_Device_t GN_Device;
/* FUNCTION PROTOTYPES */
static uint32_t Module_PowerDriverCtrl(void);
static void Module_TargetPowerOutput(uint32_t SetPower);
static void Module_CheckPowerLimit(uint32_t SetPower);
static void Module_PhasePIDAdjust(uint32_t SetPower);
static void Module_VoltPIDAdjust(uint32_t SetPower);

static void Module_ACDCOutputPhase(float SetPIDPhase);
static uint32_t Phase_PID_LookupTable(uint32_t power);
static float Phase_PID_realize(float OutputPower,float Pforward);

static void Module_ACDCOutputVoltage(float SetPIDVoltage);
static float Volt_PID_LookupTable(uint32_t power);
static float Volt_PID_realize(float OutputPower,float Pforward);
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
	memset(&GN_Device, 0, sizeof(GN_Device_t));
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
	if (RF_ON == GN_Device.RFPwrState.Now)
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
	uint32_t SetPower = PowerSlowRate * GN_Device.SetPower;
	if(SetPower > 0)
	{
		IF_SL_CmdParam_SetDDSDriverState(ON); 					//功放驱动电源使能	
	}else
	{
		IF_SL_CmdParam_SetDDSDriverState(OFF); 				 	//关功放驱动电源使能
	}
	
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
	if (RF_ON == GN_Device.RFPwrState.Now && SetPower >= POWER_RATIO) 		
	{	
		if(GN_Device.RFPwrState.Now != GN_Device.RFPwrState.Last) //功率状态切换
		{
			if(SetPower >= IF_SL_InternalParam_GetInitPoint())
			{
				GN_Device.TargetPower.Point = IF_SL_InternalParam_GetInitPoint();
			}
			GN_Device.TargetPower.Phase = Phase_PID_LookupTable(GN_Device.TargetPower.Point);
			Module_ACDCOutputPhase(GN_Device.TargetPower.Phase);
			
		}else if(GN_Device.Sensor.Pfwd >= POWER_RATIO)
		{
			Module_CheckPowerLimit(SetPower);
			if(SetPower <= IF_SL_UserParam_GetForwardPowerLimit())
			{
				Module_PhasePIDAdjust(SetPower);	
			}else
			{
				Module_VoltPIDAdjust(SetPower);	
			}	
		}	
	}else
	{
		memset(&GN_Device.SetPID,0,sizeof(SetPID_t));
		memset(&GN_Device.AjustThr,0,sizeof(AjustThr_t));
		IF_SL_ClearRFPwrPIDProcessData();
		IF_SL_CmdParam_SetDDSPhase(0);              	  //设置射频驱动相位为0
		IF_SL_CmdParam_SetACDCVoltage(INIT_ACDC_VOLTAGE); //设置AC-DC初始化电压	
		IF_SL_CmdParam_SetACDCDriverState(ON); 		      //开启AC-DC电源使能			
	}
	
	GN_Device.LastPower.Point   = GN_Device.TargetPower.Point;
	GN_Device.LastPower.Phase   = GN_Device.TargetPower.Phase;
	GN_Device.LastPower.Volt    = GN_Device.TargetPower.Volt;
	GN_Device.LastAjustThr.Power = GN_Device.AjustThr.Power;
	GN_Device.LastAjustThr.Phase = GN_Device.AjustThr.Phase;
	GN_Device.LastAjustThr.Volt  = GN_Device.AjustThr.Volt;
	GN_Device.RFPwrState.Last = GN_Device.RFPwrState.Now;
}
/* FUNCTION *********************************************************************************
 * Function  	 :	Module_PhasePIDAdjust
 * Description   : 电源功率调整处理过程函数
 * Parameter     : TargetPower 设置的功率
 * return        :                
 * END ***************************************************************************************/
static  void Module_PhasePIDAdjust(uint32_t SetPower)
{
	/*************************检查目标功率与上次是否相同,驻波比大于设置值********************/
	if((GN_Device.SetPID.Phase_Cnt < PID_INIT_TIMER && GN_Device.Sensor.VSWR > g_FactorData.VSWRLimit)||(GN_Device.TargetPower.Point != GN_Device.LastPower.Point))  
	{
		GN_Device.SetPID.Phase_Cnt = 0;
		uint32_t VSWRPower = SetPower/GN_Device.Sensor.VSWR;
		uint32_t VSWRPhase = Phase_PID_LookupTable(VSWRPower);
		if(VSWRPhase  <  GN_Device.AjustThr.Phase)
		{
			GN_Device.TargetPower.Point = VSWRPower;
			GN_Device.TargetPower.Phase = VSWRPhase;	
		}else 
		{
			GN_Device.TargetPower.Point = GN_Device.AjustThr.Power;
			GN_Device.TargetPower.Phase = GN_Device.AjustThr.Phase;	
		}
	}else
	{
		GN_Device.SetPID.Phase_Cnt++;
	}
	/************************开始设置Phase PID********************************/	
	if(GN_Device.SetPID.Phase_Cnt == PID_INIT_TIMER)
	{
		GN_Device.TargetPower.Point = SetPower;
		GN_Device.TargetPower.Phase = Phase_PID_LookupTable(GN_Device.TargetPower.Point);	
		
	}else if(GN_Device.SetPID.Phase_Cnt > PID_LOOP_TIMER)
	{
		 uint32_t IncreasePhase = Phase_PID_realize(GN_Device.TargetPower.Point,GN_Device.Sensor.Pfwd);
		 GN_Device.TargetPower.Phase = GN_Device.TargetPower.Phase + IncreasePhase;
	}
	if(GN_Device.TargetPower.Phase < GN_Device.SetPID.PhaseLimit)
	{
		Module_ACDCOutputPhase(GN_Device.TargetPower.Phase);
	}
	/************************过程数据采集******************************/
	if(GN_Device.SetPID.Phase_Cnt % PID_LOOP_TIMER == 0)
	{
		if(GN_Device.Sensor.VSWR < TUNED_VSWR) 
		{
			IF_SL_ClearRFPwrPIDProcessDataFlag();
		}else
		{
			IF_SL_UpdateRFPwrPIDProcessData();
		}
	}	
}

/* FUNCTION *********************************************************************************
 * Function  	 :	Module_VoltPIDAdjust
 * Description   : 电源功率调整处理过程函数
 * Parameter     : TargetPower 设置的功率
 * return        :                
 * END ***************************************************************************************/
static  void Module_VoltPIDAdjust(uint32_t SetPower)
{
	/*************************检查目标功率与上次是否相同,驻波比大于设置值********************/
	if((GN_Device.SetPID.Volt_Cnt < PID_INIT_TIMER && GN_Device.Sensor.VSWR > g_FactorData.VSWRLimit)||(GN_Device.TargetPower.Point != GN_Device.LastPower.Point) )  
	{
		GN_Device.SetPID.Volt_Cnt = 0;
		uint32_t VSWRPower = SetPower/GN_Device.Sensor.VSWR;
		uint32_t VSWRVolt = Volt_PID_LookupTable(VSWRPower);
		if(VSWRVolt <  GN_Device.AjustThr.Volt)
		{
			GN_Device.TargetPower.Point = VSWRPower;
			GN_Device.TargetPower.Volt = VSWRVolt;	
		}else 
		{
			GN_Device.TargetPower.Point = GN_Device.AjustThr.Power;
			GN_Device.TargetPower.Volt  = GN_Device.AjustThr.Volt;	
		}	
	}else
	{
		GN_Device.SetPID.Volt_Cnt++;
	}
	/**************开始设置Volt PID********************/	
	if(GN_Device.SetPID.Volt_Cnt == PID_INIT_TIMER)
	{
		GN_Device.TargetPower.Point = SetPower;
		GN_Device.TargetPower.Volt = Volt_PID_LookupTable(GN_Device.TargetPower.Volt);		
	}else if(GN_Device.SetPID.Volt_Cnt > PID_LOOP_TIMER)
	{
		 uint32_t IncreaseVolt = Volt_PID_realize(GN_Device.TargetPower.Point,GN_Device.Sensor.Pfwd);
		 GN_Device.TargetPower.Volt = GN_Device.TargetPower.Volt + IncreaseVolt;
	}
	if(GN_Device.TargetPower.Volt < GN_Device.SetPID.VoltLimit)
	{
		Module_ACDCOutputVoltage(GN_Device.TargetPower.Volt);
	}
	/************************过程数据采集******************************/	
	if(GN_Device.SetPID.Volt_Cnt % PID_LOOP_TIMER == 0)
	{
		if(GN_Device.Sensor.VSWR < TUNED_VSWR) 
		{
			IF_SL_ClearRFPwrPIDProcessDataFlag();
		}else
		{
			IF_SL_UpdateRFPwrPIDProcessData();
		}
	}	
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
	IF_CmdParam_SetDDSPhase(SetPIDPhase);	
}	
/* FUNCTION *********************************************************************************
 * Function  : ACDCModule_OutputVoltage
 * Description   : 设置Fpga输出相位接口
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void Module_ACDCOutputVoltage(float SetPIDVoltage)
{
	/*********************设置射频输出功率值*********************/	
	if(SetPIDVoltage >= MAX_ACDC_VOLT)  //限制PID最大输出
	{
		SetPIDVoltage = MAX_ACDC_VOLT;
		g_StatusWord.bits.PowerAtLimit = 1;			//电压超出极限值
	}else
	{
		g_StatusWord.bits.PowerAtLimit = 0;
	}	
	IF_CmdParam_SetACDCVoltage(SetPIDVoltage);
	
}
/* FUNCTION *********************************************************************************
 * Function  : Module_CheckPowerLimit
 * Description   : 电源电压输出阈值处理过程函数
 * Parameter     : OutputPower 输出的功率值
 * return        :                
 * END ***************************************************************************************/
static void Module_CheckPowerLimit(uint32_t SetPower)
{	
	/**********************功率监测，开启状态*********************************/
	if(FWDREGULATION == IF_SL_UserParam_GetRegulationMode()) //前向功率模式下
	{
		if(GN_Device.Sensor.Pref > IF_SL_UserParam_GetReflectPowerThreshold())
		{ 
			GN_Device.SetPID.Phase_Cnt = 0; //PID的退出条件
			GN_Device.SetPID.Volt_Cnt  = 0; //PID的退出条件
			/***************设置当前调整阈值***************/
			GN_Device.AjustThr.Power = GN_Device.LastPower.Point/THR_RATE;   
			GN_Device.AjustThr.Phase = GN_Device.LastPower.Phase/THR_RATE;
			GN_Device.AjustThr.Volt  = GN_Device.LastPower.Volt/THR_RATE;
		}else
		{		
			if((GN_Device.TargetPower.Point == GN_Device.LastAjustThr.Power)||
			   (GN_Device.LastAjustThr.Power >= (SetPower/2)))
			{
				GN_Device.AjustThr.Power = GN_Device.LastAjustThr.Power;
				GN_Device.AjustThr.Phase = GN_Device.LastAjustThr.Phase;
				GN_Device.AjustThr.Volt	 = GN_Device.LastAjustThr.Volt;			
			}else  
			{
				GN_Device.AjustThr.Power = GN_Device.LastPower.Point*THR_RATE;   
				GN_Device.AjustThr.Phase = GN_Device.LastPower.Phase*THR_RATE;
				GN_Device.AjustThr.Volt  = GN_Device.LastPower.Volt*THR_RATE;		
			}							
		}
	}else if(LOADREGULATION == IF_SL_UserParam_GetRegulationMode()) //传输功率模式下
	{
		if(GN_Device.Sensor.Pref > GN_Device.SetPower)
		{ 	 
			GN_Device.SetPID.Phase_Cnt = 0; //PID的退出条件
			GN_Device.SetPID.Volt_Cnt  = 0; //PID的退出条件
			/***************设置当前调整阈值***************/
			GN_Device.AjustThr.Power = GN_Device.LastPower.Point/THR_RATE;   
			GN_Device.AjustThr.Phase = GN_Device.LastPower.Phase/THR_RATE;
			GN_Device.AjustThr.Volt  = GN_Device.LastPower.Volt/THR_RATE;
		}else
		{		
			if((GN_Device.TargetPower.Point == GN_Device.LastAjustThr.Power)||
			   (GN_Device.LastAjustThr.Power >= (SetPower/2)))
			{
				GN_Device.AjustThr.Power = GN_Device.LastAjustThr.Power;
				GN_Device.AjustThr.Phase = GN_Device.LastAjustThr.Phase;
				GN_Device.AjustThr.Volt	 = GN_Device.LastAjustThr.Volt;				
			}else 
			{
				GN_Device.AjustThr.Power = GN_Device.LastPower.Point*THR_RATE;   
				GN_Device.AjustThr.Phase = GN_Device.LastPower.Phase*THR_RATE;
				GN_Device.AjustThr.Volt  = GN_Device.LastPower.Volt*THR_RATE;			
			}							
		}
	}
}
	
 /* FUNCTION *********************************************************************************
 * Function  : PID_LookupTable
 * Description   : 匹配器设置初始化接口
 * Parameter     : Power功率查询
 * return        : FpgaPhase值               
 * END ***************************************************************************************/
static uint32_t Phase_PID_LookupTable(uint32_t Power)
{
	
	uint32_t value = 0;
	PhasePID.iError =	0.0F;      //当前误差值
	PhasePID.LastError = 0.0F;	   //上次误差值
	PhasePID.PrevError = 0.0F;	   //上上次误差值
	PhasePID.AdjCnt = 0;		   //PID调整次数
	PhasePID.ErrorThr = g_FactorData.PhaseEThr;
	/*********************得到PID相位的上限和当前PID的相位*****************************/
	if(GN_Device.SetPower != GN_Device.PowerLimit)
	{
		GN_Device.PowerLimit = GN_Device.SetPower;
		GN_Device.SetPID.PhaseLimit =  IF_NvmParam_GetPhaseMapTable(GN_Device.PowerLimit)*1.5F;
	}
	value =  IF_NvmParam_GetPhaseMapTable(Power)*0.9F;	
	return  value;
}	
/* FUNCTION *********************************************************************************
 * Function  : incPIDCalc
 * Description   : 增量式PID计算
 * Parmeter      : SetPower 当前设置的功率值 
 * Parmeter      : Pforward 当前响应的功率值
 * return        : IncrementVol 经过PID运算得到的电压增量值               
 * END ***************************************************************************************/
static float Phase_PID_realize(float OutPower,float Pforward)
{
	float Kp_Value = 0.0F, Ki_Value = 0.0F, Kd_Value = 0.0F,Increment = 0;
	//设置功率与Sensor采样功率的差值
	PhasePID.iError = OutPower - Pforward; 
	//调整差值阈值
	if(fabs(PhasePID.iError) > PhasePID.ErrorThr)
	{
		if(PhasePID.AdjCnt > 2)  //得到前两次误差值后，再进行增量计算
		{
			Kp_Value =  g_FactorData.PhaseKp * (PhasePID.iError - PhasePID.LastError);
			Ki_Value =  g_FactorData.PhaseKi * PhasePID.iError;
			Kd_Value =  g_FactorData.PhaseKd * (PhasePID.iError -2*PhasePID.LastError + PhasePID.PrevError);
			Increment = Kp_Value + Ki_Value + Kd_Value;	
		}
		PhasePID.LastError = PhasePID.iError;       //存储前一次误差，用于下次计算	
		PhasePID.PrevError = PhasePID.LastError;	//存储前两次误差，用于下次计算			
		if(Increment > 1000) Increment = 500;
		if(Increment < -1000)Increment = -500;		
	}
	if(PhasePID.AdjCnt < UINT16_MAX)PhasePID.AdjCnt++;
    return Increment; 							//返回增量值	
}
 /* FUNCTION *********************************************************************************
 * Function  : PID_LookupTable
 * Description   : 匹配器设置初始化接口
 * Parameter     : Power功率查询
 * return        : FpgaPhase值               
 * END ***************************************************************************************/
static float Volt_PID_LookupTable(uint32_t Power)
{
	float fvalue = 0;
	int16_t vTableIndex = 0;
	VoltPID.iError =	0.0F;      //当前误差值
	VoltPID.LastError = 0.0F;	   //上次误差值
	VoltPID.PrevError = 0.0F;	   //上上次误差值
	VoltPID.AdjCnt = 0;			   //PID调整次数
	VoltPID.ErrorThr = g_FactorData.VoltEThr;
	/*********************得到PID电压的上限和当前PID的电压*****************************/
	if(GN_Device.SetPower != GN_Device.PowerLimit)
	{
		GN_Device.PowerLimit = GN_Device.SetPower;
		vTableIndex = (GN_Device.PowerLimit/10)-1;
		if(vTableIndex <= 0)vTableIndex = 0;
		GN_Device.SetPID.VoltLimit =  IF_NvmParam_GetVoltMapTable(vTableIndex)*1.5F;
	}
	vTableIndex = (Power/10)-1;
	if(vTableIndex <= 0)vTableIndex = 0;
	fvalue =  IF_NvmParam_GetVoltMapTable(vTableIndex);
	return  fvalue;
}	
/* FUNCTION *********************************************************************************
 * Function  : incPIDCalc
 * Description   : 增量式PID计算
 * Parmeter      : SetPower 当前设置的功率值 
 * Parmeter      : Pforward 当前响应的功率值
 * return        : IncrementVol 经过PID运算得到的电压增量值               
 * END ***************************************************************************************/
static float Volt_PID_realize(float SetPower,float ActualPower)
{
	float Kp_Value = 0.0F, Ki_Value = 0.0F, Kd_Value = 0.0F,Increment = 0;
	//设置功率与Sensor采样功率的差值
	VoltPID.iError = SetPower - ActualPower;	
	//调整差值阈值
	if(fabs(VoltPID.iError) > VoltPID.ErrorThr)
	{
		if(VoltPID.AdjCnt > 2)  //得到前两次误差值后，再进行增量计算
		{
			 Kp_Value = g_FactorData.VoltKp *(VoltPID.iError - VoltPID.LastError);
			 Ki_Value = g_FactorData.VoltKi * VoltPID.iError;
			 Kd_Value = g_FactorData.VoltKd *(VoltPID.iError - 2*VoltPID.LastError + VoltPID.PrevError);
			 Increment = Kp_Value + Ki_Value + Kd_Value;	
		}
		VoltPID.LastError = VoltPID.iError;     	//存储前一次误差，用于下次计算				
		VoltPID.PrevError = VoltPID.LastError;		//存储前两次误差，用于下次计算
		if(Increment > 1000) Increment = 500;
		if(Increment < -1000)Increment = -500;		
	}
	if(VoltPID.AdjCnt < UINT16_MAX)VoltPID.AdjCnt++;	
    return Increment; 							//返回增量值	
}

//*****************************************************************************
//* END
//*****************************************************************************

