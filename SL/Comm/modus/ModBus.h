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

#define  MODBUS_ADDR    				0x11
#define  MAX_REG_NUM					126

/* 功能码 */
#define  READ_COILS_FUNC             	0x01
#define  READ_DISCRETEINPUTS_FUNC       0x02
#define  READ_HOLDREG_FUNC          	0x03
#define  READ_INPUTREG_FUNC             0x04
#define  WRITE_SINGLECOIL_FUNC          0x05
#define  WRITE_SINGLEHOLDREG_FUNC       0x06
#define  WRITE_MULTCOILS_FUNC      		0x0F
#define  WRITE_MULTHOLDREG_FUNC         0x10


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
#define HOLD_REG_SW_START	    0x0031  //Software Update Start reg;
#define HOLD_REG_SW_END 	    0x00AE  //Software Update End reg; total 126 regs, 252 bytes;
#define SOFTVER_REG_NUM		    (HOLD_REG_SW_END - HOLD_REG_SW_START + 1)

#define HOLD_REG_ERRLOG_START	0x00AF  //ErrLog Start reg;
#define HOLD_REG_ERRLOG_END 	0x0126  //ErrLog End reg; total 120 regs, 240 bytes;
#define ERRLOG_REG_NUM			(HOLD_REG_ERRLOG_END - HOLD_REG_ERRLOG_START + 1)

#define HOLD_REG_P01		    0x0127  //Modbus_PHoldReg[0]=V-Set_IntPart
#define HOLD_REG_P02		    0x0128  //Modbus_PHoldReg[1]=V-Set_FractionPart
#define HOLD_REG_P03		    0x0129  //Modbus_PHoldReg[2]=I-Set_IntPart
#define HOLD_REG_P04		    0x012A  //Modbus_PHoldReg[3]=I-Set_FractionPart
#define SETTING_REG_NUM		    (HOLD_REG_P04 - HOLD_REG_P01 + 1)

#define HOLD_REG_P05		    0x012B  //Modbus_PHoldReg[4]=Factor_Ac380_AV_IntPart;
#define HOLD_REG_P06		    0x012C  //Modbus_PHoldReg[5]=Factor_Ac380_AV_FractionPart;
#define HOLD_REG_P07		    0x012D  //Modbus_PHoldReg[6]=Factor_Ac380_AI_IntPart;
#define HOLD_REG_P08		    0x012E  //Modbus_PHoldReg[7]=Factor_Ac380_AI_FractionPart;
#define HOLD_REG_P09		    0x012F  //Modbus_PHoldReg[8]=Factor_Way3_I_IntPart;
#define HOLD_REG_P10		    0x0130  //Modbus_PHoldReg[9]=Factor_Way3_I_FractionPart;
#define HOLD_REG_P11		    0x0131  //Modbus_PHoldReg[10]=Factor_Way4_I_IntPart;
#define HOLD_REG_P12		    0x0132  //Modbus_PHoldReg[11]=Factor_Way4_I_FractionPart;
#define HOLD_REG_P13		    0x0133  //Modbus_PHoldReg[12]=Factor_DcDc1_I_IntPart;
#define HOLD_REG_P14		    0x0134  //Modbus_PHoldReg[13]=Factor_DcDc1_I_FractionPart;
#define HOLD_REG_P15		    0x0135  //Modbus_PHoldReg[14]=Factor_DcDc1_V_IntPart;
#define HOLD_REG_P16		    0x0136  //Modbus_PHoldReg[15]=Factor_DcDc1_V_FractionPart;
#define HOLD_REG_P17		    0x0137  //Modbus_PHoldReg[16]=Factor_DcDc2_I_IntPart;
#define HOLD_REG_P18		    0x0138  //Modbus_PHoldReg[17]=Factor_DcDc2_I_FractionPart;
#define HOLD_REG_P19		    0x0139  //Modbus_PHoldReg[18]=Factor_DcDc2_V_IntPart;
#define HOLD_REG_P20		    0x013A  //Modbus_PHoldReg[19]=Factor_DcDc2_V_FractionPart;
#define HOLD_REG_P21		    0x013B  //Modbus_PHoldReg[20]=Factor_Ac380_CV_IntPart;
#define HOLD_REG_P22		    0x013C  //Modbus_PHoldReg[21]=Factor_Ac380_CV_FractionPart;
#define HOLD_REG_P23		    0x013D  //Modbus_PHoldReg[22]=Factor_Ac380_CI_IntPart;
#define HOLD_REG_P24		    0x013E  //Modbus_PHoldReg[23]=Factor_Ac380_CI_FractionPart;
#define HOLD_REG_P25		    0x013F  //Modbus_PHoldReg[24]=Factor_Ac380_BV_IntPart;
#define HOLD_REG_P26		    0x0140  //Modbus_PHoldReg[25]=Factor_Ac380_BV_FractionPart;
#define HOLD_REG_P27		    0x0141  //Modbus_PHoldReg[26]=Factor_Ac380_BI_IntPart;
#define HOLD_REG_P28		    0x0142  //Modbus_PHoldReg[27]=Factor_Ac380_BI_FractionPart;
#define PARAM_REG_NUM		    (HOLD_REG_P28 - HOLD_REG_P05 + 1)

#define HOLD_REG_P29		    0x0143  //Modbus_PHoldReg[28]=Hardware[16]; LSB
#define HOLD_REG_P30		    0x0144  //Modbus_PHoldReg[29]=Hardware[16];
#define HOLD_REG_P31		    0x0145  //Modbus_PHoldReg[30]=Hardware[16];
#define HOLD_REG_P32		    0x0146  //Modbus_PHoldReg[31]=Hardware[16];
#define HOLD_REG_P33		    0x0147  //Modbus_PHoldReg[32]=Hardware[16];
#define HOLD_REG_P34		    0x0148  //Modbus_PHoldReg[33]=Hardware[16];
#define HOLD_REG_P35		    0x0149  //Modbus_PHoldReg[34]=Hardware[16];
#define HOLD_REG_P36		    0x014A  //Modbus_PHoldReg[35]=Hardware[16];
#define HOLD_REG_P37		    0x014B  //Modbus_PHoldReg[36]=Hardware[16];
#define HOLD_REG_P38		    0x014C  //Modbus_PHoldReg[37]=Hardware[16];
#define HOLD_REG_P39		    0x014D  //Modbus_PHoldReg[38]=Hardware[16];
#define HOLD_REG_P40		    0x014E  //Modbus_PHoldReg[39]=Hardware[16];
#define HOLD_REG_P41		    0x014F  //Modbus_PHoldReg[40]=Hardware[16];
#define HOLD_REG_P42		    0x0150  //Modbus_PHoldReg[41]=Hardware[16];
#define HOLD_REG_P43		    0x0151  //Modbus_PHoldReg[42]=Hardware[16];
#define HOLD_REG_P44		    0x0152  //Modbus_PHoldReg[43]=Hardware[16]; MSB
#define HARDVER_REG_NUM			(HOLD_REG_P44 - HOLD_REG_P29 + 1)

#define HOLD_REG_XX				HOLD_REG_P44
#define HOLD_REG_P_NUM        	(HOLD_REG_XX - HOLD_REG_P01 + 1)    

/* 04H 读取输入寄存器(模拟信号) */
#define INPUT_REG_A01		0x0200  //Adc0 : Modbus_InputReg[0] = sensorMsg.Monitor.Ac380_AV_IntPart;
#define INPUT_REG_A02		0x0201  //Adc0 : Modbus_InputReg[1] = sensorMsg.Monitor.Ac380_AV_FractionPart;
#define INPUT_REG_A03		0x0202  //Adc1 : Modbus_InputReg[2] = sensorMsg.Monitor.Ac380_AI_IntPart;
#define INPUT_REG_A04		0x0203  //Adc1 : Modbus_InputReg[3] = sensorMsg.Monitor.Ac380_AI_FractionPart;
#define INPUT_REG_A05		0x0204  //Adc2 : Modbus_InputReg[4] = sensorMsg.Monitor.Way3_I_IntPart;
#define INPUT_REG_A06		0x0205  //Adc2 : Modbus_InputReg[5] = sensorMsg.Monitor.Way3_I_FractionPart;
#define INPUT_REG_A07		0x0206  //Adc3 : Modbus_InputReg[6] = sensorMsg.Monitor.Way4_I_IntPart;
#define INPUT_REG_A08		0x0207  //Adc3 : Modbus_InputReg[7] = sensorMsg.Monitor.Way4_I_FractionPart;
#define INPUT_REG_A09		0x0208  //Adc6 : Modbus_InputReg[8] = sensorMsg.Monitor.DcDc1_I_IntPart;
#define INPUT_REG_A10		0x0209  //Adc6 : Modbus_InputReg[9] = sensorMsg.Monitor.DcDc1_I_FractionPart;
#define INPUT_REG_A11		0x020A  //Adc7 : Modbus_InputReg[10] = sensorMsg.Monitor.DcDc1_V_IntPart;
#define INPUT_REG_A12		0x020B  //Adc7 : Modbus_InputReg[11] = sensorMsg.Monitor.DcDc1_V_FractionPart;
#define INPUT_REG_A13		0x020C  //Adc8 : Modbus_InputReg[12] = sensorMsg.Monitor.DcDc2_I_IntPart;
#define INPUT_REG_A14		0x020D  //Adc8 : Modbus_InputReg[13] = sensorMsg.Monitor.DcDc2_I_FractionPart;
#define INPUT_REG_A15		0x020E  //Adc9 : Modbus_InputReg[14] = sensorMsg.Monitor.DcDc2_V_IntPart;
#define INPUT_REG_A16		0x020F  //Adc9 : Modbus_InputReg[15] = sensorMsg.Monitor.DcDc2_V_FractionPart;
#define INPUT_REG_A17		0x0210  //Adc10: Modbus_InputReg[16] = sensorMsg.Monitor.Ac380_CV_IntPart;
#define INPUT_REG_A18		0x0211  //Adc10: Modbus_InputReg[17] = sensorMsg.Monitor.Ac380_CV_FractionPart;
#define INPUT_REG_A19		0x0212  //Adc11: Modbus_InputReg[18] = sensorMsg.Monitor.Ac380_CI_IntPart;
#define INPUT_REG_A20		0x0213  //Adc11: Modbus_InputReg[19] = sensorMsg.Monitor.Ac380_CI_FractionPart;
#define INPUT_REG_A21		0x0214  //Adc12: Modbus_InputReg[20] = sensorMsg.Monitor.Ac380_BV_IntPart;
#define INPUT_REG_A22		0x0215  //Adc12: Modbus_InputReg[21] = sensorMsg.Monitor.Ac380_BV_FractionPart;
#define INPUT_REG_A23		0x0216  //Adc13: Modbus_InputReg[22] = sensorMsg.Monitor.Ac380_BI_IntPart;
#define INPUT_REG_A24		0x0217  //Adc13: Modbus_InputReg[23] = sensorMsg.Monitor.Ac380_BI_FractionPart;
#define INPUT_REG_A25		0x0218  //Adc14: Modbus_InputReg[24] = sensorMsg.Monitor.DcDc1_Temp_IntPart;
#define INPUT_REG_A26		0x0219  //Adc14: Modbus_InputReg[25] = sensorMsg.Monitor.DcDc1_Temp_FractionPart;
#define INPUT_REG_A27		0x021A  //Adc15: Modbus_InputReg[26] = sensorMsg.Monitor.DcDc2_Temp_IntPart;
#define INPUT_REG_A28		0x021B  //Adc15: Modbus_InputReg[27] = sensorMsg.Monitor.DcDc2_Temp_FractionPart;
#define INPUT_REG_A29       0x021C  //Modbus_InputReg[28] =FaultWord;
#define INPUT_REG_A30       0x021D  //Modbus_InputReg[29] =FaultWord;
#define INPUT_REG_A31       0x021E  //Modbus_InputReg[30] =StatusWord;
#define INPUT_REG_A32       0x021F  //Modbus_InputReg[31] =StatusWord;
#define INPUT_REG_PROCESS_NUM		(INPUT_REG_A32 - INPUT_REG_A01 + 1)

#define INPUT_REG_A33       0x0220  //Modbus_VersionReg[0]  = App Version start;
#define INPUT_REG_A48       0x022F  //Modbus_VersionReg[15] = App Version end;
#define INPUT_REG_A49       0x0230  //Modbus_VersionReg[16] = Boot Version start;
#define INPUT_REG_A64       0x023F  //Modbus_VersionReg[31] = Boot Version end;
#define INPUT_REG_A65       0x0240  //Modbus_InputReg[32]  = App Checksum start;
#define INPUT_REG_A66       0x0241  //Modbus_InputReg[33]  = App Checksum;
#define INPUT_REG_A67       0x0242  //Modbus_InputReg[34]  = App Checksum;
#define INPUT_REG_A68       0x0243  //Modbus_InputReg[35]  = App Checksum end;
#define INPUT_REG_VERSION_NUM		(INPUT_REG_A68 - INPUT_REG_A33 + 1)

#define INPUT_REG_AXX		INPUT_REG_A68
#define INPUT_REG_NUM				(INPUT_REG_AXX - INPUT_REG_A01 + 1)

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
	uint16_t len;
	uint8_t buf[BUFFER_SIZE];
}ModBuffer_t;

typedef struct _Modbus_Frame_t
{
	uint8_t  Address;					 //从机地址
	uint8_t  Func_Code;                  //功能码
	uint16_t RegAddress;                 //寄存器地址
	uint16_t RegData[MAX_REG_NUM];  	 //寄存器数据
	uint16_t RegLen;  	 				 //寄存器长度
	uint16_t DataLen;					 //数据长度
	uint16_t crc16;
	
}Modbus_Frame_t;	

#pragma pack(push)
#pragma pack(1)

typedef struct _ModbusParam_t
{
	uint32_t Factor_Ac380_AV;		//Factor_Ac380_AV;
	uint32_t Factor_Ac380_AI;		//Factor_Ac380_AI;
	uint32_t Factor_Ac380_BV;       //Factor_Ac380_BV;
	uint32_t Factor_Ac380_BI;		//Factor_Ac380_BI;
	uint32_t Factor_Ac380_CV;		//Factor_Ac380_CV;
	uint32_t Factor_Ac380_CI;		//Factor_Ac380_CI;
	uint32_t Factor_DcDc1_I;		//Factor_DcDc1_I;
	uint32_t Factor_DcDc1_V;		//Factor_DcDc1_V;
	uint32_t Factor_DcDc2_I;		//Factor_DcDc2_I;
	uint32_t Factor_DcDc2_V;		//Factor_DcDc2_V;
	uint32_t Factor_Way3_I;			//Factor_Way3_I;
	uint32_t Factor_Way4_I;			//Factor_Way4_I; 	
}ModbusParam_t;

typedef struct _ModbusSetup_t
{
	uint32_t V_Set;					//V-Set
	uint32_t I_Set;					//I-Set
}ModbusSetup_t;	

#define MODBUS_PARAM_LEN                      sizeof(ModbusParam_t)
#define MODBUS_SETTING_LEN                    sizeof(ModbusSetup_t)  
 	
#pragma pack(pop)
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

void MB_ReadCoil_01H(uint8_t addr, uint16_t reg, uint16_t num);
void MB_ReadInput_02H(uint8_t addr, uint16_t reg, uint16_t num);
void MB_ReadHoldingReg_03H(uint8_t addr, uint16_t reg, uint16_t num);
void MB_ReadInputReg_04H(uint8_t addr, uint16_t reg, uint16_t num);
void MB_WriteCoil_05H(uint8_t addr, uint16_t reg, uint16_t sta);
void MB_WriteHoldingReg_06H(uint8_t addr, uint16_t reg, uint16_t data);
void MB_WriteNumCoil_0FH(uint8_t addr, uint16_t reg, uint16_t num,uint16_t  *databuf);
void MB_WriteNumHoldingReg_10H(uint8_t addr, uint16_t reg, uint16_t num,uint16_t *databuf);

#endif  //_MODBUS_H_


/***************************************************************************
End
****************************************************************************/
