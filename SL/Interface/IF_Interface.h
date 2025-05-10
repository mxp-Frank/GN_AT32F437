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
	//
	IOSWITCH_PANEL1DECT				 = 0,
	IOSWITCH_PANEL2DECT				 = 1,
    IOSWITCH_INTLKDECT   			 = 2,  
    
	IOSWITCH_RFLOADPOWER             = 3,   
    IOSWITCH_RFPOWERDCBIAS      	 = 4,   
    IOSWITCH_RFPOWERONDECT           = 5,   
	
    IOSWITCH_TUNEDDECT		         = 6,   
    IOSWITCH_MOTORING      		 	 = 7,   
    
	IOSWITCH_OVERVOLALARM            = 8,   
    IOSWITCH_OVERCURALARM            = 9,   
	IOSWITCH_UNDERVOLALARM           = 10,   
    IOSWITCH_OVERTEMPALARM           = 11,   
	
    //Record array len
    IOSWITCH_MAX                	 = 12

}IOSwitchEnum;

typedef enum
{	
	 //DEVICE OUTPUT SIGN
	IOSIGN_PWR5VDRVONOFF             = 0,   //rfpower drain enable switch output
	IOSIGN_PWR12VDRVONOFF			 = 1,   //rfpower drain enable switch output
	IOSIGN_INTLOCKENABLE			 = 2,	//interlock switch output
	
	//LED OUTPUT SIGN
	IOSIGN_INTLKLEDONOFF		     = 3,	//interlock led output
    IOSIGN_RFONLEDONOFF      	 	 = 4,   //rfstate led switch output
    IOSIGN_RFPWRLEDONOFF             = 5,   //rfpower led switch output

	IOSIGN_DEBUGLEDONOFF      		 = 6,   //degug led  enable output
	IOSIGN_FAULTLEDONOFF			 = 7,   //Fault led  enable output
	IOSIGN_WARNLEDONOFF				 = 8,   //warn led  enable output

	IOSIGN_OUTPUTMAX				 = 9,
}IOSignEnum;
typedef enum
{
     
    IOSIG_PWR12VON             	= 1, 
	IOSIG_DC5VON        		= 2,   	
    IOSIG_INTLOCKEN        		= 3,   
    IOSIG_PWRONLED				= 4, 
	IOSIG_FAULTLED				= 5, 
	IOSIG_WARNINGLED			= 6, 
	IOSIG_INTLOCKLED		    = 7, 
	IOSIG_RFONLED		    	= 8, 
	//Record array len
    IOSIG_MAX            		= 9
}IOSWEnum;
/******************************************************************************
* Interface functions                                                         *
******************************************************************************/
extern void IF_InterfaceInit(void);
extern void IF_InterfaceInput(void);
extern void IF_SetNormalOutput_SIG(IOSignEnum SIGx,uint8_t OnorOff);
#endif

 /*
*******************************************************************************
* END
*******************************************************************************
*/

