/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Sensor_1G.c
 * @Brief :      
 * @Author :     Chenghanbing (iwanthaha@sina.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-21
 * ****************************************************************************
 */

/*
 * sample_app.c
 *
 *  Created on: 2022年4月19日
 *      Author: Huxian
 */

//#include "board.h"
#include "IF_Sensor.h"
#include "params_impl.h"
#include "IF_HAL_Cfg.h"

#include "Sensor_1G.h"

static Sample_t Sample;
static uint8_t channel = 0;
/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/
static void EX_HF_ADC_Sample_Once(void);

/* FUNCTION *********************************************************************************
 * Function Name : Sensor_1G_Task
 * Description   : 得到一代传感器数据任务
 * Parameter     : 无
 * return        : 无                                
 * END ***************************************************************************************/
void Sensor_1G_Task(void)
{
	EX_HF_ADC_Sample_Once();
}

/* FUNCTION *********************************************************************************
 * Function Name : Sensor_1G_RFOnChk
 * Description   : 得到一代传感器功率状态监测
 * Parameter     :   -----匹配器单元
 * return 	     : 匹配器功率状态                               
 * END ***************************************************************************************/
uint8_t Sensor_1G_RFOnChk(void)
{
    uint8_t ret = OFF;
	if ((Sample.PhaseErrAbs >= g_InternalParam.RFOnThrPhaseErr)||
		(Sample.MagErrAbs >= g_InternalParam.RFOnThrMagErr)	   || 
		(Sample.InputRFVoltage >= g_InternalParam.RFOnThrRFV)  || 
		(Sample.InputRFPower >= g_InternalParam.RFOnThrPower))
    {
        ret = ON;
    }
    return ret;
}
/* FUNCTION *********************************************************************************
 * Function Name : EX_HF_ADC_Sample_Once
 * Description   : 得到一代传感器的数据
 * Parameter     : 无
 * Return   	 :                            
 * END ***************************************************************************************/
static void EX_HF_ADC_Sample_Once(void)
{
	uint16_t adVolt1 = 0;
	uint16_t adVolt2 = 0;
	//同时转换2个ADC芯片的CHN0数据
	IF_HAL_MAGPHA_START();

	//获取2个ADC芯片的CHN0数据，并为下次转换CHN1做准备
	adVolt1 = IF_HAL_MAG_Read(CHN1);   //电压幅度采样
	adVolt2 = IF_HAL_PHA_Read(CHN1);   //电流幅值采样

	switch(channel)
	{
	case 0:
		Sample.Vpp = adVolt2;
		break;
	case 1:
		Sample.InputRFVoltage = adVolt1;
		Sample.InputRFCurrent = adVolt2;
		Sample.InputRFPower = (adVolt1  + adVolt2) / 2;

		break;
	case 2:
		 Sample.DCBias = adVolt2;
		break;
	default:
		break;
	}

	//切换通道, 为下次转换做准备
	channel++;
	if(channel >= 3)
	{
		channel = 0;
	}
	IF_HAL_MAGPHA_SEL(channel);

	//开始转换2个ADC芯片的CHN1数据
	IF_HAL_MAGPHA_START();

	//获取2个ADC芯片的CHN1数据，并为下次转换CHN0做准备
	Sample.MagErrAbs = IF_HAL_MAG_Read(CHN0);   	//幅度差采样
	Sample.PhaseErrAbs = IF_HAL_PHA_Read(CHN0);   	//相位差采样

	Sample.MagErr = (IF_HAL_GETMAGSIGN() == 0) ? (-Sample.MagErrAbs) : (Sample.MagErrAbs);
	Sample.PhaseErr = (IF_HAL_GETPHASIGN() == 0) ? (-Sample.PhaseErrAbs) : (Sample.PhaseErrAbs);
}
/********************************一代匹配器采样数据********************************************/
int16_t Get_Sensor_1G_PhaseErr(void)
{
	int16_t value = 0;
	value = Sample.PhaseErr;
    return value ; 
}

uint16_t Get_Sensor_1G_PhaseErrAbs(void)
{
	uint16_t value = 0; 
	value = Sample.PhaseErrAbs;
    return value; 
}

int16_t Get_Sensor_1G_MagErr(void)
{
	int16_t value = 0;
	value = Sample.MagErr;
    return value;
}

uint16_t Get_Sensor_1G_MagErrAbs(void)
{
	uint16_t value = 0;
	value = Sample.MagErrAbs;
    return value;
}

uint16_t Get_Sensor_1G_InputRFPower(void)
{
	uint16_t value = 0;
	value = Sample.InputRFPower;
    return value; 
}

uint16_t Get_Sensor_1G_InputRFCurrent(void)
{
	uint16_t value = 0;
	value = g_InternalParam.CurrentAtten * Sample.InputRFCurrent;
	return value;
}
uint16_t Get_Sensor_1G_InputRFVoltage(void)
{
	uint16_t value = 0;
	value = g_InternalParam.RFProbeAtten *Sample.InputRFVoltage;
	return value; 
}
uint16_t Get_Sensor_1G_DCBias(void)
{
	uint16_t value = 0;
	value = g_InternalParam.DCBiasAtten *Sample.DCBias;
    return value; 
}
uint16_t Get_Sensor_1G_Vpp(void)
{
	uint16_t value = 0;
	value = g_InternalParam.VppAtten * Sample.Vpp / 1000 + 60;
    return value; 
}
//*****************************************************************************
//* END
//*****************************************************************************

