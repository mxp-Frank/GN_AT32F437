/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_AEBus.h
 * @Brief :      
 * @Author :     Chenghanbing (iwanthaha@sina.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-09
 * ****************************************************************************
 */
 
#ifndef _IF_AEBUS_H_
#define _IF_AEBUS_H_

#include "IF_Board.h"
#include "fgipv2.h"
#include "stm32_crc32.h"
#include "if_comm.h"
#define  DATA_SIZE           	255
//User defined type for CSIP command
typedef struct _CommData_t
{
	union 
	{
		uint8_t Header;       			//命令头
		struct
		{
			uint8_t Address    : 5;   // Header Address 
			uint8_t Length     : 3;   // Header length 
		};
	};
	uint16_t Index;
	uint8_t FrameFlag;
	uint8_t CmdNumber;				//接收或发送的命令号
	uint8_t OptLength;				//命令长度
	uint8_t Data[DATA_SIZE];     	//命令数据
	uint8_t CheckSum;               //校验位
}AECommData_t, *pAECommData_t;
/******************************************************************************
* Interface functions                                                         *
******************************************************************************/
extern void IF_ProtecolReceiveChar(uint8_t port, uint8_t ch);
extern void Dealwith_CSIPv1_Layer2(CommMsg_t *rxMsg);


#endif

 /*
*******************************************************************************
* END
*******************************************************************************
*/

