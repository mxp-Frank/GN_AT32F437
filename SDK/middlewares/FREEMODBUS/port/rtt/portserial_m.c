/*
 * FreeModbus Libary: RT-Thread Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions $
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "rtdevice.h"
#include "modbusdevice.h"	

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- Static variables ---------------------------------*/
ALIGN(RT_ALIGN_SIZE)

/* serial event */
static struct rt_event event_serial;
/* modbus master serial device */
static rt_serial_t *serial;

/* ----------------------- Defines ------------------------------------------*/
/* serial transmit event */
#define EVENT_SERIAL_TRANS_START    (1<<0)

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR(void);
static void prvvUARTRxISR(void);


/* software simulation serial transmit IRQ handler thread stack */
static rt_uint8_t serial_soft_trans_irq_stack[512];
/* software simulation serial transmit IRQ handler thread */
static struct rt_thread thread_serial_soft_trans_irq;
/* serial event */
static struct rt_event event_serial;
/* modbus slave serial device */
static rt_serial_t *serial;


static void serial_soft_trans_irq(void* parameter);

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBMasterPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
        eMBParity eParity)
{
    /**
     * set 485 mode receive and transmit control IO
     * @note MODBUS_MASTER_RT_CONTROL_PIN_INDEX need be defined by user
     */
	modbus_uart_init(ulBaudRate);

    /* software initialize */
    rt_event_init(&event_serial, "master event serial", RT_IPC_FLAG_PRIO);

    /* software initialize */
    rt_thread_init(&thread_serial_soft_trans_irq,
                   "slave trans",
                   serial_soft_trans_irq,
                   RT_NULL,
                   serial_soft_trans_irq_stack,
                   sizeof(serial_soft_trans_irq_stack),
                   10, 5);
    rt_thread_startup(&thread_serial_soft_trans_irq);	
	
    return TRUE;
}

void vMBMasterPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
    rt_uint32_t recved_event;
    if (xRxEnable)
    {
		rs485_rx_en();
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    }
    else
    {
		rs485_tx_en();
		USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
    }
    if (xTxEnable)
    {
		rs485_tx_en();
		rt_event_send(&event_serial, EVENT_SERIAL_TRANS_START);
    }
    else
    {
		rs485_rx_en();
        /* stop serial transmit */
        rt_event_recv(&event_serial, EVENT_SERIAL_TRANS_START,
                RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, 0,
                &recved_event);
    }
}

void vMBMasterPortClose(void)
{
    serial->parent.close(&(serial->parent));
}

BOOL xMBMasterPortSerialPutByte(CHAR ucByte)
{
    USART_SendData(USART2, ucByte);
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
    return TRUE;
}

BOOL xMBMasterPortSerialGetByte(CHAR * pucByte)
{
     *pucByte = USART_ReceiveData(USART2); 
    return TRUE;
}

/* 
 * Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
void prvvUARTTxReadyISR(void)
{
    pxMBMasterFrameCBTransmitterEmpty();
}

/* 
 * Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
void prvvUARTRxISR(void)
{
    pxMBMasterFrameCBByteReceived();
}

/**
 * Software simulation serial transmit IRQ handler.
 *
 * @param parameter parameter
 */
static void serial_soft_trans_irq(void* parameter) {
    rt_uint32_t recved_event;
    while (1)
    {
        /* waiting for serial transmit start */
        rt_event_recv(&event_serial, EVENT_SERIAL_TRANS_START, RT_EVENT_FLAG_OR,
                RT_WAITING_FOREVER, &recved_event);
        /* execute modbus callback */
        prvvUARTTxReadyISR();
    }
}


void USART2_IRQHandler(void)
{
	rt_interrupt_enter();
	//发生接收中断
	if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
		prvvUARTRxISR(); 
		//清除中断标志位    
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);   
	}
	rt_interrupt_leave();
}


#endif
