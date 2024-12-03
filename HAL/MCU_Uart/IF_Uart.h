/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_Uart.h--->external use
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#ifndef _IF_UART_H_
#define _IF_UART_H_

#include "IF_PrjLib.h"
#include "uart.h"
#define DEBUG_Print(format,argent...)  printf(""format"",##argent)
//if not use, comment printf...
//#define DEBUG_Print(format,argent...)  //printf(""format"",##argent)

typedef enum
{
  COM1 = 0,
  COM2 = 1,
  COM3 = 2,
  COM4 = 3,
  COM5 = 4,
  COM6 = 5,
  COMn = 6
}COM_TypeDef;

typedef enum
{
  BAUD9600 = 9600,
  BAUD19200 = 19200,
  BAUD38400 = 38400,
  BAUD57600 = 57600,
  BAUD115200 = 115200
}UARTBAUD_TypeDef;

#define RS485_RX_EN()		IF_GpioOutPut(COM4_EN_PORT,COM4_EN_PIN,0,LOGIC_POUT);
#define RS485_TX_EN()		IF_GpioOutPut(COM4_EN_PORT,COM4_EN_PIN,1,LOGIC_POUT);
/******************************************************************************
 * Interface functions                                                         *
 ******************************************************************************/
extern void IF_UartInit(void);
extern void IF_UartSetBaudRate(uint8_t port, UARTBAUD_TypeDef baudrate);
extern void IF_UartEnableTx(uint8_t port);
extern void IF_UartDisableTx(uint8_t port);
extern void IF_UartSendByte(uint8_t port, uint8_t byte);
extern uint8_t IF_UartGetFlag(uint8_t port, uint32_t flag);
extern void IF_UartClearFlag(uint8_t port, uint32_t flag);
extern uint8_t IF_UartReadByte(uint8_t port);
extern UARTBAUD_TypeDef IF_Uart_GetBaudRateValue(uint8_t dataRateKey);
extern uint8_t IF_Uart_GetBaudRateKey(UARTBAUD_TypeDef baudRateValue);
#endif

/*
*******************************************************************************
* END
*******************************************************************************
*/

