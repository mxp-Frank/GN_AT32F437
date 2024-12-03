/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  PT_CmdInput.h
 * @Brief :      
 * @Author :     mengxingping (mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2023-05-23
 * ****************************************************************************
 */
#ifndef _PT_MODULEINPUT_H_
#define _PT_MODULEINPUT_H_
#include "stdint.h"

extern void IF_PT_SensorInput_Task(void);
extern void IF_PT_CmdExecute_Task(void);
extern void IF_PT_KeyInput_Task(void);
extern void IF_PT_PortInput_Task(void);
#endif /* _PT_CMDINPUT_H_ */
