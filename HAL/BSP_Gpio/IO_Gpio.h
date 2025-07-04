/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IO_Gpio.h
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-10-07
 * ****************************************************************************
 */

#ifndef _IO_GPIO_H_
#define _IO_GPIO_H_

#include "IF_Board.h"
#include "IF_Gpio.h"
#define 	PCBDebug_LED_GPIO					GPIOB
#define 	PCBDebug_LED_PIN					LED_Pin76_PB15
												
#define 	PW12V_ON_GPIO						GPIOH
#define 	PW12V_ON_PIN						PW12V_ON_Pin71_PH3

#define 	DC5V_ON_GPIO						GPIOF
#define 	DC5V_ON_PIN							DC5V_ON_Pin55_PF15

#define 	IntLock_Detect_GPIO					GPIOG
#define 	IntLock_Detect_PIN					IntLock_Detect_Pin88_PG3

#define 	IntLock_EN_GPIO						GPIOD
#define 	IntLock_EN_PIN						IntLock_EN_Pin116_PD2

#define 	PWR_ON_LED_GPIO						GPIOG
#define 	PWR_ON_LED_PIN						PWR_ON_LED_Pin93_PG8

#define 	Fault_LED_GPIO						GPIOG
#define 	Fault_LED_PIN						Fault_LED_Pin89_PG4

#define 	Warning_LED_GPIO					GPIOG
#define 	Warning_LED_PIN						Warning_LED_Pin90_PG5

#define 	IntLock_LED_GPIO					GPIOG
#define 	IntLock_LED_PIN						IntLock_LED_Pin91_PG6

#define 	RF_ON_LED_GPIO						GPIOG
#define 	RF_ON_LED_PIN						RF_ON_LED_Pin92_PG7

#endif


//*****************************************************************************
//* END
//*****************************************************************************

