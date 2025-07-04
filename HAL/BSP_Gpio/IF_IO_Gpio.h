/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_IO_Gpio.h
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-10-07
 * ****************************************************************************
 */

#ifndef _IF_IO_GPIO_H_
#define _IF_IO_GPIO_H_

#include "IF_PrjLib.h"


/******************************************************************************
 * Interface functions                                                         *
 ******************************************************************************/
extern void IF_IOGpioInit(void);

extern void IF_PCBDebugLed_Switch(uint8_t para);
extern void IF_Power12VOn_Switch(uint8_t para);
extern void IF_DC5VOn_Switch(uint8_t para);
extern void IF_IntLock_En_Switch(uint8_t para);					
extern void IF_PowerOnOffLed_Switch(uint8_t para);
extern void IF_FaultLed_Switch(uint8_t para);
extern void IF_WarningLed_Switch(uint8_t para);
extern void IF_IntLockLed_Switch(uint8_t para);
extern void IF_RFOnOffLed_Switch(uint8_t para);
extern uint8_t IF_InterLockDect_SigRead(void);
extern uint8_t IF_InterLockEn_SigRead(void);

#endif

/*
*******************************************************************************
* END
*******************************************************************************
*/

