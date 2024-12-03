/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  fgipv2_layer2.c
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-21
 * ****************************************************************************
 */

#include "fgipv2_layer2.h"
#include "IF_HAL_Cfg.h"
#include "IF_SL_Cfg.h"

//中断接收使用的变量
CommMsg_t g_RxBuf[PORT_NUM]={0};
CommMsg_t g_TxBuf[PORT_NUM]={0};
uint32_t last_rx_time[PORT_NUM] = {0};
uint32_t current_rx_time[PORT_NUM] = {0};

//用于帧处理使用的变量，一次只能处理一条数据帧
GlobeStatus_t g_GS[PORT_NUM];    //设备状态  

extern uint32_t Now(void);


/* FUNCTION *********************************************************************************
 * Function Name : ReceiveChar
 * Description   : 接收字节处理，一般在中断中调用
 * Parameter     : port: 端口号
 * Parameter     : ch: 需要处理的字节
 * Return        :
 * END ***************************************************************************************/
void ReceiveChar(uint8_t port, uint8_t ch)
{
	current_rx_time[port] = Now();

	if ((current_rx_time[port] - last_rx_time[port]) > 10)  //Frame timeout
	{
		g_RxBuf[port].flag_status = 0;                     // 置标志
	}
	last_rx_time[port] = current_rx_time[port];

	switch(g_RxBuf[port].flag_status)
	{
	case 0:
		if (ch == 0x7E)                                       // 如果收到的为开始的0x7E
		{
			g_RxBuf[port].len = 0;
			g_RxBuf[port].index = 0;
			memset(g_RxBuf[port].data, 0, BUFFER_SIZE);
			g_RxBuf[port].port = port;
			g_RxBuf[port].data[g_RxBuf[port].len++] = ch;     // 把数据放入存储区
			g_RxBuf[port].flag_status = 1;                    // 置标志
		}
		break;
	case 1:
		if (ch != 0x7E)                              		// 如果接下来的数据不为0x7E，开始接收数据
		{                                                    // 反正如果接下来的数据仍然为0x7E，那么重新视为开头
			g_RxBuf[port].data[g_RxBuf[port].len++] = ch;        // 把数据放入存储区，同时长度加1
			g_RxBuf[port].flag_status = 2;                     // 置标志
		}
		break;
	case 2:
		g_RxBuf[port].data[g_RxBuf[port].len++] = ch;        // 把数据放入存储区，同时长度加1
		if (g_RxBuf[port].len >= BUFFER_SIZE)                // 保证数据不过界
		{
			g_RxBuf[port].len--;                             // 保证数据不过界
		}
		if (ch == 0x7E)                                      // 收到结尾的0x7E
		{
			SendToRxQueueFromISR(port, &g_RxBuf[port]);
			g_RxBuf[port].flag_status = 0;                     // 置标志
		}
		break;
	default:break;
	}
}

//该函数在中断中调用，发送一个字符，发送完成后关闭中断
void TransmitChar(uint8_t port)
{
	if (g_TxBuf[port].index < g_TxBuf[port].len)
	{
		IF_HAL_UartSendByte(port, g_TxBuf[port].data[g_TxBuf[port].index]);
		g_TxBuf[port].index++;
	}
	else
	{
		memset((void*)&g_TxBuf[port],0,sizeof(CommMsg_t));
		IF_HAL_UartDisableTx(port);    //关闭中断
	}
}

void SendLayer2Frame(uint8_t port,uint8_t *layer2Buf, uint16_t layer2Len)
{
	uint16_t i; 
	uint16_t crc;
	uint16_t len;

	len = layer2Len;
	crc = CalCRC16_X25(layer2Buf, len);

	layer2Buf[len++] = Byte0_UINT16(crc);
	layer2Buf[len++] = Byte1_UINT16(crc);

	memset(&g_TxBuf[port], 0, sizeof(CommMsg_t));

	g_TxBuf[port].port = port;
	g_TxBuf[port].len = 0;

	g_TxBuf[port].data[g_TxBuf[port].len++] = 0x7E;          // Start Flag

	for (i = 0; i < len; i++)
	{
		if ((layer2Buf[i] == 0x7E) || (layer2Buf[i] == 0x7D))
		{
			g_TxBuf[port].data[g_TxBuf[port].len++] = 0x7D;
			g_TxBuf[port].data[g_TxBuf[port].len++] = layer2Buf[i] ^ 0x20;
		}
		else
		{
			g_TxBuf[port].data[g_TxBuf[port].len++] = layer2Buf[i];
		}
	}

	g_TxBuf[port].data[g_TxBuf[port].len++] = 0x7E;          // End Flag
	//中断发送
	IF_HAL_UartEnableTx(port);
}

void Dealwith_FGIPv2_Layer2(uint8_t port,pCommMsg_t pRxMsg)
{
	uint16_t crc16;
	FGIPv2Frame_t rxFrame;
	pRxMsg->len = Decapsulate(pRxMsg->data, pRxMsg->len);
	if (pRxMsg->len < 6)return; //整帧包括 7E Addr Control Info CRC 7E, 最小长度为6
	
	memset(&rxFrame, 0, sizeof(FGIPv2Frame_t));
    rxFrame.port = port;
	rxFrame.address = pRxMsg->data[1];
	rxFrame.control.byte = pRxMsg->data[2];
	rxFrame.infoLen = pRxMsg->len - 6;
	rxFrame.info = &pRxMsg->data[3];
	
	if (((rxFrame.address != DEFAULT_ADDRESS)
			&& (rxFrame.address != IF_CommParam_GetDeviceAddress()))
			|| rxFrame.control.bits.dir != 0 )
	{
		return;
	}

	rxFrame.crc16 = MAKE_UINT16(pRxMsg->data[pRxMsg->len - 2], pRxMsg->data[pRxMsg->len - 3]);
	crc16 = CalCRC16_X25(pRxMsg->data + 1, pRxMsg->len - 4);

	if (crc16 != rxFrame.crc16 )  // Incorrect FCS
	{
		// Send Telegram Error Frame
		SendTelegramError(PacketCRCError);
		return;
	}
	// Call function of layer 7
	Dealwith_FGIPv2_Layer7(&rxFrame);
}

void FGIPv2_Init(void)
{
	g_GS[0].byte = 0x04;
	g_GS[1].byte = 0x04;	
}

//*****************************************************************************
//* END
//*****************************************************************************
