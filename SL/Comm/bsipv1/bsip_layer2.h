/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  bsip_layer2.h
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2023-05-19
 * ****************************************************************************
 */

#ifndef _BSIP_LAYER2_H_
#define _BSIP_LAYER2_H_

#include "IF_HAL_Cfg.h"
#include "IF_SL_Cfg.h"

#define MIN_FRAME_LEN          6
#define MAX_INFO_LEN           255

typedef union _SubIndex_t
{
    uint8_t byte;
    struct
    {      
        uint8_t subidx   : 6;    // only for preset number
		uint8_t chn      : 2;    // 0: CHN0, 1: CHN1
    } bits;

}SubIndex_t;

typedef enum
{
  INIT_MSG          = 0x10,
  CMD_MSG           = 0x20,
  STATUS_MSG        = 0x30,
  DEBUG_MSG         = 0x40,
  ACK_MSG           = 0xFF,
  
}MsgClassEnum;


typedef struct _BSIPFrame_t
{
    MsgClassEnum msgclass;
    uint8_t msgid;
    SubIndex_t subindex;
    uint8_t* info;
    uint16_t crc16;

}BSIPFrame_t;

typedef struct _BSIPInfo_t
{
    uint16_t infoLen; //infoLen is the most part of bisp's length, it includes "msgclass, msgid,-->
                      //-->subindex, and *info", but except crc16's length.
    BSIPFrame_t bsip;

}BSIPInfo_t;

extern BSIPInfo_t g_rxBsipFrame;

extern void BSIP_Init(void);
extern void Dealwith_BSIP_Layer2(uint8_t port,pCommMsg_t pRxMsg);
extern void BSIP_TxManagment(uint8_t port);



#endif /* _BSIP_LAYER2_H_ */

