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
//Adc switch threshold value: min to max

#define ADC_VALUE(x)   			(x*EXT_ADC_VREF/EXT_ADC_VRES)

#define ADC_RATE1				3.0F
#define ADC_RATE2				4.72F
#define ADC_RATE3               23.0F	//((220+10)/10)
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


typedef struct _AD_Ctrl_Input_t
{
   uint32_t PowerValue;
   uint32_t PlasmVDC;
   uint32_t DCBias;
   uint32_t tuneCapPosition;
   uint32_t loadCapPosition;
   uint32_t ACDC50VDect;
   uint32_t Encode;
   uint32_t RFTemp;
 
}AD_Ctrl_Input_t;

#endif


//*****************************************************************************
//* END
//*****************************************************************************

