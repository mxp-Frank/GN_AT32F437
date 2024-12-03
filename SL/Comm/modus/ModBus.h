/******************************************************************
 * 文件：modbus.h
 * 功能：声明modbus相关函数
 * 日期：2023-05-15
 * 作者：xingping.meng
 * 版本：Ver.1.0 
 * 
 * Copyright (C) 2019 chengyuan-tech. All rights reserved.
*******************************************************************/
#ifndef _MODBUS_H_
#define _MODBUS_H_

#include "board.h"
#include "fgipv2_layer2.h"

#define MODBUS_ADDR     0x11

/* 功能码 */

#define  READ_MULTHOLDREG_FUNC          0x03
#define  READ_INPUTREG_FUNC             0x04
#define  WRITE_MULTHOLDREG_FUNC         0x10
#define  MAX_REG_NUM					126
/* 01H 读强制单线圈 */
/* 05H 写强制单线圈 */
#define OUTPUT_IO_D01     0x0001  //Kout OnOff
#define OUTPUT_IO_D02     0x0002  //Dcdc OnOff
#define OUTPUT_IO_DXX 	  OUTPUT_IO_D02

#define OUTPUT_IO_NUM     2

/* 02H 读取输入状态 */
#define INPUT_IO_T01		0x0021  //Kout feedback input
#define INPUT_IO_T02		0x0022  //En AC input
#define INPUT_IO_T03		0x0023  //En PA input
#define INPUT_IO_TXX		INPUT_IO_T03

#define INPUT_IO_NUM        3



/* 03H 读保持寄存器 */
/* 06H 写保持寄存器 */
/* 10H 写多个保存寄存器 */
#define HOLD_REG_P01		    0x0031  //V-Set
#define HOLD_REG_P02		    0x0032  //I-Set
#define HOLD_REG_SW_START	    0x0032  //Software Update Start reg;
#define HOLD_REG_SW_END 	    0x00AF  //Software Update End reg; total 126 regs, 252 bytes;
#define HOLD_REG_ERRLOG_START	0x00B0  //Software Update Start reg;
#define HOLD_REG_ERRLOG_END 	0x0127  //Software Update End reg; total 120 regs, 240 bytes;


#define HOLD_REG_PXX		HOLD_REG_P02
#define HOLD_REG_NUM		2


/* 0x10H 读取输入寄存器(模拟信号) */
#define HOLD_REG_A01		0x0141  //Adc0 : Modbus_InputReg[0] = sensorMsg.Monitor.Ac380_AV;
#define HOLD_REG_A02		0x0142  //Adc1 : Modbus_InputReg[1] = sensorMsg.Monitor.Ac380_AI;
#define HOLD_REG_A03		0x0143  //Adc2 : Modbus_InputReg[2] = sensorMsg.Monitor.Way3_I;
#define HOLD_REG_A04		0x0144  //Adc3 : Modbus_InputReg[3] = sensorMsg.Monitor.Way4_I;
#define HOLD_REG_A05		0x0145  //Adc6 : Modbus_InputReg[4] = sensorMsg.Monitor.DcDc1_I;
#define HOLD_REG_A06		0x0146  //Adc7 : Modbus_InputReg[5] = sensorMsg.Monitor.DcDc1_V;
#define HOLD_REG_A07		0x0147  //Adc8 : Modbus_InputReg[6] = sensorMsg.Monitor.DcDc2_I;
#define HOLD_REG_A08		0x0148  //Adc9 : Modbus_InputReg[7] = sensorMsg.Monitor.DcDc2_V;
#define HOLD_REG_A09		0x0149  //Adc10: Modbus_InputReg[8] = sensorMsg.Monitor.Ac380_CV;
#define HOLD_REG_A10		0x014A  //Adc11: Modbus_InputReg[9] = sensorMsg.Monitor.Ac380_CI;
#define HOLD_REG_A11		0x014B  //Adc12: Modbus_InputReg[10] = sensorMsg.Monitor.Ac380_BV;
#define HOLD_REG_A12		0x014C  //Adc13: Modbus_InputReg[11] = sensorMsg.Monitor.Ac380_BI;
#define HOLD_REG_A13		0x014D  //Adc14: Modbus_InputReg[12] = sensorMsg.Monitor.DcDc1_Temp;
#define HOLD_REG_A14		0x014E  //Adc15: Modbus_InputReg[13] = sensorMsg.Monitor.DcDc2_Temp;
#define HOLD_REG_A15        0x014F  //FaulutWord;
#define HOLD_REG_A16        0x0150  //StatusWord;
#define PARAM_REG_NUM		16

/* 0x04H 读取输入寄存器(模拟信号) */
#define INPUT_REG_A01		0x0141  //Adc0 : Modbus_InputReg[0] = sensorMsg.Monitor.Ac380_AV;
#define INPUT_REG_A02		0x0142  //Adc1 : Modbus_InputReg[1] = sensorMsg.Monitor.Ac380_AI;
#define INPUT_REG_A03		0x0143  //Adc2 : Modbus_InputReg[2] = sensorMsg.Monitor.Way3_I;
#define INPUT_REG_A04		0x0144  //Adc3 : Modbus_InputReg[3] = sensorMsg.Monitor.Way4_I;
#define INPUT_REG_A05		0x0145  //Adc6 : Modbus_InputReg[4] = sensorMsg.Monitor.DcDc1_I;
#define INPUT_REG_A06		0x0146  //Adc7 : Modbus_InputReg[5] = sensorMsg.Monitor.DcDc1_V;
#define INPUT_REG_A07		0x0147  //Adc8 : Modbus_InputReg[6] = sensorMsg.Monitor.DcDc2_I;
#define INPUT_REG_A08		0x0148  //Adc9 : Modbus_InputReg[7] = sensorMsg.Monitor.DcDc2_V;
#define INPUT_REG_A09		0x0149  //Adc10: Modbus_InputReg[8] = sensorMsg.Monitor.Ac380_CV;
#define INPUT_REG_A10		0x014A  //Adc11: Modbus_InputReg[9] = sensorMsg.Monitor.Ac380_CI;
#define INPUT_REG_A11		0x014B  //Adc12: Modbus_InputReg[10] = sensorMsg.Monitor.Ac380_BV;
#define INPUT_REG_A12		0x014C  //Adc13: Modbus_InputReg[11] = sensorMsg.Monitor.Ac380_BI;
#define INPUT_REG_A13		0x014D  //Adc14: Modbus_InputReg[12] = sensorMsg.Monitor.DcDc1_Temp;
#define INPUT_REG_A14		0x014E  //Adc15: Modbus_InputReg[13] = sensorMsg.Monitor.DcDc2_Temp;
#define INPUT_REG_A15       0x014F  //FaulutWord;
#define INPUT_REG_A16       0x0150  //StatusWord;
#define INPUT_REG_NUM		16

typedef enum
{
	ErrorFunction 		=	0x01,
	ErrorDataAddress 	=	0x02,
	ErrorDataValue	 	=	0x03,
	ErrorSlaveStation	=	0x04,
	SlaveStationBusy	= 	0x07,
	ErrorDataCRC	 	=	0x08,
}FUNCODE_ERR;
typedef struct
{
	uint8_t len;
	uint8_t buf[MAX_INFO_LEN];
}ModBuffer_t;
typedef enum
{//uinit: ms
	DEVICE_INPUTREG                            = 0,
    DEVICE_HOLDREG                             = 1,
    DEVICE_UPDATE                              = 2,
	DEVICE_NUM								   = 3
} ModBusTypeEnum;
/****************CRC余式表*******************************************************/
static const unsigned int ModbusCRC_Table[256] =
{
    0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
    0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
    0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
    0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
    0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
    0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
    0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
    0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
    0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
    0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
    0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
    0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
    0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
    0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
    0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
    0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
    0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
    0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
    0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
    0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
    0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
    0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
    0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
    0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
    0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
    0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
    0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
    0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
    0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
    0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
    0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
    0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040,
};

extern ModBusTypeEnum ModBusSendType;    	   	 //Modbus发送标志状态
extern uint16_t Modbus_ParamReg[PARAM_REG_NUM];   //参数寄存器指针
extern uint16_t Modbus_InputReg[INPUT_REG_NUM];   //输入寄存器指针
extern uint16_t Modbus_HoldReg[MAX_REG_NUM];	  //保持寄存器指针

extern void MB_ReadCoil_01H(uint8_t addr, uint16_t reg, uint16_t num);
extern void MB_ReadInput_02H(uint8_t addr, uint16_t reg, uint16_t num);
extern void MB_ReadHoldingReg_03H(uint8_t addr, uint16_t reg, uint16_t num);
extern void MB_ReadInputReg_04H(uint8_t addr, uint16_t reg, uint16_t num);
extern void MB_WriteCoil_05H(uint8_t addr, uint16_t reg, uint16_t sta);
extern void MB_WriteHoldingReg_06H(uint8_t addr, uint16_t reg, uint16_t data);
extern void MB_WriteNumCoil_0FH(uint8_t addr, uint16_t reg, uint16_t num,uint8_t  *databuf);
extern void MB_WriteNumHoldingReg_10H(uint8_t addr, uint16_t reg, uint16_t num,uint8_t *databuf);

extern void Dealwith_Modbus_Service(pCommMsg_t pRxMsg);
extern void Modbus_ReceiveChar(uint8_t port, uint8_t ch);
extern void Modbus_TransmitChar(uint8_t port);
extern void Check_Modbus_CallBack(uint8_t port);
extern void Modbus_TxBufManagment(uint8_t port);
#endif  //_MODBUS_H_


/***************************************************************************
End
****************************************************************************/
