/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  bsip_layer2.c
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2023-05-19
 * ****************************************************************************
 */
#include "bsip_layer2.h"
#include "IF_HAL_Cfg.h"
#include "IF_SL_Cfg.h"
/* CONST & MACROS */

/* DATA STRUCTURES */

/* LOCAL VARIABLES */

/* GLOBAL VARIABLES */
BSIPInfo_t g_rxBsipFrame;

/* FUNCTION PROTOTYPES */

/* FUNCTION PROTOTYPES */


/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/
void Dealwith_BSIP_Layer2(uint8_t port,pCommMsg_t pRxMsg)
{
	uint16_t crc16;
	pRxMsg->port = port;
	pRxMsg->len = Decapsulate(pRxMsg->data, pRxMsg->len);

	if (pRxMsg->len < 7) //整帧包括 7E class id subindex CRC 7E, 最小长度为7(include 0x7E)
	{
		return;
	}
	
	memset(&g_rxBsipFrame, 0, sizeof(BSIPInfo_t));

	g_rxBsipFrame.bsip.msgclass = (MsgClassEnum)pRxMsg->data[1];
	g_rxBsipFrame.bsip.msgid = (MsgIdEnum)pRxMsg->data[2];
    g_rxBsipFrame.bsip.subindex.byte = pRxMsg->data[3];
	g_rxBsipFrame.bsip.info = &pRxMsg->data[4];  //if DataType is N/A, ".info" is not used yet...

	g_rxBsipFrame.bsip.crc16 = MAKE_UINT16(pRxMsg->data[pRxMsg->len - 2], pRxMsg->data[pRxMsg->len - 3]);
	crc16 = CalCRC16_X25(pRxMsg->data + 1, pRxMsg->len - 4);

    g_rxBsipFrame.infoLen = pRxMsg->len - 4;  //infoLen = all Rxbytes - 2(7E:head-tail) - 2(crc)
                                          //infoLen-3 = msgLoad's len = the length of "uint8_t* info"
	if (crc16 == g_rxBsipFrame.bsip.crc16 )
	{
        Dealwith_BSIP_Layer7();
	}

}

void BSIP_TxManagment(uint8_t port)
{
    BSIP_TxBufManagment(port);
    BSIP_TxTimeManagment(port);  
}

void BSIP_Init(void)
{

}

//*****************************************************************************
//* END
//*****************************************************************************

