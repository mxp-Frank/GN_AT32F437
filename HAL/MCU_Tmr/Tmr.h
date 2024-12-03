/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Tmr.h--->internal use
 * @Brief :      from bsp_motor.h Created on: 2022年4月20日 Author: Huxian
 * @Author :     Chenghanbing (iwanthaha@sina.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#ifndef _TMR_H_
#define _TMR_H_

#include "IF_Board.h"
#include "IF_Gpio.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
	
#define DEBUG_ON

#define TMRPRD_1MS          28800       //1ms period with TMRCLK_28M clk
#define TMRPRD_25US         720         //25us period with TMRCLK_28M clk
#define TMRCLK_28M          28800000    //28.8MHz clock(288/28.8=10, now use!)

#define TMRPRD_5US         144

#define FAN_PWM_PORT			GPIOA			
#define FAN_PWM_PIN				Fan_PWM_Pin100_PA8
#define FAN_PWM_PIN_SOURCES		GPIO_PINS_SOURCE8
#define FAM_PWM_MUX				GPIO_MUX_1	


#define Fan1_FG_PORT			GPIOA			
#define Fan1_FG_PIN				Fan1_FG_RS_Pin99_PC9   //TMR8_CH4
#define Fan1_FG_PIN_SOURCES		GPIO_PINS_SOURCE8
#define Fan1_FG_MUX				GPIO_MUX_2	


#define Fan2_FG_PORT			GPIOA			
#define Fan2_FG_PIN				Fan2_FG_RS_Pin98_PC8    //TMR3_CH3
#define Fan2_FG_PIN_SOURCES		GPIO_PINS_SOURCE8
#define Fan2_FG_MUX				GPIO_MUX_2	


#define Fan3_FG_PORT			GPIOA			
#define Fan3_FG_PIN				Fan3_FG_RS_Pin97_PC7   //TMR3_CH2
#define Fan3_FG_PIN_SOURCES		GPIO_PINS_SOURCE8
#define Fan3_FG_MUX				GPIO_MUX_2	

#define Fan4_FG_PORT			GPIOA			
#define Fan4_FG_PIN				Fan4_FG_RS_Pin96_PC6   //TMR3_CH1
#define Fan4_FG_PIN_SOURCES		GPIO_PINS_SOURCE8
#define Fan4_FG_MUX				GPIO_MUX_2	




/*
//25us interrupt test result: change to use clk=28.8!!!--->TMR1,TMR8,TMR6 check pass!
//Note: #define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	0x05
//The priority of the timer interrupt must be higher than the interrupt level that FreeRTOS can manage
    18946	
    21492	2546
    23992	2500
    26493	2501
    28993	2500
    31494	2501
    33994	2500
    36495	2501
    38995	2500
    41495	2500
    43995	2500

*/


///////////////////////////////////////////////////////////////////////////////////////////////////
// 用于电机编码器计数的定时器的相关宏定义

#define START_VALUE                     0
#define MODULE_VALUE                    0xFFFF

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif


//*****************************************************************************
//* END
//*****************************************************************************

