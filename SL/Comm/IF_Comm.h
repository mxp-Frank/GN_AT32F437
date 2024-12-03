/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_Comm.h
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-12
 * ****************************************************************************
 */
 
#ifndef _IF_COMM_H_
#define _IF_COMM_H_

#include "Common.h"
#include "fgipv2.h"
#include "fgipv2_layer2.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */


typedef enum
{
  USER_UART     = 0, 
  DEBUG_UART    = 1,
  FPGA_UART     = 2, 
  RS485_UART    = 3,
  PRINT_UART    = 4, 
  TOTAL_UART    = 5
}UART_TypeDef;

typedef enum
{   //写入NVM参数
	CMD_SAVE_FACTORYPARAM 		=0x01,
	CMD_SAVE_FSCOMMONPARAM 		=0x02,
	CMD_SAVE_FSINTERNALPARAM 	=0x03,
	CMD_SAVE_FSUSERPARAM 		=0x04,
	
	CMD_SAVE_COMMONPARAM 		=0x05,
	CMD_SAVE_INTERNALPARAM	    =0x06,
	CMD_SAVE_USERPARAM		    =0x07,
	CMD_SAVE_CONFIGPARAM		=0x08,
	//读取NVM参数
	CMD_READ_FACTORYPARAM 		=0x10,
	CMD_READ_FSCOMMONPARAM 		=0x11,
	CMD_READ_FSINTERNALPARAM 	=0x12,
	CMD_READ_FSUSERPARAM 		=0x13,
	
	CMD_READ_COMMONPARAM 		=0x14,
	CMD_READ_INTERNALPARAM	    =0x15,
	CMD_READ_USERPARAM		    =0x16,
	
	CMD_RESUME_ALLPARAM         =0x17,
	CMD_READ_CONFIGPARAM        =0x18,

	//控制匹配器
	CMD_MOVETUNE				=0x80,
	CMD_MOVELOAD				=0x81,
	CMD_MOVEAUXI				=0x82,
	CMD_MATCHWORKMODE			=0x83,
	
	//设置其他
	CMD_SETBAUDRATE				=0xF0,
	CMD_DEVICERESET             =0xF1
}APP_CmdTypeDef;

typedef union
{
	uint32_t Val;
	uint8_t v[4];
	uint16_t w[2];
	struct
	{
		uint8_t LB;
		uint8_t HB;	
		uint8_t UB;
		uint8_t MB;
	}byte;
}cmdData_t;

typedef struct _DeviceCmdMsg_t
{
	APP_CmdTypeDef cmdId;               //APP需要的设备命令
	cmdData_t cmdData;    				//设备命令数据内容
}DeviceCmdMsg_t, *pDeviceCmdMsg_t;

extern uint8_t FpgaUpdate_NoAck;
extern QueueHandle_t UserQueue;
extern QueueHandle_t FGIPv2Queue;
extern QueueHandle_t BsipTxQueue;
extern QueueHandle_t FpgaQueue;
extern QueueHandle_t ModbusQueue;
extern QueueHandle_t CmdQueue;

extern SemaphoreHandle_t FpgaSemaphore;
extern SemaphoreHandle_t ModbusSemaphore;
/******************************************************************************
* Interface functions                                                         *
******************************************************************************/
extern void IF_CommInit(void);

extern void IF_UartTask1(void);
extern void IF_UartTask2(void);
extern void IF_UartTask3(void);
extern void IF_UartTask4(void);
extern uint8_t SendToRxQueueFromISR(uint8_t port, pCommMsg_t pRxMsg);
#if defined(__cplusplus)
 }
#endif /* __cplusplus */

#endif

 /*
*******************************************************************************
* END
*******************************************************************************
*/

