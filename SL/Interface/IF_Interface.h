/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_Interface.h
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-10-09
 * ****************************************************************************
 */
 
#ifndef _IF_INTERFACE_H_
#define _IF_INTERFACE_H_

#include "IF_Board.h"

#define STATUS_MANUAL                  0
#define STATUS_AUTO                    1

#define ANALOG_LEVEL				  (20)    			//模拟接口电压分辨率
/*****************************************************
接口增益							5
******************************************************/
#define EXDAC_ATTEN			  	     (5)     			//模拟接口DAC增益

typedef enum
{
	IOSIGN_INTLKENABLE				 = 0,	//interlock switch output
	//ACDC OUTPUT SIGN
	IOSIGN_PWR5VDRV             	 = 1,   //rfpower drain enable switch output
    IOSIGN_RFPWRLEDONOFF      	 	 = 2,   //rfpower enable switch output
    IOSIGN_RFPWRLEDSTATUS            = 3,   //rfpower state switch output

    //DEVICE OUTPUT SIGN	
	IOSIGN_DEBUGLEDONOFF      		 = 4,   //degug led  enable output
	IOSIGN_FAULTLEDONOFF			 = 5,   //Fault led  enable output
	IOSIGN_WARNLEDONOFF				 = 6,   //warn led  enable output
	//Record array len
	IOSIGN_OUTPUTMAX				 = 7,
}IOSignEnum;

/******************************************************************************
* Interface functions                                                         *
******************************************************************************/
extern void IF_InterfaceInit(void);
extern void IF_InterfaceInput(void);
extern void IF_SetNormalOutput_SIG(IOSignEnum SIGx,uint8_t OnorOff);
extern uint16_t IF_GetNormal_SEL_AdCtrl(uint8_t  chnl);

#endif

 /*
*******************************************************************************
* END
*******************************************************************************
*/

