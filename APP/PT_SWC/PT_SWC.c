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
static PID_Typedef vPID;
/* GLOBAL VARIABLES */

GN_Device_t Device;
/* FUNCTION PROTOTYPES */
static uint16_t PT_Module_PowerMonitor(void);
/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/
 /* FUNCTION *********************************************************************************
 * Function  : IF_PT_SWCInit
 * Description   : 匹配器设置初始化接口
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
void IF_PT_SWCInit(void)
{

    //Global Init...
	memset(&Device, 0, sizeof(GN_Device_t));
}
/* FUNCTION *********************************************************************************
 * Function  : IF_PT_Main_Task
 * Description   : 匹配器输入任务函数接口
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
void IF_PT_Main_Task(void)
{
	PT_Module_PowerMonitor();
}
/* FUNCTION *********************************************************************************
 * Function  : PT_Module_PowerMonitor
 * Description   : 功率监测限制，判射频输出限制状态
 * Parameter     : SettingPower ---->设置功率
 * return        :                
 * END ***************************************************************************************/
static uint16_t PT_Module_PowerMonitor(void)
{
	uint16_t value = 0;
	return value;
}

//*****************************************************************************
//* END
//*****************************************************************************

