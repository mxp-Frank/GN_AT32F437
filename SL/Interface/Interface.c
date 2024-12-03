/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Interface.c
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-10-09
 * ****************************************************************************
 */

#include "IF_Interface.h"
#include "Interface.h"
#include "IF_HAL_Cfg.h"
#include "IF_SL_Cfg.h"
#include "params_impl.h"


/* CONST & MACROS */

/* DATA STRUCTURES */

/* LOCAL VARIABLES */

/* GLOBAL VARIABLES */

static AD_Ctrl_Input_t ADC_Input = 
{
	.PowerValue = 0,
	.PlasmVDC = 0,
	.DCBias = 0,
	.loadCapPosition = 0,
	.tuneCapPosition = 0,
	.ACDC50VDect = 0,
	.Encode = 0,
	.RFTemp = 0,		
};

/* FUNCTION PROTOTYPES */
static void EXT_ADC_CtrlPin_Scan(void);

/************************************************************************/
/* Global Functions Definitions                                         */
/************************************************************************/

/* FUNCTION *********************************************************************************
 * Function Name : IF_InterfaceInit
 * Description   : 模拟接口初始化
 * Parameter     : 无
 *                                 
 * END ***************************************************************************************/
void IF_InterfaceInit(void)
{
	IF_SetNormalOutput_SIG(IOSIGN_INTLKENABLE,OFF); 			//关闭InterLock开关 	
	IF_SetNormalOutput_SIG(IOSIGN_PWR5VDRV,OFF); 		//开启风扇1开关 	
	IF_SetNormalOutput_SIG(IOSIGN_RFPWRLEDONOFF,OFF); 	//关闭风扇2开关  	
	IF_SetNormalOutput_SIG(IOSIGN_RFPWRLEDSTATUS,OFF);	//关闭射频输出灯状态	
	IF_SetNormalOutput_SIG(IOSIGN_DEBUGLEDONOFF,OFF);	//关闭调试灯输出状态
	IF_SetNormalOutput_SIG(IOSIGN_WARNLEDONOFF,OFF);		//关闭警告灯输出状态	
	IF_SetNormalOutput_SIG(IOSIGN_FAULTLEDONOFF,OFF);	//关闭故障灯输出状态
}	
/* FUNCTION *********************************************************************************
 * Function Name : IF_InterfaceInput
 * Description   : 模拟接口输入
 * Parameter     : 无
 *                                 
 * END ***************************************************************************************/
void IF_InterfaceInput(void)
{
	EXT_ADC_CtrlPin_Scan();
}

/* FUNCTION *********************************************************************************
 * Function Name : IF_SetNormalOutput_SIG
 * Description   : 开关接口电平输出
 * Parameter     : sWx 开关序号
* return		 : 输出状态                                
 * END ***************************************************************************************/
void IF_SetNormalOutput_SIG(IOSignEnum SIGx,uint8_t OnorOff)
{
    switch (SIGx)
    {
		case IOSIGN_INTLKENABLE:	IF_HAL_InterLockLed_Switch(OnorOff);	break;
		case IOSIGN_PWR5VDRV:		IF_HAL_DC5VOnEnable_Switch(OnorOff);	break;
		case IOSIGN_RFPWRLEDONOFF:	IF_HAL_RFOnOffLed_Switch(OnorOff);		break;
		case IOSIGN_RFPWRLEDSTATUS:	IF_HAL_PwrOnOffLed_Switch(OnorOff);		break;
		case IOSIGN_DEBUGLEDONOFF:	IF_HAL_PCBDebugLed_Switch(OnorOff);		break;
		case IOSIGN_WARNLEDONOFF:	IF_HAL_WarningLed_Switch(OnorOff);		break;
		case IOSIGN_FAULTLEDONOFF:	IF_HAL_FaultLed_Switch(OnorOff);		break;
		default:														break;
	}
}
/* FUNCTION *********************************************************************************
 * Function Name : IF_GetNormal_SEL_AdCtrl
 * Description   : Load电容ADC接口输出状态
 * Parameter     : 
 * return		 : 输出值                                
 * END ***************************************************************************************/
uint16_t IF_GetNormal_SEL_AdCtrl(uint8_t  chnl)
{
	uint16_t value = 0;
	switch(chnl)
	{
		case POWER_VALUE_CHNL:  value = ADC_Input.PowerValue;  				break;      //using CHANNEL 0:  Power_value
		case PLASMA_VDC_CHNL:   value = ADC_Input.PlasmVDC;  				break;      //using CHANNEL 1:  plasma_vdc
		case DCBIAS_VDC_CHNL:   value = ADC_Input.DCBias; 					break;      //using CHANNEL 2:  match_vdc			
		case TUNE_POS_CHNL:  	value = ADC_Input.tuneCapPosition;  		break;		//using CHANNEL 3:  tune_position
		case LOAD_POS_CHNL:  	value = ADC_Input.loadCapPosition;  		break;		//using CHANNEL 4:  load_position
		case DETEC50V_CHNL:		value = ADC_Input.ACDC50VDect; 				break;	    //using CHANNEL 5:  50v_detect
		case KEY_ENCODE_CHNL:   value = ADC_Input.Encode; 					break;      //using CHANNEL 6:  encode
		case RF_TEMP_CHNL:      value = ADC_Input.RFTemp; 					break;      //using CHANNEL 7:  temp_RF
	}
	return value;
}
/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/


//**********************
//Input Interface...
/* FUNCTION *********************************************************************************
 * Function Name : IO_Switch_Scan
 * Description   : IO Switch开关接口输出状态轮询
 * Parameter     : 
 * return		 :                              
 * END ***************************************************************************************/
static void IO_Switch_Scan(void)
{
    for(uint8_t i = IOSWITCH_PANEL1DECT; i < IOSWITCH_MAX; i++)
    {
        IO_Switch_Input[i].ioSwitchValueNow = IO_Switch_Input[i].ioSwitchScan();

        if(IO_Switch_Input[i].ioSwitchValueLast != IO_Switch_Input[i].ioSwitchValueNow)
        {
            IO_Switch_Input[i].ioSwitchValueLast = IO_Switch_Input[i].ioSwitchValueNow;
            IO_Switch_Input[i].filterCnt = 0;
        }
        else
        {
            IO_Switch_Input[i].filterCnt++;

            if(IO_Switch_Input[i].filterCnt >= IO_Switch_Input[i].filterTime)
            {
                IO_Switch_Input[i].ioSwitchValue = IO_Switch_Input[i].ioSwitchValueNow;
                IO_Switch_Input[i].filterCnt = IO_Switch_Input[i].filterTime;
            }
        }
    }
}

/* FUNCTION *********************************************************************************
 * Function Name : AD_CtrlPin_Scan
 * Description   : ADC Switch开关接口状态轮询
 * Parameter     : 
 * return		 :                              
 * END ***************************************************************************************/
static void EXT_ADC_CtrlPin_Scan(void)
{
	uint32_t ADCValue[EXT_ADC_CHNL_NUM] = {0};
	
	ADCValue[POWER_VALUE_CHNL] = IF_HAL_GetEXADCValue(1); 
	ADCValue[PLASMA_VDC_CHNL]  = IF_HAL_GetEXADCValue(2); 
	ADCValue[DCBIAS_VDC_CHNL]  = IF_HAL_GetEXADCValue(3); 
	ADCValue[TUNE_POS_CHNL]    = IF_HAL_GetEXADCValue(4); 
	ADCValue[LOAD_POS_CHNL]    = IF_HAL_GetEXADCValue(5); 
	ADCValue[DETEC50V_CHNL]    = IF_HAL_GetEXADCValue(6); 
	ADCValue[KEY_ENCODE_CHNL]  = IF_HAL_GetEXADCValue(7); 
	ADCValue[RF_TEMP_CHNL] 	   = IF_HAL_GetEXADCValue(0); 
	
    ADC_Input.PowerValue 	  =  ADC_RATE1*ADC_VALUE(ADCValue[POWER_VALUE_CHNL]);
    ADC_Input.PlasmVDC        =  ADC_RATE1*ADC_VALUE(ADCValue[PLASMA_VDC_CHNL]);
	ADC_Input.DCBias          =  ADC_RATE2*ADC_VALUE(ADCValue[DCBIAS_VDC_CHNL]);
	ADC_Input.tuneCapPosition =  ADC_RATE2*ADC_VALUE(ADCValue[TUNE_POS_CHNL]);
    ADC_Input.loadCapPosition =  ADC_RATE2*ADC_VALUE(ADCValue[LOAD_POS_CHNL]);
	ADC_Input.ACDC50VDect     =  ADC_RATE3*ADC_VALUE(ADCValue[DETEC50V_CHNL]);
	ADC_Input.Encode          =  ADC_VALUE(ADCValue[KEY_ENCODE_CHNL]);
	ADC_Input.RFTemp          =  ADC_VALUE(ADCValue[RF_TEMP_CHNL]);
}

//**********************Output Interface*************************************************/

//*****************************************************************************
//* END
//*****************************************************************************

