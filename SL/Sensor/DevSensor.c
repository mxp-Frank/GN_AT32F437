/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Sensor_1G.c
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-21
 * ****************************************************************************
 */
#include "DevSensor.h"
#include "IF_HAL_Cfg.h"
#include "IF_SL_Cfg.h"

/* CONST & MACROS */

/* DATA STRUCTURES */
static AD_Ctrl_Input_t ADC_Input = 
{
	.RFTemperature1 = 0,
	.RFTemperature2 = 0,
	.RFTemperature3 = 0,
	.RFTemperature4 = 0,
	.loadCapPosition = 0,
	.ACDC50VDect = 0,
	.Encode = 0,
	.RFTemp = 0,		
};
/* LOCAL VARIABLES */

/* GLOBAL VARIABLES */

/* FUNCTION PROTOTYPES */
static void EXT_ADC_CtrlChannel_Scan(void);
static uint16_t IF_GetNormalAdc__GetVoltage(uint8_t  chnl);
//*************************************************************************
//* Global Functions Definitions                                          
//*************************************************************************
int16_t IF_Sensor_GetRFTemperature(uint8_t Channel)
{
	int16_t temp = 0;
	uint16_t adVolt = IF_GetNormalAdc__GetVoltage(Channel);
	uint16_t RValue=(TEMP_RATE*adVolt-TEMP_VOLT*TEMP_OFFSET)/(TEMP_VOLT-adVolt);
	for(temp = 0;temp < 171;temp ++)
	{
		if(RValue >= NTCtemperature[temp])break;
	}	
	return (temp-20);
}
int16_t IF_Sensor_GetPCBTemperature(void)
{
	uint16_t adVolt = 0;
	adVolt = IF_HAL_Temperature_ReadAdc()*ADC_VREF/ADC_VRES;
	adVolt = adVolt - 600;
	return adVolt;
}
uint16_t IF_Sensor_GetPowerSupply(void)
{
	uint16_t adVolt = 0;
	adVolt = IF_HAL_Power_ReadAdc()*ADC_VREF/ADC_VRES;
	return (adVolt * 11);
}
/* FUNCTION *********************************************************************************
 * Function Name : Sensor_Device_Sample
 * Description   : Sensor设备任务函数
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
void Sensor_Device_Sample(void)
{	
	EXT_ADC_CtrlChannel_Scan();		
	/******获取射频电源状态*****************/
	if(IF_HAL_InterLockDect_SigRead())
	{
		g_FaultWord.bits.InterlockOpen = 1;
	}else
	{
		g_FaultWord.bits.InterlockOpen = 0;
	}
	uint16_t Power24Volt = IF_Sensor_GetPowerSupply();
    if ( (Power24Volt > POWER_24V_MAX)||(Power24Volt < POWER_24V_MIN))
    {        
         g_FaultWord.bits.Power24VAlarm = 1;   
    }
    else 
    {
        g_FaultWord.bits.Power24VAlarm = 0;
    }
	
    if ( IF_Sensor_GetPCBTemperature()> TEMP_MAX)
    {      	
        g_FaultWord.bits.OverPCBTempAlarm = 1;        
    }
    else 
    {						
        g_FaultWord.bits.OverPCBTempAlarm = 0;
    }
	if(IF_Fpga_GetMcuAlgSensor(ChnN_Temp)> TEMP_MAX)
	{
		 g_FaultWord.bits.OverFpgaTempAlarm = 1;     
	}else
	{
		 g_FaultWord.bits.OverFpgaTempAlarm = 0;   
	}
	if( IF_Sensor_GetRFTemperature(ADC_CHNL1)>TEMP_MAX||
		IF_Sensor_GetRFTemperature(ADC_CHNL2)>TEMP_MAX||
		IF_Sensor_GetRFTemperature(ADC_CHNL3)>TEMP_MAX||
		IF_Sensor_GetRFTemperature(ADC_CHNL4)>TEMP_MAX)
	{
		g_FaultWord.bits.OverRFTempAlarm = 1;
	}else
	{
		g_FaultWord.bits.OverRFTempAlarm = 0;
	}
}

/* FUNCTION *********************************************************************************
 * Function Name : IF_GetNormalAdc__GetVoltage
 * Description   : Load电容ADC接口输出状态
 * Parameter     : 
 * return		 : 输出值                                
 * END ***************************************************************************************/
static uint16_t IF_GetNormalAdc__GetVoltage(uint8_t  chnl)
{
	uint16_t value = 0;
	switch(chnl)
	{
		case ADC_CHNL1:  	value = ADC_Input.RFTemperature1;  			break;      //using CHANNEL 0:  RFTemperature1
		case ADC_CHNL2:   	value = ADC_Input.RFTemperature2;  			break;      //using CHANNEL 1:  RFTemperature2
		case ADC_CHNL3:   	value = ADC_Input.RFTemperature3; 			break;      //using CHANNEL 2:  RFTemperature3			
		case ADC_CHNL4:  	value = ADC_Input.RFTemperature4;  			break;		//using CHANNEL 3:  RFTemperature4
	}
	return value;
}
/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/
/* FUNCTION *********************************************************************************
 * Function Name : AD_CtrlPin_Scan
 * Description   : ADC Switch开关接口状态轮询
 * Parameter     : 
 * return		 :                              
 * END ***************************************************************************************/
static void EXT_ADC_CtrlChannel_Scan(void)
{
	uint32_t ADCValue[EXT_ADC_CHNL_NUM] = {0};
	
	ADCValue[ADC_CHNL1]    = IF_HAL_GetEXADCValue(1); 
	ADCValue[ADC_CHNL2]    = IF_HAL_GetEXADCValue(2); 
	ADCValue[ADC_CHNL3]    = IF_HAL_GetEXADCValue(3); 
	ADCValue[ADC_CHNL4]    = IF_HAL_GetEXADCValue(4); 
	ADCValue[ADC_CHNL5]    = IF_HAL_GetEXADCValue(5); 
	ADCValue[ADC_CHNL6]    = IF_HAL_GetEXADCValue(6); 
	ADCValue[ADC_CHNL7]    = IF_HAL_GetEXADCValue(7); 
	ADCValue[ADC_CHNL8]    = IF_HAL_GetEXADCValue(0); 
	
    ADC_Input.RFTemperature1  =  ADC_VOLT(ADCValue[ADC_CHNL1]);
    ADC_Input.RFTemperature2  =  ADC_VOLT(ADCValue[ADC_CHNL2]);
	ADC_Input.RFTemperature3  =  ADC_VOLT(ADCValue[ADC_CHNL3]);
	ADC_Input.RFTemperature4  =  ADC_VOLT(ADCValue[ADC_CHNL4]);

}
