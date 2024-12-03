/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  fgipv2_layer7.c
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-21
 * ****************************************************************************
 */
#include "fgipv2_layer7.h"
#include "IF_HAL_Cfg.h"
#include "IF_SL_Cfg.h"

static InfoFiled_t rxInfo;
static InfoFiled_t txInfo;

static uint8_t txDOBsDataBuf[DOB_MAX_LEN];
static uint16_t txDOBsDataLen;

static uint8_t  txLayer2DataBuf[MAX_INFO_LEN + 2];
static uint16_t txLayer2DataLen;

static uint32_t txDataBlockRemainLen = 0;

//软件升级相关变量
static uint8_t DataBlockFlag = 0;
static uint8_t ExpectedDataBlockID = 0;
static uint8_t ExpectedDataBlockCmd = 0;
static uint16_t ExpectedFrameNo = 0;

static uint32_t DownFileLen;
static uint8_t FirmwareFlag;
static uint8_t McuFileSign[APP_FILE_SIGN_LEN] = APP_FILE_SIGN;
static uint8_t FpgaFileSign[APP_FILE_SIGN_LEN] = FPGA_FILE_SIGN;
static uint8_t ModbusFileSign[APP_FILE_SIGN_LEN] = MODBUS_FILE_SIGN;
static uint8_t DownFileSign[APP_FILE_SIGN_LEN];
static uint32_t DownloadFileIndex;

static uint8_t NvmBuffer[FLASH_SECTOR_SIZE];
static uint16_t NvmBufIndex;

uint8_t GetDataLen(uint8_t dataType)
{
	uint8_t len;
	switch(dataType)
	{
	case DataType_BOOL:
	case DataType_UINT8:
		len = 1;
		break;
	case DataType_SINT16:
	case DataType_UINT16:
		len = 2;
		break;
	case DataType_SINT32:
	case DataType_UINT32:
	case DataType_FLT32:
		len = 4;
		break;
	case DataType_FLT64:
		len = 8;
		break;
	case DataType_VECT:
		len = MAX_VECT_LEN;
		break;
	default:
		len = 0;
		break;
	}

	return len;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
static void Dealwith_DOB_DeviceType(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}

	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT8;
		pTxDOB->DataLen  = 1;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		uint8_t value = IF_IdentParam_GetDeviceType();
		txDOBsDataBuf[txDOBsDataLen++] = value;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (IF_CmdParam_GetFactoryMode())
		{
			if (pRxDOB->DataType == DataType_UINT8)
			{
				uint8_t value = pRxDOB->pData[0];
				IF_IdentParam_SetDeviceType(value);
				pTxDOB->Status = NoErrorNoData;

			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = NotPermittedParam;
		}
	}
}

static void Dealwith_DOB_VendorCode(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT8;
		pTxDOB->DataLen  = 1;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		uint8_t value = IF_IdentParam_GetVendorCode();
		txDOBsDataBuf[txDOBsDataLen++] = value;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (IF_CmdParam_GetFactoryMode())
		{
			if (pRxDOB->DataType == DataType_UINT8)
			{
				uint8_t value = pRxDOB->pData[0];
				IF_IdentParam_SetVendorCode(value);
				pTxDOB->Status = NoErrorNoData;
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = NotPermittedParam;
		}
	}
}

static void Dealwith_DOB_HardwareVersion(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_VECT;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		pTxDOB->DataLen  = IF_IdentParam_GetHardwareVersion(&pTxDOB->pData[1]);
		pTxDOB->pData[0] = pTxDOB->DataLen;
		pTxDOB->DataLen += 1;

		txDOBsDataLen += pTxDOB->DataLen;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (IF_CmdParam_GetFactoryMode())
		{
			if (pRxDOB->DataType == DataType_VECT)
			{
				
				uint8_t* pBuf;
				uint8_t len = pRxDOB->DataLen;
				pBuf = pRxDOB->pData;
				IF_IdentParam_SetHardwareVersion(pBuf,len);
				pTxDOB->Status = NoErrorNoData;
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = NotPermittedParam;
		}
	}
}

static void Dealwith_DOB_FpgawareVersion(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_VECT;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		pTxDOB->DataLen  = IF_Get_Sensor_Version(&pTxDOB->pData[1]);
		pTxDOB->pData[0] = pTxDOB->DataLen;
		pTxDOB->DataLen += 1;

		txDOBsDataLen += pTxDOB->DataLen;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}
static void Dealwith_DOB_SoftwareVersion(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}

	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_VECT;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		pTxDOB->DataLen  = IF_IdentParam_GetSoftwareVersion(&pTxDOB->pData[1]);

		pTxDOB->pData[0] = pTxDOB->DataLen;
		pTxDOB->DataLen += 1;

		txDOBsDataLen += pTxDOB->DataLen;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}

static void Dealwith_DOB_BootloaderVersion(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}

	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_VECT;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		pTxDOB->DataLen  = IF_IdentParam_GetBootloaderVersion(&pTxDOB->pData[1]);

		pTxDOB->pData[0] = pTxDOB->DataLen;
		pTxDOB->DataLen += 1;

		txDOBsDataLen += pTxDOB->DataLen;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}

static void Dealwith_DOB_ProductModel(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}

	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_VECT;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		pTxDOB->DataLen  = IF_IdentParam_GetProductModel(&pTxDOB->pData[1]);
		pTxDOB->pData[0] = pTxDOB->DataLen;
		pTxDOB->DataLen += 1;

		txDOBsDataLen += pTxDOB->DataLen;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (IF_CmdParam_GetFactoryMode())
		{
			if (pRxDOB->DataType == DataType_VECT)
			{
				uint8_t* pBuf;
				uint8_t len = pRxDOB->DataLen;  
				pBuf = pRxDOB->pData;  

				IF_IdentParam_SetProductModel(pBuf,len); 
				pTxDOB->Status = NoErrorNoData;
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = NotPermittedParam;
		}
	}
}

static void Dealwith_DOB_SerialNumber(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}

	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_VECT;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		pTxDOB->DataLen  = IF_IdentParam_GetSerialNumber(&pTxDOB->pData[1]); //只在低频参数有效

		pTxDOB->pData[0] = pTxDOB->DataLen;
		pTxDOB->DataLen += 1;

		txDOBsDataLen += pTxDOB->DataLen;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (IF_CmdParam_GetFactoryMode())
		{
			if (pRxDOB->DataType == DataType_VECT)
			{
				uint8_t* pBuf;
				uint8_t len = pRxDOB->DataLen;  //len = pRxDOB->pData[0];
				pBuf = pRxDOB->pData;   		//pBuf = &pRxDOB->pData[1];
				IF_IdentParam_SetSerialNumber(pBuf,len); 
				pTxDOB->Status = NoErrorNoData;
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = NotPermittedParam;
		}
	}
}

static void Dealwith_DOB_TrackingNumber(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}

	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_VECT;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		pTxDOB->DataLen  = IF_IdentParam_GetTrackingNumber(&pTxDOB->pData[1]);
		pTxDOB->pData[0] = pTxDOB->DataLen;
		pTxDOB->DataLen += 1;

		txDOBsDataLen += pTxDOB->DataLen;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (IF_CmdParam_GetFactoryMode())
		{
			if (pRxDOB->DataType == DataType_VECT)
			{
				//数据类型正确才设置参数
				uint8_t* pBuf;
				uint8_t len = pRxDOB->DataLen;  	//len = pRxDOB->pData[0];
				pBuf = pRxDOB->pData;   			//pBuf = &pRxDOB->pData[1];
				IF_IdentParam_SetTrackingNumber(pBuf,len);
				pTxDOB->Status = NoErrorNoData;
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = NotPermittedParam;
		}
	}
}

static void Dealwith_DOB_CheckSum(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	//
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}

	if (rxInfo.Cmd == ParameterRead)
	{
		uint32_t value;

		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		value = IF_IdentParam_GetCheckSum();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}

static void Dealwith_DOB_DeviceAddress(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}

	if (rxInfo.Cmd == ParameterRead)
	{
		uint8_t value;

		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT8;
		pTxDOB->DataLen  = 1;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		value = IF_CommParam_GetDeviceAddress();
		txDOBsDataBuf[txDOBsDataLen++] = value;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT8)
		{
			//数据类型正确才设置参数
			uint8_t value = pRxDOB->pData[0];
			IF_CommParam_SetDeviceAddress(value); //只在低频参数有效
			pTxDOB->Status = NoErrorNoData;	
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}

static void Dealwith_DOB_DataRate(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}

	if (rxInfo.Cmd == ParameterRead)
	{
		uint8_t value;

		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT8;
		pTxDOB->DataLen  = 1;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		value = IF_CommParam_GetDataRate();
		txDOBsDataBuf[txDOBsDataLen++] = value;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT8)
		{
			//数据类型正确才设置参数
			uint8_t value = pRxDOB->pData[0];
			if (value <= 4)
			{
				IF_CommParam_SetDataRate(value);
				pTxDOB->Status = NoErrorNoData;
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}


static void Dealwith_DOB_NET_MAC(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}

	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_VECT;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		pTxDOB->DataLen  = IF_CommParam_GetNetMAC(&pTxDOB->pData[1]);

		pTxDOB->pData[0] = pTxDOB->DataLen;
		pTxDOB->DataLen += 1;

		txDOBsDataLen += pTxDOB->DataLen;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if ((pRxDOB->DataType == DataType_VECT) && (pRxDOB->DataLen == 6))
		{
			IF_CommParam_SetNetMAC(pRxDOB->pData);
			pTxDOB->Status = NoErrorNoData;
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}

static void Dealwith_DOB_NET_IP(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}

	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		uint32_t value = IF_CommParam_GetNetIP();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT32)
		{
			uint32_t value = MAKE_UINT32(pRxDOB->pData[3], pRxDOB->pData[2], pRxDOB->pData[1], pRxDOB->pData[0]);
			IF_CommParam_SetNetIP(value);
			pTxDOB->Status = NoErrorNoData;
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}

static void Dealwith_DOB_NET_Mask(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}

	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		uint32_t value = IF_CommParam_GetNetMask();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT32)
		{
			//数据类型正确才设置参数
			uint32_t value = MAKE_UINT32(pRxDOB->pData[3], pRxDOB->pData[2], pRxDOB->pData[1], pRxDOB->pData[0]);
			IF_CommParam_SetNetMask(value);
			pTxDOB->Status = NoErrorNoData;
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}

static void Dealwith_DOB_NET_Gateway(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}

	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		uint32_t value = IF_CommParam_GetNetGateway();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT32)
		{
			//数据类型正确才设置参数
			uint32_t value = MAKE_UINT32(pRxDOB->pData[3], pRxDOB->pData[2], pRxDOB->pData[1], pRxDOB->pData[0]);
			IF_CommParam_SetNetGateway(value);
			pTxDOB->Status = NoErrorNoData;
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_EtherCatAddress(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}

	if (rxInfo.Cmd == ParameterRead)
	{
		uint8_t value;

		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		value = IF_CommParam_GetEtherCatAddress();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT16)
		{
			//数据类型正确才设置参数
			uint16_t value = MAKE_UINT16(pRxDOB->pData[0],pRxDOB->pData[0]);
			IF_CommParam_SetEtherCatAddress(value); //只在低频参数有效
			pTxDOB->Status = NoErrorNoData;	
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}

static void Dealwith_DOB_Reset(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}

	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status = NotPermittedParam;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NoErrorNoData;
		IF_CmdParam_SetResetDevice();
	}
}

static void Dealwith_DOB_ResumeFactorySettings(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status = NotPermittedParam;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status   = NoErrorNoData;

		IF_CmdParam_ResumeFactorySettings();
	}
}

static void Dealwith_DOB_RegulationMode(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT8;
		pTxDOB->DataLen  = 1;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		uint8_t value = IF_UserParam_GetRegulationMode();
		
		txDOBsDataBuf[txDOBsDataLen++] = value;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT8)
		{
			//数据类型正确才设置参数
			uint8_t value = pRxDOB->pData[0];
			if (value <= MAX_REGUALATION)
			{
				IF_UserParam_SetRegulationMode(value);
				pTxDOB->Status = NoErrorNoData;
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_CommunicateType(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT8;
		pTxDOB->DataLen  = 1;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		uint8_t value = IF_CommParam_GetCommunicateType();
		
		txDOBsDataBuf[txDOBsDataLen++] = value;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT8)
		{
			//数据类型正确才设置参数
			uint8_t value = pRxDOB->pData[0];
			if (value <= MAX_COMTYPE)
			{
				IF_CommParam_SetCommunicateType(value);
				pTxDOB->Status = NoErrorNoData;
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_SlowMode(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT8;
		pTxDOB->DataLen  = 1;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		uint8_t value = IF_UserParam_GetSlowMode();
		
		txDOBsDataBuf[txDOBsDataLen++] = value;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT8)
		{
			//数据类型正确才设置参数
			uint8_t value = pRxDOB->pData[0];
			if (value==0||value ==1)
			{
				IF_UserParam_SetSlowMode(value);
				pTxDOB->Status = NoErrorNoData;
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_ForwardPowerLimit(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	
	if (rxInfo.Cmd == ParameterRead)
	{
		
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		uint16_t value = IF_UserParam_GetForwardPowerLimit();
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT16)
		{
			//数据类型正确才设置参数
			uint16_t value = MAKE_UINT16(pRxDOB->pData[1], pRxDOB->pData[0]);
			IF_UserParam_SetForwardPowerLimit(value);
			pTxDOB->Status = NoErrorNoData;
			
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}

static void Dealwith_DOB_ReflectPowerLimit(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		uint16_t value = IF_UserParam_GetReflectPowerLimit();
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT16)
		{
			//数据类型正确才设置参数
			uint16_t value = MAKE_UINT16(pRxDOB->pData[1], pRxDOB->pData[0]);
			IF_UserParam_SetReflectPowerLimit(value);
			pTxDOB->Status = NoErrorNoData;
			
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_VdcPowerLimit(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		uint16_t value = IF_UserParam_GetDCBiasPowerLimit();
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT16)
		{
			//数据类型正确才设置参数
			uint16_t value = MAKE_UINT16(pRxDOB->pData[1], pRxDOB->pData[0]);
			IF_UserParam_SetDCBiasPowerLimit(value);
			pTxDOB->Status = NoErrorNoData;
			
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}

static void Dealwith_DOB_ReflectPowerSwitchOff(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT8;
		pTxDOB->DataLen  = 1;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		uint8_t value = IF_UserParam_GetReflectPowerSwitchOff();
		txDOBsDataBuf[txDOBsDataLen++] = value;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT8)
		{
			//数据类型正确才设置参数
			uint8_t value =  pRxDOB->pData[0];
			if (value ==0||value == 1)
			{
				IF_UserParam_SetReflectPowerSwitchOff(value);
				pTxDOB->Status = NoErrorNoData;	
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}

static void Dealwith_DOB_ReflectPowerThreshold(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		uint16_t value = IF_UserParam_GetReflectPowerThreshold();
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT16)
		{
			//数据类型正确才设置参数
			uint16_t value = MAKE_UINT16(pRxDOB->pData[1], pRxDOB->pData[0]);
			IF_UserParam_SetReflectPowerThreshold(value);
			pTxDOB->Status = NoErrorNoData;
			
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_SlowStartDelay(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		uint16_t value = IF_UserParam_GetSlowStartDelay();
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT16)
		{
			//数据类型正确才设置参数
			uint16_t value = MAKE_UINT16(pRxDOB->pData[1], pRxDOB->pData[0]);
			if(value<MAX_ENCODER)
			{
				IF_UserParam_SetSlowStartDelay(value);
				pTxDOB->Status = NoErrorNoData;
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_SlowStopDelay(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		uint16_t value = IF_UserParam_GetSlowStopDelay();
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT16)
		{
			//数据类型正确才设置参数
			uint16_t value = MAKE_UINT16(pRxDOB->pData[1], pRxDOB->pData[0]);
			if(value<MAX_ENCODER)
			{
				IF_UserParam_SetSlowStopDelay(value);
				pTxDOB->Status = NoErrorNoData;
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_DelayReachTime(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT8;
		pTxDOB->DataLen  = 1;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		uint8_t value = IF_UserParam_GetReflectPowerDelayOff();	
		txDOBsDataBuf[txDOBsDataLen++] = value;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT8)
		{
			//数据类型正确才设置参数
			uint8_t value = pRxDOB->pData[0];
			if(value<MAX_REFLECTDELAY)
			{
				IF_UserParam_SetReflectPowerDelayOff(value);
				pTxDOB->Status = NoErrorNoData;		
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}		
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}

static void Dealwith_DOB_PulseMode(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT8;
		pTxDOB->DataLen  = 1;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		uint8_t value = IF_UserParam_GetPulseMode();
		txDOBsDataBuf[txDOBsDataLen++] = value;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT8)
		{
			//数据类型正确才设置参数
			uint8_t value = pRxDOB->pData[0];
			if (value==0||value == 1)
			{
				IF_UserParam_SetPulseMode(value);
				pTxDOB->Status = NoErrorNoData;
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}

static void Dealwith_DOB_FpgaPulsePowerThr(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		uint32_t value = IF_InternalParam_GetFpgaPulsePowerThr();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT32)
		{
			//数据类型正确才设置参数
			uint32_t value = MAKE_UINT32(pRxDOB->pData[3], pRxDOB->pData[2],pRxDOB->pData[1], pRxDOB->pData[0]);	
			IF_InternaleParam_SetFpgaPulsePowerThr(value);
			pTxDOB->Status = NoErrorNoData;
			
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}

static void Dealwith_DOB_FpgaPulseSyncDelay(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		uint16_t value = IF_InternalParam_GetFpgaPulseSyncDelay();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT16)
		{
			//数据类型正确才设置参数
			uint16_t value = MAKE_UINT16(pRxDOB->pData[1], pRxDOB->pData[0]);
			uint32_t Period =1000000/IF_UserParam_GetPulseFrequency();
			uint32_t Limit = Period*IF_UserParam_GetPulseDutyCircle()/100;
			if(value < Limit)
			{
				IF_InternalParam_SetFpgaPulseSyncDelay(value);
				pTxDOB->Status = NoErrorNoData;	
			}else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}

static void Dealwith_DOB_PulseFrequency(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		uint32_t value = IF_UserParam_GetPulseFrequency();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT32)
		{
			//数据类型正确才设置参数
			uint32_t value = MAKE_UINT32(pRxDOB->pData[3], pRxDOB->pData[2],pRxDOB->pData[1], pRxDOB->pData[0]);
			if (value >0&&value <= MAX_ENCODER)
			{
				IF_UserParam_SetPulseFrequency(value);
				pTxDOB->Status = NoErrorNoData;
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_PulseDutyCircle(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT8;
		pTxDOB->DataLen  = 1;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		uint8_t value = IF_UserParam_GetPulseDutyCircle();
		txDOBsDataBuf[txDOBsDataLen++] = value;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT8)
		{
			//数据类型正确才设置参数
			uint8_t value =  pRxDOB->pData[0];	
			if(value>0&&value< MAX_PULSEDUTY&& value > IF_InternalParam_GetFpgaPulseSyncDelay())
			{
				IF_UserParam_SetPulseDutyCircle(value);	
				pTxDOB->Status = NoErrorNoData;
			}else
			{
				pTxDOB->Status = CheckFailedParam;
			}	
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_CmdSetVolotage(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		uint32_t value = IF_CmdParam_GetACDCVoltage();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_SINT32)
		{
			//数据类型正确才设置参数
			int32_t value = MAKE_UINT32(pRxDOB->pData[3],pRxDOB->pData[2], pRxDOB->pData[1],pRxDOB->pData[0]);
			if (value >=0&&value <= MAX_ACDC_VOLT)
			{
				if(OFF == IF_CmdParam_GetPowerWorkMode())
				{
					IF_CmdParam_SetACDCVoltage(value);
					pTxDOB->Status = NoErrorNoData;
				}else
				{
					pTxDOB->Status = NotPermittedParam;
				}
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_CmdSetPoint(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		uint16_t value = IF_CmdParam_GetSetPoint();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT16)
		{
			//数据类型正确才设置参数
			uint16_t value = MAKE_UINT16( pRxDOB->pData[1],pRxDOB->pData[0]);
			if (value <= MAX_POWER)
			{
				IF_CmdParam_SetSetPoint(value);
				pTxDOB->Status = NoErrorNoData;
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}

static void Dealwith_DOB_CmdACDCVolotage(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int32_t value = IF_CmdParam_GetACDCVoltage();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_SINT32)
		{
			//数据类型正确才设置参数
			int32_t value = MAKE_UINT32(pRxDOB->pData[3],pRxDOB->pData[2], pRxDOB->pData[1],pRxDOB->pData[0]);
			if (value >=0&&value <= MAX_ACDC_VOLT)
			{
				IF_CmdParam_SetACDCVoltage(value);
				pTxDOB->Status = NoErrorNoData;	
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_CmdACDCCurrent(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int32_t value = IF_CmdParam_GetACDCCurrent();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_SINT32)
		{
			//数据类型正确才设置参数
			int32_t value = MAKE_UINT32(pRxDOB->pData[3],pRxDOB->pData[2], pRxDOB->pData[1],pRxDOB->pData[0]);
			if (value >=0&&value <= MAX_ACDC_CURRENT)
			{
				IF_CmdParam_SetACDCCurrent(value);
				pTxDOB->Status = NoErrorNoData;
				
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_AnalogVoltageRange(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		uint16_t value = IF_InternalParam_GetAnalogVoltRange();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT16)
		{
			//数据类型正确才设置参数
			uint16_t value = MAKE_UINT16(pRxDOB->pData[1], pRxDOB->pData[0]);
			if (value<= 15000)
			{
				IF_InternalParam_SetAnalogVoltRange(value);
				pTxDOB->Status = NoErrorNoData;
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}

static void Dealwith_DOB_InputVDCFactor(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		uint32_t value = IF_UserParam_GetVDCFactor();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT32)
		{
			//数据类型正确才设置参数
			uint32_t value = MAKE_UINT32(pRxDOB->pData[3], pRxDOB->pData[2],pRxDOB->pData[1], pRxDOB->pData[0]);
			
			if(value<=4000)
			{	
				IF_UserParam_SetVDCFactor(value);
				pTxDOB->Status = NoErrorNoData;
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_InputVrmsFactor(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{

	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		int32_t value = IF_InternalParam_GetSensorVrmsFactor();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_SINT32)
		{
			//数据类型正确才设置参数
			int32_t value = MAKE_UINT32(pRxDOB->pData[3], pRxDOB->pData[2],pRxDOB->pData[1], pRxDOB->pData[0]);
			IF_InternalParam_SetSensorVrmsFactor(value);
			pTxDOB->Status = NoErrorNoData;
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}

static void Dealwith_DOB_InputIrmsFactor(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		int32_t value = IF_InternalParam_GetSensorIrmsFactor();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_SINT32)
		{
			//数据类型正确才设置参数
			int32_t value = MAKE_UINT32(pRxDOB->pData[3], pRxDOB->pData[2],pRxDOB->pData[1], pRxDOB->pData[0]);
			IF_InternalParam_SetSensorIrmsFactor(value);
			pTxDOB->Status = NoErrorNoData;
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_InputPhaseFactor(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		int32_t value = IF_InternalParam_GetSensorPhaseFactor();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_SINT32)
		{
			//数据类型正确才设置参数
			int32_t value = MAKE_UINT32(pRxDOB->pData[3], pRxDOB->pData[2],pRxDOB->pData[1], pRxDOB->pData[0]);
			IF_InternalParam_SetSensorPhaseFactor(value);
			pTxDOB->Status = NoErrorNoData;
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_PIDProportion(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		int32_t value = IF_InternalParam_GetProportion();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_SINT32)
		{
			//数据类型正确才设置参数
			int32_t value = MAKE_UINT32(pRxDOB->pData[3], pRxDOB->pData[2],pRxDOB->pData[1], pRxDOB->pData[0]);
			IF_InternalParam_SetProportion(value);
			pTxDOB->Status = NoErrorNoData;
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_PIDIntegral(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{

	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		int32_t value = IF_InternalParam_GetIntegral();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_SINT32)
		{
			//数据类型正确才设置参数
			int32_t value = MAKE_UINT32(pRxDOB->pData[3], pRxDOB->pData[2],pRxDOB->pData[1], pRxDOB->pData[0]);
			IF_InternalParam_SetIntegral(value);
			pTxDOB->Status = NoErrorNoData;
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_PIDDerivatice(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		int32_t value = IF_InternalParam_GetDerivatice();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_SINT32)
		{
			//数据类型正确才设置参数
			int32_t value = MAKE_UINT32(pRxDOB->pData[3], pRxDOB->pData[2],pRxDOB->pData[1], pRxDOB->pData[0]);
			IF_InternalParam_SetDerivatice(value);
			pTxDOB->Status = NoErrorNoData;
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_PIDErrorThr(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		int32_t value = IF_InternalParam_GetPIDErrorThr();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_SINT32)
		{
			//数据类型正确才设置参数
			int32_t value = MAKE_UINT32(pRxDOB->pData[3], pRxDOB->pData[2],pRxDOB->pData[1], pRxDOB->pData[0]);
			IF_InternalParam_SetPIDErrorThr(value);
			pTxDOB->Status = NoErrorNoData;
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_PIDVSWRLimit(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		int32_t value = IF_InternalParam_GetVSWRLimit();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_SINT32)
		{
			//数据类型正确才设置参数
			int32_t value = MAKE_UINT32(pRxDOB->pData[3], pRxDOB->pData[2],pRxDOB->pData[1], pRxDOB->pData[0]);
			IF_InternalParam_SetVSWRLimit(value);
			pTxDOB->Status = NoErrorNoData;
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_PIDStartPower(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		int32_t value = IF_InternalParam_GetInitPoint();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_SINT32)
		{
			//数据类型正确才设置参数
			int32_t value = MAKE_UINT32(pRxDOB->pData[3], pRxDOB->pData[2],pRxDOB->pData[1], pRxDOB->pData[0]);
			IF_InternalParam_StartPoint(value);
			pTxDOB->Status = NoErrorNoData;
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_ACDCVoltGain(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{

	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		int32_t value = IF_InternalParam_GetACDCVoltGain();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_SINT32)
		{
			//数据类型正确才设置参数
			int32_t value = MAKE_UINT32(pRxDOB->pData[3], pRxDOB->pData[2],pRxDOB->pData[1], pRxDOB->pData[0]);
			IF_InternalParam_SetACDCVoltGain(value);
			pTxDOB->Status = NoErrorNoData;
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_ACDCVoltOffset(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{

	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		int32_t value = IF_InternalParam_GetACDCVoltOffset();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_SINT32)
		{
			//数据类型正确才设置参数
			int32_t value = MAKE_UINT32(pRxDOB->pData[3], pRxDOB->pData[2],pRxDOB->pData[1], pRxDOB->pData[0]);
			IF_InternalParam_SetACDCVoltOffset(value);
			pTxDOB->Status = NoErrorNoData;
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_ACDCCurrentGain(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{

	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		int32_t value = IF_InternalParam_GetACDCCurrentGain();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_SINT32)
		{
			//数据类型正确才设置参数
			int32_t value = MAKE_UINT32(pRxDOB->pData[3], pRxDOB->pData[2],pRxDOB->pData[1], pRxDOB->pData[0]);
			IF_InternalParam_SetACDCCurrentGain(value);
			pTxDOB->Status = NoErrorNoData;
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}

static void Dealwith_DOB_ACDCCurrentOffset(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{

	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		int32_t value = IF_InternalParam_GetACDCCurrentOffset();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_SINT32)
		{
			//数据类型正确才设置参数
			int32_t value = MAKE_UINT32(pRxDOB->pData[3], pRxDOB->pData[2],pRxDOB->pData[1], pRxDOB->pData[0]);
			IF_InternalParam_SetACDCCurrentOffset(value);
			pTxDOB->Status = NoErrorNoData;
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_DrainVoltGain(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{

	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		int32_t value = IF_InternalParam_GetDrainVoltGain();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_SINT32)
		{
			//数据类型正确才设置参数
			int32_t value = MAKE_UINT32(pRxDOB->pData[3], pRxDOB->pData[2],pRxDOB->pData[1], pRxDOB->pData[0]);
			IF_InternalParam_SetDrainVoltGain(value);
			pTxDOB->Status = NoErrorNoData;
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_DrainVoltOffset(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{

	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		int32_t value = IF_InternalParam_GetDrainVoltOffset();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_SINT32)
		{
			//数据类型正确才设置参数
			int32_t value = MAKE_UINT32(pRxDOB->pData[3], pRxDOB->pData[2],pRxDOB->pData[1], pRxDOB->pData[0]);
			IF_InternalParam_SetDrainVoltOffset(value);
			pTxDOB->Status = NoErrorNoData;
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_DCBias(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		int16_t value = IF_Get_Sensor_DCBias();
	
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}


static void Dealwith_DOB_PCBTemperature(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int16_t value = IF_Sensor_GetPCBTemperature();
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}
static void Dealwith_DOB_FanCurrent(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if (rxInfo.Cmd == ParameterRead)
	{	
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		uint16_t value = IF_Timer_GetFanSpeedValue(pTxDOB->SubIndex);
			
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}
static void Dealwith_DOB_24VPowerSupply(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		uint16_t value = IF_Sensor_GetPowerSupply();

		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}

static void Dealwith_DOB_PowerUpTime(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		uint32_t value = IF_Timer_GetPowerUpTimer();
		
        txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
        txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
        txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
        txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);

	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}

static void Dealwith_DOB_RFOnTime(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		uint32_t value = IF_Timer_GetRFOnUpTimer();
		
        txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
        txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
        txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
        txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}

static void Dealwith_DOB_StatusWord(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(g_StatusWord.word);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(g_StatusWord.word);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}

static void Dealwith_DOB_FaultWord(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
			
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(g_FaultWord.word);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(g_FaultWord.word);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}
static void Dealwith_DOB_SensorTemperature(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int16_t value = IF_Fpga_GetSensor(ChnN_Temp)*10;
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}
static void Dealwith_DOB_SensorFreq(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
        
		int32_t value = (IF_Fpga_GetSensor(ChnN_Freq)+0.0005F)*1000;;
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}
static void Dealwith_DOB_SensorVrms(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
        
		int32_t value = (IF_Fpga_GetSensor(ChnN_Vrms)+0.0005F)*1000;;
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}
static void Dealwith_DOB_SensorIrms(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
			
		int32_t value = (IF_Fpga_GetSensor(ChnN_Irms)+0.0005F)*1000;
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}
static void Dealwith_DOB_SensorPhase(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int32_t value = (IF_Fpga_GetSensor(ChnN_Phase)+0.0005F)*1000;
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}

static void Dealwith_DOB_SensorPdlv(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int32_t value = (IF_Fpga_GetSensor(ChnN_Pdlv)+0.0005F)*1000;
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}
static void Dealwith_DOB_SensorPfwd(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int32_t value = (IF_Fpga_GetSensor(ChnN_Pfwd)+0.0005F)*1000;
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}

static void Dealwith_DOB_SensorPref(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}

	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int32_t value = (IF_Fpga_GetSensor(ChnN_Pref)+0.0005F)*1000;
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}
static void Dealwith_DOB_SensorZ(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int32_t value = (IF_Fpga_GetSensor(ChnN_Z)+0.0005F)*1000;
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}

static void Dealwith_DOB_SensorR(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int32_t value = (IF_Fpga_GetSensor(ChnN_R)+0.0005F)*1000;
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}

static void Dealwith_DOB_SensorX(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int32_t value = (IF_Fpga_GetSensor(ChnN_X)+0.0005F)*1000;
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}

static void Dealwith_DOB_SensorVSWR(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int32_t value = (IF_Fpga_GetSensor(ChnN_VSWR)+0.0005F)*1000;
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}

static void Dealwith_DOB_SensorDrainVolt(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		uint16_t value = IF_Fpga_GetSensor(ChnN_Drain);
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(value);
		
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}

static void Dealwith_DOB_MatchWorkMode(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT8;
		pTxDOB->DataLen  = 1;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		uint8_t value = IF_UserParam_GetMatchMode();
		txDOBsDataBuf[txDOBsDataLen++] = value;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT8)
		{
			//数据类型正确才设置参数
			uint8_t value =  pRxDOB->pData[0];
			if (value ==0||value == 1)
			{
				IF_UserParam_SetMatchMode(value);
				pTxDOB->Status = NoErrorNoData;	
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}

static void Dealwith_DOB_ACInputVolt1(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int32_t value = IF_Get_Sensor_ACInputVoltage(ADC_CHNL1);
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}

static void Dealwith_DOB_ACInputVolt2(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int32_t value = IF_Get_Sensor_ACInputVoltage(ADC_CHNL2);
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}
static void Dealwith_DOB_ACInputVolt3(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int32_t value = IF_Get_Sensor_ACInputVoltage(ADC_CHNL3);
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}
static void Dealwith_DOB_ACInputCurrent1(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int32_t value = IF_Get_Sensor_ACInputCurrent(ADC_CHNL1);
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}

static void Dealwith_DOB_ACInputCurrent2(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int32_t value = IF_Get_Sensor_ACInputCurrent(ADC_CHNL2);
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}
static void Dealwith_DOB_ACInputCurrent3(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int32_t value = IF_Get_Sensor_ACInputCurrent(ADC_CHNL3);
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}
static void Dealwith_DOB_PAVoltage1(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int32_t value = IF_Get_Sensor_PAVoltage(ADC_CHNL1);
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}

static void Dealwith_DOB_PAVoltage2(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int32_t value = IF_Get_Sensor_PAVoltage(ADC_CHNL2);
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}
static void Dealwith_DOB_PACurrent1(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int32_t value = IF_Get_Sensor_PACurrent(ADC_CHNL1);
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}

static void Dealwith_DOB_PACurrent2(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int32_t value = IF_Get_Sensor_PACurrent(ADC_CHNL2);
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}
static void Dealwith_DOB_PACurrent3(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int32_t value = IF_Get_Sensor_PACurrent(ADC_CHNL3);
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}
static void Dealwith_DOB_PACurrent4(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int32_t value = IF_Get_Sensor_PACurrent(ADC_CHNL4);
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}

static void Dealwith_DOB_PATemperature1(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int16_t value = IF_Sensor_GetRFTemperature(ADC_CHNL1);
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}

static void Dealwith_DOB_PATemperature2(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int16_t value = IF_Sensor_GetRFTemperature(ADC_CHNL2);
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}
static void Dealwith_DOB_PATemperature3(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int16_t value = IF_Sensor_GetRFTemperature(ADC_CHNL3);
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(value);
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}
static void Dealwith_DOB_PATemperature4(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int16_t value = IF_Sensor_GetRFTemperature(ADC_CHNL4);
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(value);
		
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}

static void Dealwith_DOB_ACDCTemperature1(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int16_t value = IF_Get_Sensor_ACDCTemperature(ADC_CHNL1);
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(value);
		
	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}

static void Dealwith_DOB_ACDCTemperature2(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if(rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		int16_t value = IF_Get_Sensor_ACDCTemperature(ADC_CHNL2);
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(value);

	}else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}
static void Dealwith_DOB_CmdMatchMoveToCapPos(uint8_t capIndex,DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status = NotPermittedParam;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT16)
		{
			//数据类型正确才设置参数
			uint16_t value = MAKE_UINT16(pRxDOB->pData[1], pRxDOB->pData[0]);
			if (value <= 10000&&g_StatusWord.bits.MatchConnected)
			{
				IF_Cmd_SetMatchMoveToPos(capIndex, value);
				pTxDOB->Status = NoErrorNoData;
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}

static void Dealwith_DOB_CmdMatchCapPos(uint8_t capIndex,DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT16;
		pTxDOB->DataLen  = 2;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		
		uint16_t value = IF_CmdParam_GetMatchCurrentPos(capIndex);
		
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		pTxDOB->Status = NotPermittedParam;
	}
}

static void Dealwith_DOB_CmdPowerDrviveState(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status = NotPermittedParam;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT8)
		{
			//数据类型正确才设置参数
			uint8_t value =  pRxDOB->pData[0];
			if (value == 1||value == 0)
			{
				IF_CmdParam_SetDDSDriverState(value);
				pTxDOB->Status = NoErrorNoData;
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}

static void Dealwith_DOB_CmdSetWorkMode(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT8;
		pTxDOB->DataLen  = 1;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		uint8_t value = IF_CmdParam_GetPowerWorkMode();
		txDOBsDataBuf[txDOBsDataLen++] = value;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT8)
		{
			//数据类型正确才设置参数
			uint8_t value =  pRxDOB->pData[0];
			if (value == 1||value ==0)
			{
				IF_CmdParam_SetPowerWorkMode(value);
				pTxDOB->Status = NoErrorNoData;
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_CmdPowerState(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status = NotPermittedParam;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT8)
		{
			//数据类型正确才设置参数
			uint8_t value =  pRxDOB->pData[0];
			if(value== 1||value ==0)
			{
				IF_CmdParam_SetRFPowerState(value);
				pTxDOB->Status = NoErrorNoData;
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_CmdDDSDrviveState(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT8;
		pTxDOB->DataLen  = 1;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		uint8_t value = IF_CmdParam_GetDDSDriverState();
		txDOBsDataBuf[txDOBsDataLen++] = value;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT8)
		{
			//数据类型正确才设置参数
			uint8_t value =  pRxDOB->pData[0];
			if (value == 1||value == 0)
			{
				IF_CmdParam_SetDDSDriverState(value);
				pTxDOB->Status = NoErrorNoData;				
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_CmdACDCDrviveState(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		uint8_t value;

		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT8;
		pTxDOB->DataLen  = 1;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		value = IF_CmdParam_GetACDCDriverState();
		txDOBsDataBuf[txDOBsDataLen++] = value;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT8)
		{
			//数据类型正确才设置参数
			uint8_t value =  pRxDOB->pData[0];
			if (value == 1||value == 0)
			{	
				IF_CmdParam_SetACDCDriverState(value);
				pTxDOB->Status = NoErrorNoData;				
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}

static void Dealwith_DOB_CmdSensorChannelNo(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT8;
		pTxDOB->DataLen  = 1;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
		uint8_t value = IF_CmdParam_GetSensorChannelNo();
		txDOBsDataBuf[txDOBsDataLen++] = value;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT8)
		{
			//数据类型正确才设置参数
			uint8_t value =  pRxDOB->pData[0];
			IF_CmdParam_SetSensorChannelNo(value);
			pTxDOB->Status = NoErrorNoData;
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_CmdWorkFrequency(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		uint32_t value = IF_CmdParam_GetWorkFrequency();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_SINT32)
		{
			//数据类型正确才设置参数
			uint32_t value =  MAKE_UINT32(pRxDOB->pData[3],pRxDOB->pData[2],pRxDOB->pData[1],pRxDOB->pData[0]);	
			IF_CmdParam_SetWorkFrequency(value);
			pTxDOB->Status = NoErrorNoData;
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_CmdWorkPhase(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_SINT32;
		pTxDOB->DataLen  = 4;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		uint32_t value = IF_CmdParam_GetWorkPhase();
		txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte2_UINT32(value);
		txDOBsDataBuf[txDOBsDataLen++] = Byte3_UINT32(value);
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_SINT32)
		{
			//数据类型正确才设置参数
			uint32_t value =  MAKE_UINT32(pRxDOB->pData[3],pRxDOB->pData[2],pRxDOB->pData[1],pRxDOB->pData[0]);	
			IF_CmdParam_SetWorkPhase(value);
			pTxDOB->Status = NoErrorNoData;
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}

static void Dealwith_DOB_CmdLocalTime(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}

	if (rxInfo.Cmd == ParameterRead)
	{
		pTxDOB->Status = NotPermittedParam;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT32)
		{
			//数据类型正确才设置参数
			uint32_t value = MAKE_UINT32(pRxDOB->pData[3],pRxDOB->pData[2],pRxDOB->pData[1],pRxDOB->pData[0]);
			IF_HAL_DS1339_SetTimer((time_t)value);
			pTxDOB->Status = NoErrorNoData;		
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}
static void Dealwith_DOB_FactoryMode(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if ((pTxDOB->SubIndex) != 0x01)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}

	if (rxInfo.Cmd == ParameterRead)
	{
		uint8_t value;

		pTxDOB->Status   = NoErrorHaveData;
		pTxDOB->DataType = DataType_UINT8;
		pTxDOB->DataLen  = 1;
		pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];

		value = IF_CmdParam_GetFactoryMode();
		txDOBsDataBuf[txDOBsDataLen++] = value;
	}
	else if (rxInfo.Cmd == ParameterWrite)
	{
		if (pRxDOB->DataType == DataType_UINT8)
		{
			//数据类型正确才设置参数
			uint8_t value = pRxDOB->pData[0];
			if ((value == 0) || (value == 1))
			{
				IF_CmdParam_SetFactoryMode(value);
				pTxDOB->Status = NoErrorNoData;
			}
			else
			{
				pTxDOB->Status = CheckFailedParam;
			}
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
}


static void Dealwith_UnknownDOB(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	pTxDOB->Status = UnknownParam;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
static void DataBlockRead_ResponseOK(
		DataObject_t* pTxDOB,
		uint8_t datablockFlag,
		uint8_t datablockID,
		uint16_t frameNo,
		uint8_t* data,
		uint8_t len)
{
	pTxDOB->Index = MAKE_UINT16(datablockFlag, datablockID);
	pTxDOB->SubIndex = 0x00;

	pTxDOB->Status   = NoErrorHaveData;
	pTxDOB->DataType = DataType_VECT;

	pTxDOB->pData    = &txDOBsDataBuf[txDOBsDataLen];
	pTxDOB->pData[0] = len + 2;
	pTxDOB->pData[1] = Byte0_UINT16(frameNo);
	pTxDOB->pData[2] = Byte1_UINT16(frameNo);
	memcpy(&pTxDOB->pData[3], data, len);

	pTxDOB->DataLen  = len + 3;

	txDOBsDataLen += pTxDOB->DataLen;
}

static void Dealwith_Datablock_AutoMatchProcessData(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	uint16_t frameNo;
	uint8_t dataBlockFlag;
	if (pRxDOB->SubIndex != 0x00)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}
	if (rxInfo.Cmd == DataBlockRead)
	{
		if (pRxDOB->DataType != DataType_UINT16)
		{
			pTxDOB->Status = CheckFailedParam;
			return;
		}

		dataBlockFlag = Byte1_UINT16(pRxDOB->Index);
		if (dataBlockFlag != NotUsed)
		{
			pTxDOB->Status = CheckFailedParam;
			return;
		}

		frameNo = MAKE_UINT16(pRxDOB->pData[1], pRxDOB->pData[0]);

		if (frameNo == 0)
		{
			ExpectedFrameNo = 0;
			ExpectedDataBlockID = ID_ProcessData;
			ExpectedDataBlockCmd = DataBlockRead;

			txDataBlockRemainLen = IF_GetTotalAMPDLength();
		}
		else
		{
			if (ExpectedFrameNo == 0)
			{
				pTxDOB->Status = CheckFailedParam;
				return;
			}

			if (ExpectedDataBlockCmd != DataBlockRead)
			{
				ExpectedFrameNo = 0;
				pTxDOB->Status = NotAcceptedCommand;
				return;
			}

			if (ExpectedDataBlockID != ID_ProcessData)
			{
				ExpectedFrameNo = 0;
				pTxDOB->Status = IllegalDatablock;
				return;
			}
		}

		if (frameNo != ExpectedFrameNo)
		{
			ExpectedFrameNo = 0;
			pTxDOB->Status = CheckFailedParam;
			return;
		}

		memset(NvmBuffer, 0, FLASH_BUF_SIZE);
		NvmBufIndex = 0;

		NvmBufIndex = IF_GetAMPD(frameNo, NvmBuffer);

		txDataBlockRemainLen -= NvmBufIndex;

		if (txDataBlockRemainLen > 0)
		{
			DataBlockRead_ResponseOK(pTxDOB, MoreBlock, ID_ProcessData,
					frameNo, NvmBuffer, NvmBufIndex);

			ExpectedFrameNo++;
		}
		else
		{
			DataBlockRead_ResponseOK(pTxDOB, LastBlock, ID_ProcessData,
					frameNo, NvmBuffer, NvmBufIndex);

			ExpectedFrameNo = 0;
		}
	}
	else if (rxInfo.Cmd == DataBlockWrite)
	{
		pTxDOB->Status = IllegalDatablock;
	}
}
static void Dealwith_Datablock_Firmware(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	uint16_t frameNo;

	if (pRxDOB->SubIndex != 0x00)
	{
		pTxDOB->Status = UnknownSubIndex;
		return;
	}

	if (rxInfo.Cmd == DataBlockRead)
	{
		pTxDOB->Status = NotPermittedParam;
		return;
	}

	if (pRxDOB->DataType != DataType_VECT)
	{
		pTxDOB->Status = CheckFailedParam;
		return;
	}

	frameNo = MAKE_UINT16(pRxDOB->pData[1], pRxDOB->pData[0]);

	if (frameNo == 0)
	{
		ExpectedFrameNo = 0;
		ExpectedDataBlockID = ID_Firmware;
		ExpectedDataBlockCmd = DataBlockWrite;
		
		NvmBufIndex = 0;
		DownloadFileIndex = 0;
		DownFileLen = 0;
	}
	else
	{
		if (ExpectedFrameNo == 0)
		{
			pTxDOB->Status = CheckFailedParam;
			return;
		}

		if (ExpectedDataBlockCmd != DataBlockWrite)
		{
			ExpectedFrameNo = 0;
			pTxDOB->Status = NotAcceptedCommand;
			return;
		}

		if (ExpectedDataBlockID != ID_Firmware)
		{
			ExpectedFrameNo = 0;
			pTxDOB->Status = IllegalDatablock;
			return;
		}
	}

	if (frameNo == ExpectedFrameNo)
	{
		ExpectedFrameNo++;
		DataBlockFlag = Byte1_UINT16(pRxDOB->Index);

		if ((DataBlockFlag == MoreBlock) || (DataBlockFlag == LastBlock))
		{
			uint8_t i;

			pTxDOB->Index = MAKE_UINT16(NotUsed, ID_Firmware);
			pTxDOB->SubIndex = 0x00;

			//处理数据
			for (i = 2; i < pRxDOB->DataLen; i++)
			{
				//记录文件标识
				if ((DownloadFileIndex >= APP_FILE_SIGN_OFFSET)
						&& (DownloadFileIndex < (APP_FILE_SIGN_OFFSET + APP_FILE_SIGN_LEN)))
				{
				    DownFileSign[DownloadFileIndex - APP_FILE_SIGN_OFFSET] = pRxDOB->pData[i];
				}

				//接收到完整的识别码后
				if(DownloadFileIndex == (APP_FILE_SIGN_OFFSET + APP_FILE_SIGN_LEN))
				{
					//如果接受的文件识别码与设置的识别码不相同
					if (0 == memcmp(DownFileSign, McuFileSign,APP_FILE_SIGN_LEN))
					{
						//记录下载文件长度
						DownFileLen = MAKE_UINT32(NvmBuffer[11],NvmBuffer[10],NvmBuffer[9],NvmBuffer[8]);
						if ((DownFileLen == 0) || (DownFileLen > MAX_APP_FILE_SIZE))
						{
							pTxDOB->Status = DataBlockLengthError;
							return;
						}else
						{
							FirmwareFlag = 1; //升级状态MCU
						}
					}else if (0 == memcmp(DownFileSign, FpgaFileSign,APP_FILE_SIGN_LEN)) 
					{
						FirmwareFlag = 2;//升级状态FPGA
					}else if (0 == memcmp(DownFileSign, ModbusFileSign,APP_FILE_SIGN_LEN)) 
					{
						FirmwareFlag = 3;//升级状态FPGA
					}else
					{
						FirmwareFlag = 0;
						pTxDOB->Status = HardwareError;
						return;
					}
				}
				//把数据放入缓冲区
				NvmBuffer[NvmBufIndex++] = pRxDOB->pData[i];
				if(NvmBufIndex == FLASH_BUF_SIZE && FirmwareFlag == 1)
				{
					//缓冲区满了，把数据写入Flash
					if (!IF_NvmParam_WriteDownloadFile(
							DownloadFileIndex - FLASH_BUF_SIZE + 1,
							(uint16_t *)NvmBuffer,
							FLASH_BUF_SIZE/2))
					{
						pTxDOB->Status = HardwareError;
						return;
					}
					NvmBufIndex = 0;
				} 
				DownloadFileIndex++;
			}
			if(FirmwareFlag == 1)  //Upgrade MCU software
			{
				//如果是最后一帧
				if (DataBlockFlag == LastBlock)
				{	
					if(NvmBufIndex != 0)
					{
						if (!IF_NvmParam_WriteDownloadFile(
								DownloadFileIndex - NvmBufIndex,
								(uint16_t *)NvmBuffer,
								FLASH_BUF_SIZE/2))
						{
							pTxDOB->Status = HardwareError;
							return;
						}
					}

					if (DownloadFileIndex != IF_NvmParam_ReadDownloadFileLen())
					{
						pTxDOB->Status = DataBlockLengthError;
						return;
					}

					if (!IF_NvmParam_CheckDownloadFile())
					{
						pTxDOB->Status = DataBlockCRCError;
						return;
					}
					// 设置升级标识
					if (!IF_NvmParam_WriteDownloadFlag())
					{
						pTxDOB->Status = HardwareError;
						return;
					}

					IF_CmdParam_SetResetDevice();	
				}
			}else if(FirmwareFlag == 2)   // Upgrade FPGA  software
			{
				BSIPInfo_t txBsipFrame;	
				txBsipFrame.bsip.msgclass=(MsgClassEnum)0x20;  		   //msg_class_command 
				txBsipFrame.bsip.msgid = 0xF1;	   		   			   //msg_id     
				txBsipFrame.bsip.subindex.byte=0x00;	   			   //msg_subindex 	
				txBsipFrame.bsip.info = NvmBuffer;								
				if(frameNo == 0)
				{					
					txBsipFrame.bsip.info[txBsipFrame.infoLen++] = FIRMWARE_SOF;
				}else if(DataBlockFlag == LastBlock)
				{
					txBsipFrame.bsip.info[txBsipFrame.infoLen++] = FIRMWARE_EOF;
				}else
				{
					txBsipFrame.bsip.info[txBsipFrame.infoLen++] = FIRMWARE_COF;
				}  
				txBsipFrame.bsip.info[txBsipFrame.infoLen++] = pRxDOB->DataLen - 2;	   //FrameLength
				txBsipFrame.bsip.info[txBsipFrame.infoLen++] = Byte0_UINT16(frameNo);  //FrameNo
				txBsipFrame.bsip.info[txBsipFrame.infoLen++] = Byte1_UINT16(frameNo);  //FrameNo
				for(uint8_t i = 0;i< NvmBufIndex;i++)
				{					
					txBsipFrame.bsip.info[txBsipFrame.infoLen++] = NvmBuffer[i];
				}
				txBsipFrame.infoLen = txBsipFrame.infoLen + 3;	 //msg_class_command + msg_id + msg_subindex
				
				if(FpgaUpdate_NoAck == 0)  
				{								
					xQueueSend(FpgaQueue,&txBsipFrame,(TickType_t)0);	
				}else
				{
					FpgaUpdate_NoAck = 1;
					pTxDOB->Status = HardwareError;
					return;	
				}			
			}else if(FirmwareFlag == 3)   // Upgrade Modbus  software
			{
				memset(&NvmBuffer,0,FLASH_BUF_SIZE);					
				if(frameNo == 0)
				{					
					NvmBuffer[NvmBufIndex++] = FIRMWARE_SOF;
				}else
				{
					NvmBuffer[NvmBufIndex++] =(DataBlockFlag == LastBlock)? FIRMWARE_EOF: FIRMWARE_COF;
				} 
				NvmBuffer[NvmBufIndex++] = pRxDOB->DataLen - 2;	
				for(i = 0;i < pRxDOB->DataLen;i++)
				{
					NvmBuffer[NvmBufIndex++]= pRxDOB->pData[i];
				}		
				if(FpgaUpdate_NoAck == 0)  
				{
					BSIPInfo_t txBsipFrame;	
					txBsipFrame.bsip.msgclass=(MsgClassEnum)0x20;  		   //msg_class_command 
					txBsipFrame.bsip.msgid = 0xF1;	   		   			   //msg_id     
					txBsipFrame.bsip.subindex.byte=0x00;	   			   //msg_subindex 	
					txBsipFrame.infoLen = NvmBufIndex + 3;              //msg_class_command+msg_id+msg_subindex 	
					txBsipFrame.bsip.info = NvmBuffer;												
					xQueueSend(FpgaQueue,&txBsipFrame,(TickType_t)0);	
				}else
				{
					FpgaUpdate_NoAck = 1;
					pTxDOB->Status = HardwareError;
					return;	
				}			
			}else
			{
				pTxDOB->Status = HardwareError;
				return;	
			} 
			//正确响应的DOB
			pTxDOB->Status = NoErrorHaveData;
			pTxDOB->DataType = DataType_UINT16;
			pTxDOB->DataLen = 2;
			pTxDOB->pData = &txDOBsDataBuf[txDOBsDataLen];
			txDOBsDataBuf[txDOBsDataLen++] = Byte0_UINT16(frameNo);
			txDOBsDataBuf[txDOBsDataLen++] = Byte1_UINT16(frameNo);
			
		}
		else
		{
			pTxDOB->Status = CheckFailedParam;
		}
	}
	else
	{
		pTxDOB->Status = CheckFailedParam;
	}
}

static void Dealwith_UnknownDatablock(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	pTxDOB->Index = pRxDOB->Index;
	pTxDOB->SubIndex = pRxDOB->SubIndex;
	pTxDOB->Status = UnknownDatablockID;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
static void ParseOneParamDOB(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	if (!g_GS[rxInfo.port].bits.active)
	{
		pTxDOB->Status = NotAcceptedCommand;
		return;
	}

	switch(pTxDOB->Index)
	{
	//--------------  Common Parameters  -----------------
	// Identification parameters
	case ID_DeviceType:
		Dealwith_DOB_DeviceType(pRxDOB, pTxDOB);
		break;
	case ID_VendorCode:
		Dealwith_DOB_VendorCode(pRxDOB, pTxDOB);
		break;
	case ID_HardwareVersion:
		Dealwith_DOB_HardwareVersion(pRxDOB, pTxDOB);
		break;
	case ID_FpgawareVersion:
		Dealwith_DOB_FpgawareVersion(pRxDOB, pTxDOB);
		break;
	case ID_SoftwareVersion:
		Dealwith_DOB_SoftwareVersion(pRxDOB, pTxDOB);
		break;
	case ID_BootloaderVersion:
		Dealwith_DOB_BootloaderVersion(pRxDOB, pTxDOB);
		break;
	case ID_ProductModel:
		Dealwith_DOB_ProductModel(pRxDOB, pTxDOB);
		break;
	case ID_SerialNumber:
		Dealwith_DOB_SerialNumber(pRxDOB, pTxDOB);
		break;
	case ID_TrackingNumber:
		Dealwith_DOB_TrackingNumber(pRxDOB, pTxDOB);
		break;
	case ID_CheckSum:
		Dealwith_DOB_CheckSum(pRxDOB, pTxDOB);
		break;

	// Communication parameters
	case ID_DeviceAddress:
		Dealwith_DOB_DeviceAddress(pRxDOB, pTxDOB);
		break;
	case ID_DataRate:
		Dealwith_DOB_DataRate(pRxDOB, pTxDOB);
		break;
	case ID_NET_MAC:
		Dealwith_DOB_NET_MAC(pRxDOB, pTxDOB);
		break;
	case ID_NET_IP:
		Dealwith_DOB_NET_IP(pRxDOB, pTxDOB);
		break;
	case ID_NET_MASK:
		Dealwith_DOB_NET_Mask(pRxDOB, pTxDOB);
		break;
	case ID_NET_Gateway:
		Dealwith_DOB_NET_Gateway(pRxDOB, pTxDOB);
		break;
	case ID_EtherCATAddess:
		Dealwith_DOB_EtherCatAddress(pRxDOB, pTxDOB);
		break;
	
	// Executable Action
	case ID_Reset:
		Dealwith_DOB_Reset(pRxDOB, pTxDOB);
		break;
	case ID_ResumeFactorySettings:
		Dealwith_DOB_ResumeFactorySettings(pRxDOB, pTxDOB);
		break;

	//--------------  RF Power Parameters  -----------------
	case ID_RegulationMode:
		Dealwith_DOB_RegulationMode(pRxDOB, pTxDOB);
		break;
	case ID_CommunicateType:
		Dealwith_DOB_CommunicateType(pRxDOB, pTxDOB);
		break;
	case ID_SlowMode:
		Dealwith_DOB_SlowMode(pRxDOB, pTxDOB);
		break;
	case ID_SlowStartDelay:
		Dealwith_DOB_SlowStartDelay(pRxDOB, pTxDOB);
		break;
	case ID_SlowStopDelay:
		Dealwith_DOB_SlowStopDelay(pRxDOB, pTxDOB);
		break;
	case ID_PfwdLimit:
		Dealwith_DOB_ForwardPowerLimit(pRxDOB, pTxDOB);
		break;
	case ID_PrefLimit:
		Dealwith_DOB_ReflectPowerLimit(pRxDOB, pTxDOB);
		break;
	case ID_PvdcLimit:
		Dealwith_DOB_VdcPowerLimit(pRxDOB, pTxDOB);
		break;
	case ID_PrefThrMode:
		Dealwith_DOB_ReflectPowerSwitchOff(pRxDOB, pTxDOB);
		break;
	case ID_PrefThreshold:
		Dealwith_DOB_ReflectPowerThreshold(pRxDOB, pTxDOB);
		break;
	case ID_DelayReachTime:
		Dealwith_DOB_DelayReachTime(pRxDOB, pTxDOB);
		break;
	case ID_PulseMode:
		Dealwith_DOB_PulseMode(pRxDOB, pTxDOB);
		break;
	case ID_PulseFreq:
		Dealwith_DOB_PulseFrequency(pRxDOB, pTxDOB);
		break;
	case ID_PulseDuty:
		Dealwith_DOB_PulseDutyCircle(pRxDOB, pTxDOB);
		break;
	case ID_FpgaPulsePowerThr:
		Dealwith_DOB_FpgaPulsePowerThr(pRxDOB, pTxDOB);
		break;
	case ID_FpgaPulseSyncDelay:
		Dealwith_DOB_FpgaPulseSyncDelay(pRxDOB, pTxDOB);
		break;
	// Configurable Internal Parameters
	case ID_AnalogVoltageRange:
		Dealwith_DOB_AnalogVoltageRange(pRxDOB, pTxDOB);
		break;

	case ID_InputVDCFactor:
		Dealwith_DOB_InputVDCFactor(pRxDOB, pTxDOB);
		break;
	case ID_InputVrmsFactor:
		Dealwith_DOB_InputVrmsFactor(pRxDOB, pTxDOB);
		break;
    case ID_InputIrmsFactor:
        Dealwith_DOB_InputIrmsFactor(pRxDOB, pTxDOB);
        break;

    case ID_InputPhaseFactor:
        Dealwith_DOB_InputPhaseFactor(pRxDOB, pTxDOB);
        break;
    case ID_PIDProportion:
        Dealwith_DOB_PIDProportion(pRxDOB, pTxDOB);
        break;
	case ID_PIDIntegral:
		Dealwith_DOB_PIDIntegral(pRxDOB, pTxDOB);
		break;
	case ID_PIDDerivatice:
		Dealwith_DOB_PIDDerivatice(pRxDOB, pTxDOB);
		break;
	case ID_PIDErrorThr:
		Dealwith_DOB_PIDErrorThr(pRxDOB, pTxDOB);
		break;
	case ID_VSWRLimit:
		Dealwith_DOB_PIDVSWRLimit(pRxDOB, pTxDOB);
		break;
	case ID_StartPower:
		Dealwith_DOB_PIDStartPower(pRxDOB, pTxDOB);
		break;
	
	case ID_ACDCVoltGain:
		Dealwith_DOB_ACDCVoltGain(pRxDOB, pTxDOB);
		break;
    case ID_ACDCVoltOffset:
        Dealwith_DOB_ACDCVoltOffset(pRxDOB, pTxDOB);
        break;
	case ID_ACDCCurrentGain:
		Dealwith_DOB_ACDCCurrentGain(pRxDOB, pTxDOB);
		break;
    case ID_ACDCCurrentOffset:
        Dealwith_DOB_ACDCCurrentOffset(pRxDOB, pTxDOB);
        break;
	case ID_DrainVoltGain:
		Dealwith_DOB_DrainVoltGain(pRxDOB, pTxDOB);
		break;
    case ID_DrainVoltOffset:
        Dealwith_DOB_DrainVoltOffset(pRxDOB, pTxDOB);
        break;
    // Readonly Monitor Parameters
	case ID_MatchDCBias:
		Dealwith_DOB_DCBias(pRxDOB, pTxDOB);
		break;
	case ID_PCBTemperature:
		Dealwith_DOB_PCBTemperature(pRxDOB, pTxDOB);
		break;
	case ID_FanCurrent:
		Dealwith_DOB_FanCurrent(pRxDOB, pTxDOB);
		break;
	case ID_24VPowerSupply:
		Dealwith_DOB_24VPowerSupply(pRxDOB, pTxDOB);
		break;

	case ID_PowerUpTime:
		Dealwith_DOB_PowerUpTime(pRxDOB, pTxDOB);
		break;
	case ID_RFOnTime:
		Dealwith_DOB_RFOnTime(pRxDOB, pTxDOB);
		break;

	case ID_StatusWord:
		Dealwith_DOB_StatusWord(pRxDOB, pTxDOB);
		break;
	case ID_FaultWord:
		Dealwith_DOB_FaultWord(pRxDOB, pTxDOB);
		break;
	//Sensor value
	case ID_SensorFreq:
		Dealwith_DOB_SensorFreq(pRxDOB, pTxDOB);
		break;
	case ID_SensorVrms:
		Dealwith_DOB_SensorVrms(pRxDOB, pTxDOB);
		break;

	case ID_SensorIrms:
		Dealwith_DOB_SensorIrms(pRxDOB, pTxDOB);
		break;

	case ID_SensorPhase:
		Dealwith_DOB_SensorPhase(pRxDOB, pTxDOB);
		break;

	case ID_SensorPdlv:
		Dealwith_DOB_SensorPdlv(pRxDOB, pTxDOB);
		break;

	case ID_SensorPfwd:
		Dealwith_DOB_SensorPfwd(pRxDOB, pTxDOB);
		break;

	case ID_SensorPref:
		Dealwith_DOB_SensorPref(pRxDOB, pTxDOB);
		break;

	case ID_SensorZ:
		Dealwith_DOB_SensorZ(pRxDOB, pTxDOB);
		break;

	case ID_SensorR:
		Dealwith_DOB_SensorR(pRxDOB, pTxDOB);
		break;

	case ID_SensorX:
		Dealwith_DOB_SensorX(pRxDOB, pTxDOB);
		break;
	case ID_SensorVSWR:
		Dealwith_DOB_SensorVSWR(pRxDOB, pTxDOB);
		break;
	case ID_SensorDrain:
		Dealwith_DOB_SensorDrainVolt(pRxDOB, pTxDOB);
		break;
	case ID_SensorTemp:
		Dealwith_DOB_SensorTemperature(pRxDOB, pTxDOB);
		break;
	case ID_ACInputVolt1:
		Dealwith_DOB_ACInputVolt1(pRxDOB, pTxDOB);
		break;
	case ID_ACInputVolt2:
		Dealwith_DOB_ACInputVolt2(pRxDOB, pTxDOB);
		break;
	case ID_ACInputVolt3:
		Dealwith_DOB_ACInputVolt3(pRxDOB, pTxDOB);
		break;
	case ID_ACInputCurrent1:
		Dealwith_DOB_ACInputCurrent1(pRxDOB, pTxDOB);
		break;
	case ID_ACInputCurrent2:
		Dealwith_DOB_ACInputCurrent2(pRxDOB, pTxDOB);
		break;
	case ID_ACInputCurrent3:
		Dealwith_DOB_ACInputCurrent3(pRxDOB, pTxDOB);
		break;
	case ID_PAVoltage1:
		Dealwith_DOB_PAVoltage1(pRxDOB, pTxDOB);
		break;
	case ID_PAVoltage2:
		Dealwith_DOB_PAVoltage2(pRxDOB, pTxDOB);
		break;
	case ID_PACurrent1:
		Dealwith_DOB_PACurrent1(pRxDOB, pTxDOB);
		break;
	case ID_PACurrent2:
		Dealwith_DOB_PACurrent2(pRxDOB, pTxDOB);
		break;
	case ID_PACurrent3:
		Dealwith_DOB_PACurrent3(pRxDOB, pTxDOB);
		break;
	case ID_PACurrent4:
		Dealwith_DOB_PACurrent4(pRxDOB, pTxDOB);
		break;
	case ID_PATemperature1:
		Dealwith_DOB_PATemperature1(pRxDOB, pTxDOB);
		break;
	case ID_PATemperature2:
		Dealwith_DOB_PATemperature2(pRxDOB, pTxDOB);
		break;
	case ID_PATemperature3:
		Dealwith_DOB_PATemperature3(pRxDOB, pTxDOB);
		break;
	case ID_PATemperature4:
		Dealwith_DOB_PATemperature4(pRxDOB, pTxDOB);
		break;
	case ID_ACDCTemperature1:
		Dealwith_DOB_ACDCTemperature1(pRxDOB, pTxDOB);
		break;
	case ID_ACDCTemperature2:
		Dealwith_DOB_ACDCTemperature2(pRxDOB, pTxDOB);
		break;	
	// Control and Other Parameters
	case ID_SetWorkMode:
		Dealwith_DOB_CmdSetWorkMode(pRxDOB, pTxDOB);
		break;
	case ID_SetVoltage:
		Dealwith_DOB_CmdSetVolotage(pRxDOB, pTxDOB);
	break;
	case ID_SetPoint:
		Dealwith_DOB_CmdSetPoint(pRxDOB, pTxDOB);
		break;
	case ID_CmdACDCVoltage:
		Dealwith_DOB_CmdACDCVolotage(pRxDOB, pTxDOB);
		break;
	case ID_CmdACDCCurrent:
		Dealwith_DOB_CmdACDCCurrent(pRxDOB, pTxDOB);
		break;
	case ID_CmdACDCDriveState:
		Dealwith_DOB_CmdACDCDrviveState(pRxDOB, pTxDOB);
		break;
	case ID_RFState:
		Dealwith_DOB_CmdPowerState(pRxDOB, pTxDOB);
		break;
	case ID_CmdDDSState:
		Dealwith_DOB_CmdDDSDrviveState(pRxDOB, pTxDOB);
		break;	
	case ID_SensorChannelNo:
		Dealwith_DOB_CmdSensorChannelNo(pRxDOB, pTxDOB);
		break;
	case ID_CmdWorkFreq:
		Dealwith_DOB_CmdWorkFrequency(pRxDOB, pTxDOB);
		break;
	case ID_CmdWorkPhase:
		Dealwith_DOB_CmdWorkPhase(pRxDOB, pTxDOB);
		break;
	case ID_FactoryMode:
		Dealwith_DOB_FactoryMode(pRxDOB, pTxDOB);
		break;
	case ID_RTCStamps:
		Dealwith_DOB_CmdLocalTime(pRxDOB, pTxDOB);
		break;
	case ID_RFDriveState:
		Dealwith_DOB_CmdPowerDrviveState(pRxDOB, pTxDOB);
		break;
	case ID_MatchMode:
		Dealwith_DOB_MatchWorkMode(pRxDOB, pTxDOB);
		break;
	case ID_Load_MoveToCapPos:
		Dealwith_DOB_CmdMatchMoveToCapPos(LOAD,pRxDOB, pTxDOB);
		break;
	case ID_Tune_MoveToCapPos:
		Dealwith_DOB_CmdMatchMoveToCapPos(TUNE,pRxDOB, pTxDOB);
		break;
	case ID_Load_ActualCapPos:
		Dealwith_DOB_CmdMatchCapPos(LOAD,pRxDOB, pTxDOB);
		break;
	case ID_Tune_ActualCapPos:
		Dealwith_DOB_CmdMatchCapPos(TUNE,pRxDOB, pTxDOB);
		break;
	default:
		Dealwith_UnknownDOB(pRxDOB, pTxDOB);
		break;
	}
}

static void ParseOneDataBlock(DataObject_t* pRxDOB, DataObject_t* pTxDOB)
{
	pTxDOB->Index = pRxDOB->Index;
	pTxDOB->SubIndex = pRxDOB->SubIndex;

	if (!g_GS[rxInfo.port].bits.active)
	{
		pTxDOB->Status = NotAcceptedCommand;
		return;
	}

	switch (Byte0_UINT16(pRxDOB->Index))
	{
	case ID_ProcessData:
		Dealwith_Datablock_AutoMatchProcessData(pRxDOB, pTxDOB);
		break;
	case ID_Firmware:
		Dealwith_Datablock_Firmware(pRxDOB, pTxDOB);
		break;

	default:
		Dealwith_UnknownDatablock(pRxDOB, pTxDOB);
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//返回: 1成功， 0失败
static uint8_t ParseRxDataObjects(FGIPv2Frame_t *pRxFrame)
{
	uint8_t* dobsBuf;
	uint8_t dobsBufLen;
	uint8_t dobsBufIndex;
	uint8_t remainLen;
	uint8_t dobDataLen;
	
	dobsBuf = &pRxFrame->info[2];
	dobsBufLen = pRxFrame->infoLen - 2;
	remainLen = dobsBufLen;
	dobsBufIndex = 0;
	dobDataLen = 0;
	
	while(dobsBufIndex < dobsBufLen)
	{
		if (remainLen >= 4)  //确保下标不会越界
		{
			rxInfo.DOBs[rxInfo.NumOfDOBs].Index = MAKE_UINT16(dobsBuf[dobsBufIndex + 1], dobsBuf[dobsBufIndex]);
			rxInfo.DOBs[rxInfo.NumOfDOBs].SubIndex = dobsBuf[dobsBufIndex + 2];
			rxInfo.DOBs[rxInfo.NumOfDOBs].Status = dobsBuf[dobsBufIndex + 3];

			dobsBufIndex += 4;
			remainLen -= 4;

			if (rxInfo.DOBs[rxInfo.NumOfDOBs].Status == 0x00)
			{
				if (remainLen > 1)  //确保下标不会越界
				{
					rxInfo.DOBs[rxInfo.NumOfDOBs].DataType = dobsBuf[dobsBufIndex];

					dobDataLen = GetDataLen(rxInfo.DOBs[rxInfo.NumOfDOBs].DataType);

					if (dobDataLen == MAX_VECT_LEN)   //如果是VECT类型
					{
						dobDataLen = dobsBuf[dobsBufIndex + 1];
                        if (dobDataLen >= MAX_VECT_LEN)
                        {
                        	// VECT的长度超过最大值
                        	return 0;
                        }

						if (dobDataLen <= (remainLen - 2) )   //确保下标不会越界
						{
							rxInfo.DOBs[rxInfo.NumOfDOBs].DataLen = dobDataLen;
							rxInfo.DOBs[rxInfo.NumOfDOBs].pData = &dobsBuf[dobsBufIndex + 2];

							dobsBufIndex += (dobDataLen + 2);
							remainLen -= (dobDataLen + 2);

							rxInfo.NumOfDOBs++;    //一个VECT类型的DOB解析完成
						}
						else
						{
							// VECT类型数据，指示的长度大于后面剩余的数据长度
							return 0;
						}
					}
					else if (dobDataLen > 0)
					{
						if (dobDataLen <= (remainLen - 1))
						{
							rxInfo.DOBs[rxInfo.NumOfDOBs].DataLen = dobDataLen;
							rxInfo.DOBs[rxInfo.NumOfDOBs].pData = &dobsBuf[dobsBufIndex + 1];

							dobsBufIndex += (dobDataLen + 1);
							remainLen -= (dobDataLen + 1);

							rxInfo.NumOfDOBs++;    //一个简单类型的DOB解析完成
						}
						else
						{
							// 简单数据类型，其要求的长度大于后面剩余的数据长度 (Frame Format Error)
							return 0;
						}
					}
					else
					{
						// DOB带数据，但数据类型不在定义的范围内 (Frame Format Error)
						return 0;
					}

				}
				else
				{
					// DOB带数据，但不够最小的数据长度 (Frame Format Error)
					return 0;

				}
			}
			else if (rxInfo.DOBs[rxInfo.NumOfDOBs].Status == 0x7F)
			{
				rxInfo.NumOfDOBs++;    //一个不带数据的DOB解析完成
			}
			else
			{
				// DOB的Status为0x00和0x7F之外的其他值 (Frame Format Error)
				return 0;
			}
		}
		else
		{
			// 不够一个DOB的长度 (Frame Format Error)
			return 0;
		}
	}
	return 1;  //所有DOB成功解析完成
}

///////////////////////////////////////////////////////////////////////////////////////////////////
static void SendLayer7Frame(PInfoFiled_t pInfoField)
{
	uint8_t i, j;
	uint8_t port = pInfoField->port;
	memset(txLayer2DataBuf, 0, MAX_INFO_LEN + 2);
	txLayer2DataLen = 0;

	txLayer2DataBuf[txLayer2DataLen++] = IF_CommParam_GetDeviceAddress();    // Addr
	txLayer2DataBuf[txLayer2DataLen++] = 0x01;                   // Control

	txLayer2DataBuf[txLayer2DataLen++] = pInfoField->GS;
	txLayer2DataBuf[txLayer2DataLen++] = pInfoField->Cmd;

	for (i = 0; i < pInfoField->NumOfDOBs; i++)
	{
		txLayer2DataBuf[txLayer2DataLen++] = Byte0_UINT16(pInfoField->DOBs[i].Index);
		txLayer2DataBuf[txLayer2DataLen++] = Byte1_UINT16(pInfoField->DOBs[i].Index);
		txLayer2DataBuf[txLayer2DataLen++] = pInfoField->DOBs[i].SubIndex;
		txLayer2DataBuf[txLayer2DataLen++] = pInfoField->DOBs[i].Status;

		if (pInfoField->DOBs[i].DataLen > 0)
		{
			txLayer2DataBuf[txLayer2DataLen++] = pInfoField->DOBs[i].DataType;

			for (j = 0; j < pInfoField->DOBs[i].DataLen; j++)
			{
				txLayer2DataBuf[txLayer2DataLen++] = pInfoField->DOBs[i].pData[j];
			}
		}
	}

	SendLayer2Frame(port,txLayer2DataBuf, txLayer2DataLen);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SendTelegramError(uint8_t errorCode)
{
	memset(&txInfo, 0, sizeof(InfoFiled_t));
    txInfo.port = rxInfo.port;
	txInfo.GS = rxInfo.GS;
	txInfo.Cmd = TelegramError;
	txInfo.NumOfDOBs = 1;
	txInfo.DOBs[0].Index = errorCode;
	txInfo.DOBs[0].SubIndex = 0x00;
	txInfo.DOBs[0].Status = NoErrorNoData;

	SendLayer7Frame(&txInfo);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Dealwith_ParameterRead(void)
{
	
	uint8_t i;
	uint16_t dobsTotalLen;
	uint8_t bResponseLenExceed;

	memset(&txInfo, 0, sizeof(InfoFiled_t));
	txInfo.port = rxInfo.port;
	txInfo.GS = rxInfo.GS;
	txInfo.Cmd = ParameterRead;

	memset(txDOBsDataBuf, 0, DOB_MAX_LEN);
	txDOBsDataLen = 0;

	dobsTotalLen = DOB_LEN_NO_DATA * rxInfo.NumOfDOBs;
	bResponseLenExceed = 0;

	for (i = 0; i < rxInfo.NumOfDOBs; i++)
	{
		txInfo.DOBs[i].Index = rxInfo.DOBs[i].Index;
		txInfo.DOBs[i].SubIndex = rxInfo.DOBs[i].SubIndex;

		if (!bResponseLenExceed)
		{
			ParseOneParamDOB(&rxInfo.DOBs[i], &txInfo.DOBs[i]);

			dobsTotalLen += txInfo.DOBs[i].DataLen;

			if (txInfo.DOBs[i].DataLen > 0) //带数据的DOB，type占一个字节
			{
				dobsTotalLen++;
			}

			if (dobsTotalLen > DOB_MAX_LEN)
			{
				bResponseLenExceed = 1;
			}
		}

		if (bResponseLenExceed)
		{
			txInfo.DOBs[i].Status = InsufficientSpace;
			txInfo.DOBs[i].DataLen = 0;
		}

		txInfo.NumOfDOBs++;
	}
	// 发送响应帧数据
	SendLayer7Frame(&txInfo);
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Dealwith_ParameterWrite(void)
{
	uint8_t i;

	memset(&txInfo, 0, sizeof(InfoFiled_t));
	txInfo.port = rxInfo.port;
	txInfo.GS = rxInfo.GS;
	txInfo.Cmd = ParameterWrite;

	for (i = 0; i < rxInfo.NumOfDOBs; i++)
	{
		txInfo.DOBs[i].Index = rxInfo.DOBs[i].Index;
		txInfo.DOBs[i].SubIndex = rxInfo.DOBs[i].SubIndex;

		ParseOneParamDOB(&rxInfo.DOBs[i], &txInfo.DOBs[i]);

		txInfo.NumOfDOBs++;
	}
	// 发送响应帧数据
	SendLayer7Frame(&txInfo);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Dealwith_DataBlockRead(void)
{
	if (rxInfo.NumOfDOBs == 1)
	{
		memset(&txInfo, 0, sizeof(InfoFiled_t));
		txInfo.port = rxInfo.port;
		txInfo.GS = rxInfo.GS;
		txInfo.Cmd = DataBlockRead;

		txInfo.NumOfDOBs = 1;

		memset(txDOBsDataBuf, 0, DOB_MAX_LEN);
		txDOBsDataLen = 0;

		ParseOneDataBlock(&rxInfo.DOBs[0], &txInfo.DOBs[0]);

		// 发送响应帧数据
		SendLayer7Frame(&txInfo);
	}
	else
	{
		//解析接收命令中的DOBs格式错误
		SendTelegramError(FrameFormatError);
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Dealwith_DataBlockWrite(void)
{
	if (rxInfo.NumOfDOBs == 1)
	{
		memset(&txInfo, 0, sizeof(InfoFiled_t));
		txInfo.port = rxInfo.port;
		txInfo.GS = rxInfo.GS;
		txInfo.Cmd = DataBlockWrite;

		txInfo.NumOfDOBs = 1;

		memset(txDOBsDataBuf, 0, DOB_MAX_LEN);
		txDOBsDataLen = 0;

		ParseOneDataBlock(&rxInfo.DOBs[0], &txInfo.DOBs[0]);

		// 发送响应帧数据
		SendLayer7Frame(&txInfo);
	}
	else
	{
		//解析接收命令中的DOBs格式错误
		SendTelegramError(FrameFormatError);
	}
}

void Dealwith_InterfaceActive(FGIPv2Frame_t *pRxFrame)
{
	memset(&txInfo, 0, sizeof(InfoFiled_t));
	txInfo.port = rxInfo.port;
	txInfo.GS = rxInfo.GS;
	txInfo.Cmd = InterfaceActive;
	txInfo.NumOfDOBs = 1;
	if(pRxFrame->info[2]==0x01)
	{
		g_GS[rxInfo.port].bits.active = 1;
		txInfo.DOBs[0].Index = 0x01;
	}else if(pRxFrame->info[2]==0x02)
	{
		g_GS[rxInfo.port].bits.active = 0;
		txInfo.DOBs[0].Index = 0x02;
	}
	txInfo.DOBs[0].SubIndex = 0x00;
	txInfo.DOBs[0].Status = NoErrorNoData;
	SendLayer7Frame(&txInfo);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Dealwith_FGIPv2_Layer7(FGIPv2Frame_t *pRxFrame)
{
	memset(&rxInfo, 0, sizeof(InfoFiled_t));
	rxInfo.port = pRxFrame->port;
	rxInfo.GS =   pRxFrame->info[0];
	rxInfo.Cmd =  pRxFrame->info[1];
	//解析接收命令中的DOBs格式
	if(ParseRxDataObjects(pRxFrame) == 0)
	{
		SendTelegramError(FrameFormatError);
	}		
	switch(rxInfo.Cmd)
	{
	case ParameterRead:
		Dealwith_ParameterRead();
		break;
	case ParameterWrite:
		Dealwith_ParameterWrite();
		break;
	case DataBlockRead:
		Dealwith_DataBlockRead();
		break;
	case DataBlockWrite:
		Dealwith_DataBlockWrite();
		break;
	case InterfaceActive:
		Dealwith_InterfaceActive(pRxFrame);
		break;
	default:
		SendTelegramError(UnknownCommand);
		break;
	}
}
