/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Am_SWC.c
 * @Brief :      
 * @Author :     Chenghanbing (iwanthaha@sina.com)
 * @Version :    1.0
 * @Creat Date : 2022-12-06
 * ****************************************************************************
 */

#include "IF_HAL_Cfg.h"
#include "IF_SL_Cfg.h"
#include "IF_Am_SWC.h"
#include "IF_Comm.h"
#include "Am_SWC.h"
#include "AmAlgorithm2G.h"
#include "AmAlgorithmPi2G.h"
/* CONST & MACROS */

/* DATA STRUCTURES */

/* LOCAL VARIABLES */
static AM_Device_t Am_Device;

#if(config_TargetVSWRLimit == 1)
	static float TargetVSWRLimit = 1.06F;
#endif

static float TargetR = 50.0F;
static float TargetX = 0.0F;
static float RFPwrOnThr	 = 0.0F;
static uint8_t AMPD_End = 0;
static uint16_t TargetPos[CAP_NUM]={0};


/* GLOBAL VARIABLES */
float Load_RI,Load_XI,currentCl_Val, currentCt_Val, targetCl_Val, targetCt_Val;
SemaphoreHandle_t maintainSemaphore = NULL;
/* FUNCTION PROTOTYPES */
static void AM_Module_Input(void);
static void AM_Module_Port_Input(void);
static void AM_Module_Cmd_Input(void);
static void AM_Module_Sensor_Input(void);
static void AM_Module_Fpga_Input(void);
static void AM_Module_Process(void);
static void AM_Module_Output(void);

static void Check_TriggerPresetCondition(void);
static void AM_Module_Algorithm_2G(void);
static void AM_Module_Algorithm_2G_Target(void);
static void AM_Module_AutoMatch_2G(void);
/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/

/**
 * ************************************************************************
 * @Brief :      
 * ************************************************************************
 */
 /* FUNCTION *********************************************************************************
 * Function Name : IF_Am_Algorithm_2GInit
 * Description   : 匹配器算法初始化接口
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
 static void AM_Module_Algorithm_2G(void)
 {
#if DEBUG_ALG    
    uint8_t optOnVal, optConVal, bigLoadSTEPVal, bigTuneSTEPVal;
    uint16_t currentLoadPos, currentTunePos,  outLoadPos, outTunePos;
#endif 
#if(config_AlgorithmModel== 0)		 
	//Alg Init...
	float FrontL 	= 0.001f * IF_SL_NvmParam_GetFrontL(1);
	float ShuntL	= 0.001f * IF_SL_NvmParam_GetShuntL(1);
	float RearL 	= 0.001f * IF_SL_NvmParam_GetRearL(1);
	 
	uint16_t CLPercent 	= IF_SL_NvmParam_GetCapPercent(LOAD);
	uint16_t CLZero 	= IF_SL_NvmParam_GetCapZero(LOAD);
	uint16_t CTPercent 	= IF_SL_NvmParam_GetCapPercent(TUNE);
	uint16_t CTZero 	= IF_SL_NvmParam_GetCapZero(TUNE);
	 
	Am_Algorithm_2G_LSetting(FrontL,ShuntL,RearL);
	Am_Algorithm_2G_CSetting(CLPercent, CLZero, CTPercent, CTZero, CAP_MAX_SCOPE);

	Am_Algorithm_2G(Am_Device.Sensor.R, 
					Am_Device.Sensor.X, 
					Am_Device.Sensor.Freq,
					Am_Device.Sensor.Vrms,
					Am_Device.Sensor.Irms, 
					IF_SL_Motor_GetCapPos(LOAD), 
					IF_SL_Motor_GetCapPos(TUNE), 
					&TargetPos[LOAD],&TargetPos[TUNE],
					MAIN_TASK_PERIOD);
	
	Am_Algorithm_2G_Debug(&currentCl_Val, &currentCt_Val, 
						  &Load_RI, 	  &Load_XI, 
						  &targetCl_Val,  &targetCt_Val, 
						  &g_Load_RI, 	  &g_Load_XI);
	#if DEBUG_ALG
       Am_Algorithm_2G_OptimizeDebug(&optOnVal, &optConVal, 
						   &bigLoadSTEPVal, &bigTuneSTEPVal, 
						   &currentLoadPos, &currentTunePos, 
						   &outLoadPos, &outTunePos);
	   g_Load_RI = 0xFFFFF & ((((uint32_t)optOnVal)<<18) | (((uint32_t)optConVal)<<16) | ((uint32_t)outLoadPos)) ;
       g_Load_XI = 0xFFFFF & ((((uint32_t)bigLoadSTEPVal)<<18) | (((uint32_t)bigTuneSTEPVal)<<16) | ((uint32_t)outTunePos));
	#endif      
#else
	//Alg Init...
	float L0 	= 0.001f * IF_SL_NvmParam_GetPIModelL(1);
	float L1 	= 0.001f * IF_SL_NvmParam_GetPIModelL(2);
	float L2 	= 0.001f * IF_SL_NvmParam_GetPIModelL(3);
	float L3 	= 0.001f * IF_SL_NvmParam_GetPIModelL(4);
 
	uint16_t CLPercent 	= IF_SL_NvmParam_GetCapPercent(LOAD);
	uint16_t CLZero 	= IF_SL_NvmParam_GetCapZero(LOAD);
	uint16_t CTPercent 	= IF_SL_NvmParam_GetCapPercent(TUNE);
	uint16_t CTZero 	= IF_SL_NvmParam_GetCapZero(TUNE);
	 
	Am_AlgorithmPi_2G_LSetting(L0,L1,L2,L3);
	Am_AlgorithmPi_2G_CSetting(CLPercent, CLZero, CTPercent, CTZero, CAP_MAX_SCOPE);

	Am_AlgorithmPi_2G(Am_Device.Sensor.R, 
					Am_Device.Sensor.X, 
					Am_Device.Sensor.Freq,
					Am_Device.Sensor.Vrms,
					Am_Device.Sensor.Irms, 
					IF_SL_Motor_GetCapPos(LOAD), 
					IF_SL_Motor_GetCapPos(TUNE), 
					&TargetPos[LOAD],&TargetPos[TUNE],
					MAIN_TASK_PERIOD);
	
	Am_AlgorithmPi_2G_Debug(&currentCl_Val, &currentCt_Val, 
						  &Load_RI, 	  &Load_XI, 
						  &targetCl_Val,  &targetCt_Val, 
						  &g_Load_RI, 	  &g_Load_XI);
	#if DEBUG_ALG
       Am_AlgorithmPi_2G_OptimizeDebug(&optOnVal, &optConVal, 
						   &bigLoadSTEPVal, &bigTuneSTEPVal, 
						   &currentLoadPos, &currentTunePos, 
						   &outLoadPos, &outTunePos);
	   g_Load_RI = 0xFFFFF & ((((uint32_t)optOnVal)<<18) | (((uint32_t)optConVal)<<16) | ((uint32_t)outLoadPos)) ;
       g_Load_XI = 0xFFFFF & ((((uint32_t)bigLoadSTEPVal)<<18) | (((uint32_t)bigTuneSTEPVal)<<16) | ((uint32_t)outTunePos));
	#endif 					  
#endif
 }
 /* FUNCTION *********************************************************************************
 * Function Name : IF_Am_SWCInit
 * Description   : 匹配器设置初始化接口
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
void IF_Am_SWCInit(void)
{
    //Global Init...
	memset(&Am_Device, 0, sizeof(AM_Device_t));
#if(config_InterLockInput ==1)
	Am_Device.IntlkState = 0;
#else	
	Am_Device.IntlkState = 1;
#endif
	Am_Device.State = STATE_INIT;
	Am_Device.MaintMode = MODE_NOMAINT;
	Am_Device.RFOnOff.Now = AM_RF_OFF;
	Am_Device.Tuned.Now = AM_NOT_TUNED; 
	//设备开启默认参数设置成自动模式
	Am_Device.RunMode.Now = MODE_AUTO;
	//Am_Device.RunMode.Now = (RunModeEnum)IF_SL_NvmParam_GetWorkMode();
	
}
 /* FUNCTION *********************************************************************************
 * Function Name : AM_Module_Algorithm_2G_Target
 * Description   : 匹配器算法目标值接口
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void AM_Module_Algorithm_2G_Target(void)
{

	if(ON ==  IF_SL_Get_Sensor_2G_PulseMode(LF_CHN))  //脉冲模式下
	{	
		TargetR  =  0.001f*IF_SL_NvmParam_GetTargetReal(2);
		TargetX  =  0.001f*IF_SL_NvmParam_GetTargetImage(2);
	}else
	{
		TargetR  =  0.001f*IF_SL_NvmParam_GetTargetReal(1);
		TargetX  =  0.001f*IF_SL_NvmParam_GetTargetImage(1);	
	}

#if(config_AlgorithmModel== 0)	
	Am_Algorithm_2G_ZSetting(TargetR,TargetX);
#else
	Am_AlgorithmPi_2G_ZSetting(TargetR,TargetX);
#endif
}	
/* FUNCTION *********************************************************************************
 * Function Name : IF_AM_Main_Task
 * Description   : 匹配器输入任务函数接口
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
void IF_AM_Main_Task(void)
{
	AM_Module_Input();
	AM_Module_Process();
	AM_Module_Output();
}

 /* FUNCTION *********************************************************************************
 * Function Name : IF_AM_DataStorage_Task
 * Description   : 匹配器数据存储任务函数接口
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
void IF_AM_DataStorage_Task(void)
{
	
	//自动匹配成功后,非匹配过程中记录当前匹配位置
	if (Am_Device.Tuned.Now == AM_NOT_TUNED)
	{
		if (ON==IF_SL_Timer_GetAMSuccessTimeOut())
		{
			IF_SL_NvmParam_SaveMatchPos();
			IF_SL_Timer_ClearAMSuccessTimeOut();
		}
	}	 		
}
/* FUNCTION *********************************************************************
 * Function Name : IF_AM_Maintains_Task
 * Description   : 匹配器维护任务函数
 * Parameter     : 
 * return        :                
 * END **************************************************************************/
void IF_AM_Maintains_Task(void)
{
	uint8_t maintSts = E_NOT_OK;

	if(pdTRUE ==xSemaphoreTake(maintainSemaphore, portMAX_DELAY))
	{
		if (Am_Device.MaintMode == MODE_INDEX)
		{
			maintSts = IF_SL_Motor_IndexCapacitor();	
		}
		else if (Am_Device.MaintMode == MODE_REGREASE)
		{	
			maintSts = IF_SL_Motor_RegreaseCapacitor();		
		}
		if(maintSts == E_OK)
		{
			Am_Device.MaintStatus = MAINT_SUCESSED;
		}
		else
		{
			Am_Device.MaintStatus = MAINT_FAILED;
		}
	}
}

/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/
 /* FUNCTION *********************************************************************************
 * Function Name : AM_Module_Input
 * Description   : 匹配器输入端口入口函数(port,Cmd,Sensor,Monitor)
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void AM_Module_Input(void)
{
    AM_Module_Port_Input();
    AM_Module_Cmd_Input();
    AM_Module_Sensor_Input();
	
}
/* FUNCTION *********************************************************************************
 * Function Name : AM_HF_Port_Input
 * Description   : 高频单元匹配器输入端口函数(port)
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void AM_Module_Port_Input(void)
{	
	if(Am_Device.State == STATE_RUN)
	{
		AM_Module_Fpga_Input();
    //get interlock state.
	#if(config_InterLockInput ==1)	
		Am_Device.IntlkState = IF_SL_Get_NormalInput_SW(IOSWITCH_COVER_INTLK);
	#endif	
	}
}
/* FUNCTION *********************************************************************************
 * Function Name : AM_Module_Fpga_Input
 * Description   : 匹配器FPGA输入端口函数(port)
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void AM_Module_Fpga_Input(void)
{
#if(config_PulseFunction == 1)		
	//Setup Pulse RFPwrThr
	if(Am_Device.Pulse.RFPwrThr != IF_SL_NvmParam_GetPulseRFPowerThr())
	{
		Am_Device.Pulse.RFPwrThr = IF_SL_NvmParam_GetPulseRFPowerThr();
		IF_SL_Set_Sensor_2G_PulseRFPowerThr(LF_CHN,Am_Device.Pulse.RFPwrThr);
	}
	#if(config_PulseSyncMode ==1)
		//Setup Pulse Sync Mode RFPower Sync/Fpga Sync
		if(Am_Device.Pulse.SyncMode != IF_SL_NvmParam_GetPulseSyncMode())
		{
			Am_Device.Pulse.SyncMode = IF_SL_NvmParam_GetPulseSyncMode();
			IF_SL_Set_Sensor_2G_PulseSyncMode(LF_CHN,Am_Device.Pulse.SyncMode);
		}
		
		//Setup Pulse Sync delay  Static /Dynamic Sync 
		if(Am_Device.RFOnOff.Now == AM_RF_ON)
		{
			if(ON == IF_SL_NvmParam_GetPulseDynamicSyncMode()) 
			{
				if(Am_Device.Pulse.SyncDelay != IF_SL_Get_Sensor_2G_PulseDynamicSyncDelay(LF_CHN))
				{
					Am_Device.Pulse.SyncDelay = IF_SL_Get_Sensor_2G_PulseDynamicSyncDelay(LF_CHN);
					IF_SL_Set_Sensor_2G_PulseSyncDelay(LF_CHN,Am_Device.Pulse.SyncDelay);
				}	
			}else
			{
				if(Am_Device.Pulse.SyncDelay != IF_SL_NvmParam_GetPulseStaticSyncDelay())
				{
					Am_Device.Pulse.SyncDelay = IF_SL_NvmParam_GetPulseStaticSyncDelay();
					IF_SL_Set_Sensor_2G_PulseSyncDelay(LF_CHN,Am_Device.Pulse.SyncDelay);
				}		
			}
		}else
	#endif
		{
			if(Am_Device.Pulse.SyncDelay != IF_SL_NvmParam_GetPulseStaticSyncDelay())
			{
				Am_Device.Pulse.SyncDelay = IF_SL_NvmParam_GetPulseStaticSyncDelay();
				IF_SL_Set_Sensor_2G_PulseSyncDelay(LF_CHN,Am_Device.Pulse.SyncDelay);	
			}	
		}	

#endif
}	

/* FUNCTION *********************************************************************************
 * Function Name : AM_Module_Cmd_Input
 * Description   : 匹配器输入端口函数(Cmd)
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void AM_Module_Cmd_Input(void)
{	
    DeviceCmdMsg_t amDeviceCmd;
    if(pdTRUE ==xQueueReceive(CmdQueue, &amDeviceCmd, (TickType_t)0))
	{    
        switch (amDeviceCmd.cmdId)
        {
        //global control Cmd
        case AM_CMD_RESET:   
             IF_SL_ResetDevice();
			break;
		case AM_CMD_WORKMODE:   
			if( OFF == IF_SL_NvmParam_GetAnalogControl())
			{	if(amDeviceCmd.cmdData[0]==2)
				{
					Am_Device.RunMode.Now = MODE_HOLD;
				}else if(amDeviceCmd.cmdData[0]==1)
				{
					Am_Device.RunMode.Now = MODE_AUTO;
				}else if(amDeviceCmd.cmdData[0]==0)
				{
					Am_Device.RunMode.Now = MODE_MANUAL;
				}	
			}
			break;
		case AM_CMD_REGREASE:
            Am_Device.MaintMode = MODE_REGREASE;  //Enter Regrease mode
            break;
        case AM_CMD_INDEX:
            Am_Device.MaintMode = MODE_INDEX;  	//Enter Index mode
            break;
		 case AM_CMD_MOVELOAD:
            if(OFF == IF_SL_NvmParam_GetAnalogControl()&&Am_Device.Tuned.Now == AM_NOT_TUNED)
            { 
                uint16_t TargetPos= MAKE_UINT16(amDeviceCmd.cmdData[1],amDeviceCmd.cmdData[0]);
                IF_SL_Motor_SetTargetPos(LOAD, TargetPos);
            }
			break;
        case AM_CMD_MOVETUNE:
            if(OFF == IF_SL_NvmParam_GetAnalogControl()&&Am_Device.Tuned.Now == AM_NOT_TUNED)
            { 
                uint16_t TargetPos= MAKE_UINT16(amDeviceCmd.cmdData[1],amDeviceCmd.cmdData[0]);
                IF_SL_Motor_SetTargetPos(TUNE, TargetPos);
            }
			break;
	#if(config_Auxiliary_Cap ==1)
		case AM_CMD_MOVEAUXI:
            if(OFF == IF_SL_NvmParam_GetAnalogControl())
            {
                uint16_t TargetPos= MAKE_UINT16(amDeviceCmd.cmdData[1],amDeviceCmd.cmdData[0]);
                IF_SL_Motor_SetTargetPos(AUXI, TargetPos);
            }
            break;
	#endif
		case AM_CMD_CLEARFAULT:
             g_StatusWord.bits.SystemStatusOK = ON;
            break;
		default:break;
        }
	}		
}
/* FUNCTION *********************************************************************************
 * Function Name : AM_Module_Sensor_Input
 * Description   : 匹配器输入端口函数(Sensor)
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void AM_Module_Sensor_Input(void)
{	
	//设备Sensor输入
	Am_Device.Sensor.Freq = IF_SL_Get_Sensor_2G_InputFreq(LF_CHN);
	Am_Device.Sensor.Vrms = IF_SL_Get_Sensor_2G_InputVrms(LF_CHN);
	Am_Device.Sensor.Irms = IF_SL_Get_Sensor_2G_InputIrms(LF_CHN);
	Am_Device.Sensor.R = IF_SL_Get_Sensor_2G_InputR(LF_CHN);
	Am_Device.Sensor.X = IF_SL_Get_Sensor_2G_InputX(LF_CHN);
	Am_Device.Sensor.Pfwd = IF_SL_Get_Sensor_2G_InputPfwd(LF_CHN);
	
#if(config_TargetVSWRLimit ==1)
	Am_Device.Sensor.VSWR =IF_SL_Get_Sensor_2G_InputVSWR(LF_CHN);
	TargetVSWRLimit = 0.001f*IF_SL_NvmParam_GetVSWRLimit();
#endif		
	RFPwrOnThr = 0.001F * IF_SL_NvmParam_GetRFOnThrInputPower();
	
	if((Am_Device.Sensor.Vrms > RFPwrOnThr)||(Am_Device.Sensor.Irms > RFPwrOnThr))	
	{
		IF_SL_Timer_SetRFOnDelayFlag(ON);   
		if(ON == IF_SL_Timer_GetRFOnDelayTimeOut())
		{
			IF_SL_Timer_ClearRFOnDelayTimeOut();
			Am_Device.RFOnOff.Now = AM_RF_ON;
		}
	}
	else
	{
		if (Am_Device.RFOnOff.Now == AM_RF_ON)
		{
			IF_SL_Timer_SetRFOffDelayFlag(ON);   			
			if(ON == IF_SL_Timer_GetRFOffDelayTimeOut())
			{
				IF_SL_Timer_ClearRFOffDelayTimeOut();
				Am_Device.RFOnOff.Now = AM_RF_OFF; 		
			}
		}
	}
}

/* FUNCTION *********************************************************************************
 * Function Name : AM_Module_Process
 * Description   : 匹配器运行过程函数
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void AM_Module_Process(void)
{
	//Am device global Am_Action process
    switch(Am_Device.State)
    {
    case STATE_INIT:
   
        if(Am_Device.MaintStatus == MAINT_INIT)
        {
           Am_Device.MaintStatus = MAINT_RUN;
		   Am_Device.MaintMode = MODE_INDEX;
           xSemaphoreGive(maintainSemaphore);
        }
        //Check maintain result
        if (Am_Device.MaintStatus == MAINT_SUCESSED)
        {
            Am_Device.State = STATE_RUN;
            Am_Device.MaintStatus = MAINT_INIT;  //end maintain operation
            Am_Device.MaintMode = MODE_NOMAINT;
        }
        else if (Am_Device.MaintStatus == MAINT_FAILED)
        {
            Am_Device.State = STATE_IDLE;
            Am_Device.MaintStatus = MAINT_INIT;  //end maintain operation
            Am_Device.MaintMode = MODE_NOMAINT;
        }
	
        break;

    case STATE_RUN:
		
		if((Am_Device.MaintMode == MODE_REGREASE)||
		   (Am_Device.MaintMode == MODE_INDEX))
        {
           Am_Device.State = STATE_MAINTAIN;   
        }
		else 
        {
            Check_TriggerPresetCondition();
			if( (IF_SL_Motor_GetPresetStatus(LOAD) == STATUS_PRESET_ENDED)&&
				(IF_SL_Motor_GetPresetStatus(TUNE) == STATUS_PRESET_ENDED))
			{	
				AM_Module_AutoMatch_2G();
			}				
		}   
        break;

       case STATE_MAINTAIN:
        //Start maintain operation
        if (Am_Device.MaintStatus == MAINT_INIT)
        {
            Am_Device.MaintStatus = MAINT_RUN;
            xSemaphoreGive(maintainSemaphore);
        }

        //Check maintain result
        if (Am_Device.MaintStatus == MAINT_SUCESSED)
        {
            Am_Device.MaintStatus = MAINT_INIT;  //end maintain operation
            Am_Device.MaintMode = MODE_NOMAINT;  
            Am_Device.State = STATE_RUN;   
        }
        else if (Am_Device.MaintStatus == MAINT_FAILED)
        {
            Am_Device.MaintStatus = MAINT_INIT;  //end maintain operation
            
            Am_Device.State = STATE_IDLE;
        }
        break;

    case STATE_IDLE:
        break;

    default:
        Am_Device.State = STATE_IDLE;
        break;
    }
}

/* FUNCTION *********************************************************************************
 * Function Name : Check_TriggerPresetCondition
 * Description   : 匹配器预设触发状态监测函数
 * Parameter     : ---匹配器单元
 * return        :                
 * END ***************************************************************************************/
static void Check_TriggerPresetCondition(void)
{

	// 1. 触发RF Off Preset
	// (1) RF Off且, A->M 或 M->A 设置预设位置和延时
//	if ( (Am_Device.RFOnOff.Now == AM_RF_OFF)
//		&& (Am_Device.RunMode.Last != Am_Device.RunMode.Now))
//  {
//		IF_SL_Motor_TriggerRFOffPreset();  //20221116 no need preset when A->M or M->A
//	}

	// (2) A或M时，On->Off 设置预设位置和延时
	if ((Am_Device.RFOnOff.Last == AM_RF_ON) && (Am_Device.RFOnOff.Now == AM_RF_OFF))
    {
		IF_SL_Motor_TriggerRFOffPreset();
	}

	// 2. 触发 RF On Preset
	// (3) RF On时，A->M 设置预设位置和延时
//	if ((Am_Device.RFOnOff.Now == AM_RF_ON)&& (Am_Device.RunMode.Last ==  MODE_AUTO)&& (Am_Device.RunMode.Now == MODE_MANUAL))
//  {
//		IF_SL_Motor_TriggerRFOnPreset();  //20221116 no need preset when A->M or M->A
//	}

	// (4) A或M时，射频功率 Off->On，设置预设位置和延时
    if ((Am_Device.RFOnOff.Last == AM_RF_OFF) && (Am_Device.RFOnOff.Now == AM_RF_ON))
	{
		IF_SL_Motor_TriggerRFOnPreset();
	}
	//等待电源开启预设延时完成
	if((ON == IF_SL_Timer_GetRFOnPresetDelayTimeOut()))
    {
		IF_SL_Timer_ClearRFOnPresetDelayTimeOut();
		IF_SL_Motor_SetPresetStatus(LOAD, STATUS_PRESET_SETTING);  //set LOAD PresetStatus=2;
		IF_SL_Motor_SetPresetStatus(TUNE, STATUS_PRESET_SETTING);  //set TUNE PresetStatus=2;
	#if(config_Auxiliary_Cap == 1)
		IF_SL_Motor_SetPresetStatus(AUXI, STATUS_PRESET_SETTING);  //set AUXI PresetStatus=2;
	#endif
		
	}
	//等待电源关闭预设延时完成
	if(ON == IF_SL_Timer_GetRFOffPresetDelayTimeOut())
	{
		IF_SL_Timer_ClearRFOffPresetDelayTimeOut();
		IF_SL_Motor_SetPresetStatus(LOAD, STATUS_PRESET_SETTING);  //set LOAD PresetStatus=2;
		IF_SL_Motor_SetPresetStatus(TUNE, STATUS_PRESET_SETTING);  //set TUNE PresetStatus=2;
	#if(config_Auxiliary_Cap == 1)
		IF_SL_Motor_SetPresetStatus(AUXI, STATUS_PRESET_SETTING);  //set AUXI PresetStatus=2;
	#endif
		
	}
	
	if (IF_SL_Motor_GetPresetStatus(LOAD) == STATUS_PRESET_SETTING)
	{
		TargetPos[LOAD] = IF_SL_Motor_GetCapPreset(LOAD);
		IF_SL_Motor_SetTargetPos(LOAD, TargetPos[LOAD]);
	}
	if(IF_SL_Motor_GetPresetStatus(TUNE) == STATUS_PRESET_SETTING)
	{
		TargetPos[TUNE] = IF_SL_Motor_GetCapPreset(TUNE);
		IF_SL_Motor_SetTargetPos(TUNE, TargetPos[TUNE]);	
	} 
#if(config_Auxiliary_Cap == 1)
	if(IF_SL_Motor_GetPresetStatus(AUXI) == STATUS_PRESET_SETTING)
	{
		TargetPos[AUXI] = IF_SL_Motor_GetCapPreset(AUXI);
		IF_SL_Motor_SetTargetPos(AUXI, TargetPos[AUXI]);	
	} 
#endif
}
/* FUNCTION *********************************************************************************
 * Function Name : AM_Module_AutoMatch_2G
 * Description   : 匹配器匹配过程函数
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void AM_Module_AutoMatch_2G(void)
{
	//电源开启,并且自动模式,进入自动匹配模式
	if ((Am_Device.RFOnOff.Now == AM_RF_ON) && (Am_Device.RunMode.Now == MODE_AUTO))
	{	
		AM_Module_Algorithm_2G_Target();
		if((Am_Device.Sensor.Vrms > RFPwrOnThr)||(Am_Device.Sensor.Irms > RFPwrOnThr))				
		{
			AM_Module_Algorithm_2G();
			if(AMPD_End == OFF)IF_SL_NvmParam_UpdateAMPD();
		}
		//自动匹配的4个条件满足时，自动匹配成功 
#if(config_TargetVSWRLimit == 1)	  //使用驻波比作为目标	
	if( (Am_Device.Sensor.VSWR <= TargetVSWRLimit)&&
		(IF_SL_Motor_GetStartFlag(LOAD) == STATUS_MOTOR_STOP) &&
		(IF_SL_Motor_GetStartFlag(TUNE) == STATUS_MOTOR_STOP))	
#else 							//使用阻抗作为目标
	if( (Am_Device.Sensor.R <=(TargetR+3.0F) && Am_Device.Sensor.R >=(TargetR-3.0F)) && 
		(Am_Device.Sensor.X <=(TargetX+3.0F) && Am_Device.Sensor.X >=(TargetX-3.0F)) &&
		(IF_SL_Motor_GetStartFlag(LOAD) == STATUS_MOTOR_STOP) &&
		(IF_SL_Motor_GetStartFlag(TUNE) == STATUS_MOTOR_STOP))	
#endif
		{
			/*匹配状态切换后启动计时，用于保存当前匹配位置的延时*/
			if (Am_Device.Tuned.Now != AM_TUNED)
			{ 
				AMPD_End = ON;						
				IF_SL_Timer_SetAMSuccessFlag(ON);
				Am_Device.Tuned.Now = AM_TUNED;								
			}
			g_TunedLoad_RO = g_Load_RI;
			g_TunedLoad_XO = g_Load_XI;
			g_LastMatchPos[LOAD] = IF_SL_Motor_GetCapPos(LOAD);
			g_LastMatchPos[TUNE] = IF_SL_Motor_GetCapPos(TUNE);
		#if(config_Auxiliary_Cap ==1)
			g_LastMatchPos[AUXI] = IF_SL_Motor_GetCapPos(AUXI);
		#endif			
		}else
		{
			if (Am_Device.Tuned.Now != AM_TUNING)   
			{	
			#if(config_AlgorithmModel ==0)
				Am_Algorithm_2G_End();
			#else	
				Am_AlgorithmPi_2G_End();				
			#endif				
				Am_Device.AMFineTimer = 0;
				Am_Device.RegreaseCnt++;
				IF_SL_Timer_SetTuningTimerVal(0);	
				IF_SL_Timer_SetAMSuccessFlag(OFF);		
				Am_Device.Tuned.Now = AM_TUNING;			
			}
			 /*如果在匹配范围内并且电容在运动状态，精细调整*/	
	#if(config_TargetVSWRLimit == 1)	//使用驻波比作为目标	
		if( (Am_Device.Sensor.VSWR <= TargetVSWRLimit)) 					 
	#else								//使用阻抗作为目标
		if( (Am_Device.Sensor.R <=(TargetR+3.0F) && Am_Device.Sensor.R >=(TargetR-3.0F)) && 
			(Am_Device.Sensor.X <=(TargetX+3.0F) && Am_Device.Sensor.X >=(TargetX-3.0F)))
	#endif
			{
				
		#if(config_TargetVSWRLimit == 1)	//使用驻波比作为目标
			if((Am_Device.Sensor.VSWR <= (TargetVSWRLimit-0.015F)) ||(Am_Device.AMFineTimer >= AMFINE_PERIOD) )
		#else								//使用阻抗作为目标
			if( (Am_Device.Sensor.R <=(TargetR+2.5F) && Am_Device.Sensor.R >=(TargetR-2.5F)) ||
				(Am_Device.Sensor.X <=(TargetX+2.5F) && Am_Device.Sensor.X >=(TargetX-2.5F)) ||
				(Am_Device.AMFineTimer >= AMFINE_PERIOD) )	
		#endif
	
				{
					TargetPos[LOAD] = IF_SL_Motor_GetCapPos(LOAD);
					TargetPos[TUNE] = IF_SL_Motor_GetCapPos(TUNE);
				}
				Am_Device.AMFineTimer++;				
			}
			if(IF_SL_Timer_GetTuningTimerVal() >= IF_SL_NvmParam_GetMaxAutoMatchTime())
			{
				AMPD_End = ON;
			}
			IF_SL_Motor_SetTargetPos(LOAD, TargetPos[LOAD]);
			IF_SL_Motor_SetTargetPos(TUNE, TargetPos[TUNE]);											
		}
	}
	else //非自动匹配模式
	{	
		IF_SL_Timer_SetTuningTimerVal(0);	
		IF_SL_Timer_SetLastTuningTimerVal(0);	
		if(Am_Device.Tuned.Now != AM_NOT_TUNED) //自动匹配模式切换     
		{	
			AMPD_End = OFF;
			IF_SL_NvmParam_ClearAMPD();	
			IF_SL_Motor_SetAllStop();
			Am_Device.Tuned.Now = AM_NOT_TUNED;			
		}					
	}
}

//Output Am_LF_Module Status word...
/* FUNCTION *********************************************************************************
 * Function Name : AM_Module_Output
 * Description   : 匹配器模块输出函数
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void AM_Module_Output(void)
{	
	if(Am_Device.State == STATE_RUN)
	{
		 //设置匹配器维护状态,监测InterLock状态
		g_StatusWord.bits.IndexBad = 0;
		if(Am_Device.IntlkState == 1)   
		{
			g_FaultWord.bits.coverIntlk =  0;
		}else
		{
			g_FaultWord.bits.coverIntlk =  1;
		}	
	}else
	{
		g_StatusWord.bits.IndexBad = 1;	
		g_FaultWord.bits.coverIntlk = 1;
	}	
	//设置匹配器故障状态字
	if(Am_Device.State == STATE_IDLE)
	{
		g_FaultWord.bits.MotorInitialization = ON;
	}else
	{
		g_FaultWord.bits.MotorInitialization = OFF;
	}
		
	//设置默认匹配器InterLock开关状态
	if(	g_FaultWord.bits.coverIntlk || 
		g_FaultWord.bits.OvervoltageOnC2 || 
		g_FaultWord.bits.OverTemperature)
	{
		IF_SL_Set_NormalOutput_SIG(IOSIG_INTERLOCK,OFF);
	}else
	{
		IF_SL_Set_NormalOutput_SIG(IOSIG_INTERLOCK,ON);
	}	
	//设置匹配器电容索引状态字
	if(Am_Device.MaintMode == MODE_INDEX)
	{
		g_StatusWord.bits.Indexing = ON;	
	}else
	{
		g_StatusWord.bits.Indexing = OFF;	
	}
	
	//设置匹配器电容润滑状态字
	if(Am_Device.MaintMode == MODE_REGREASE)
	{
		g_StatusWord.bits.Regreasing = ON;	
	}else
	{
		g_StatusWord.bits.Regreasing = OFF;	
	}
	
	//设置匹配器运行模式的状态字
	if(Am_Device.RunMode.Now == MODE_MANUAL)
	{
		g_StatusWord.bits.ManualOrAuto = OFF;
	}
	else if(Am_Device.RunMode.Now == MODE_AUTO)
	{
		g_StatusWord.bits.ManualOrAuto = ON;
	}
	Am_Device.RunMode.Last = Am_Device.RunMode.Now;
	
	//设置匹配器监控模拟口状态的状态字
	if(ON == IF_NvmParam_GetAnalogControl())
	{
		g_StatusWord.bits.LocalOrRemote = ON;
		
	}else
	{
		g_StatusWord.bits.LocalOrRemote = OFF;
	}
	//设置匹配器监控射频电源状态的状态字
	if(Am_Device.RFOnOff.Now == AM_RF_OFF)
	{
		g_StatusWord.bits.RFOnOff = OFF;
		IF_SL_Set_NormalOutput_SIG(IOSIG_RFONLED,OFF);
	}
	else
	{
		g_StatusWord.bits.RFOnOff = ON;
		IF_SL_Set_NormalOutput_SIG(IOSIG_RFONLED,ON);
	}
	Am_Device.RFOnOff.Last = Am_Device.RFOnOff.Now;

	//设置匹配器监控射频电源状态的状态字
	if(Am_Device.Tuned.Now == AM_TUNED)
	{
	   g_StatusWord.bits.Tuned = ON;
	}
	else
	{
		g_StatusWord.bits.Tuned = OFF;
	}
	Am_Device.Tuned.Last = Am_Device.Tuned.Now;
	//设置匹配器是否需要维护状态
	if(Am_Device.RegreaseCnt >= MATCHER_REGEASE_CNT)
	{
		g_StatusWord.bits.NeedRegrease = ON;	
	}else
	{
		g_StatusWord.bits.NeedRegrease = OFF;	
	}
	//设置匹配器LOAD电容状态的状态字
	if (ON == IF_SL_Motor_GetStartFlag(LOAD))   
	{
		g_StatusWord.bits.LoadMotorMoving = ON;		
	}
	else
	{
		g_StatusWord.bits.LoadMotorMoving = OFF;			
	}
	//设置匹配器TUNE电容状态的状态字
	if (ON == IF_SL_Motor_GetStartFlag(TUNE))
	{
		g_StatusWord.bits.TuneMotorMoving = ON;	
	}
	else
	{ 
		g_StatusWord.bits.TuneMotorMoving = OFF;		
	}


#if(config_VDCbiasSwitch ==1)
	//设置匹配器VDC Control开关状态
	if(IF_SL_NvmParam_GetVdcBiasControl() ==ON)
	{
		g_StatusWord.bits.VDCSwitch =  ON;
		IF_SL_Set_NormalOutput_SIG(IOSIG_VDCFAN,ON);
	}else
	{
		g_StatusWord.bits.VDCSwitch =  OFF;
		IF_SL_Set_NormalOutput_SIG(IOSIG_VDCFAN,OFF);	
	}	
#endif	
	if(abs(IF_SL_Motor_GetCapPos(TUNE)-IF_SL_Motor_GetEncoderPos(TUNE)) > 50 ||
	   abs(IF_SL_Motor_GetCapPos(LOAD)-IF_SL_Motor_GetEncoderPos(LOAD)) > 50 )
	{
		g_FaultWord.bits.MotorLoss =1;
	}else
	{
		g_FaultWord.bits.MotorLoss =0;
	}
	//设置匹配器系统状态字
	g_StatusWord.bits.SystemStatusOK = (g_FaultWord.word > 0) ? 0 : 1;
}

//*****************************************************************************
//* END
//*****************************************************************************

