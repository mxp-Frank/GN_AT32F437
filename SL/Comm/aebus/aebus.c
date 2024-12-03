/********************************************************************************
  * @doc     		： AEBus.c
  * @author  		： xingping.meng
  * @fw       		： V1.0.0
  * @date       	： 2022年8月15日
  * @information 	：用户协议驱动程序源文件
  ******************************************************************************/
#include "stdint.h"
#include "stdio.h"
#include "math.h"
#include "fgipv2.h"
#include "stm32_crc32.h"
#include "if_comm.h"
#include "AEBus.h"
#include "IF_AEBus.h"
#include "Common.h"

//中断接收使用的变量
static AECommData_t rxAEBus;
static uint32_t last_time = 0;
static uint32_t current_time = 0;

static void AEBus_SendAck(uint8_t port);
/* FUNCTION *********************************************************************************
 * Function Name : ReceiveChar
 * Description   : 接收字节处理，一般在中断中调用
 * Parameter     : port: 端口号
 * Parameter     : ch: 需要处理的字节
 * Return        :
 * END ***************************************************************************************/
void Dealwith_CSIPv1_Layer2(CommMsg_t *rxMsg)
{

}
/* FUNCTION *********************************************************************************
 * Function Name : ReceiveChar
 * Description   : 接收字节处理，一般在中断中调用
 * Parameter     : port: 端口号
 * Parameter     : ch: 需要处理的字节
 * Return        :
 * END ***************************************************************************************/
void IF_AEBusReceiveChar(uint8_t port, uint8_t ch)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	
	current_time = Now();
	if ((current_time - last_time) > 10)  //Frame timeout
	{
		rxAEBus.FrameFlag = 0;
	}

	last_time = current_time;
	switch(rxAEBus.FrameFlag)
	{
	case 0:	
		memset(&rxAEBus, 0, sizeof(AECommData_t)); //清除接收数据帧	          
		rxAEBus.Header = ch;    				   //把数据放入指令帧头
	    //如果地址是设备地址或者广播地址，接收数据帧，否则数据帧无效
		if(rxAEBus.Address ==IF_CommParam_GetDeviceAddress()||rxAEBus.Address == 0)
		{	
			rxAEBus.FrameFlag = 1;                 // 置标志1,接收到帧头消息
		}else
		{
			rxAEBus.FrameFlag = 0;                 // 置标志0,接收消息错误
		}
		break;
	
	case 1:
		rxAEBus.CmdNumber = ch;    				  // 把数据放入指令号
		rxAEBus.FrameFlag = 2;                    // 置标志,接收到指令号
		break;
	
	case 2:
		rxAEBus.OptLength = ch;    				  // 把接收字符放入数据控制段	
		rxAEBus.FrameFlag = 3;                    // 置标志3,接收完成,跳转到数据段	
		break;
	
	case 3:
		rxAEBus.Data[rxAEBus.Index++] = ch;    	 	 // 把接收字符放入放入数据段
		//如果帧头定义数据长度小于7，则以帧头的长度为接收数据长度，否则就以数据控制段为数据长度
		if(rxAEBus.Length < 7)					 	
		{
			if(rxAEBus.Index >= rxAEBus.Length)rxAEBus.FrameFlag = 4;     // 置标志4,接收到数据段完成
			
		}else
		{
			if(rxAEBus.Index >= rxAEBus.OptLength)rxAEBus.FrameFlag = 4;  // 置标志4,接收到数据段完成
		}
		break;
	case 4:
		rxAEBus.CheckSum = ch;    	 	 					  // 把接收字符放入CheckSum
		if(AEBUSQueue != NULL)
		{	
			if(rxAEBus.Address ==IF_CommParam_GetDeviceAddress())  //接收地址是设备地址
			{
				AEBus_SendAck(port);     						  //发送应答0x07
			}
			xQueueSendFromISR(AEBUSQueue, &rxAEBus, &xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);	
			memset(&rxAEBus, 0, sizeof(AECommData_t)); //清除接收数据帧	     			
		}
		break;
	default:
		break;
	}
}

static void AEBus_SendAck(uint8_t port)
{
	IF_UartSendByte(port,0x06);
}


/************************ (C) COPYRIGHT FINGU *****END OF FILE***********************/
