/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  bsip_layer7.c
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2023-05-19
 * ****************************************************************************
 */
#include "bsip_layer7.h"
#include "IF_SL_Cfg.h"
#include "IF_HAL_Cfg.h"

/* LOCAL VARIABLES */
static FirmwareDataMsg_t FirmwareData = 
{
    .TargetDevice = DEVICE_COREBOARD,
    .ExpectedFrameNo = 0,
    .DownFileLen = 0,
    .FileSign = APP_FILE_SIGN,
    .FpgaSign = FPGA_FILE_SIGN,
    .DownFileSign = "00000000",
    .FlashBufIndex = 0,
    .DownloadFileIndex = 0,
};

static DebugDataMsg_t DebugData = 
{
    .FrameNo = 0,
    .ExpectedFrameNo = 0,
    .TxDataRemainLen = 0,
    .CurrentBufLen = 0,
};
static uint8_t g_FlashBuffer[FLASH_SECTOR_SIZE];
static uint8_t NeedSend_ACK = 0;
static uint8_t NeedSend_FactoryParams = 0;
static uint8_t NeedSend_InitParams = 1;
static uint8_t NeedSend_RunningStatus = 0;
static uint8_t NeedSend_DebugData = 0;

/* FUNCTION PROTOTYPES */
static void Dealwith_Param_SetPartsParams(uint8_t chnNo, uint8_t* pBuf, uint16_t paramsLen);
static void Dealwith_Param_SetUserParams(uint8_t chnNo, uint8_t* pBuf, uint16_t paramsLen);
static void Dealwith_Param_SetInternalParams(uint8_t chnNo, uint8_t* pBuf, uint16_t paramsLen);
static void Dealwith_Cmd_Reset(uint8_t chnNo, uint8_t* pBuf);
static void Dealwith_Cmd_Mute(uint8_t chnNo, uint8_t* pBuf);
static void Dealwith_Cmd_Sync(uint8_t chnNo, uint8_t* pBuf);
static void Dealwith_Cmd_SetFactorMode(uint8_t chnNo, uint8_t* pBuf);
static void Dealwith_Cmd_ResumeDefaultParam(uint8_t chnNo, uint8_t* pBuf);
static void Dealwith_Debug_GetProcessData(uint8_t chnNo, uint8_t* pBuf);
static void Dealwith_Debug_FirmwareUpdate(uint8_t chnNo, uint8_t* pBuf);
static void SendDatalinkLayerACK(uint8_t port);
static uint8_t SendInitParams(void);
static void SendRunningStatus(uint8_t port);
static uint8_t SendDebugData(uint8_t port,uint8_t chnNo, uint16_t frameNo);

/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/

/**
 * ************************************************************************
 * @Brief :      
 * ************************************************************************
 */
void Dealwith_BSIP_Layer7(void)
{
    uint8_t chnNo = g_rxBsipFrame.bsip.subindex.bits.chn;
	uint8_t paramslen = g_rxBsipFrame.infoLen-3;
    NeedSend_ACK = 1;//if there is any process error, the Dealwithxxx code will set NeedSend_ACK = 0.
    xQueueReset(BsipTxQueue);

    switch(g_rxBsipFrame.bsip.msgid)
	{
    case ID_Param_SetPartsParams:
        Dealwith_Param_SetPartsParams(chnNo, g_rxBsipFrame.bsip.info, paramslen);
        break;
    
    case ID_Param_SetUserParams:
        Dealwith_Param_SetUserParams(chnNo, g_rxBsipFrame.bsip.info, paramslen);
        break;

    case ID_Param_SetInternalParams:
        Dealwith_Param_SetInternalParams(chnNo, g_rxBsipFrame.bsip.info, paramslen);
        break;

    case ID_Cmd_Reset:
        Dealwith_Cmd_Reset(chnNo, g_rxBsipFrame.bsip.info);
        break;
    case ID_Cmd_Mute:
        Dealwith_Cmd_Mute(chnNo, g_rxBsipFrame.bsip.info);
        break;
    case ID_Cmd_Sync:
        Dealwith_Cmd_Sync(chnNo, g_rxBsipFrame.bsip.info);
        break;
	case ID_Cmd_FactorMode:
        Dealwith_Cmd_SetFactorMode(chnNo, g_rxBsipFrame.bsip.info);
        break;
	case ID_Cmd_ResumeDefaultParam:
        Dealwith_Cmd_ResumeDefaultParam(chnNo, g_rxBsipFrame.bsip.info);
        break;
    case ID_Debug_GetProcessData:
        Dealwith_Debug_GetProcessData(chnNo, g_rxBsipFrame.bsip.info);
        break;
    case ID_Debug_FirmwareUpdate:
        Dealwith_Debug_FirmwareUpdate(chnNo, g_rxBsipFrame.bsip.info);
        break;
	default:
		break;
	}
	//IF_NvmParam_ExecuteActionsAfterRsp();
}

/**
 * ************************************************************************
 * @Brief :      
 * ************************************************************************
 */
void BSIP_TxTimeManagment(uint8_t port)
{      
    CommMsg_t txMsg;
	uint8_t  txLayer2DataBuf[MAX_INFO_LEN + 2];
	uint16_t txLayer2DataLen; 
    static uint8_t sendCnt = 0;
    static uint8_t sendTimeOut = 0;
    static uint16_t fromAckToSend = 0;  //for test only!!!

    if (sendCnt == sendTimeOut)
    {
        if(pdTRUE == xQueueReceive(BsipTxQueue, &txMsg, (TickType_t)0))
        {
			sendCnt = 0;
            txLayer2DataLen=txMsg.len;
			memcpy(&txLayer2DataBuf,&txMsg.data,txMsg.len);
			SendLayer2Frame(port, txLayer2DataBuf,txLayer2DataLen);
            
            //set interval timer’s timeout value according to the type of the tx buf;
            switch (txMsg.data[0])
            {
            case ACK_MSG:
                sendTimeOut = TIME_INTERVAL_ACK_MSG/BSIP_TASK_PERIOD; //max time is 5ms...etc...
                fromAckToSend = 0;  //for test only!!!
                break;
            case INIT_MSG:
                sendTimeOut = TIME_INTERVAL_INIT_MSG/BSIP_TASK_PERIOD;
                break;
            case STATUS_MSG:
                sendTimeOut = TIME_INTERVAL_STATUS_MSG/BSIP_TASK_PERIOD;
                break;
            case DEBUG_MSG:
                sendTimeOut = TIME_INTERVAL_DEBUG_MSG/BSIP_TASK_PERIOD;
                break;

            default:
                break;
            }            
        }
        else
        {//Messages maybe not immediately pushed into the queue, fromAckToSend can be used to calculate the time interval.
            fromAckToSend++;  //time interval = fromAckToSend * BSIP_TASK_PERIOD, unit is ms!!!
        }
    }
    else
    {
        sendCnt++;
    }
}
/**
 * ************************************************************************
 * @Brief :      
 * ************************************************************************
 */
void BSIP_TxBufManagment(uint8_t port)
{
    uint8_t result = E_NOT_OK;

    if (NeedSend_ACK == 1)
    {//single-frame: send data link layer ACK...
        SendDatalinkLayerACK(port);
        NeedSend_ACK = 0;
    }
    else if (NeedSend_InitParams == 1||NeedSend_FactoryParams == 1)
    {
		NeedSend_FactoryParams = 0;
		//multi-frames: send 6 class parmas...
        result = SendInitParams();
        
        if (E_OK == result)
        {//all frames has been sended...
            NeedSend_InitParams = 0;
            NeedSend_RunningStatus = 1;
        }
    }
    else if (NeedSend_DebugData == 1)
    {//multi-frames: send debug data...
        result = SendDebugData(port,0, DebugData.FrameNo);
        NeedSend_DebugData = 0;

        if ((DEBUGDATA_FINISH == result) || (DEBUGDATA_ERROR == result))
        {//all debug_frames has been sended...
            NeedSend_RunningStatus = 1;
        }
    }
    else if (NeedSend_DebugData == 2)
    {
        result = SendDebugData(port,0, DebugData.FrameNo);
        NeedSend_DebugData = 0;

        if ((DEBUGDATA_FINISH == result) || (DEBUGDATA_ERROR == result))
        {
            NeedSend_RunningStatus = 1;
        }
    }
    else if (NeedSend_RunningStatus == 1)
    {//single-frame: send running status data...
        SendRunningStatus(port);
    }

}
/**
 * ************************************************************************
 * @Brief :      
 * ************************************************************************
 */
void BSIP_FpgaFirmwareUpdate(uint8_t port)
{
	BSIPInfo_t BsipFrame;
	if(pdTRUE == xQueueReceive(FpgaQueue, &BsipFrame, portMAX_DELAY)) //接受到FPGA升级包队列
	{	
		uint8_t  txLayer2DataBuf[MAX_INFO_LEN + 2];
		uint16_t txLayer2DataLen; 
		txLayer2DataBuf[txLayer2DataLen++]=BsipFrame.bsip.msgclass;  
		txLayer2DataBuf[txLayer2DataLen++]=BsipFrame.bsip.msgid;
		txLayer2DataBuf[txLayer2DataLen++]=BsipFrame.bsip.subindex.byte;
		for(uint16_t i= 0;i<BsipFrame.infoLen-3;i++)
		{	
			txLayer2DataBuf[txLayer2DataLen++]=BsipFrame.bsip.info[i];
		}
		SendLayer2Frame(port,txLayer2DataBuf,txLayer2DataLen);
		if(pdTRUE == xSemaphoreTake(FpgaSemaphore, 200))
		{	
			NeedSend_ACK = 1;
			FpgaUpdate_NoAck = 0;
		}else
		{
			SendLayer2Frame(port,txLayer2DataBuf,txLayer2DataLen);
			if(pdTRUE == xSemaphoreTake(FpgaSemaphore, 200))
			{	
				NeedSend_ACK = 1;
				FpgaUpdate_NoAck = 0;
			}else
			{
				FpgaUpdate_NoAck = 1;
				//ReSend failed, do nothings;
				FirmwareData.TargetDevice = DEVICE_COREBOARD;
			}
		}
	}
}
/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/

/**
 * ************************************************************************
 * @Brief :      
 * @param  chnNo            
 * @param  pBuf             
 * ************************************************************************
 */
static void Dealwith_Param_SetPartsParams(uint8_t chnNo, uint8_t* pBuf, uint16_t paramsLen)
{
    IF_NvmParam_SetPartsParams(pBuf, paramsLen);
}



static void Dealwith_Param_SetUserParams(uint8_t chnNo, uint8_t* pBuf, uint16_t paramsLen)
{
    IF_NvmParam_SetUserParams(pBuf, paramsLen);
}


static void Dealwith_Param_SetInternalParams(uint8_t chnNo, uint8_t* pBuf, uint16_t paramsLen)
{
    IF_NvmParam_SetInternalParams(pBuf, paramsLen);
}
static void Dealwith_Cmd_Reset(uint8_t chnNo, uint8_t* pBuf)
{
	IF_CmdParam_SetResetDevice();
}

static void Dealwith_Cmd_Mute(uint8_t chnNo, uint8_t* pBuf)
{
    if (pBuf[0] == 1)
    {
        NeedSend_RunningStatus = 0;
    }
    else
    {
        NeedSend_RunningStatus = 1;
    }
}

static void Dealwith_Cmd_Sync(uint8_t chnNo, uint8_t* pBuf)
{
    NeedSend_InitParams = 1;  
    NeedSend_RunningStatus = 0;
}
static void Dealwith_Cmd_SetFactorMode(uint8_t chnNo, uint8_t* pBuf)
{	
	uint8_t value = pBuf[0];
	 if(value <= 1&& IF_CmdParam_GetFactoryMode()!= value)
	 {
		 NeedSend_FactoryParams = 1;
		 NeedSend_InitParams = 1;  
		 NeedSend_RunningStatus = 0;
		 IF_CmdParam_SetFactoryMode(value);	
	 }  
}

static void Dealwith_Cmd_ResumeDefaultParam(uint8_t chnNo, uint8_t* pBuf)
{
   IF_CmdParam_ResumeFactorySettings();
}
/**
 * ************************************************************************
 * @Brief : Deal with Get debug data
 * if the coreboard has received the correct GetProcessData info, then the coreboard mute itself 
 * immediately. This func can't response two channel's debug data at the same time, it should be
 * processed by sequence. If one chn's operation is not end, the new cmd of the same chn has been
 * received, do nothings before the previous operation has done. That's OK!
 * @param  chnNo            
 * @param  pBuf             
 * ************************************************************************
 */
static void Dealwith_Debug_GetProcessData(uint8_t chnNo, uint8_t* pBuf)
{//TBD...

    NeedSend_ACK = 0;
    DebugData.FrameNo = MAKE_UINT16(pBuf[1], pBuf[0]);
    if (NeedSend_DebugData == 0)
    {
        NeedSend_DebugData = 1;
        NeedSend_RunningStatus = 0;
    }
}

/**
 * ************************************************************************
 * @Brief :      
 * @param  chnNo            
 * @param  pBuf             
 * ************************************************************************
 */
static void Dealwith_Debug_FirmwareUpdate(uint8_t chnNo, uint8_t* pBuf)
{//logic check pass!
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t k = 0;

    FirmwareFrameTypeEnum frameType = FIRMWARE_NOF;
    uint8_t frameLength = 0;
    uint16_t frameNo = 0;

    frameType = (FirmwareFrameTypeEnum)pBuf[0];
    frameLength = pBuf[1];
    frameNo = MAKE_UINT16(pBuf[3], pBuf[2]);  //pBuf[3]~pBuf[0]: SOF/COF/EOF, LENGTH, FRAMENO;

    if (frameNo == 0)
    {
        FirmwareData.ExpectedFrameNo = 0;
        
        FirmwareData.FlashBufIndex = 0;
        FirmwareData.DownloadFileIndex = 0;
        FirmwareData.DownFileLen = 0;

        NeedSend_RunningStatus = 0;
    }
    else
    {
        if (FirmwareData.ExpectedFrameNo == 0)
        {
            NeedSend_ACK = 0;  //if there is any problem, then don't feedback ACK;
            NeedSend_RunningStatus = 1;  //restore the running status msg.
            return;
        }

    }

    if (frameNo == FirmwareData.ExpectedFrameNo)
    {
        FirmwareData.ExpectedFrameNo++;

        if ((FirmwareData.TargetDevice == DEVICE_COREBOARD) && ((frameType == FIRMWARE_SOF) || (frameType == FIRMWARE_COF) || (frameType == FIRMWARE_EOF)))
        {
            for (i = 4; i < frameLength + 4; i++)
            {
                //记录下载文件长度
                if (FirmwareData.DownloadFileIndex == 12)
                {
                    uint8_t a = g_FlashBuffer[11];
                    uint8_t b = g_FlashBuffer[10];
                    uint8_t c = g_FlashBuffer[9];
                    uint8_t d = g_FlashBuffer[8];

                    FirmwareData.DownFileLen = MAKE_UINT32(a,b,c,d);

                }

                //记录文件标识
                if ((FirmwareData.DownloadFileIndex >= APP_FILE_SIGN_OFFSET)
                        && (FirmwareData.DownloadFileIndex < (APP_FILE_SIGN_OFFSET + APP_FILE_SIGN_LEN)))
                {
                    FirmwareData.DownFileSign[FirmwareData.DownloadFileIndex - APP_FILE_SIGN_OFFSET] = pBuf[i];
                }

                //接收到完整的识别码后
                if(FirmwareData.DownloadFileIndex == (APP_FILE_SIGN_OFFSET + APP_FILE_SIGN_LEN))
                {

                    for (j = 0; j < APP_FILE_SIGN_LEN; j++)
                    {
                        if (FirmwareData.FileSign[j] != FirmwareData.DownFileSign[j])
                        {
                            break;
                        }
                    }

                    if (j < APP_FILE_SIGN_LEN)
                    {
                        for (k = 0; k < APP_FILE_SIGN_LEN; k++)
                        {
                            if (FirmwareData.FpgaSign[k] != FirmwareData.DownFileSign[k])
                            {
                                break;
                            }
                        }

                        if (k < APP_FILE_SIGN_LEN)
                        {
                            NeedSend_ACK = 0;
                            NeedSend_RunningStatus = 1;
                            return;
                        }

                        NeedSend_ACK = 0;
                        FirmwareData.TargetDevice = DEVICE_FPGA;
                        xQueueSend(FpgaQueue, &g_rxBsipFrame, (TickType_t)0);
                        return;

                    }

                    FirmwareData.TargetDevice = DEVICE_COREBOARD;
                    
                    if ((FirmwareData.DownFileLen == 0) || (FirmwareData.DownFileLen > MAX_APP_FILE_SIZE))
                    {
                        NeedSend_ACK = 0;
                        NeedSend_RunningStatus = 1;
                        return;
                    }
                }

                //把数据放入缓冲区
                g_FlashBuffer[FirmwareData.FlashBufIndex++] = pBuf[i];

                if(FirmwareData.FlashBufIndex == FLASH_BUF_SIZE)
                {
                    //缓冲区满了，把数据写入Flash
                    if (!IF_NvmParam_WriteDownloadFile(
                            FirmwareData.DownloadFileIndex - FLASH_BUF_SIZE + 1,
                            (uint16_t *)g_FlashBuffer,
                            FLASH_BUF_SIZE/2))
                    {
                        NeedSend_ACK = 0;
                        NeedSend_RunningStatus = 1;
                        return;
                    }

                    FirmwareData.FlashBufIndex = 0;
                }

                FirmwareData.DownloadFileIndex++;
            }

            //如果是最后一帧
            if (frameType == FIRMWARE_EOF)
            {
                if(FirmwareData.FlashBufIndex != 0)
                {
                    if (!IF_NvmParam_WriteDownloadFile(
                            FirmwareData.DownloadFileIndex - FirmwareData.FlashBufIndex,
                            (uint16_t *)g_FlashBuffer,
                            FLASH_BUF_SIZE/2))
                    {
                        NeedSend_ACK = 0;
                        NeedSend_RunningStatus = 1;
                        return;
                    }
                }

                if (FirmwareData.DownloadFileIndex != IF_NvmParam_ReadDownloadFileLen())
                {
                    NeedSend_ACK = 0;
                    NeedSend_RunningStatus = 1;
                    return;
                }

                if (!IF_NvmParam_CheckDownloadFile())
                {
                    NeedSend_ACK = 0;
                    NeedSend_RunningStatus = 1;
                    return;
                }

                // 设置升级标识
                if (!IF_NvmParam_WriteDownloadFlag())
                {
                    NeedSend_ACK = 0;
                    NeedSend_RunningStatus = 1;
                    return;
                }

                IF_CmdParam_SetResetDevice();
            }

        }
        else if ((FirmwareData.TargetDevice == DEVICE_FPGA) && ((frameType == FIRMWARE_SOF) || (frameType == FIRMWARE_COF) || (frameType == FIRMWARE_EOF)))
        {//FIRMWARE_SOF may be resended...
            if (frameType == FIRMWARE_EOF)
            {
                FirmwareData.TargetDevice = DEVICE_COREBOARD;
            }

            NeedSend_ACK = 0;
            xQueueSend(FpgaQueue, &g_rxBsipFrame, (TickType_t)0);
        }
        else
        {
            NeedSend_ACK = 0;
            NeedSend_RunningStatus = 1;
        }
    }
    else
    {
        NeedSend_ACK = 0;
        NeedSend_RunningStatus = 1;
    }

}

/**
 * ************************************************************************
 * @Brief :      
 * ************************************************************************
 */
static void SendDatalinkLayerACK(uint8_t port)
{//7e ff ff 00 ff ff 7e
    CommMsg_t txInfo;

	memset(&txInfo, 0, sizeof(CommMsg_t));

    txInfo.data[txInfo.len++] = 0xFF;
    txInfo.data[txInfo.len++] = 0xFF;
    txInfo.data[txInfo.len++] = 0x00;
    
    xQueueSend(BsipTxQueue, &txInfo, (TickType_t)0);
}

/**
 * ************************************************************************
 * @Brief :      
 * @return uint8_t          
 * ************************************************************************
 */
static uint8_t SendInitParams(void)
{
    CommMsg_t txInfo;
    uint8_t ret = E_NOT_OK;
    uint8_t sendInterval = 0;
    static uint8_t sendCnt = 0;
    static uint8_t sendStep = 0;  //step0: send parts params; step1: send factory params...etc...

#if((config_CHN0 == 1) && (config_HF_CHN == 1))
    sendInterval = TIME_INTERVAL_INIT_MSG*CHN_NUM/BSIP_TASK_PERIOD;
#else
    sendInterval = TIME_INTERVAL_INIT_MSG/BSIP_TASK_PERIOD;
#endif

    switch (sendStep)
    {
    case 0:
        memset(&txInfo, 0, sizeof(CommMsg_t));

        txInfo.data[txInfo.len++] = INIT_MSG;
        txInfo.data[txInfo.len++] = ID_Param_InitPartsParams;
        txInfo.data[txInfo.len++] = 0x00;

        IF_NvmParam_GetPartsParams(&txInfo.data[3],PARTS_PARAM_LEN);
        txInfo.len += PARTS_PARAM_LEN;
            
        xQueueSend(BsipTxQueue, &txInfo, (TickType_t)0);
        sendCnt = 0;
        sendStep++;
        
        break;

    case 1:
        if (sendCnt == sendInterval)
        {//Period of pushing to the queue is 50ms

			memset(&txInfo, 0, sizeof(CommMsg_t));

			txInfo.data[txInfo.len++] = INIT_MSG;
			txInfo.data[txInfo.len++] = ID_Param_InitUserParams;
			txInfo.data[txInfo.len++] = 0x00;

			IF_NvmParam_GetUserParams(&txInfo.data[3],USER_PARAM_LEN);
			txInfo.len += USER_PARAM_LEN;

			 xQueueSend(BsipTxQueue, &txInfo, (TickType_t)0);
 
            sendStep++;
            sendCnt = 0;
        }
        else
        {
            sendCnt++;
        }
        break;
    case 2:
        if (sendCnt == sendInterval)
        {//Period of pushing to the queue is 50ms

			memset(&txInfo, 0, sizeof(CommMsg_t));
			
			txInfo.data[txInfo.len++] = INIT_MSG;
			txInfo.data[txInfo.len++] = ID_Param_InitInternalParams;
			txInfo.data[txInfo.len++] = 0;

			IF_NvmParam_GetInternalParams(&txInfo.data[3],INTERNAL_PARAM_LEN);
			txInfo.len += INTERNAL_PARAM_LEN;

			xQueueSend(BsipTxQueue, &txInfo, (TickType_t)0);
  
            sendStep++;
            sendCnt = 0;
        }
        else
        {
            sendCnt++;
        }
        break;

    case 3:
        sendStep = 0;
        sendCnt = 0;
        
        ret = E_OK;

        break;

    default:
        break;
    }

    return ret;
}

/**
 * ************************************************************************
 * @Brief :      
 * ************************************************************************
 */
static void SendRunningStatus(uint8_t port)
{
    CommMsg_t txInfo;
    uint8_t sendInterval = 0;

    static uint8_t sendCnt = TIME_INTERVAL_STATUS_MSG/BSIP_TASK_PERIOD;
    sendInterval = TIME_INTERVAL_STATUS_MSG/BSIP_TASK_PERIOD;

    if (sendCnt == sendInterval)
    {//Period of pushing to the queue should be the twice interval sending time if CHN_NUM=2...
               
		memset(&txInfo, 0, sizeof(CommMsg_t));
		
		txInfo.data[txInfo.len++] = STATUS_MSG;
		txInfo.data[txInfo.len++] = ID_Status_RunningInfo;
		txInfo.data[txInfo.len++] = 0x00;
		
		memset((void*)&g_RunningStatus,0,sizeof(g_RunningStatus));
		memcpy(&txInfo.data[txInfo.len], (uint8_t*)&g_RunningStatus, RUNNING_STATUS_LEN);
		txInfo.len += RUNNING_STATUS_LEN;

		xQueueSend(BsipTxQueue, &txInfo, (TickType_t)0);
        
        sendCnt = 0;
    }
    else
    {
        sendCnt++;
    }

}

/**
 * ************************************************************************
 * @Brief :      
 * @param  chnNo            
 * @return uint8_t          
 * ************************************************************************
 */
static uint8_t SendDebugData(uint8_t port,uint8_t chnNo, uint16_t frameNo)
{
    uint8_t ret = DEBUGDATA_NOT_FINISH;
    CommMsg_t txInfo;

    if (frameNo == 0)
    {
        DebugData.ExpectedFrameNo = 0;
        DebugData.TxDataRemainLen = IF_GetTotalAMPDLength();
    }
    else
    {
        if ( DebugData.ExpectedFrameNo == 0)
        {
            ret = DEBUGDATA_ERROR;
            return ret;
        }

    }

    if (frameNo !=  DebugData.ExpectedFrameNo)
    {
        DebugData.ExpectedFrameNo = 0;
        ret = DEBUGDATA_ERROR;
        return ret;
    }

    memset(g_FlashBuffer, 0, FLASH_BUF_SIZE);
    DebugData.CurrentBufLen = 0;

    DebugData.CurrentBufLen = IF_GetAMPD(frameNo, g_FlashBuffer);

    DebugData.TxDataRemainLen -= DebugData.CurrentBufLen;

    if (DebugData.TxDataRemainLen > 0)
    {
        memset(&txInfo, 0, sizeof(CommMsg_t));

        txInfo.data[txInfo.len++] = DEBUG_MSG;
        txInfo.data[txInfo.len++] = ID_Debug_ProcessData;
        txInfo.data[txInfo.len++] = ((uint8_t)chnNo<<6) & 0xC0;

        if (frameNo == 0)
        {
            txInfo.data[txInfo.len++] = FIRMWARE_SOF;
        }
        else
        {
            txInfo.data[txInfo.len++] = FIRMWARE_COF;
        }

        txInfo.data[txInfo.len++] = DebugData.CurrentBufLen;
        txInfo.data[txInfo.len++] = Byte0_UINT16(frameNo);
        txInfo.data[txInfo.len++] = Byte1_UINT16(frameNo);
        memcpy(&txInfo.data[txInfo.len], g_FlashBuffer, DebugData.CurrentBufLen);
        txInfo.len += DebugData.CurrentBufLen;
            
        xQueueSend(BsipTxQueue, &txInfo, (TickType_t)0);

        DebugData.ExpectedFrameNo++;
    }
    else
    {
        memset(&txInfo, 0, sizeof(CommMsg_t));

        txInfo.data[txInfo.len++] = DEBUG_MSG;
        txInfo.data[txInfo.len++] = ID_Debug_ProcessData;
        txInfo.data[txInfo.len++] = ((uint8_t)chnNo<<6) & 0xC0;

        txInfo.data[txInfo.len++] = FIRMWARE_EOF;
        txInfo.data[txInfo.len++] = DebugData.CurrentBufLen;
        txInfo.data[txInfo.len++] = Byte0_UINT16(frameNo);
        txInfo.data[txInfo.len++] = Byte1_UINT16(frameNo);
        memcpy(&txInfo.data[txInfo.len], g_FlashBuffer, DebugData.CurrentBufLen);
        txInfo.len += DebugData.CurrentBufLen;
            
        if (xQueueSend(BsipTxQueue, &txInfo, (TickType_t)0) == pdTRUE)
        {
            ret = DEBUGDATA_FINISH;
        }

        DebugData.ExpectedFrameNo = 0;
        
    }

    return ret;
}
//*****************************************************************************
//* END
//*****************************************************************************
