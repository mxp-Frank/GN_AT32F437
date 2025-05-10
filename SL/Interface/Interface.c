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
static IO_Switch_t IO_Switch_Input[IOSWITCH_MAX] = 
{  
	/******************InterLock Input*************************/
    {
        .ioSwitchID = IOSWITCH_INTLKDECT,
        .ioSwitchScan = IF_HAL_InterLockDect_SigRead,
        .filterTime = 15,
        .filterCnt = 15,
        .ioSwitchValue = 0,
        .ioSwitchValueNow = 0,
        .ioSwitchValueLast = 0,
    },	
};

/* LOCAL VARIABLES */

/* GLOBAL VARIABLES */

/* FUNCTION PROTOTYPES */
static void IO_Switch_Scan(void);
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
	IF_SetNormalOutput_SIG(IOSIGN_INTLKLEDONOFF,OFF); 	//关闭InterLock输出灯状态 		
	IF_SetNormalOutput_SIG(IOSIGN_RFONLEDONOFF,OFF);	//关闭射频输出灯状态
	IF_SetNormalOutput_SIG(IOSIGN_RFPWRLEDONOFF,OFF);	//关闭电源输出灯状态
	IF_SetNormalOutput_SIG(IOSIGN_DEBUGLEDONOFF,OFF);	//关闭调试灯输出状态
	IF_SetNormalOutput_SIG(IOSIGN_WARNLEDONOFF,OFF);	//关闭警告灯输出状态	
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
	IO_Switch_Scan();
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
		case IOSIGN_PWR5VDRVONOFF:	IF_HAL_DC5VOnEnable_Switch(OnorOff);	break;
		case IOSIGN_PWR12VDRVONOFF:	IF_HAL_Power12VOn_Switch(OnorOff);  	break;
		case IOSIGN_INTLOCKENABLE:	IF_HAL_IntLock_Enable_Switch(OnorOff);   break;
		case IOSIGN_INTLKLEDONOFF:	IF_HAL_InterLockLed_Switch(OnorOff);	break;
		case IOSIGN_RFONLEDONOFF:	IF_HAL_RFOnOffLed_Switch(OnorOff);		break;
		case IOSIGN_RFPWRLEDONOFF:	IF_HAL_PwrOnOffLed_Switch(OnorOff);		break;
		case IOSIGN_DEBUGLEDONOFF:	IF_HAL_PCBDebugLed_Switch(OnorOff);		break;
		case IOSIGN_WARNLEDONOFF:	IF_HAL_WarningLed_Switch(OnorOff);		break;
		case IOSIGN_FAULTLEDONOFF:	IF_HAL_FaultLed_Switch(OnorOff);		break;
		default:break;
	}	
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

//*****************************************************************************
//* END
//*****************************************************************************

