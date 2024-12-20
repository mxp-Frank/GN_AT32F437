/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_Protecol.h
 * @Brief :      
 * @Author :     Chenghanbing (iwanthaha@sina.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-09
 * ****************************************************************************
 */
 
#ifndef _IF_CSIPV1_H_
#define _IF_CSIPV1_H_

#include "IF_Board.h"
#include "fgipv2.h"
#include "stm32_crc32.h"
#include "if_comm.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#define  CMD_SIZE  				64
#define  DATA_SIZE           	64

//User defined type for CSIP command
typedef struct _CommData_t
{
	char  	 cmd[CMD_SIZE];       	//命令内容
	uint8_t  cmdLen;				//接收或发送的命令内容长度
	uint8_t  Space;					//命令分隔符<sp>
	char  	 data[DATA_SIZE];     	//命令数据
	uint8_t  IsFloat;				//命令数据类型
	uint16_t dataLen;               //接收或发送的数据长度
}CommData_t, *pCommData_t;

/******************************************************************************
* Interface functions                                                         *
******************************************************************************/
extern void CSIPv1_ReceiveChar(uint8_t port, uint8_t ch);
extern void Dealwith_CSIPv1_Layer2(uint8_t port,CommMsg_t *rxMsg);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif

 /*
*******************************************************************************
* END
*******************************************************************************
*/

