 /*******************************************************************************************
 * modbus.c
 *
 *  Created on: 2023年5月9日
 *  Author:  mengxingping
 ****************************************************************************************/
#include "Modbus.h"
#include "IF_Modbus.h"
#include "IF_HAL_Cfg.h"
#include "IF_SL_Cfg.h"
static uint16_t ModbusSendTypeTimer = 0;
static uint16_t ModbusTimer = 0; 		 				//modBus 接收帧计时
static ModBusTypeEnum ModBusSendType = READ_MB_VERSIONREG;    //Modbus发送、接收数据类型
static  ModbusParam_t  	ModbusParam;
static  ModbusSetup_t   ModbusSetting;
static uint16_t Modbus_InputCoils = 0;					//Modbus的输入线圈值
static uint16_t Modbus_OutputCoils = 0;					//Modbus的输出线圈值
static uint16_t Modbus_InputReg[INPUT_REG_PROCESS_NUM] = {0};  	//Modbus输入寄存器指针
static uint16_t Modbus_VersionReg[INPUT_REG_VERSION_NUM] = {0};  //Modbus输入寄存器指针
static uint16_t Modbus_PHoldReg[HOLD_REG_P_NUM] ={0};	//Modbus保持寄存器指针
static Layer2Data_t ModBuffer;
extern CommMsg_t g_RxBuf[PORT_NUM];
extern CommMsg_t g_TxBuf[PORT_NUM];

static void ModBus_SendTypeManagment(uint8_t ModbusAddress);
static void Update_ACDCSettingToPHoldRegister(void);
static void Update_PHoldRegisterToACDCSetting(void);
static void Update_ACDCParamToPHoldRegister(void);
static void Update_PHoldRegisterToACDCParam(void);
static void Update_RegisterToStatusAndFualtWord(void);
static void RS485_SendData(uint8_t port,Layer2Data_t txMsg);
static uint16_t ModbusCRC16(uint8_t *ptr, uint16_t len);
/* FUNCTION *********************************************************************************
 * Function Name : Modbus_TxTimeManagment
 * Description   : Modbus发送查询数据
 * Parameter     : port 发送数据端口
 * END ***************************************************************************************/
void Modbus_TxBufManagment(uint8_t port)
{ 	
	if(ModBusSendType == WRITE_MB_SOFTWAREREG) //软件版本保持寄存器
	{
		BSIP_ModbusFirmwareUpdate(port,MODBUS_ADDR);
	}
	else
	{
		ModBus_SendTypeManagment(MODBUS_ADDR);
	}
}
/* FUNCTION *********************************************************************************
 * Function Name : Dealwith_Modbus_Service
 * Description   : Modbus介绍查询数据
 * Parameter     : pRxMsg 接收数据
 * END ***************************************************************************************/
void Dealwith_Modbus_Service(pCommMsg_t pRxMsg)
{	
	Modbus_Frame_t ModRxFrame;
	memset(&ModRxFrame,0,sizeof(Modbus_Frame_t));	
	ModRxFrame.Address   = pRxMsg->data[0];    //接收从机地址                                       
	ModRxFrame.Func_Code = pRxMsg->data[1];    //接收数据的功能码
	ModRxFrame.crc16 =(pRxMsg->data[pRxMsg->len-1]<<8) | pRxMsg->data[pRxMsg->len-2]; //验证CRC
	uint16_t CrcCheck16 = ModbusCRC16(pRxMsg->data,(pRxMsg->len-2));
	if((ModRxFrame.Address == MODBUS_ADDR)&&(ModRxFrame.crc16 == CrcCheck16))
	{
		switch(ModRxFrame.Func_Code) 		 //Func_Code
		{ 
			case READ_COILS_FUNC:	
				if(pRxMsg->data[2]%2==0)
				{
					ModRxFrame.DataLen = pRxMsg->data[2];
				}
				if(ModRxFrame.DataLen == (INPUT_IO_NUM/8+1))
				{
					Modbus_InputCoils = pRxMsg->data[3];
					//跳转到readprocessregdata
					IF_ACDC_SetParamsRWType(READ_MB_PROCSSREG); 					
				}					
				break;
				
			case READ_INPUTREG_FUNC:		
				if(pRxMsg->data[2]%2==0)
				{
					ModRxFrame.RegLen  = pRxMsg->data[2]/2;
				}	
				if(ModRxFrame.RegLen == INPUT_REG_PROCESS_NUM)   
				{
					//读ACDC过程数据
					for(uint8_t i=0;i< INPUT_REG_PROCESS_NUM;i++)
					{
						Modbus_InputReg[i] = MAKE_UINT16(pRxMsg->data[3+2*i],pRxMsg->data[4+2*i]);
					}
					//更新ACDC的状态和错误字
					Update_RegisterToStatusAndFualtWord();
										
				}
				else if(ModRxFrame.RegLen == INPUT_REG_VERSION_NUM)  
				{		
					//读ACDC版本数据
					for(uint8_t i=0;i< INPUT_REG_VERSION_NUM;i++)
					{
						Modbus_VersionReg[i] = MAKE_UINT16(pRxMsg->data[3+2*i],pRxMsg->data[4+2*i]);
					}
					//跳转到readholdregdata
					IF_ACDC_SetParamsRWType(READ_MB_HOLDREG); 			
				}						
			break;
				
			case READ_HOLDREG_FUNC:			
				if(pRxMsg->data[2]%2==0)
				{
					ModRxFrame.RegLen  = pRxMsg->data[2]/2;
				}
				if(ModRxFrame.RegLen == HOLD_REG_P_NUM) 
				{	
					//读保持参数数据	
					for(uint8_t i= 0;i < HOLD_REG_P_NUM;i++)
					{
						Modbus_PHoldReg[i] = MAKE_UINT16(pRxMsg->data[3+2*i],pRxMsg->data[4+2*i]);
					}
					//更新ACDC参数数据
					Update_PHoldRegisterToACDCParam(); 
					//跳转到readprocessregdata
					IF_ACDC_SetParamsRWType(READ_MB_PROCSSREG); 			
				}
			break;		
			case WRITE_MULTCOILS_FUNC:	
				ModRxFrame.RegAddress = MAKE_UINT16(pRxMsg->data[2],pRxMsg->data[3]);	 //寄存器地址
				ModRxFrame.RegLen = MAKE_UINT16(pRxMsg->data[4],pRxMsg->data[5]);	 	//寄存器
				if((ModRxFrame.RegAddress == OUTPUT_IO_D01)||
				   (ModRxFrame.RegAddress == OUTPUT_IO_D02))
				{
					//跳转到readprocessregdata
					IF_ACDC_SetParamsRWType(READ_MB_PROCSSREG); 			
				}	
			break;
				
			case WRITE_SINGLEHOLDREG_FUNC:		
				ModRxFrame.RegAddress = MAKE_UINT16(pRxMsg->data[2],pRxMsg->data[3]);	
				//参数寄存器地址				
				if((ModRxFrame.RegAddress >= HOLD_REG_P01)&&
				   (ModRxFrame.RegAddress <= HOLD_REG_P44)) 	
				{
					//更新ACDC参数数据
					Update_PHoldRegisterToACDCSetting();
					Update_PHoldRegisterToACDCParam();
					//跳转到readprocessregdata
					IF_ACDC_SetParamsRWType(READ_MB_PROCSSREG); 			
				}
			break;
				
			case WRITE_MULTHOLDREG_FUNC:	
				ModRxFrame.RegAddress = MAKE_UINT16(pRxMsg->data[2],pRxMsg->data[3]);
				if((ModRxFrame.RegAddress == HOLD_REG_P01)||
				   (ModRxFrame.RegAddress == HOLD_REG_P05)||
				   (ModRxFrame.RegAddress == HOLD_REG_P29)) //设置寄存器	
				{ 
					//跳转到readprocessregdata
					IF_ACDC_SetParamsRWType(READ_MB_PROCSSREG); 								
				}
				else if(ModRxFrame.RegAddress == HOLD_REG_SW_START)  //软件寄存器	
				{
					xSemaphoreGive(ModbusNfSemaphore);
					xSemaphoreGive(ModbusReSemaphore);
					IF_ACDC_SetParamsRWType(WRITE_MB_SOFTWAREREG); 
				}
				
			break;
				
			default:
				//跳转到readprocessregdata
				IF_ACDC_SetParamsRWType(READ_MB_PROCSSREG); 				
			break;
		}	
	}		
}	
/* FUNCTION *********************************************************************************
 * Function Name : ModBus_SendTypeManagment
 * Description   : Modbus发送查询数据
 * Parameter     : port 发送数据端口
 * END ***************************************************************************************/
static void ModBus_SendTypeManagment(uint8_t Modbus_Address)
{
	if(ModBusSendType == READ_MB_VERSIONREG)    //读版本输入寄存器		
	{		
		MB_ReadInputReg_04H(Modbus_Address, INPUT_REG_A33,INPUT_REG_VERSION_NUM);	
	}
	else if(ModBusSendType == READ_MB_HOLDREG) 
	{
		MB_ReadHoldingReg_03H(Modbus_Address, HOLD_REG_P01,HOLD_REG_P_NUM);
	}
	else if(ModBusSendType ==READ_MB_PROCSSREG)    //读过程输入寄存器
	{
		MB_ReadInputReg_04H(Modbus_Address, INPUT_REG_A01,INPUT_REG_PROCESS_NUM);			
	}
	else if(ModBusSendType ==	READ_MB_COILS)	//读线圈状态
	{				
		MB_ReadCoil_01H(Modbus_Address, INPUT_IO_T01,INPUT_IO_NUM);					
	}
	else if(ModBusSendType == WRITE_MB_MULTCOILS)  //写线圈状态
	{			
		MB_WriteNumCoil_0FH(Modbus_Address, OUTPUT_IO_D01, 1,(uint16_t*)&Modbus_OutputCoils);		
	}
	else if(ModBusSendType ==  WRITE_MB_SETTINGEREG)//写设置保持寄存器
	{			
		Update_ACDCSettingToPHoldRegister();
		uint8_t AddressOffset = HOLD_REG_P01 - HOLD_REG_P01;				
		MB_WriteNumHoldingReg_10H(Modbus_Address, HOLD_REG_P01,SETTING_REG_NUM,(uint16_t*)&Modbus_PHoldReg[AddressOffset]);
	}
	else if(ModBusSendType == WRITE_MB_HARDWAREREG)//写版本保持寄存器
	{	
		uint8_t AddressOffset = HOLD_REG_P29 - HOLD_REG_P01;			
		MB_WriteNumHoldingReg_10H(Modbus_Address, HOLD_REG_P29,HARDVER_REG_NUM,(uint16_t*)&Modbus_PHoldReg[AddressOffset]);	
	}
	else if(ModBusSendType ==WRITE_MB_PARAMREG)//写参数保持寄存器		
	{	
		Update_ACDCParamToPHoldRegister();	
		uint8_t AddressOffset = HOLD_REG_P05 - HOLD_REG_P01;	
		MB_WriteNumHoldingReg_10H(Modbus_Address, HOLD_REG_P05,PARAM_REG_NUM,(uint16_t*)&Modbus_PHoldReg[AddressOffset]);
	}	

	vTaskDelay(MODBUS_TX_PERIOD);
}
/* FUNCTION *********************************************************************************
 * Function Name : Modbus_ReceiveChar
 * Description   : 查表法计算crc，
 * Parameter     : ptr: 需要处理的字节
 * Parameter     : len: 字节的长度
 * Return        : crc  crc16校验值
 * END ***************************************************************************************/
static uint16_t ModbusCRC16(uint8_t *ptr, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    
    while(len--) 
    {
        crc = (crc >> 8) ^ ModbusCRC_Table[(crc ^ *ptr++) & 0xff];
    }
    
    return (crc);
}
/* FUNCTION *********************************************************************************
 * Function Name : Modbus_ReceiveChar
 * Description   : 接收字节处理，一般在中断中调用(接收到一个字符，定时器开始计数3.5ms间隔)
 * Parameter     : port: 端口号
 * Parameter     : ch: 需要处理的字节
 * Return        :
 * END ***************************************************************************************/
static void RS485_SendData(uint8_t port,Layer2Data_t txMsg)
{	
	/*使能发送数据接口*/
	for(uint16_t i = 0;i <= txMsg.Len;i++)
	{
		g_TxBuf[port].data[g_TxBuf[port].len++]= txMsg.Buf[i];
	}
	while(g_RxBuf[RS485_UART].flag)
	{
		delay_ms(1);
	}
	RS485_TX_EN();
	IF_UartEnableTx(port);
	
		
}
/* FUNCTION *********************************************************************************
 * Function Name : Modbus_ReceiveChar
 * Description   : 接收字节处理，一般在中断中调用(接收到一个字符，定时器开始计数3.5ms间隔)
 * Parameter     : port: 端口号
 * Parameter     : ch: 需要处理的字节
 * Return        :
 * END ***************************************************************************************/
void Modbus_ReceiveChar(uint8_t port, uint8_t ch)
{
	RS485_RX_EN();                			   //接受使能
	g_RxBuf[port].flag = 1;  
	g_RxBuf[port].port = port;  
	if(g_RxBuf[port].len < BUFFER_SIZE)        //接收的字符超出边界
	{
		g_RxBuf[port].data[g_RxBuf[port].len++] = ch;
	}	
	ModbusTimer = 0;
}
/* FUNCTION *********************************************************************************
 * Function Name : Check_Modbus_CallBack
 * Description   : 接收字节时间回调函数，接收字符间隔超过3.5ms(4ms)，接收数据完成
 * Parameter     : port: 端口号
 * Parameter     : ch: 需要处理的字节
 * Return        :
 * END ***************************************************************************************/
void TMR_Modbus_ISR_Callback(void)
{	
	if(g_RxBuf[RS485_UART].flag == 1)  //接收帧开始
	{
		ModbusTimer++;
		if(ModbusTimer >= 5)   //接收字符间隔在3.5ms~20ms
		{
			SendToRxQueueFromISR(RS485_UART, &g_RxBuf[RS485_UART]);     //接收完成,发送到队列中	
			memset(&g_RxBuf[RS485_UART],0,sizeof(CommMsg_t));	
		}
	}
	else //接收帧完成
	{
		ModbusTimer = 0;
	}	
}
/* FUNCTION *********************************************************************************
 * Function Name : Modbus_TransmitChar
 * Description   : 发送中断处理，一般在中断中调用
 * Parameter     : port: 端口号
 * Parameter     : ch: 需要处理的字节
 * Return        :
 * END ***************************************************************************************/
void Modbus_TransmitChar(uint8_t port)
{
	if (g_TxBuf[port].index < g_TxBuf[port].len)
	{
		IF_HAL_UartSendByte(port, g_TxBuf[port].data[g_TxBuf[port].index]);	
		g_TxBuf[port].index++;
	}else
	{
		RS485_RX_EN();                 //发送使能
		IF_HAL_UartDisableTx(port);    //关闭中断
		memset((void*)&g_TxBuf[port],0,sizeof(CommMsg_t));	
	}
	
}

/* FUNCTION *********************************************************************************
 * Function Name : MB_ReadCoil_01H
 * Description   : Modbus读线圈量
 * Parameter     : addr:从站地址，reg:寄存器地址，num:读多个线圈个数
 * Return        :
 * END ***************************************************************************************/
void MB_ReadCoil_01H(uint8_t addr, uint16_t reg, uint16_t num)
{
	uint16_t crc = 0;	
	memset(&ModBuffer,0,sizeof(Layer2Data_t));
	ModBuffer.Buf[ModBuffer.Len++]= addr;         /*从站地址*/
	ModBuffer.Buf[ModBuffer.Len++]= 0x01;			/*功能码*/
	ModBuffer.Buf[ModBuffer.Len++]= reg >>8;		/*寄存器地址，高字节*/		
	ModBuffer.Buf[ModBuffer.Len++]= reg ;			/*寄存器地址，低字节*/	
	ModBuffer.Buf[ModBuffer.Len++]= num >>8;		/*线圈(bit)个数，高字节*/	
	ModBuffer.Buf[ModBuffer.Len++]= num;			/*线圈(bit)个数，低字节*/		
	crc = ModbusCRC16(ModBuffer.Buf,ModBuffer.Len);	
	ModBuffer.Buf[ModBuffer.Len++]= crc;			/*Mobus crc，低字节*/	
	ModBuffer.Buf[ModBuffer.Len++]= crc >>8;		/*Mobus crc，高字节*/
	RS485_SendData(RS485_UART,ModBuffer);
}
/* FUNCTION *********************************************************************************
 * Function Name : MB_WriteCoil_05H
 * Description   : 写单个线圈
 * Parameter     : addr:从站地址，reg:寄存器地址，sta:读线圈状态
 * Return        :
 * END ***************************************************************************************/
void MB_WriteCoil_05H(uint8_t addr, uint16_t reg, uint16_t sta)
{
	uint16_t crc = 0;
	memset(&ModBuffer,0,sizeof(Layer2Data_t));
	ModBuffer.Buf[ModBuffer.Len++]= addr;         /*从站地址*/
	ModBuffer.Buf[ModBuffer.Len++]= 0x05;			/*功能码*/
	ModBuffer.Buf[ModBuffer.Len++]= reg >>8;		/*寄存器地址，高字节*/		
	ModBuffer.Buf[ModBuffer.Len++]= reg;			/*寄存器地址，低字节*/	
	ModBuffer.Buf[ModBuffer.Len++]= sta >>8;		/*线圈(bit)个数，高字节*/	
	ModBuffer.Buf[ModBuffer.Len++]= sta;			/*线圈(bit)个数，低字节*/		
	crc = ModbusCRC16(ModBuffer.Buf,ModBuffer.Len);	
	ModBuffer.Buf[ModBuffer.Len++]= crc;			/*Mobus crc，低字节*/	
	ModBuffer.Buf[ModBuffer.Len++]= crc >>8;		/*Mobus crc，高字节*/
	RS485_SendData(RS485_UART,ModBuffer);
}

/* FUNCTION *********************************************************************************
 * Function Name : MB_ReadInput_02H
 * Description   : 读输入状态
 * Parameter     : addr:从站地址，reg:寄存器地址，num:读取的输入数量
 * Return        :
 * END ***************************************************************************************/
void MB_ReadInput_02H(uint8_t addr, uint16_t reg, uint16_t num) 
{
	uint16_t crc =0;
	memset(&ModBuffer,0,sizeof(Layer2Data_t));
	ModBuffer.Buf[ModBuffer.Len++]= addr;         /*从站地址*/
	ModBuffer.Buf[ModBuffer.Len++]= 0x02;			/*功能码*/
	ModBuffer.Buf[ModBuffer.Len++]= reg >>8;		/*寄存器地址，高字节*/		
	ModBuffer.Buf[ModBuffer.Len++]= reg;			/*寄存器地址，低字节*/	
	ModBuffer.Buf[ModBuffer.Len++]= num >>8;		/*开关(bit)个数，高字节*/	
	ModBuffer.Buf[ModBuffer.Len++]= num;			/*开关(bit)个数，低字节*/		
	crc = ModbusCRC16(ModBuffer.Buf,ModBuffer.Len);	
	ModBuffer.Buf[ModBuffer.Len++]= crc;			/*Mobus crc，低字节*/	
	ModBuffer.Buf[ModBuffer.Len++]= crc >>8;		/*Mobus crc，高字节*/
	RS485_SendData(RS485_UART,ModBuffer);		
}

/* FUNCTION *********************************************************************************
 * Function Name : MB_ReadInputReg_04H
 * Description   :读多个输入寄存器(InputRegister)
 * Parameter     :addr:从站地址，reg:寄存器地址，num:读多个输出开关量个数
 * Return        :
 * END ***************************************************************************************/
void MB_ReadInputReg_04H(uint8_t addr, uint16_t reg, uint16_t num) 
{
	uint16_t crc =0;
	memset(&ModBuffer,0,sizeof(Layer2Data_t));
	ModBuffer.Buf[ModBuffer.Len++]= addr;         /*从站地址*/
	ModBuffer.Buf[ModBuffer.Len++]= 0x04;			/*功能码*/
	ModBuffer.Buf[ModBuffer.Len++]= reg >>8;		/*寄存器地址，高字节*/		
	ModBuffer.Buf[ModBuffer.Len++]= reg;			/*寄存器地址，低字节*/	
	ModBuffer.Buf[ModBuffer.Len++]= num >>8;		/*寄存器个数，高字节*/	
	ModBuffer.Buf[ModBuffer.Len++]= num;			/*寄存器个数，低字节*/		
	crc = ModbusCRC16(ModBuffer.Buf,ModBuffer.Len);	
	ModBuffer.Buf[ModBuffer.Len++]= crc;			/*Mobus crc，低字节*/	
	ModBuffer.Buf[ModBuffer.Len++]= crc >>8;		/*Mobus crc，高字节*/
	RS485_SendData(RS485_UART,ModBuffer);	
}

/* FUNCTION *********************************************************************************
 * Function Name : MB_ReadHoldingReg_03H
 * Description   : 读取多个保持寄存器(HoldingRegister)
 * Parameter     : addr:从站地址，reg:寄存器地址，num:读多个寄存器个数
 * Return        :
 * END ***************************************************************************************/
void MB_ReadHoldingReg_03H(uint8_t addr, uint16_t reg, uint16_t num) 
{
	uint16_t crc =0;
	memset(&ModBuffer,0,sizeof(Layer2Data_t));
	ModBuffer.Buf[ModBuffer.Len++]= addr;         /*从站地址*/
	ModBuffer.Buf[ModBuffer.Len++]= 0x03;			/*功能码*/
	ModBuffer.Buf[ModBuffer.Len++]= reg >>8;		/*寄存器地址，高字节*/		
	ModBuffer.Buf[ModBuffer.Len++]= reg;			/*寄存器地址，低字节*/	
	ModBuffer.Buf[ModBuffer.Len++]= num >>8;		/*寄存器个数，高字节*/	
	ModBuffer.Buf[ModBuffer.Len++]= num;			/*寄存器个数，低字节*/		
	crc = ModbusCRC16(ModBuffer.Buf,ModBuffer.Len);	
	ModBuffer.Buf[ModBuffer.Len++]= crc;			/*Mobus crc，低字节*/	
	ModBuffer.Buf[ModBuffer.Len++]= crc >>8;		/*Mobus crc，高字节*/
	RS485_SendData(RS485_UART,ModBuffer);
}

/* FUNCTION *********************************************************************************
 * Function Name :MB_WriteHoldingReg_06H
 * Description   : 写单个保持寄存器(HoldingRegister)，Modbus功能码05处理程序
* Parameter      : addr:从站地址，reg:寄存器地址，data:待写入的寄存器数据
 * Return        :
 * END ***************************************************************************************/
void MB_WriteHoldingReg_06H(uint8_t addr, uint16_t reg, uint16_t data) 
{
	uint16_t crc = 0;
	memset(&ModBuffer,0,sizeof(Layer2Data_t));
	ModBuffer.Buf[ModBuffer.Len++]= addr;         /*从站地址*/
	ModBuffer.Buf[ModBuffer.Len++]= 0x06;			/*功能码*/
	ModBuffer.Buf[ModBuffer.Len++]= reg >>8;		/*寄存器地址，高字节*/		
	ModBuffer.Buf[ModBuffer.Len++]= reg;			/*寄存器地址，低字节*/	
	ModBuffer.Buf[ModBuffer.Len++]= data >>8;		/*寄存器个数，高字节*/	
	ModBuffer.Buf[ModBuffer.Len++]= data;			/*寄存器个数，低字节*/		
	crc = ModbusCRC16(ModBuffer.Buf,ModBuffer.Len);	
	ModBuffer.Buf[ModBuffer.Len++]= crc;			/*Mobus crc，低字节*/	
	ModBuffer.Buf[ModBuffer.Len++]= crc >>8;		/*Mobus crc，高字节*/
	RS485_SendData(RS485_UART,ModBuffer);	
}

/* FUNCTION *********************************************************************************
 * Function Name : MB_WriteCoil_05H
 * Description   : 写多个线圈状态(CoilStatus)
 * Parameter     : addr:从站地址，reg:寄存器地址，num：线圈个数,sta:读线圈状态
 * Return        :
 * END ***************************************************************************************/
void MB_WriteNumCoil_0FH(uint8_t addr, uint16_t reg, uint16_t num,uint16_t  *databuf)
{
	uint16_t crc = 0,Count = 0;
	if(num%8==0)
	{
		Count = num/8;
	}else
	{
		Count = num/8+1;
	}	
	memset(&ModBuffer,0,sizeof(Layer2Data_t));
	ModBuffer.Buf[ModBuffer.Len++]= addr;         /*从站地址*/
	ModBuffer.Buf[ModBuffer.Len++]= 0x0F;			/*功能码*/
	ModBuffer.Buf[ModBuffer.Len++]= reg >>8;		/*寄存器地址，高字节*/		
	ModBuffer.Buf[ModBuffer.Len++]= reg;			/*寄存器地址，低字节*/	
	ModBuffer.Buf[ModBuffer.Len++]= num >>8;		/*线圈(16bit)个数，高字节*/	
	ModBuffer.Buf[ModBuffer.Len++]= num;			/*线圈(16bit)个数，低字节*/
	ModBuffer.Buf[ModBuffer.Len++]= Count;		/*数据个数*/
	for(uint16_t i=0;i<(num<<1)+1;i++)
	{
		ModBuffer.Buf[ModBuffer.Len++]= databuf[i];		/*后面数据长度*/
	}
	crc = ModbusCRC16(ModBuffer.Buf,ModBuffer.Len);	
	ModBuffer.Buf[ModBuffer.Len++]= crc;			/*Mobus crc，低字节*/	
	ModBuffer.Buf[ModBuffer.Len++]= crc >>8;		/*Mobus crc，高字节*/
	RS485_SendData(RS485_UART,ModBuffer);	
}
/* FUNCTION *********************************************************************************
 * Function Name : MB_WriteNumHoldingReg_10H
 * Description   : Modbus写N个保持寄存器(HoldingRegister)
 * Parameter     :
 * Return        :
 * END ***************************************************************************************/
void MB_WriteNumHoldingReg_10H(uint8_t addr, uint16_t reg, uint16_t num,uint16_t *databuf)
{
	uint16_t crc =0;
	memset(&ModBuffer,0,sizeof(Layer2Data_t));
	ModBuffer.Buf[ModBuffer.Len++]= addr;         	/*从站地址*/
	ModBuffer.Buf[ModBuffer.Len++]= 0x10;			/*功能码*/
	ModBuffer.Buf[ModBuffer.Len++]= reg >>8;		/*寄存器地址，高字节*/		
	ModBuffer.Buf[ModBuffer.Len++]= reg;			/*寄存器地址，低字节*/	
	ModBuffer.Buf[ModBuffer.Len++]= num>>8;			/*寄存器个数，高字节*/	
	ModBuffer.Buf[ModBuffer.Len++]= num;			/*寄存器个数，低字节*/			
	ModBuffer.Buf[ModBuffer.Len++]= num <<1;		/*数据个数，低字节*/	
	for(uint16_t i=0;i< num;i++)
   	{
		ModBuffer.Buf[ModBuffer.Len++]= Byte1_UINT16(databuf[i]);		/*寄存器个数，高字节*/	
		ModBuffer.Buf[ModBuffer.Len++]= Byte0_UINT16(databuf[i]);		/*寄存器个数，低字节*/		
	}
	crc = ModbusCRC16(ModBuffer.Buf,ModBuffer.Len);	
	ModBuffer.Buf[ModBuffer.Len++]= crc;			/*Mobus crc，低字节*/	
	ModBuffer.Buf[ModBuffer.Len++]= crc >>8;		/*Mobus crc，高字节*/
	RS485_SendData(RS485_UART,ModBuffer);
 }

 /************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/
 /**********************Modbus Coils Parameters*****************************/
uint8_t IF_Sensor_GetInputKout_feedback(void)
{
	return Modbus_InputCoils&(0x01<<0);
}
uint8_t IF_Sensor_GetInputEn_AC(void)
{
	return Modbus_InputCoils&(0x01<<1);
}
uint8_t IF_Sensor_GetInputEn_PA(void)
{
	return Modbus_InputCoils&(0x01<<2);
} 
//////////////////////////////////////////////////////////////////
//		Modbus Input Register
//////////////////////////////////////////////////////////////////////
/**********************ProsessData Parameters*****************************/
int32_t IF_Sensor_GetACInputVoltage(uint8_t Channel)
{
	int32_t retValue;
	switch(Channel)
	{
		case 0: 
			retValue = Modbus_InputReg[0]*1000  + Modbus_InputReg[1];
		break;
		case 1: 
			retValue = Modbus_InputReg[20]*1000 + Modbus_InputReg[21];
		break;
		case 2: 
			retValue = Modbus_InputReg[16]*1000 + Modbus_InputReg[17]; 
		break;
		default:break;
	}		
	return retValue;
}

int32_t IF_Sensor_GetACInputCurrent(uint8_t Channel)
{
	int32_t retValue;
	switch(Channel)
	{
		case 0: 
			retValue = Modbus_InputReg[2]*1000 + Modbus_InputReg[3]; 
		break;
		case 1: 
			retValue = Modbus_InputReg[22]*1000 + Modbus_InputReg[23]; 
		break;
		case 2: 
			retValue = Modbus_InputReg[18]*1000 + Modbus_InputReg[19]; 
		break;
		default:break;
	}		
	return retValue;
}

int32_t IF_Sensor_GetPAVoltage(uint8_t Channel)
{
	int32_t retValue;
	switch(Channel)
	{
		case 0: 
			retValue = Modbus_InputReg[10]*1000 + Modbus_InputReg[11]; 
		break;
		case 1: 
			retValue = Modbus_InputReg[14]*1000 + Modbus_InputReg[15]; 
		break;
		default:break;
	}		
	return retValue;
}
int32_t IF_Sensor_GetPACurrent(uint8_t Channel)
{
	int32_t retValue;
	switch(Channel)
	{
		case 0: 
			retValue = Modbus_InputReg[8]*1000 + Modbus_InputReg[9]; 
		break;
		case 1: 
			retValue = Modbus_InputReg[12]*1000 + Modbus_InputReg[13]; 
		break;
		case 2: 
			retValue = Modbus_InputReg[4]*1000 + Modbus_InputReg[5]; 
		break;
		case 3: 
			retValue = Modbus_InputReg[6]*1000 + Modbus_InputReg[7]; 
		break;
		default:break;
	}		
	return retValue;
}

int32_t IF_Sensor_GetACDCTemperature(uint8_t Channel)
{
	int32_t retValue;
	switch(Channel)
	{
		case 0: 
			retValue = (Modbus_InputReg[24]*1000 + Modbus_InputReg[25])/100; 
		break;
		case 1: 
			retValue = (Modbus_InputReg[26]*1000 + Modbus_InputReg[27])/100; 
		break;
		default:break;
	}		
	return retValue;
}
/**********************Version Parameters*****************************/
uint8_t IF_Sensor_GetACDCAppVersion(uint8_t *pBuf)
{
	uint8_t i= 0;
	uint8_t regAddress = (INPUT_REG_A33-INPUT_REG_A33);
	for( i=0;i<(INPUT_REG_A48-INPUT_REG_A33+1);i++)
	{
		pBuf[i]= Byte0_UINT16(Modbus_VersionReg[regAddress+i]);
		if(pBuf[i]==0xFF)break;
	}
	return i;	
}
uint8_t IF_Sensor_GetACDCBootVersion(uint8_t *pBuf)
{
	uint8_t i= 0;
	uint8_t regAddress =(INPUT_REG_A49-INPUT_REG_A33);
	for( i=0;i<(INPUT_REG_A64-INPUT_REG_A49+1);i++)
	{
		pBuf[i]= Byte0_UINT16(Modbus_VersionReg[regAddress+i]);
		if(pBuf[i]==0xFF)break;
	}
	return i;	
}

uint32_t IF_Sensor_GetACDCAppChecksum(void)
{
	UINT32_VAL retValue;
	uint8_t regAddress =(INPUT_REG_A65-INPUT_REG_A33);
	for(uint8_t i=0;i<(INPUT_REG_A68-INPUT_REG_A65+1);i++)
	{
		retValue.v[i]= Byte0_UINT16(Modbus_VersionReg[regAddress+i]);
	}
	return retValue.Val;	
}
//////////////////////////////////////////////////////////////////////
//			Hold Register
////////////////////////////////////////////////////////////////////////
/**********************Settings Parameters*****************************/
void IF_ModbusSetting_SetV_Set(int32_t value)
{
	ModbusSetting.V_Set = value;
	ModBusSendType = WRITE_MB_PARAMREG;
}

int32_t IF_ModbusSetting_GetV_Set(void)
{	
	return ModbusSetting.I_Set;
}	

void IF_ModbusSetting_SetI_Set(int32_t value)
{
	ModbusSetting.I_Set = value;
	ModBusSendType = WRITE_MB_SETTINGEREG;
}

int32_t IF_ModbusSetting_GetI_Set(void)
{
	return ModbusSetting.I_Set;
}
/**********************param Parameters*****************************/
void IF_ModbusParam_SetFactor_Ac380_AV(int32_t value)
{
	ModbusParam.Factor_Ac380_AV = value;
	ModBusSendType = WRITE_MB_PARAMREG;
}

int32_t IF_ModbusParam_GetFactor_Ac380_AV(void)
{
	return ModbusParam.Factor_Ac380_AV;
}
//---------------------------------------------------------------------
void IF_ModbusParam_SetFactor_Ac380_BV(int32_t value)
{
	ModbusParam.Factor_Ac380_BV = value;
	ModBusSendType = WRITE_MB_PARAMREG;
}

int32_t IF_ModbusParam_GetFactor_Ac380_BV(void)
{
	return ModbusParam.Factor_Ac380_BV;
}
//---------------------------------------------------------------------
void IF_ModbusParam_SetFactor_Ac380_CV(int32_t value)
{
	ModbusParam.Factor_Ac380_CV = value;
	ModBusSendType = WRITE_MB_PARAMREG;
}
int32_t IF_ModbusParam_GetFactor_Ac380_CV(void)
{
	return  ModbusParam.Factor_Ac380_CV;
}
//---------------------------------------------------------------------
void IF_ModbusParam_SetFactor_Ac380_AI(int32_t value)
{
	ModbusParam.Factor_Ac380_AI = value;
	ModBusSendType = WRITE_MB_PARAMREG;
}
int32_t IF_ModbusParam_GetFactor_Ac380_AI(void)
{
	return  ModbusParam.Factor_Ac380_AI;
}
//----------------------------------------------------------------------
void IF_ModbusParam_SetFactor_Ac380_BI(int32_t value)
{
	ModbusParam.Factor_Ac380_BI = value;
	ModBusSendType = WRITE_MB_PARAMREG;	
}
int32_t IF_ModbusParam_GetFactor_Ac380_BI(void)
{
	return  ModbusParam.Factor_Ac380_BI;
}
//-----------------------------------------------------------------------
void IF_ModbusParam_SetFactor_Ac380_CI(int32_t value)
{
	ModbusParam.Factor_Ac380_CI = value;
	ModBusSendType = WRITE_MB_PARAMREG;
}
int32_t IF_ModbusParam_GetFactor_Ac380_CI(void)
{
	return ModbusParam.Factor_Ac380_CI;
}
//-----------------------------------------------------------------------
void IF_ModbusParam_SetFactor_Way3_I(int32_t value)
{
	ModbusParam.Factor_Way3_I = value;
	ModBusSendType = WRITE_MB_PARAMREG;
}
int32_t IF_ModbusParam_GetFactor_Way3_I(void)
{
	return ModbusParam.Factor_Way3_I;
}
//-----------------------------------------------------------------------
void IF_ModbusParam_SetFactor_Way4_I(int32_t value)
{
	ModbusParam.Factor_Way4_I = value;
	ModBusSendType = WRITE_MB_PARAMREG;
}
int32_t IF_ModbusParam_GetFactor_Way4_I(void)
{
	return  ModbusParam.Factor_Way4_I;
}
//-----------------------------------------------------------------------
void IF_ModbusParam_SetFactor_DcDc1_I(int32_t value)
{
	ModbusParam.Factor_DcDc1_I = value;
	ModBusSendType = WRITE_MB_PARAMREG;
}
int32_t IF_ModbusParam_GetFactor_DcDc1_I(void)
{
	return  ModbusParam.Factor_DcDc1_I;
}
//-----------------------------------------------------------------------
void IF_ModbusParam_SetFactor_DcDc1_V(int32_t value)
{
	ModbusParam.Factor_DcDc1_V = value;
	ModBusSendType = WRITE_MB_PARAMREG;
}
int32_t IF_ModbusParam_GetFactor_DcDc1_V(void)
{
	return  ModbusParam.Factor_DcDc1_V;
}
//-----------------------------------------------------------------------
void IF_ModbusParam_SetFactor_DcDc2_I(int32_t value)
{
	ModbusParam.Factor_DcDc2_I = value;
	ModBusSendType = WRITE_MB_PARAMREG;
}
int32_t IF_ModbusParam_GetFactor_DcDc2_I(void)
{
	return  ModbusParam.Factor_DcDc2_I;
}
//-----------------------------------------------------------------------
void IF_ModbusParam_SetFactor_DcDc2_V(int32_t value)
{
	ModbusParam.Factor_DcDc2_V = value;
	ModBusSendType = WRITE_MB_PARAMREG;
}

int32_t IF_ModbusParam_GetFactor_DcDc2_V(void)
{
	return  ModbusParam.Factor_DcDc2_V;
}
/**********************Version Parameters*****************************/
uint8_t IF_Sensor_GetACDCHardWareVersion(uint8_t *pBuf)
{
	uint8_t i= 0;
	uint8_t regAddress =  28;
	for( i=0;i< HARDVER_REG_NUM;i++)
	{
		pBuf[i]= Byte0_UINT16(Modbus_PHoldReg[regAddress+i]);
		if(pBuf[i]==0xFF)break;
	}
	return i;	
}
void IF_Sensor_SetACDCHardWareVersion(uint8_t *pBuf,uint8_t len)
{
	if(len > HARDVER_REG_NUM)len = HARDVER_REG_NUM;
	uint8_t i= 0;
	uint8_t regAddress = 28;
	for( i=0;i<len;i++)
	{
		Modbus_PHoldReg[regAddress+i]= pBuf[i];
	}
	ModBusSendType = WRITE_MB_HARDWAREREG;
}
////////////////////////////////////////////////////////////////////////////////
//      parameters 
///////////////////////////////////////////////////////////////////////////////
void IF_NvmParam_SetModbusSettings(uint8_t *pBuf, uint16_t len)
{
	if(len>= MODBUS_SETTING_LEN)len = MODBUS_SETTING_LEN;
	memcpy((uint8_t*)&ModbusSetting,pBuf,len); 	
	ModBusSendType = WRITE_MB_SETTINGEREG;	
}
void IF_NvmParam_GetModbusSettings(uint8_t *pBuf, uint16_t len)
{	
	memcpy(pBuf,(uint8_t *)&ModbusSetting,len);	
}
//--------------------------------------------------------------------------
void IF_NvmParam_SetModbusParams(uint8_t *pBuf, uint16_t len)
{
	if(len>= MODBUS_PARAM_LEN)len = MODBUS_PARAM_LEN;
	memcpy((uint8_t*)&ModbusParam,pBuf,len); 	
	ModBusSendType = WRITE_MB_PARAMREG;	
}
void IF_NvmParam_GetModbusParams(uint8_t *pBuf, uint16_t len)
{	
	memcpy(pBuf,(uint8_t *)&ModbusParam,len);	
}
//--------------------------------------------------------------------------
void IF_NvmParam_SetModbusVersons(uint8_t *pBuf, uint16_t len)
{
	if(len >= HARDVER_REG_NUM)len = HARDVER_REG_NUM;
	for(uint8_t i=0;i<len;i++)
	{
		Modbus_PHoldReg[HOLD_REG_P29-HOLD_REG_P01+i]= pBuf[i];
	}
	ModBusSendType = WRITE_MB_HARDWAREREG;
}
//-------------------------------------------------------------------------------------------------
void IF_NvmParam_GetModbusVersons(uint8_t *pBuf, uint16_t len)
{	
	len = 0; 
	//HardwareVersion
	for(uint8_t i=0;i<(HOLD_REG_P44-HOLD_REG_P29+1);i++)
	{
		pBuf[len++] =Byte0_UINT16(Modbus_PHoldReg[HOLD_REG_P29-HOLD_REG_P01+i]);
	}
	//AppVerion
	for( uint8_t i=0;i<(INPUT_REG_A48-INPUT_REG_A33+1);i++)
	{
		pBuf[len++] = Byte0_UINT16(Modbus_VersionReg[INPUT_REG_A33-INPUT_REG_A33+i]);
	}
	//BootVerion
	for(uint8_t i=0;i<(INPUT_REG_A64-INPUT_REG_A49+1);i++)
	{
		pBuf[len++] = Byte0_UINT16(Modbus_VersionReg[INPUT_REG_A49-INPUT_REG_A33+i]);
	}
	//checksum
	for(uint8_t i=0;i<(INPUT_REG_A68-INPUT_REG_A65+1);i++)
	{
		pBuf[len++] = Byte0_UINT16(Modbus_VersionReg[INPUT_REG_A65-INPUT_REG_A33+i]);
	}
}
/**********************Modbus ParameterType Read/Write**************************/
void IF_ACDC_SetParamsRWType(ModBusTypeEnum ModbusType)
{
	if(ModbusType != ModBusSendType)
	{
		xQueueReset(ModbusRxQueue);    //清除Modbus发送队列
		ModBusSendType = ModbusType;   //改变数据发送类型.
	}
}
ModBusTypeEnum IF_ACDC_GetParamsRWType(void)
{
	return  ModBusSendType;
}
/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/
static void Update_ACDCSettingToPHoldRegister(void)
{	
	Modbus_PHoldReg[0]= ModbusSetting.V_Set/1000;
	Modbus_PHoldReg[1]= ModbusSetting.V_Set%1000;	
	Modbus_PHoldReg[2]= ModbusSetting.I_Set/1000;
	Modbus_PHoldReg[3]= ModbusSetting.I_Set%1000;
}

static void Update_PHoldRegisterToACDCSetting(void)
{
	ModbusSetting.V_Set = Modbus_PHoldReg[0]* 1000 + Modbus_PHoldReg[1];
	ModbusSetting.I_Set = Modbus_PHoldReg[2]* 1000 + Modbus_PHoldReg[3];
}	

static void Update_ACDCParamToPHoldRegister(void)
{	
	Modbus_PHoldReg[4] =  ModbusParam.Factor_Ac380_AV/1000;
	Modbus_PHoldReg[5] =  ModbusParam.Factor_Ac380_AV%1000;	
	Modbus_PHoldReg[6] =  ModbusParam.Factor_Ac380_AI/1000;
	Modbus_PHoldReg[7] =  ModbusParam.Factor_Ac380_AI%1000;	
	Modbus_PHoldReg[8] =  ModbusParam.Factor_Way3_I/1000;
	Modbus_PHoldReg[9] =  ModbusParam.Factor_Way3_I%1000;	
	Modbus_PHoldReg[10]=  ModbusParam.Factor_Way4_I/1000;
	Modbus_PHoldReg[11]=  ModbusParam.Factor_Way4_I%1000;	
	Modbus_PHoldReg[12]=  ModbusParam.Factor_DcDc1_I/1000;
	Modbus_PHoldReg[13]=  ModbusParam.Factor_DcDc1_I%1000;	
	Modbus_PHoldReg[14]=  ModbusParam.Factor_DcDc1_V/1000;
	Modbus_PHoldReg[15]=  ModbusParam.Factor_DcDc1_V%1000;	
	Modbus_PHoldReg[16]=  ModbusParam.Factor_DcDc2_I/1000;
	Modbus_PHoldReg[17]=  ModbusParam.Factor_DcDc2_I%1000;	
	Modbus_PHoldReg[18]=  ModbusParam.Factor_DcDc2_V/1000;
	Modbus_PHoldReg[19]=  ModbusParam.Factor_DcDc2_V%1000;	
	Modbus_PHoldReg[20]=  ModbusParam.Factor_Ac380_CV/1000;
	Modbus_PHoldReg[21]=  ModbusParam.Factor_Ac380_CV%1000;	
	Modbus_PHoldReg[22]=  ModbusParam.Factor_Ac380_CI/1000;
	Modbus_PHoldReg[23]=  ModbusParam.Factor_Ac380_CI%1000;	
	Modbus_PHoldReg[24]=  ModbusParam.Factor_Ac380_BV/1000;
	Modbus_PHoldReg[25]=  ModbusParam.Factor_Ac380_BV%1000;	
	Modbus_PHoldReg[26]=  ModbusParam.Factor_Ac380_BI/1000;
	Modbus_PHoldReg[27]=  ModbusParam.Factor_Ac380_BI%1000;	
}	

static void Update_PHoldRegisterToACDCParam(void)
{
	ModbusParam.Factor_Ac380_AV = Modbus_PHoldReg[4]* 1000 + Modbus_PHoldReg[5];	
	ModbusParam.Factor_Ac380_AI = Modbus_PHoldReg[6]* 1000 + Modbus_PHoldReg[7];		
	ModbusParam.Factor_Way3_I   = Modbus_PHoldReg[8]* 1000 + Modbus_PHoldReg[9];	
	ModbusParam.Factor_Way4_I   = Modbus_PHoldReg[10]* 1000 + Modbus_PHoldReg[11];	
	ModbusParam.Factor_DcDc1_I  = Modbus_PHoldReg[12]* 1000 + Modbus_PHoldReg[13];	
	ModbusParam.Factor_DcDc1_V  = Modbus_PHoldReg[14]* 1000 + Modbus_PHoldReg[15];	
	ModbusParam.Factor_DcDc2_I  = Modbus_PHoldReg[16]* 1000 + Modbus_PHoldReg[17];
	ModbusParam.Factor_DcDc2_V  = Modbus_PHoldReg[18]* 1000 + Modbus_PHoldReg[19];		
	ModbusParam.Factor_Ac380_CV = Modbus_PHoldReg[20]* 1000 + Modbus_PHoldReg[21];	
	ModbusParam.Factor_Ac380_CI = Modbus_PHoldReg[22]* 1000 + Modbus_PHoldReg[23];	
	ModbusParam.Factor_Ac380_BV = Modbus_PHoldReg[24]* 1000 + Modbus_PHoldReg[25];
	ModbusParam.Factor_Ac380_BI = Modbus_PHoldReg[26]* 1000 + Modbus_PHoldReg[27];
}

static void Update_RegisterToStatusAndFualtWord(void)
{
	UINT32_VAL StatusValue,FaultValue;	
	
	FaultValue.w[0] = Modbus_InputReg[28];
	FaultValue.w[1] = Modbus_InputReg[29];
	
	StatusValue.w[0] = Modbus_InputReg[30];
	StatusValue.w[1] = Modbus_InputReg[31]; 
	
	g_StatusWord.bits.ACDCStatusOK =	StatusValue.bits.b16;			
	g_StatusWord.bits.ACDCEn_AC    =	StatusValue.bits.b17;				    
	g_StatusWord.bits.ACDCEn_PA	   =	StatusValue.bits.b18;			   
	g_StatusWord.bits.ACDCKout_Fb  =	StatusValue.bits.b19;
	
	g_FaultWord.bits.AV380_HAlarm  = 	FaultValue.bits.b11;		   			
	g_FaultWord.bits.AV380_LAlarm  = 	FaultValue.bits.b12;					   	
	g_FaultWord.bits.AI380_HAlarm  = 	FaultValue.bits.b13;					   	
	g_FaultWord.bits.BV380_HAlarm  = 	FaultValue.bits.b14;		   				
	g_FaultWord.bits.BV380_LAlarm  = 	FaultValue.bits.b15;					   
	g_FaultWord.bits.BI380_HAlarm  = 	FaultValue.bits.b16;					   	
	g_FaultWord.bits.CV380_HAlarm  = 	FaultValue.bits.b17;		   				
	g_FaultWord.bits.CV380_LAlarm  = 	FaultValue.bits.b18;					   	
	g_FaultWord.bits.CI380_HAlarm  = 	FaultValue.bits.b19;					   	
	g_FaultWord.bits.DCDC1V_HAlarm =	FaultValue.bits.b20;					   	
	g_FaultWord.bits.DCDC1V_LAlarm =	FaultValue.bits.b21;					   
	g_FaultWord.bits.DCDC1I_HAlarm =	FaultValue.bits.b22;					   
	g_FaultWord.bits.DCDC1T_HAlarm =	FaultValue.bits.b23;					   	
	g_FaultWord.bits.DCDC2V_HAlarm =	FaultValue.bits.b24;					   
	g_FaultWord.bits.DCDC2V_LAlarm =	FaultValue.bits.b25;					   	
	g_FaultWord.bits.DCDC2_HAlarm  =	FaultValue.bits.b26;					   
	g_FaultWord.bits.DCDC2T_HAlarm =	FaultValue.bits.b27;	
	ModbusSendTypeTimer++;					
	if(ModbusSendTypeTimer > 100)//跳转到readVersiondata
	{
		ModbusSendTypeTimer = 0;
		IF_ACDC_SetParamsRWType(READ_MB_VERSIONREG); 
	}	
}

//*****************************************************************************
//* END
//*****************************************************************************

