 /*******************************************************************************************
 * modbus.c
 *
 *  Created on: 2023年5月9日
 *  Author:  mengxingping
 ****************************************************************************************/
#include "Modbus.h"
#include "IF_HAL_Cfg.h"
#include "IF_SL_Cfg.h"

static uint32_t ModbusTimer = 0; 	//modBus 接收帧计时
static ModBuffer_t txModBuf;		//发送数据

ModBusTypeEnum ModBusSendType;    	   			//Modbus发送标志状态

uint16_t Modbus_HoldReg[MAX_REG_NUM] = {0};   	 //保持寄存器指针
uint16_t Modbus_InputReg[INPUT_REG_NUM] = {0};   //输入寄存器指针
uint16_t Modbus_ParamReg[PARAM_REG_NUM] = {0};


extern CommMsg_t g_RxBuf[PORT_NUM];
extern CommMsg_t g_TxBuf[PORT_NUM];
static void RS485_SendData(uint8_t port,uint8_t *buf,uint16_t len);
static uint16_t getModbusCRC16(uint8_t *ptr, uint16_t len);
/* FUNCTION *********************************************************************************
 * Function Name : Modbus_TxTimeManagment
 * Description   : Modbus发送查询数据
 * Parameter     : port 发送数据端口
 * END ***************************************************************************************/
void Modbus_TxBufManagment(uint8_t port)
{	
	switch(ModBusSendType)
	{
		case DEVICE_INPUTREG:
		{		
			MB_ReadInputReg_04H(MODBUS_ADDR, INPUT_REG_A01,INPUT_REG_NUM);	
		}
		
		break;
		case DEVICE_HOLDREG:
		{		
			MB_WriteNumHoldingReg_10H(MODBUS_ADDR, HOLD_REG_A01,HOLD_REG_NUM,(uint8_t*)Modbus_ParamReg);
			ModBusSendType = DEVICE_INPUTREG;
		}
		break;
		case DEVICE_UPDATE:
		{
			if(pdTRUE == xSemaphoreTake(ModbusSemaphore, 200))
			{
				MB_WriteNumHoldingReg_10H(MODBUS_ADDR, HOLD_REG_SW_START,INPUT_REG_NUM,(uint8_t*)Modbus_HoldReg);
			}else
			{
				ModBusSendType = DEVICE_INPUTREG;
			}
		}
		break;
		default:break;
	}
}
/* FUNCTION *********************************************************************************
 * Function Name : Dealwith_Modbus_Service
 * Description   : Modbus介绍查询数据
 * Parameter     : pRxMsg 接收数据
 * END ***************************************************************************************/
void Dealwith_Modbus_Service(pCommMsg_t pRxMsg)
{
	static uint16_t crc_check = 0;	
    if(pRxMsg->data[0] == MODBUS_ADDR)
	{
		crc_check = (pRxMsg->data[pRxMsg->len-1]<<8) | pRxMsg->data[pRxMsg->len-2]; //验证CRC
		if(crc_check ==getModbusCRC16(pRxMsg->data,(pRxMsg->len-2)))
		{
			switch(pRxMsg->data[1])  //Func_Code
			{ 
				case READ_INPUTREG_FUNC:
				if(pRxMsg->data[2] == (INPUT_REG_NUM*2)) //		
				{
										
					Modbus_InputReg[0] =MAKE_UINT16(pRxMsg->data[3],pRxMsg->data[4]);
					Modbus_InputReg[1] =MAKE_UINT16(pRxMsg->data[5],pRxMsg->data[6]);
					Modbus_InputReg[2] =MAKE_UINT16(pRxMsg->data[7],pRxMsg->data[8]);
					Modbus_InputReg[3] =MAKE_UINT16(pRxMsg->data[9],pRxMsg->data[10]);
					Modbus_InputReg[4] =MAKE_UINT16(pRxMsg->data[11],pRxMsg->data[12]);
					Modbus_InputReg[5] =MAKE_UINT16(pRxMsg->data[13],pRxMsg->data[14]);
					Modbus_InputReg[6] =MAKE_UINT16(pRxMsg->data[15],pRxMsg->data[16]);
					Modbus_InputReg[7] =MAKE_UINT16(pRxMsg->data[17],pRxMsg->data[18]);
					Modbus_InputReg[8] =MAKE_UINT16(pRxMsg->data[19],pRxMsg->data[20]);
					Modbus_InputReg[9] =MAKE_UINT16(pRxMsg->data[21],pRxMsg->data[22]);
					Modbus_InputReg[10]=MAKE_UINT16(pRxMsg->data[23],pRxMsg->data[24]);
					Modbus_InputReg[11]=MAKE_UINT16(pRxMsg->data[25],pRxMsg->data[26]);
					Modbus_InputReg[12]=MAKE_UINT16(pRxMsg->data[27],pRxMsg->data[28]);
					Modbus_InputReg[13]=MAKE_UINT16(pRxMsg->data[29],pRxMsg->data[30]);
					Modbus_InputReg[14]=MAKE_UINT16(pRxMsg->data[31],pRxMsg->data[32]);
					Modbus_InputReg[15]=MAKE_UINT16(pRxMsg->data[33],pRxMsg->data[34]);
				}
				break;
				case WRITE_MULTHOLDREG_FUNC:
				 if(HOLD_REG_SW_START==MAKE_UINT16(pRxMsg->data[2],pRxMsg->data[3]))
				 {
					 xSemaphoreGive(ModbusSemaphore);
				 }else
				 {
					
				 }
				 break;			
			}	
		}	
	}	
}	
/* FUNCTION *********************************************************************************
 * Function Name : Modbus_ReceiveChar
 * Description   : 查表法计算crc，
 * Parameter     : ptr: 需要处理的字节
 * Parameter     : len: 字节的长度
 * Return        : crc  crc16校验值
 * END ***************************************************************************************/
static uint16_t getModbusCRC16(uint8_t *ptr, uint16_t len)
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
static void RS485_SendData(uint8_t port,uint8_t *buf,uint16_t len)
{	
	/*使能发送数据接口*/	
	for(uint8_t i = 0;i <= len;i++)
	{
		g_TxBuf[port].data[g_TxBuf[port].len++]= buf[i];
	}
	/*等待接受完成*/
	while(g_RxBuf[port].flag_status);  
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
	g_RxBuf[port].port = port;
	g_RxBuf[port].flag_status = 1;    
	g_RxBuf[port].data[g_RxBuf[port].len++] = ch;
	if(g_RxBuf[port].len >= BUFFER_SIZE)        //接收的字符超出边界
	{
		g_RxBuf[port].flag_status = 0;
		memset((void*)&g_RxBuf[port],0,sizeof(CommMsg_t));
	}
	ModbusTimer = 0;  					//清除上次modbus定时器计时	
}
/* FUNCTION *********************************************************************************
 * Function Name : Check_Modbus_CallBack
 * Description   : 接收字节时间回调函数，接收字符间隔超过3.5ms(4ms)，接收数据完成
 * Parameter     : port: 端口号
 * Parameter     : ch: 需要处理的字节
 * Return        :
 * END ***************************************************************************************/
void Check_Modbus_CallBack(uint8_t port)
{	
	if(g_RxBuf[port].flag_status == 1)  //接收帧开始
	{
		if(ModbusTimer >= 4)   //接收字符间隔在3.5ms~20ms
		{
			SendToRxQueueFromISR(port, &g_RxBuf[port]);     //接收完成,发送到队列中	
			memset((void*)&g_RxBuf[port],0,sizeof(CommMsg_t));	
			g_RxBuf[port].flag_status = 0;					 		//接收帧完成
		}
		ModbusTimer++;
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
		memset((void*)&g_TxBuf[port],0,sizeof(CommMsg_t));
		IF_HAL_UartDisableTx(port);    //关闭中断
		RS485_RX_EN();                 //发送使能
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
	uint16_t crc =0;
	memset(&txModBuf,0,sizeof(ModBuffer_t));
	txModBuf.buf[txModBuf.len++]= addr;         /*从站地址*/
	txModBuf.buf[txModBuf.len++]= 0x01;			/*功能码*/
	txModBuf.buf[txModBuf.len++]= reg >>8;		/*寄存器地址，高字节*/		
	txModBuf.buf[txModBuf.len++]= reg ;			/*寄存器地址，低字节*/	
	txModBuf.buf[txModBuf.len++]= num >>8;		/*线圈(bit)个数，高字节*/	
	txModBuf.buf[txModBuf.len++]= num;			/*线圈(bit)个数，低字节*/		
	crc = getModbusCRC16(txModBuf.buf,txModBuf.len);	
	txModBuf.buf[txModBuf.len++]= crc;			/*Mobus crc，低字节*/	
	txModBuf.buf[txModBuf.len++]= crc >>8;		/*Mobus crc，高字节*/
	RS485_SendData(RS485_UART,txModBuf.buf,txModBuf.len);
}
/* FUNCTION *********************************************************************************
 * Function Name : MB_WriteCoil_05H
 * Description   : 写单个线圈
 * Parameter     : addr:从站地址，reg:寄存器地址，sta:读线圈状态
 * Return        :
 * END ***************************************************************************************/
void MB_WriteCoil_05H(uint8_t addr, uint16_t reg, uint16_t sta)
{
	uint16_t crc =0;
	memset(&txModBuf,0,sizeof(ModBuffer_t));
	txModBuf.buf[txModBuf.len++]= addr;         /*从站地址*/
	txModBuf.buf[txModBuf.len++]= 0x05;			/*功能码*/
	txModBuf.buf[txModBuf.len++]= reg >>8;		/*寄存器地址，高字节*/		
	txModBuf.buf[txModBuf.len++]= reg;			/*寄存器地址，低字节*/	
	txModBuf.buf[txModBuf.len++]= sta >>8;		/*线圈(bit)个数，高字节*/	
	txModBuf.buf[txModBuf.len++]= sta;			/*线圈(bit)个数，低字节*/		
	crc = getModbusCRC16(txModBuf.buf,txModBuf.len);	
	txModBuf.buf[txModBuf.len++]= crc;			/*Mobus crc，低字节*/	
	txModBuf.buf[txModBuf.len++]= crc >>8;		/*Mobus crc，高字节*/
	RS485_SendData(RS485_UART,txModBuf.buf,txModBuf.len);	
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
	memset(&txModBuf,0,sizeof(ModBuffer_t));
	txModBuf.buf[txModBuf.len++]= addr;         /*从站地址*/
	txModBuf.buf[txModBuf.len++]= 0x02;			/*功能码*/
	txModBuf.buf[txModBuf.len++]= reg >>8;		/*寄存器地址，高字节*/		
	txModBuf.buf[txModBuf.len++]= reg;			/*寄存器地址，低字节*/	
	txModBuf.buf[txModBuf.len++]= num >>8;		/*开关(bit)个数，高字节*/	
	txModBuf.buf[txModBuf.len++]= num;			/*开关(bit)个数，低字节*/		
	crc = getModbusCRC16(txModBuf.buf,txModBuf.len);	
	txModBuf.buf[txModBuf.len++]= crc;			/*Mobus crc，低字节*/	
	txModBuf.buf[txModBuf.len++]= crc >>8;		/*Mobus crc，高字节*/
	RS485_SendData(RS485_UART,txModBuf.buf,txModBuf.len);		
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
	memset(&txModBuf,0,sizeof(ModBuffer_t));
	txModBuf.buf[txModBuf.len++]= addr;         /*从站地址*/
	txModBuf.buf[txModBuf.len++]= 0x04;			/*功能码*/
	txModBuf.buf[txModBuf.len++]= reg >>8;		/*寄存器地址，高字节*/		
	txModBuf.buf[txModBuf.len++]= reg;			/*寄存器地址，低字节*/	
	txModBuf.buf[txModBuf.len++]= num >>8;		/*寄存器个数，高字节*/	
	txModBuf.buf[txModBuf.len++]= num;			/*寄存器个数，低字节*/		
	crc = getModbusCRC16(txModBuf.buf,txModBuf.len);	
	txModBuf.buf[txModBuf.len++]= crc;			/*Mobus crc，低字节*/	
	txModBuf.buf[txModBuf.len++]= crc >>8;		/*Mobus crc，高字节*/
	RS485_SendData(RS485_UART,txModBuf.buf,txModBuf.len);	
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
	memset(&txModBuf,0,sizeof(ModBuffer_t));
	txModBuf.buf[txModBuf.len++]= addr;         /*从站地址*/
	txModBuf.buf[txModBuf.len++]= 0x03;			/*功能码*/
	txModBuf.buf[txModBuf.len++]= reg >>8;		/*寄存器地址，高字节*/		
	txModBuf.buf[txModBuf.len++]= reg;			/*寄存器地址，低字节*/	
	txModBuf.buf[txModBuf.len++]= num >>8;		/*寄存器个数，高字节*/	
	txModBuf.buf[txModBuf.len++]= num;			/*寄存器个数，低字节*/		
	crc = getModbusCRC16(txModBuf.buf,txModBuf.len);	
	txModBuf.buf[txModBuf.len++]= crc;			/*Mobus crc，低字节*/	
	txModBuf.buf[txModBuf.len++]= crc >>8;		/*Mobus crc，高字节*/
	RS485_SendData(RS485_UART,txModBuf.buf,txModBuf.len);	
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
	memset(&txModBuf,0,sizeof(ModBuffer_t));
	txModBuf.buf[txModBuf.len++]= addr;         /*从站地址*/
	txModBuf.buf[txModBuf.len++]= 0x06;			/*功能码*/
	txModBuf.buf[txModBuf.len++]= reg >>8;		/*寄存器地址，高字节*/		
	txModBuf.buf[txModBuf.len++]= reg;			/*寄存器地址，低字节*/	
	txModBuf.buf[txModBuf.len++]= data >>8;		/*寄存器个数，高字节*/	
	txModBuf.buf[txModBuf.len++]= data;			/*寄存器个数，低字节*/		
	crc = getModbusCRC16(txModBuf.buf,txModBuf.len);	
	txModBuf.buf[txModBuf.len++]= crc;			/*Mobus crc，低字节*/	
	txModBuf.buf[txModBuf.len++]= crc >>8;		/*Mobus crc，高字节*/
	RS485_SendData(RS485_UART,txModBuf.buf,txModBuf.len);	
}

/* FUNCTION *********************************************************************************
 * Function Name : MB_WriteCoil_05H
 * Description   : 写多个线圈状态(CoilStatus)
 * Parameter     : addr:从站地址，reg:寄存器地址，num：线圈个数,sta:读线圈状态
 * Return        :
 * END ***************************************************************************************/
void MB_WriteNumCoil_0FH(uint8_t addr, uint16_t reg, uint16_t num,uint8_t  *databuf)
{
	uint16_t crc = 0,Count = 0;
	if(num%8==0)
	{
		Count = num/8;
	}else
	{
		Count = num/8+1;
	}	
	memset(&txModBuf,0,sizeof(ModBuffer_t));
	txModBuf.buf[txModBuf.len++]= addr;         /*从站地址*/
	txModBuf.buf[txModBuf.len++]= 0x0F;			/*功能码*/
	txModBuf.buf[txModBuf.len++]= reg >>8;		/*寄存器地址，高字节*/		
	txModBuf.buf[txModBuf.len++]= reg;			/*寄存器地址，低字节*/	
	txModBuf.buf[txModBuf.len++]= num >>8;		/*线圈(16bit)个数，高字节*/	
	txModBuf.buf[txModBuf.len++]= num;			/*线圈(16bit)个数，低字节*/
	txModBuf.buf[txModBuf.len++]= Count;		/*数据个数*/
	for(uint16_t i=0;i<(num<<1)+1;i++)
	{
		txModBuf.buf[txModBuf.len++]= databuf[i];		/*后面数据长度*/
	}
	crc = getModbusCRC16(txModBuf.buf,txModBuf.len);	
	txModBuf.buf[txModBuf.len++]= crc;			/*Mobus crc，低字节*/	
	txModBuf.buf[txModBuf.len++]= crc >>8;		/*Mobus crc，高字节*/
	RS485_SendData(RS485_UART,txModBuf.buf,txModBuf.len);	
}
/* FUNCTION *********************************************************************************
 * Function Name : Modbus_03_Solve
 * Description   : Modbus写多个输出开关量，Modbus功能码15处理程序
 * Parameter     :
 * Return        :
 * END ***************************************************************************************/
void MB_WriteNumHoldingReg_10H(uint8_t addr, uint16_t reg, uint16_t num,uint8_t *databuf)
{
	uint16_t crc =0;
	memset(&txModBuf,0,sizeof(ModBuffer_t));
	txModBuf.buf[txModBuf.len++]= addr;         /*从站地址*/
	txModBuf.buf[txModBuf.len++]= 0x10;			/*功能码*/
	txModBuf.buf[txModBuf.len++]= reg >>8;		/*寄存器地址，高字节*/		
	txModBuf.buf[txModBuf.len++]= reg;			/*寄存器地址，低字节*/	
	txModBuf.buf[txModBuf.len++]= num>>8;		/*寄存器个数，高字节*/	
	txModBuf.buf[txModBuf.len++]= num;			/*寄存器个数，低字节*/			
	txModBuf.buf[txModBuf.len++]= num <<1;		/*数据个数，低字节*/	
	for(uint16_t i=0;i<2*num;i++)
   	{
		txModBuf.buf[txModBuf.len++] = databuf[i];  /*后面数据长度*/
	}
	crc = getModbusCRC16(txModBuf.buf,txModBuf.len);	
	txModBuf.buf[txModBuf.len++]= crc;			/*Mobus crc，低字节*/	
	txModBuf.buf[txModBuf.len++]= crc >>8;		/*Mobus crc，高字节*/
	RS485_SendData(RS485_UART,txModBuf.buf,txModBuf.len);
}

//*****************************************************************************
//* END
//*****************************************************************************

