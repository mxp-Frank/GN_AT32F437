/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Uart.h--->internal use
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#ifndef _UART_H_
#define _UART_H_

#include "IF_Board.h"
#include "IF_Gpio.h"

//COM1  PORT DEFINE
#define   COM1_TX_PORT					GPIOD
#define   COM1_TX_PIN					UART3_TX_Pin77_PD8	
#define   COM1_TX_PINS_SOURCE			GPIO_PINS_SOURCE8
#define   COM1_TX_MUX					GPIO_MUX_7

#define   COM1_RX_PORT					GPIOD
#define   COM1_RX_PIN					UART3_RX_Pin78_PD9	
#define   COM1_RX_PINS_SOURCE			GPIO_PINS_SOURCE9
#define   COM1_RX_MUX					GPIO_MUX_7
/*UART instance and clock*/
#define   COM1_UART                     USART3
#define   COM1_UART_CLK                 CRM_USART3_PERIPH_CLOCK
#define   COM1_UART_IRQn                USART3_IRQn
#define   COM1_UART_BAUD                115200
#define   COM1_UART_IRQHandler          USART3_IRQHandler


//COM2  PORT DEFINE
#define   COM2_TX_PORT					GPIOD
#define   COM2_TX_PIN					UART2_TX_Pin119_PD5	
#define   COM2_TX_PINS_SOURCE			GPIO_PINS_SOURCE5
#define   COM2_TX_MUX					GPIO_MUX_7

#define   COM2_RX_PORT					GPIOD
#define   COM2_RX_PIN					UART2_RX_Pin122_PD6	
#define   COM2_RX_PINS_SOURCE			GPIO_PINS_SOURCE6
#define   COM2_RX_MUX					GPIO_MUX_7
/*UART instance and clock*/
#define   COM2_UART                     USART2
#define   COM2_UART_CLK                 CRM_USART2_PERIPH_CLOCK
#define   COM2_UART_IRQn                USART2_IRQn
#define   COM2_UART_BAUD                38400
#define   COM2_UART_IRQHandler          USART2_IRQHandler

//COM3  PORT DEFINE
#define   COM3_TX_PORT					GPIOB
#define   COM3_TX_PIN					UART5_RX_Pin135_PB5	
#define   COM3_TX_PINS_SOURCE			GPIO_PINS_SOURCE5
#define   COM3_TX_MUX					GPIO_MUX_0

#define   COM3_RX_PORT					GPIOB
#define   COM3_RX_PIN					UART5_TX_Pin136_PB6	
#define   COM3_RX_PINS_SOURCE			GPIO_PINS_SOURCE6
#define   COM3_RX_MUX					GPIO_MUX_0

/*UART instance and clock*/
#define   COM3_UART                     UART5 //@attention: ! not USART6
#define   COM3_UART_CLK                 CRM_UART5_PERIPH_CLOCK
#define   COM3_UART_IRQn                UART5_IRQn
#define   COM3_UART_BAUD                115200
#define   COM3_UART_IRQHandler          UART5_IRQHandler

//COM4  PORT DEFINE
#define   COM4_TX_PORT					GPIOF
#define   COM4_TX_PIN					UART7_TX_Pin19_PF7	
#define   COM4_TX_PINS_SOURCE			GPIO_PINS_SOURCE6
#define   COM4_TX_MUX					GPIO_MUX_8

#define   COM4_RX_PORT					GPIOF
#define   COM4_RX_PIN					UART7_RX_Pin18_PF6	
#define   COM4_RX_PINS_SOURCE			GPIO_PINS_SOURCE7
#define   COM4_RX_MUX					GPIO_MUX_8

#define  COM4_EN_PORT					GPIOD
#define  COM4_EN_PIN					RS485_EN_Pin82_PD13
/*UART instance and clock*/
#define   COM4_UART                     UART7 //@attention: ! not USART6
#define   COM4_UART_CLK                 CRM_UART7_PERIPH_CLOCK
#define   COM4_UART_IRQn                UART7_IRQn
#define   COM4_UART_BAUD                115200
#define   COM4_UART_IRQHandler          UART7_IRQHandler


//COM5  PORT DEFINE
#define   COM5_TX_PORT					GPIOA
#define   COM5_TX_PIN					UART1_TX_Pin101_PA9	
#define   COM5_TX_PINS_SOURCE			GPIO_PINS_SOURCE9
#define   COM5_TX_MUX					GPIO_MUX_7

#define   COM5_RX_PORT					GPIOA
#define   COM5_RX_PIN					UART1_RX_Pin102_PA10	
#define   COM5_RX_PINS_SOURCE			GPIO_PINS_SOURCE10
#define   COM5_RX_MUX					GPIO_MUX_7
/*UART instance and clock*/
#define   COM5_UART                     USART1
#define   COM5_UART_CLK                 CRM_USART1_PERIPH_CLOCK
#define   COM5_UART_IRQn                USART1_IRQn
#define   COM5_UART_BAUD                38400
#define   COM5_UART_IRQHandler          USART1_IRQHandler

#endif


//*****************************************************************************
//* END
//*****************************************************************************

