/********************************************************************************
  * @doc     		： CSIPV1.c
  * @author  		： xingping.meng
  * @fw       		： V1.0.0
  * @date       	： 2022年8月15日
  * @information 	：用户协议驱动程序源文件
  ******************************************************************************/
#include "IF_HAL_Cfg.h"
#include "IF_SL_Cfg.h"
#include "csipv1.h"

extern CommMsg_t g_RxBuf[PORT_NUM];
extern CommMsg_t g_TxBuf[PORT_NUM];
extern uint32_t last_rx_time[PORT_NUM];
extern uint32_t current_rx_time[PORT_NUM];
//中断接收使用的变量

static uint8_t  DealwithCsipQueueCommand(pCommData_t rxCmd,char *txBuf);
static uint8_t  DealwithCsipSetupCommand(uint8_t port,pCommData_t CmdData);
static uint8_t  ParseOneParamMsg(pCommMsg_t pRxMsg,pCommData_t pRxCmd);
static uint8_t  CSIPv1_ProtecolParse(pCommMsg_t pRxMsg,char *txBuf);
static void  CSIPv1_SendFrame(uint8_t port,uint8_t state,char *txBuf);

/* FUNCTION **********************************************************************************
 *
 * Function Name : uint8_t  ParseOneParamMsg(pCommMsg_t pRxMsg,pCommData_t pRxCmd)
 * Description   : 命令参数解析
 * Parameters	 : pRxMsg----接受到的消息    pRxCmd------消息解析出来的命令
 * Return		 : ret
 * END ********************************************************************************************/
static uint8_t  ParseOneParamMsg(pCommMsg_t pRxMsg,pCommData_t pRxCmd)
{
	uint8_t ret = E_NOT_OK;	
	uint8_t inum = 0,knum = 0;
	uint8_t MsgLength = pRxMsg->len-1; //字符串长度减去1   字符<CR>
	for(uint8_t i = 0;i < MsgLength;i++) 
	{
		if(pRxCmd->Space == 0)  //没有找到第一个空格
		{
			//消息指令解析
			if((pRxMsg->data[i]>='A'&&pRxMsg->data[i]<='Z')||(pRxMsg->data[i]>='0'&&pRxMsg->data[i]<='9'))
			{
				pRxCmd->cmd[pRxCmd->cmdLen++]= pRxMsg->data[i]; //得到命令字符和长度
			}
			else if(pRxMsg->data[i] ==0x20) //找到第一个空格
			{
				pRxCmd->Space = 1;
			}else
			{
				return ret;
			}		
		}else  //已经找到第一个空格字符
		{
			//命令数据类型(整数、负数、小数)
			if((pRxMsg->data[i]>='0'&&pRxMsg->data[i]<='9')||(pRxMsg->data[i] =='-')||(pRxMsg->data[i] =='.'))
			{
				if(pRxMsg->data[i] =='.')
				{
					pRxCmd->IsFloat = 1;
					inum++;
				}
				if(pRxMsg->data[i] =='-')knum++;
				if((inum >1)||(knum >1))return ret;
				if((pRxCmd->dataLen != 0) && (pRxMsg->data[i] =='-'))return ret;  //符号位不是第一位
				if((pRxCmd->dataLen == 0)&&(pRxMsg->data[i] =='.'))return ret; //小数点位是第一位
				pRxCmd->data[pRxCmd->dataLen++] =pRxMsg->data[i]; 			   //得到命令数据和长度  	
			}else
			{
				return ret;
			}					
		}		
	}
	ret = E_OK;
	return ret;
}
/* FUNCTION **********************************************************************************
 *
 * Function Name : uint8_t  CSIPv1_ProtecolParse(pCommMsg_t pRxMsg)
 * Description   : 协议内容解析
  * Parameters	 : pRxMsg----接受到的消息   
 * Return		 : ret -----解析内容是否正确
 * END ********************************************************************************************/	
static uint8_t  CSIPv1_ProtecolParse(pCommMsg_t pRxMsg,char *txBuf)
{
		uint8_t ret = E_NOT_OK;	
		CommData_t rxCmd;
		memset(&rxCmd,0,sizeof(CommData_t));
		if(E_NOT_OK == ParseOneParamMsg(pRxMsg,&rxCmd))return ret; //解析不正确		
		if(rxCmd.Space == 0)   //如果没有空格，该指令为查询(或者无参数设置)指令
		{	
			ret =  DealwithCsipQueueCommand(&rxCmd, txBuf);
		} 
		else if(rxCmd.dataLen)  //如果有第二个空格并且有命令数据，该指令为设置指令
		{
			ret = DealwithCsipSetupCommand(pRxMsg->port,&rxCmd);
		}		
		return ret;
}
/* FUNCTION *********************************************************************************
 * Function Name : DealwithCsipQueueCommand
 * Description   : 查询命令处理
 * Parameter     : CommData_t: 输入参数    需要解析的指令
 * Parameter     : txBuf     ：输出参数    需要发送的字节
 * Return        : ret       : 接受数据状态
 * END ***************************************************************************************/
static uint8_t DealwithCsipQueueCommand(pCommData_t CmdData,char *txBuf)
{	
	uint8_t ret = E_NOT_OK;		
	//Query Hardware version
	if (0==strcmp(CmdData->cmd, "QHV"))     		        
	{
		uint8_t Buf[DATA_SIZE] ={0}; 
		IF_IdentParam_GetHardwareVersion(Buf);
		sprintf(txBuf, "%s", Buf);
		ret = E_OK;	
	}
	//Query Software version
	else if (0==strcmp(CmdData->cmd, "QSV"))      	
	{
		uint8_t Buf[DATA_SIZE] ={0}; 
		IF_IdentParam_GetSoftwareVersion(Buf);
		sprintf(txBuf, "%s", Buf);
		ret = E_OK;	
	}
	//Query Product Code 
	else if (0==strcmp(CmdData->cmd, "QPC"))      		  		
	{
		uint8_t Buf[DATA_SIZE] ={0}; 
		IF_IdentParam_GetProductModel(Buf);
		sprintf(txBuf, "%s", Buf);
		ret = E_OK;
	}
	//Query Serial Number  
	else if (0==strcmp(CmdData->cmd, "QSN"))      			  		
	{
		uint8_t Buf[DATA_SIZE] ={0}; 
		IF_IdentParam_GetSerialNumber(Buf);
		sprintf(txBuf, "%s", Buf);
		ret = E_OK;
		
	}
	
	//Query SetPoint  
	else if (0==strcmp(CmdData->cmd,"QSP"))      					  		
	{
		uint16_t value = IF_CmdParam_GetSetPoint();
		sprintf(txBuf,"%d",value);	
		ret = E_OK;	
	}
	//Query Regulation Mode
	else if (0==strcmp(CmdData->cmd,"QRM"))      					  		
	{
		uint8_t value = IF_UserParam_GetRegulationMode();
		sprintf(txBuf,"%d",value);	
		ret = E_OK;	
	}
	//Query RF State
	else if (0==strcmp(CmdData->cmd,"QRFS"))      					  		
	{
		uint8_t value = IF_CmdParam_GetRFPowerState();
		sprintf(txBuf,"%d",value);	
		ret = E_OK;	
	}
	//Query Forward Power
	else if (0==strcmp(CmdData->cmd,"QFP"))      					  		
	{
		uint16_t value =(uint16_t)(IF_Fpga_GetSensor(ChnN_Pfwd)+0.5F);
		if(value > IF_CmdParam_GetSetPoint())value = IF_CmdParam_GetSetPoint();
		value = (g_StatusWord.bits.RFOnOrOff)?value:0;
		sprintf(txBuf,"%d",value);
			
		ret = E_OK;	
	}
	//Query Reflect Power
	else if (0==strcmp(CmdData->cmd,"QRP"))      					  		
	{
		uint16_t value = IF_Fpga_GetSensor(ChnN_Pref);
		if(value > IF_CmdParam_GetSetPoint())value = IF_CmdParam_GetSetPoint();
		value = (g_StatusWord.bits.RFOnOrOff)?value:0;
		sprintf(txBuf,"%d",value);	
		ret = E_OK;	
	}
	//Query Load Power
	else if (0==strcmp(CmdData->cmd,"QLP"))      					  		
	{
		uint16_t value = (uint16_t)(IF_Fpga_GetSensor(ChnN_Pdlv)+0.5F);
		if(value > IF_CmdParam_GetSetPoint())value = IF_CmdParam_GetSetPoint();
		value = (g_StatusWord.bits.RFOnOrOff)?value:0;
		sprintf(txBuf,"%d",value);	
		ret = E_OK;	
	}
	//Query Forward Power Limit
	else if (0==strcmp(CmdData->cmd,"QFPL"))      					  		
	{
		uint16_t value = IF_UserParam_GetForwardPowerLimit();
		sprintf(txBuf,"%d",value);	
		ret = E_OK;	
	}
	//Query Reflect Power Limit
	else if (0==strcmp(CmdData->cmd,"QRPL"))      					  		
	{
		uint16_t value = IF_UserParam_GetReflectPowerLimit();
		sprintf(txBuf,"%d",value);	
		ret = E_OK;	
	}
	//Query DC Power Limit
	else if (0==strcmp(CmdData->cmd,"QRPL"))      					  		
	{
		uint16_t value = IF_UserParam_GetDCBiasPowerLimit();
		sprintf(txBuf,"%d",value);	
		ret = E_OK;	
	}
	//Query Reflect Power Threshold
	else if (0==strcmp(CmdData->cmd,"QRPT"))      					  		
	{
		uint16_t value = IF_UserParam_GetReflectPowerThreshold();
		sprintf(txBuf,"%d",value);	
		ret = E_OK;	
	}
	//Query Reflect Power Delay Off Timer
	else if (0==strcmp(CmdData->cmd,"QDRPT"))      					  		
	{
		uint8_t value = IF_UserParam_GetReflectPowerDelayOff();
		sprintf(txBuf,"%d",value);	
		ret = E_OK;	
	}
	//Query Vdc factor
	else if (0==strcmp(CmdData->cmd,"QVDCF"))      					  		
	{
		uint16_t value = IF_UserParam_GetVDCFactor();
		sprintf(txBuf,"%d",value);	
		ret = E_OK;	
	}
	//Query Pulse mode
	else if (0==strcmp(CmdData->cmd,"QPSM"))      					  		
	{
		uint16_t value = IF_UserParam_GetPulseMode();
		sprintf(txBuf,"%d",value);	
		ret = E_OK;	
	}
	//Query Pulse Frquency
	else if (0==strcmp(CmdData->cmd,"QPSF"))      					  		
	{
		uint16_t value = IF_UserParam_GetPulseFrequency();
		sprintf(txBuf,"%d",value);	
		ret = E_OK;	
	}
	//Query Pulse Duty Cirlcle
	else if (0==strcmp(CmdData->cmd,"QPDC"))      					  		
	{
		uint8_t value = IF_UserParam_GetPulseDutyCircle();
		sprintf(txBuf,"%d",value);	
		ret = E_OK;	
	}
	//Query Vdc bias   
	else if (0==strcmp(CmdData->cmd, "QVDC"))      			  		
	{	
		int16_t value = IF_Get_Sensor_DCBias();
		sprintf(txBuf,"%d", value);
		ret = E_OK;
	}
	//Query Uart Baudrate     
	else if(0==strcmp(CmdData->cmd, "QBAUD"))  	  		
	{
		
		uint8_t value = IF_CommParam_GetDataRate();;
		sprintf(txBuf,"%u",IF_HAL_GetBaudRateValue(value));	
		ret = E_OK;	
	}
	return ret;
}
/* FUNCTION *********************************************************************************
 * Function Name : DealwithCsipQueueCommand
 * Description   : 查询命令处理
 * Parameter     : port:       输入参数     端口号
 * Parameter     : CommData_t: 输入参数    需要解析的指令
 * Return        :state      接受数据状态
 * END ***************************************************************************************/
static uint8_t DealwithCsipSetupCommand(uint8_t port,pCommData_t CmdData)
{
	uint8_t ret = E_NOT_OK;	
	int64_t value = 0;
	char *endptr;
	char numeric_str[16]={0};
	if(CmdData->IsFloat == 0) 
	{
		value = strtoll(CmdData->data,&endptr,10);
	}else
	{
		double fvalue = strtod(CmdData->data,&endptr);
		fvalue=fvalue *1000.0F;
		snprintf(numeric_str,sizeof(numeric_str),"%.0f",fvalue);
		value = strtoll(numeric_str,&endptr,10);
	}
	//Active/Disactive Seriese Interface. 
	if (0==strcmp(CmdData->cmd, "ASI"))      							 	  		
	{	
		if(CmdData->IsFloat)return ret;
		if(value==0||value== 1)
		{
			g_GS[port].bits.active = value;
			ret = E_OK;
		}	
	}else if(g_GS[port].bits.active == 0)
	{
		return ret;		
	}
	//Set RFPower State 
	else if(0==strcmp(CmdData->cmd, "RF"))      						
	{
		if(CmdData->IsFloat)return ret;
		if(value==0||value== 1)
		{	
			if(value != IF_CmdParam_GetRFPowerState())
			{
				IF_CmdParam_SetRFPowerState(value);
			}	
			ret = E_OK;
		}
	}
	//Set SetPoint 
	if(0==strcmp(CmdData->cmd, "SP"))      						
	{
		if(CmdData->IsFloat)return ret;
		if(value>=0&&value<=MAX_POWER)
		{	
			if(value != IF_CmdParam_GetSetPoint())
			{
				IF_CmdParam_SetSetPoint(value);
			}	
			ret = E_OK;
		}
	}
	//Set Regulation mode 
	if(0==strcmp(CmdData->cmd, "RM"))      						
	{
		if(CmdData->IsFloat)return ret;
		if(value <= MAX_REGUALATION)
		{	
			if(value != IF_UserParam_GetRegulationMode())
			{
				IF_UserParam_SetRegulationMode(value);
			}	
			ret = E_OK;
		}
	}
	//Set Set Forward Power Limit 
	if(0==strcmp(CmdData->cmd, "FPL"))      						
	{
		if(CmdData->IsFloat)return ret;
		if(value>=0&&value<= MAX_POWER)
		{	
			if(value != IF_UserParam_GetForwardPowerLimit())
			{
				IF_UserParam_SetForwardPowerLimit(value);
			}	
			ret = E_OK;
		}
	}
	//Set Set Relfect Power Limit 
	if(0==strcmp(CmdData->cmd, "RPL"))      						
	{
		if(CmdData->IsFloat)return ret;
		if(value >=0 &&value <= MAX_POWER)
		{	
			if(value != IF_UserParam_GetReflectPowerLimit())
			{
				IF_UserParam_SetReflectPowerLimit(value);
			}	
			ret = E_OK;
		}
	}
	//Set Set Relfect Power Threshould 
	if(0==strcmp(CmdData->cmd, "RPT"))      						
	{
		if(CmdData->IsFloat)return ret;
		if(value>=0&&value<=MAX_POWER)
		{	
			if(value != IF_UserParam_GetReflectPowerThreshold())
			{
				IF_UserParam_SetReflectPowerThreshold(value);
			}	
			ret = E_OK;
		}
	}
	//Set Set Relfect Power Delay off 
	if(0==strcmp(CmdData->cmd, "DRPT"))      						
	{
		if(CmdData->IsFloat)return ret;
		if(value>=0&&value<= MAX_REFLECTDELAY)
		{	
			if(value != IF_UserParam_GetReflectPowerDelayOff())
			{
				IF_UserParam_SetReflectPowerDelayOff(value);
			}	
			ret = E_OK;
		}
	}
	//Set Set Pulse Mode 
	if(0==strcmp(CmdData->cmd, "PSM"))      						
	{
		if(CmdData->IsFloat)return ret;
		if(value>=0&&value<=1)
		{	
			if(value != IF_UserParam_GetPulseMode())
			{
				IF_UserParam_SetPulseMode(value);
			}	
			ret = E_OK;
		}
	}
	//Set Set Pulse Frequency 
	if(0==strcmp(CmdData->cmd, "PSF"))      						
	{
		if(CmdData->IsFloat)return ret;
		if(value > 0 &&value <= MAX_PULSEFREQ)
		{	
			if(value != IF_UserParam_GetPulseFrequency())
			{
				IF_UserParam_SetPulseFrequency(value);				
			}	
			ret = E_OK;
		}
	}
	//Set Set Pulse Duty Circle 
	if(0==strcmp(CmdData->cmd, "PDC"))      						
	{
		if(CmdData->IsFloat)return ret;
		if(value>0&&value < MAX_PULSEDUTY&&value>IF_InternalParam_GetFpgaPulseSyncDelay())
		{	
			if(value != IF_UserParam_GetPulseDutyCircle())
			{
				IF_UserParam_SetPulseDutyCircle(value);
			}	
			ret = E_OK;
		}
	}
	//Set Set VDC factor
	if(0==strcmp(CmdData->cmd, "VDCF"))      						
	{
		if(CmdData->IsFloat)return ret;
		if(value >= 0&&value <= MAX_VDCATTEN)
		{	
			if(value != IF_UserParam_GetVDCFactor())
			{
				IF_UserParam_SetVDCFactor(value);
			}	
			ret = E_OK;
		}
	}
	//Set Match Mode 
	if(0==strcmp(CmdData->cmd, "MOD"))      						
	{
		if(CmdData->IsFloat)return ret;
		if(value==0||value== 1)
		{	
			if(value != IF_UserParam_GetMatchMode())
			{
				IF_UserParam_SetMatchMode(value);
			}	
			ret = E_OK;
		}
	}
	//Set Uart Baudrate     
	else if(0==strcmp(CmdData->cmd, "BAUD"))  	  		
	{				
		if(CmdData->IsFloat)return ret;				
		uint8_t bardrateKey = IF_Uart_GetBaudRateKey((UARTBAUD_TypeDef)value);
		if(bardrateKey <= 4)
		{
			if(bardrateKey != IF_CommParam_GetDataRate())
			{
				IF_CommParam_SetDataRate(bardrateKey);
			}
			ret = E_OK;
		}				
	}											
	return ret;
}
/* FUNCTION *********************************************************************************
 * Function Name : Send_CSIPv1_Frame
 * Description   : 发送字节处理
 * Parameter     : port: 端口号
 * Parameter     : state 接受数据状态: txBuf需要发送的字节
 * Return        :
 * END ***************************************************************************************/
static void CSIPv1_SendFrame(uint8_t port,uint8_t state,char *txBuf)
{
	if(state != E_OK)   			 //cmd error
	{
		sprintf(txBuf,"%c\r",'N');  //如果接受错误
	}else
	{
		strcat(txBuf,"\r");			//如果接受正确，添加字符<CR>
	}
	//清除发送缓冲区
	memset((void*)&g_TxBuf[port],0,sizeof(CommMsg_t));
	
	g_TxBuf[port].port = port;
	for(uint8_t i=0;i< strlen(txBuf);i++)
	{
		g_TxBuf[port].data[g_TxBuf[port].len++] =txBuf[i];
	}
	
	//中断发送
	IF_UartEnableTx(g_TxBuf[port].port);						//发送字符串
}
/* FUNCTION *********************************************************************************
 * Function Name : ReceiveChar
 * Description   : 接收字节处理，一般在中断中调用
 * Parameter     : port: 端口号
 * Parameter     : ch: 需要处理的字节
 * Return        :
 * END ***************************************************************************************/
void Dealwith_CSIPv1_Layer2(uint8_t port,CommMsg_t *rxMsg)
{
	 uint8_t ret = E_NOT_OK;
	 char txBuf[DATA_SIZE]={0};
	 ret =  CSIPv1_ProtecolParse(rxMsg,txBuf);
	 CSIPv1_SendFrame(port,ret,txBuf);	 
}
/* FUNCTION *********************************************************************************
 * Function Name : ReceiveChar
 * Description   : 接收字节处理，一般在中断中调用
 * Parameter     : port: 端口号
 * Parameter     : ch: 需要处理的字节
 * Return        :
 * END ***************************************************************************************/
void CSIPv1_ReceiveChar(uint8_t port, uint8_t ch)
{
	current_rx_time[port] = Now();

	if ((current_rx_time[port] - last_rx_time[port]) > 10)  //Frame timeout
	{
		g_RxBuf[port].flag_status = 0;
	}

	last_rx_time[port] = current_rx_time[port];

	switch(g_RxBuf[port].flag_status)
	{
	case 0:
		memset(g_RxBuf[port].data, 0, BUFFER_SIZE);
		g_RxBuf[port].port = port;
		g_RxBuf[port].index = 0; 
		g_RxBuf[port].len = 0;                            // 初始化数据长度
		g_RxBuf[port].data[g_RxBuf[port].len++] = ch;     // 把数据放入存储区
		g_RxBuf[port].flag_status = 1;                    // 置标志
		break;
	case 1:
		if (ch == 0x0D)                                      // 收到结尾的0x0D
		{
			g_RxBuf[port].data[g_RxBuf[port].len++] = ch;    // 把数据0x0D放入存储区，同时长度加1
			SendToRxQueueFromISR(port, &g_RxBuf[port]);
			g_RxBuf[port].flag_status = 0;                    // 置标志
		}else                                  
		{                                                    	
			g_RxBuf[port].data[g_RxBuf[port].len++] = ch;    // 把数据放入存储区，同时长度加1
		}

		if (g_RxBuf[port].len >= BUFFER_SIZE)                // 保证数据不过界
		{
			g_RxBuf[port].len--;                             // 保证数据不过界
		}
		break;
	default:
		break;
	}
}




/************************ (C) COPYRIGHT FINGU *****END OF FILE***********************/
