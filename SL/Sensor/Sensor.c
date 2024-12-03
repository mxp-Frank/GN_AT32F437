/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Sensor.c
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-09
 * ****************************************************************************
 */
#include "Sensor.h"
#include "FpgaSensor.h"
#include "DevSensor.h"
#include "IF_HAL_Cfg.h"
#include "IF_SL_Cfg.h"
/* CONST & MACROS */

/* DATA STRUCTURES */

/* LOCAL VARIABLES */


/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/

/**
 * ************************************************************************
 * @Brief :      
 * ************************************************************************
 */
void IF_SensorInit(void)
{
	IF_FpgaSensor_ParamInit();
    IF_HAL_Sensor_MCU_Start();
}
/* FUNCTION *********************************************************************************
 * Function Name : IF_Sensor_Fpga_Task
 * Description   : Sensor Fpga任务函数接口
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
void IF_SensorTask(void)
{
    Sensor_Fpga_Task();
}
/* FUNCTION *********************************************************************************
 * Function Name : IF_Sensor_Device_Task
 * Description   : Sensor设备任务函数接口
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
void IF_Sensor_Device_Task(void)
{
	 Sensor_Device_Task();
}

uint16_t IF_Get_Sensor_DCBias(void)
{
	uint16_t value = 0;
	uint16_t AttenFactor = IF_UserParam_GetVDCFactor();
	return value;
}

int32_t IF_Get_Sensor_ACInputVoltage(uint8_t Channel)
{
	int32_t value = 0;
	switch(Channel)
	{
		case 0:value = Modbus_InputReg[0]; break;
		case 1:value = Modbus_InputReg[10];break;
		case 2:value = Modbus_InputReg[8]; break;
		default:break;
	}		
	return value;
}

int32_t IF_Get_Sensor_ACInputCurrent(uint8_t Channel)
{
	int32_t value = 0;
	switch(Channel)
	{
		case 0:value = Modbus_InputReg[1]; break;
		case 1:value = Modbus_InputReg[11];break;
		case 2:value = Modbus_InputReg[9]; break;
		default:break;
	}		
	return value;
}

int32_t IF_Get_Sensor_PAVoltage(uint8_t Channel)
{
	int32_t value = 0;
	switch(Channel)
	{
		case 0:value = Modbus_InputReg[5]; break;
		case 1:value = Modbus_InputReg[7]; break;
		default:break;
	}	
	return value;
}
int32_t IF_Get_Sensor_PACurrent(uint8_t Channel)
{
	int32_t value = 0;
	switch(Channel)
	{
		case 0:value = Modbus_InputReg[4]; break;
		case 1:value = Modbus_InputReg[6]; break;
		case 2:value = Modbus_InputReg[2]; break;
		case 3:value = Modbus_InputReg[3]; break;
		default:break;
	}		
	return value;
}

int16_t IF_Get_Sensor_ACDCTemperature(uint8_t Channel)
{
	int16_t value = 0;
	switch(Channel)
	{
		case 0:value = Modbus_InputReg[11]; break;
		case 1:value = Modbus_InputReg[12]; break;
		default:break;
	}	
	return value;
}

/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/


//*****************************************************************************
//* END
//*****************************************************************************

