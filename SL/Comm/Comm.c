/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Comm.c
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-12
 * ****************************************************************************
 */
#include "Comm.h"
#include "IF_HAL_Cfg.h"
#include "IF_SL_Cfg.h"
/* CONST & MACROS */

/* DATA STRUCTURES */

/* LOCAL VARIABLES */
/* GLOBAL VARIABLES */ 

SemaphoreHandle_t ModbusReSemaphore = NULL;   //重复发送
SemaphoreHandle_t ModbusNfSemaphore = NULL;  //发送成功通知
SemaphoreHandle_t FpgaReSemaphore =NULL;    //重复发送
SemaphoreHandle_t FpgaNfSemaphore =NULL;    //发送成功通知
QueueHandle_t UserQueue = NULL;
QueueHandle_t FGIPv2Queue = NULL;
QueueHandle_t BsipTxQueue = NULL;
QueueHandle_t FpgaFWQueue =NULL;

QueueHandle_t ModbusRxQueue =NULL;
QueueHandle_t ModbusFWQueue =NULL;

QueueHandle_t CmdQueue = NULL;

/* FUNCTION PROTOTYPES */

/************************************************************************/
/* ISR Callback Functions Definitions                                          */
/************************************************************************/
/* FUNCTION *********************************************************************************
 * Function Name : USER_UART_Callback
 * Description   : 用户接口(User_Uart)回调函数
 * Parameter     : 
 * return        :               
 * END ***************************************************************************************/
void USER_UART_Callback(void)
{
    uint8_t rxChar;
    
    if(IF_HAL_UartGetFlag_RDBF(USER_UART) != RESET)
    {
        rxChar = IF_HAL_UartReadByte(USER_UART);
		ReceiveChar(USER_UART, rxChar);
		IF_HAL_UartClearFlag_RDBF(USER_UART);		
    }

    if(IF_HAL_UartGetFlag_TDBE(USER_UART) != RESET)
    {
		TransmitChar(USER_UART);
		IF_HAL_UartClearFlag_TDBE(USER_UART);	
    }
}

/* FUNCTION *********************************************************************************
 * Function Name : DEBUG_UART_Callback
 * Description   : 调试接口(Debug_Uart)回调函数
 * Parameter     : 
 * return        :               
 * END ***************************************************************************************/
void DEBUG_UART_Callback(void)
{
    uint8_t rxChar;
      
    if(IF_HAL_UartGetFlag_RDBF(DEBUG_UART) != RESET)
    {
        rxChar = IF_HAL_UartReadByte(DEBUG_UART);
		ReceiveChar(DEBUG_UART, rxChar);
		IF_HAL_UartClearFlag_RDBF(DEBUG_UART);		
    }

    if(IF_HAL_UartGetFlag_TDBE(DEBUG_UART) != RESET)
    {
		TransmitChar(DEBUG_UART);
		IF_HAL_UartClearFlag_TDBE(DEBUG_UART);	
    }
}
/* FUNCTION *********************************************************************************
 * Function Name : FPGA_UART_Callback
 * Description   : FPGA接口(FPGA_Uart)回调函数
 * Parameter     : 
 * return        :               
 * END ***************************************************************************************/
void FPGA_UART_Callback(void)
{
     
    if(IF_HAL_UartGetFlag_RDBF(FPGA_UART) != RESET)
    {
        uint8_t rxChar = IF_HAL_UartReadByte(FPGA_UART);
        ReceiveChar(FPGA_UART, rxChar);
		IF_HAL_UartClearFlag_RDBF(FPGA_UART);
    }

    if(IF_HAL_UartGetFlag_TDBE(FPGA_UART) != RESET)
    {
        TransmitChar(FPGA_UART);
		IF_HAL_UartClearFlag_TDBE(FPGA_UART);	
    }

}

/* FUNCTION *********************************************************************************
 * Function Name : RS485_UART_Callback
 * Description   : RS485接口(RS485_Uart)回调函数
 * Parameter     : 
 * return        :               
 * END ***************************************************************************************/
void RS485_UART_Callback(void)
{
     
    if(IF_HAL_UartGetFlag_RDBF(RS485_UART) != RESET)
    {
        uint8_t rxChar = IF_HAL_UartReadByte(RS485_UART);
        Modbus_ReceiveChar(RS485_UART, rxChar);
		IF_HAL_UartClearFlag_RDBF(RS485_UART);
    }

    if(IF_HAL_UartGetFlag_TDBE(RS485_UART) != RESET)
    {
        Modbus_TransmitChar(RS485_UART);
		IF_HAL_UartClearFlag_TDBE(RS485_UART);	
    }

}
/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/
/* FUNCTION *********************************************************************************
 * Function Name : IF_CommInit
 * Description   : 通信协议接口初始化函数
 * Parameter     : 
 * return        :               
 * END ***************************************************************************************/
void IF_CommInit(void)
{
	FGIPv2_Init();
	IF_ACDC_SetParamsRWType(READ_MB_VERSIONREG); 
//	uint8_t value = IF_CommParam_GetDataRate();
//	IF_HAL_SetBaudRate(value);	
}


/* FUNCTION *******************************************************************
 *
 * Function Name : IF_UartTask1
 * Description   : 函数实现串口任务的功能实现
 *
 * END ***********************************************************************/
void IF_UartTask1(void)
{
    CommMsg_t rxMsg;
    if(pdTRUE == xQueueReceive(UserQueue, &rxMsg, BSIP_TASK_PERIOD))
	{
		Dealwith_BSIP_Layer2(COM1,&rxMsg);
		IF_Param_ExecuteActionsAfterRsp();
    }	
	BSIP_TxManagment(COM1); 
}

/* FUNCTION *******************************************************************
 *
 * Function Name : IF_UartTask2
 * Description   : 函数实现串口任务的功能实现
 *
 * END ***********************************************************************/
void IF_UartTask2(void)
{
    CommMsg_t rxMsg;
    if(pdTRUE == xQueueReceive(FGIPv2Queue, &rxMsg, portMAX_DELAY))
	{
		Dealwith_FGIPv2_Layer2(DEBUG_UART,&rxMsg);
		IF_Param_ExecuteActionsAfterRsp();
    }
}

/* FUNCTION *******************************************************************
 *
 * Function Name : IF_UartTask3
 * Description   : 函数实现串口任务的功能实现
 *
 * END ***********************************************************************/
void IF_UartTask3(void)
{
	BSIP_FpgaFirmwareUpdate(FPGA_UART);	
}

/* FUNCTION *******************************************************************
 *
 * Function Name : IF_UartTask4
 * Description   : 函数实现串口任务的功能实现
 *
 * END ***********************************************************************/
void IF_UartReciveTask4(void)
{
	CommMsg_t rxMsg;	
	if(pdTRUE == xQueueReceive(ModbusRxQueue, &rxMsg,portMAX_DELAY)) //接受到Modbus队列
	{	
		 Dealwith_Modbus_Service(&rxMsg);
	}
}
void IF_UartSendTask4(void)
{
	Modbus_TxBufManagment(RS485_UART);				
}

/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/
uint8_t SendToRxQueueFromISR(uint8_t port, pCommMsg_t pRxMsg)
{
	uint8_t result = 0;

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	switch(port)
	{
		case USER_UART:
			if(pdTRUE == xQueueSendFromISR(UserQueue, pRxMsg, &xHigherPriorityTaskWoken))result = 1;	
		break;
		case DEBUG_UART:
			if(pdTRUE == xQueueSendFromISR(FGIPv2Queue, pRxMsg, &xHigherPriorityTaskWoken))result = 1;	
		break;
		case RS485_UART:
			if(pdTRUE == xQueueSendFromISR(ModbusRxQueue, pRxMsg, &xHigherPriorityTaskWoken))
			{
				result = 1;
			}	
		break;
		case FPGA_UART:	
			if(pRxMsg->data[1]==0xFF && pRxMsg->data[2]==0xFF &&pRxMsg->data[3]==0x00 &&pRxMsg->data[4]==0xFF &&pRxMsg->data[5]==0xFF)
			{	
				xSemaphoreGiveFromISR(FpgaReSemaphore,&xHigherPriorityTaskWoken);		
			}		
			break;		
	}
	
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	return result;
}

