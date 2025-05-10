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
#define THRPOWER_RATE				(float)(0.01F)
#define PID_TIMER				5	
/* DATA STRUCTURES */
/* LOCAL VARIABLES */
static PID_Typedef PhasePID;
static PID_Typedef VoltPID;
/* GLOBAL VARIABLES */

GN_Device_t GN_Device;

/* FUNCTION PROTOTYPES */
static float Module_PowerDriverCtrl(void);
static uint32_t Module_CheckOutputPower(float slowRate);
static void Module_PowerPIDAdjust(uint32_t setPower);

static void Module_ACDCOutputPhase(float SetPIDPhase);
static void Module_ACDCOutputVoltage(float SetPIDVoltage);

static uint32_t Phase_PID_LookupTable(uint32_t power);
static float Phase_PID_realize(float OutputPower,float Pforward);
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
}
/* FUNCTION *********************************************************************************
 * Function  : IF_Module_Main_Task
 * Description   : 匹配器输入任务函数接口
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
void IF_Module_Main_Task(void)
{	
	GN_Device.LoopMode.Now = (LoopModeEnum)IF_SL_CmdParam_GetLoopMode();	
	if(GN_Device.LoopMode.Now == MODE_CLOSE) //闭环模式下
	{
		float SlowRate = Module_PowerDriverCtrl();
		uint32_t setPower = Module_CheckOutputPower(SlowRate);
		Module_PowerPIDAdjust(setPower);	
		
	}else if(GN_Device.LoopMode.Last != GN_Device.LoopMode.Now)//工作模式切换
	{
		IF_SL_CmdParam_SetPwrPoint(0);              //设置功率设置值为零
		IF_SL_CmdParam_SetACDCVoltage(0);			//设置AC-DC电压为零	
		IF_SL_CmdParam_SetDDSWorkPhase(0);          //关闭功放驱动相位	
		IF_SL_CmdParam_SetDDSDriverState(OFF); 		//关闭功放驱动电源使能	
		IF_SL_CmdParam_SetACDCDriverState(OFF);     //关闭AC-DC电源使能			
	}
	GN_Device.LoopMode.Last = GN_Device.LoopMode.Now;
}
/* FUNCTION *********************************************************************************
 * Function  : Module_PowerDriverCtrl
 * Description   : 功率监测限制，判射频输出限制状态
 * Parameter     : SettingPower ---->设置功率
 * return        :                
 * END ***************************************************************************************/
static float Module_PowerDriverCtrl(void)
{
	float PowerSlowRate = 0.0F;
	GN_Device.RFPwrState.Now = (RFEnum)IF_SL_CmdParam_GetRFPowerState();
	/**********************射频功耗驱动开启状态*********************************/	
	if (AM_RF_ON == GN_Device.RFPwrState.Now)
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
			IF_SL_Timer_SetRFSlowStopDelayFlag(OFF);				//关闭缓启动定时器	
		}else 
		{	
			IF_SL_Timer_SetRFSlowStopDelayFlag(ON);
			IF_SL_Timer_SetRFSlowStartDelayFlag(OFF);	
		}	
		PowerSlowRate = IF_SL_Timer_GetRFSlowStopDelayTime();				
	}
	if(PowerSlowRate > 0)
	{
		IF_SL_CmdParam_SetDDSDriverState(ON); 					//功放驱动电源使能	
	}else
	{
		IF_SL_CmdParam_SetDDSDriverState(OFF); 				 	//关功放驱动电源使能
	}
	return PowerSlowRate;	
}
/* FUNCTION *********************************************************************************
 * Function  : Module_PowerPIDAdjust
 * Description   : 电源功率调整处理过程函数
 * Parameter     : SettingPower 设置的功率
 * return        :                
 * END ***************************************************************************************/
static void Module_PowerPIDAdjust(uint32_t SetPower)
{	
	/**************监控功率开启************************************/
	if (RF_ON == GN_Device.RFPwrState.Now && SetPower >= POWER_RATIO) 		
	{	
		if (SetPower != GN_Device.LastPower||GN_Device.Sensor.VSWR >= g_FactorData.VSWRLimit)
		{
			GN_Device.Phase_PID_Cnt = 0;  //清除功率超过阈值计数
			GN_Device.Volt_PID_Cnt = 0;   //清除功率超过阈值计数
			GN_Device.SetPIDPhase = 0;
			GN_Device.SetPIDVolt = 0;
		}
		if(GN_Device.Sensor.VSWR < TUNED_VSWR)GN_Device.PDO_End = 1;
		/**************设置功率 < 相位调制功率值 ,开始设置Phase PID********************/
		if(SetPower < IF_SL_InternalParam_GetPhasePoint()) 
		{	
			if(GN_Device.Phase_PID_Cnt == 0)
			{	
				IF_SL_CmdParam_SetACDCVoltage(INIT_ACDC_VOLTAGE);
				GN_Device.SetPIDPhase = Phase_PID_LookupTable(SetPower);	
				
			}else if(GN_Device.Phase_PID_Cnt > PID_TIMER)
			{
				GN_Device.SetPIDPhase += Phase_PID_realize(SetPower,GN_Device.Sensor.Pfwd);
			}
			Module_ACDCOutputPhase(GN_Device.SetPIDPhase);	
			if(GN_Device.Phase_PID_Cnt < UINT16_MAX)GN_Device.Phase_PID_Cnt++;
			if(GN_Device.PDO_End == 0 && GN_Device.Phase_PID_Cnt%PID_TIMER==0)
			{
				IF_SL_UpdateRFPwrPIDProcessData();
			}			
		}else
		{
			if(GN_Device.Volt_PID_Cnt == 0)
			{	
				IF_SL_CmdParam_SetDDSWorkPhase(INIT_ACDC_PHASE);	
				GN_Device.SetPIDVolt = Volt_PID_LookupTable(SetPower);
				
			}else if(GN_Device.Volt_PID_Cnt > PID_TIMER)
			{
				GN_Device.SetPIDVolt += Volt_PID_realize(SetPower,GN_Device.Sensor.Pfwd);
			}
			Module_ACDCOutputVoltage(GN_Device.SetPIDVolt);	
			if(GN_Device.Volt_PID_Cnt < UINT16_MAX)GN_Device.Volt_PID_Cnt++;
			if(GN_Device.PDO_End == 0 && GN_Device.Volt_PID_Cnt%PID_TIMER==0)
			{
				IF_SL_UpdateRFPwrPIDProcessData();	
			}			
		}
	}else
	{
		GN_Device.Phase_PID_Cnt = 0;
		GN_Device.Volt_PID_Cnt = 0;
		GN_Device.SetPIDPhase = 0;
		GN_Device.SetPIDVolt = 0;
		GN_Device.PowerThrDown = 0;
		GN_Device.PDO_End = 0;
		IF_SL_ClearRFPwrPIDProcessData();
		IF_SL_CmdParam_SetDDSWorkPhase(0);              //设置射频驱动相位为0
		IF_SL_CmdParam_SetACDCVoltage(INIT_ACDC_VOLTAGE);//设置AC-DC初始化电压	
		IF_SL_CmdParam_SetACDCDriverState(ON); 		    //开启AC-DC电源使能			
	}
	GN_Device.LastPower =  SetPower;		
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
	IF_CmdParam_SetDDSWorkPhase(SetPIDPhase);	
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
 * Function  : Module_CheckOutputPower
 * Description   : 电源电压输出阈值处理过程函数
 * Parameter     : OutputPower 输出的功率值
 * return        :                
 * END ***************************************************************************************/
static uint32_t Module_CheckOutputPower(float SlowRate)
{
	uint32_t OutputPower = 0;
	uint32_t SettingPower = SlowRate * IF_SL_CmdParam_GetPwrPoint();
	/**********************功率监测，开启状态*********************************/
	if(FWDREGULATION == IF_SL_UserParam_GetRegulationMode()) //前向功率模式下
	{
		if(GN_Device.Sensor.Pref > IF_SL_UserParam_GetReflectPowerThreshold())
		{ 
			if(GN_Device.PowerThrDown < SettingPower)
			{
				GN_Device.PowerThrDown += THRPOWER_RATE*GN_Device.Sensor.Pref;	
			}			
		}
		if(GN_Device.Sensor.Pfwd > IF_SL_UserParam_GetForwardPowerLimit())
		{
			g_StatusWord.bits.PforwAtLimit = ON;						
		}else	
		{
			g_StatusWord.bits.PforwAtLimit = OFF;	
		}
	}
	else if(LOADREGULATION == IF_SL_UserParam_GetRegulationMode()) //传输功率模式下
	{
		if(GN_Device.Sensor.Pref > IF_SL_CmdParam_GetPwrPoint())
		{	
			if(GN_Device.PowerThrDown < SettingPower)
			{
				GN_Device.PowerThrDown += THRPOWER_RATE*GN_Device.Sensor.Pref;
			}
		} 
		//如果反射功率大于反射功率最大值
		if(GN_Device.Sensor.Pref > IF_SL_UserParam_GetReflectPowerLimit())
		{	
			g_StatusWord.bits.PreflAtLimit = ON; 
			if(IF_SL_UserParam_GetReflectPowerDelayOff() > 0)
			{
				g_StatusWord.bits.RFWillswitchoff = ON; 
				IF_SL_Timer_SetRFOffDelayFlag(ON);  			//设置等待延时关闭功率开启
			}
			if( ON == IF_SL_Timer_GetRFOffDelayTimeOutFlag()) 
			{
				IF_SL_CmdParam_SetRFPowerState(RF_OFF);  	     //关闭射频电源		
				IF_SL_Timer_ClearRFOffDelayTimeOutFlag();        //清除超时延时标志位
			}		
		}else
		{ 
			IF_SL_Timer_SetRFOffDelayFlag(OFF); 
			g_StatusWord.bits.PreflAtLimit = OFF;
		}			
	}	
	OutputPower = SettingPower - GN_Device.PowerThrDown;
	return  OutputPower;
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
//	if(IF_SL_CmdParam_GetPwrPoint()> 200)
//	{
//		PhasePID.ErrorThr = IF_SL_CmdParam_GetPwrPoint()*0.003F;
//	}
	value =  IF_NvmParam_GetPhaseMapTable(Power);
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
	VoltPID.iError =	0.0F;      //当前误差值
	VoltPID.LastError = 0.0F;	   //上次误差值
	VoltPID.PrevError = 0.0F;	   //上上次误差值
	VoltPID.AdjCnt = 0;			   //PID调整次数
	VoltPID.ErrorThr = g_FactorData.VoltEThr;
//	if(IF_SL_CmdParam_GetPwrPoint() > 200)
//	{
//		VoltPID.ErrorThr = IF_SL_CmdParam_GetPwrPoint()*0.003F;
//	}
	int16_t vTableIndex = (Power/10)-1;
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

