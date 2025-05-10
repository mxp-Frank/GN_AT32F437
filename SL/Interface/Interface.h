/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Interface.h
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-10-09
 * ****************************************************************************
 */

#ifndef _INTERFACE_H_
#define _INTERFACE_H_
#include "stdint.h"
#include "IF_Interface.h"
typedef struct _IO_Switch_t
{
    IOSwitchEnum  ioSwitchID;
    uint8_t (*ioSwitchScan)(void);
    uint8_t filterTime;
    uint8_t filterCnt;
    uint8_t ioSwitchValue;
    uint8_t ioSwitchValueNow;
    uint8_t ioSwitchValueLast;    
}IO_Switch_t;

#endif


//*****************************************************************************
//* END
//*****************************************************************************

