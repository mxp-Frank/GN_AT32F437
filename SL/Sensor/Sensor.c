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

uint16_t IF_Sensor_GetDCBias(void)
{
	uint16_t value = 0;
	uint16_t AttenFactor = IF_UserParam_GetVDCFactor();
	return value;
}

/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/


//*****************************************************************************
//* END
//*****************************************************************************

