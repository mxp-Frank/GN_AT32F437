/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  DS1339.c
 * @Brief :      ref from bsp_eeprom.c(AMP/AME svn324) Author: Huxian
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */
#include "IF_DS1339.h"
#include "DS1339.h"
#include "IF_Tmr.h"

/* CONST & MACROS */
#define  IIC3_SDA_IN() 		IIC_SDA_PortInit(IIC3_SDA_INPUT)
#define  IIC3_SDA_OUT() 	IIC_SDA_PortInit(IIC3_SDA_OUTPUT)
/* DATA STRUCTURES */

/* LOCAL VARIABLES */

/* GLOBAL VARIABLES */

/* FUNCTION PROTOTYPES */
/**************Port Interface******************/
static void DS1339_PortInit(void);
static void IIC_SCL_PortInit(void);
static void IIC_SDA_PortInit(uint8_t para);
/**************IIC Interface******************/
static void IIC3_Start(void);
static void IIC3_Stop(void);
static void IIC3_Ack(void);
static void IIC3_NAck(void);
static uint8_t IIC3_WaitAck(void);
static void IIC3_SendByte(uint8_t byte);
static uint8_t IIC3_ReadByte(void);
static uint8_t IIC3_CheckDevice(uint8_t address);
/**************DS1339 Interface******************/
static uint8_t ConvertDataToSet(struct tm *time,uint8_t regType);
static void ConvertDataToGet(uint8_t rData,struct tm *time,uint8_t regType);
static uint8_t DS1339_SetTimer(struct tm *timer);
static uint8_t DS1339_GetTime(struct tm *time);


/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/

/**
 * ************************************************************************
 * @Brief :    IF_DS1339Init
 * ************************************************************************
 */
void IF_DS1339Init(void)
{
    DS1339_PortInit();
	uint8_t status = IIC3_CheckDevice(DS1339_ADDR);
	if(status)IIC3_Stop();
}
void IF_DS1339_SetTimer(time_t timer)
{
   struct tm *localtimer;
   localtimer = localtime(&timer);
   localtimer->tm_hour+=8;
   localtimer->tm_mon+=1;
   localtimer->tm_year += 1900;
   DS1339_SetTimer(localtimer);
}
void IF_DS1339_GetTimer(struct tm *timer)
{
    DS1339_GetTime(timer);
}


/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/

static void DS1339_PortInit(void)
{
    IIC_SCL_PortInit();	
    IIC_SDA_PortInit(IIC3_SDA_OUTPUT);
}

static void IIC_SCL_PortInit(void)
{
    gpio_init_type gpio_init_struct;

    /* set default parameter */
    gpio_default_para_init(&gpio_init_struct);
   
	/* configure the normal MCU OUTPUT gpio */
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
	gpio_init_struct.gpio_pull = GPIO_PULL_NONE;	
	//SCL 
	gpio_init_struct.gpio_pins = 0;
	gpio_init_struct.gpio_pins = IIC3_SCL_PIN;
	gpio_init(IIC3_SCL_GPIO, &gpio_init_struct);
}

static void IIC_SDA_PortInit(uint8_t para)
{
    gpio_init_type gpio_init_struct;

    /* set default parameter */
    gpio_default_para_init(&gpio_init_struct);

    if(para == IIC3_SDA_INPUT)
    {
        /* configure the normal MCU INPUT gpio */
        gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;   
    }
    else if(para == IIC3_SDA_OUTPUT)
    {
        /* configure the normal MCU OUTPUT gpio */
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	}
	
	gpio_init_struct.gpio_pins = 0;
	gpio_init_struct.gpio_pins = IIC3_SDA_PIN;
	gpio_init(IIC3_SDA_GPIO, &gpio_init_struct);

}

/* FUNCTION **********************************************************************
* Function Name : IIC3_Start
* Description   : CPU发起IIC3总线启动信号
* Parameter	 :
* return        :
* END ************************************************************************/
static void IIC3_Start(void)
{
    /* 当SCL高电平时，SDA出现一个下跳沿表示IIC3总线启动信号 */
    IIC3_SDA_OUT();
    IIC3_SDA_HIGH();
    IIC3_SCL_HIGH();
    delay_us(5);
    IIC3_SDA_LOW();
    delay_us(5);
    IIC3_SCL_LOW();
    delay_us(5);
}

/* FUNCTION **********************************************************************
* Function Name : IIC3_Stop
* Description   : CPU发起IIC3总线停止信号
* Parameter	 :
* return        :
* END ************************************************************************/
static void IIC3_Stop(void)
{
    /* 当SCL高电平时，SDA出现一个上跳沿表示IIC3总线停止信号 */
    IIC3_SDA_OUT();
    IIC3_SDA_LOW();
    IIC3_SCL_HIGH();
    delay_us(5);
    IIC3_SDA_HIGH();
}

/* FUNCTION **********************************************************************
* Function Name : IIC3_Ack
* Description   : CPU产生一个ACK信号
* Parameter	 :
* return        :
* END ************************************************************************/
static void IIC3_Ack(void)
{
    IIC3_SDA_OUT();   //设置SDA为输出
    IIC3_SDA_LOW();   /* CPU驱动SDA = 0 */
    delay_us(5);
    IIC3_SCL_HIGH();  /* CPU驱动SCL = 0 */
    delay_us(5);
    IIC3_SCL_LOW();	 /* CPU产生1个时钟 */
    delay_us(5);
    IIC3_SDA_HIGH();  //CPU释放SDA总线
}

/* FUNCTION **********************************************************************
* Function Name : IIC3_NAck
* Description   : CPU产生1个NACK信号
* Parameter	 :
* return        :
* END ************************************************************************/
static void IIC3_NAck(void)
{
    IIC3_SDA_OUT();    //设置SDA为输出
    IIC3_SDA_HIGH();   /* CPU驱动SDA = 1 */
    delay_us(5);
    IIC3_SCL_HIGH();	  /* CPU产生1个时钟 */
    delay_us(5);
    IIC3_SCL_LOW();
    delay_us(5);
}

/* FUNCTION **********************************************************************
* Function Name : IIC3_WaitAck
* Description   : CPU产生一个时钟，并读取器件的ACK应答信号
* Parameter	 :
* return        : 0表示正确应答，1表示无器件响应
* END ************************************************************************/
static uint8_t IIC3_WaitAck(void)
{
    uint8_t re=0;

    IIC3_SDA_OUT();     	/*SDA设置成输出*/
    IIC3_SDA_HIGH();  			/*CPU释放SDA总线 */
    delay_us(5);    		/*延时函数*/
    IIC3_SDA_IN();       	/*SDA设置成输入*/
    IIC3_SCL_HIGH();	    		/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
    delay_us(5);
    re = ((IIC3_SDA_READ() == 1) ? 1:0);  /* CPU读取SDA口线状态 */
    IIC3_SCL_LOW();
    delay_us(5);

    return re;
}

/* FUNCTION **********************************************************************
* Function Name :void IIC3_SendByte(uint8_t Byte)
* Description   : CPU从IIC3总线设备读取8bit数据
* Parameter	 :
* return        :
* END ************************************************************************/
static void IIC3_SendByte(uint8_t byte)
{
    IIC3_SDA_OUT();
    /* 先发送字节的高位bit7 */
    for ( uint8_t i = 0; i < 8; i++)
    {
        if (byte & 0x80)
        {
            IIC3_SDA_HIGH();
        }else
        {
            IIC3_SDA_LOW();
        }
        delay_us(5);
        IIC3_SCL_HIGH();
        delay_us(5);
        IIC3_SCL_LOW();
        if (i == 7)
        {
            IIC3_SDA_HIGH(); // 释放总线
        }
        byte <<= 1;	      /* 左移一个bit */
        delay_us(5);
    }
}

/* FUNCTION **********************************************************************
* Function Name :uint8_t IIC3_ReadByte(void)
* Description   : CPU从IIC3总线设备读取8bit数据
* Parameter	 :
* return        : 读到的数据
* END ************************************************************************/
static uint8_t IIC3_ReadByte(void)
{

    uint8_t value =0;

    IIC3_SDA_IN();   //SDA设置为输入

    /* 读到第1个bit为数据的bit7 */
    for (uint8_t i = 0; i < 8; i++)
    {
        value <<= 1;
        IIC3_SCL_HIGH();
        delay_us(5);
        if (IIC3_SDA_READ())
        {
			value++;
        }
        IIC3_SCL_LOW();
        delay_us(5);
    }

    return value;
}

/* FUNCTION **********************************************************************
* Function Name : IIC3_CheckDevice
* Description   : 检测IIC3总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
* Parameter	 : address：设备的IIC3总线地址
* return        : 返回值 0 表示正确， 返回1表示未探测到
* END ************************************************************************/
static uint8_t IIC3_CheckDevice(uint8_t address)
{
    uint8_t ucAck;

    IIC_SCL_PortInit();
    IIC_SDA_PortInit(IIC3_SDA_OUTPUT);
	
    IIC3_Stop();         /* 给一个停止信号, 复位IIC3总线上的所有设备到待机模式 */
    IIC3_Start();		/* 发送启动信号 */

    /* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
    IIC3_SendByte(address | IIC3_WR);

    ucAck = IIC3_WaitAck();	/* 检测设备的ACK应答 */
    IIC3_Stop();			/* 发送停止信号 */

    return ucAck;
}


static uint8_t DS1339_SetTimer(struct tm *timer)
{
	uint8_t status, sendData;
	IIC3_Start();
	IIC3_SendByte(DS1339_ADDR);
	status = IIC3_WaitAck();
	IIC3_SendByte(REGADDR_SECONDS);
	status |= IIC3_WaitAck();
	for(uint8_t i=0;i<7;i++)
	{
		sendData = ConvertDataToSet(timer,REGADDR_SECONDS+i);
		IIC3_SendByte(sendData);
		status |= IIC3_WaitAck();
	}
	IIC3_Stop();
	if(status)
	{
		return 1;
	}else
	{	
		return  0;
	}
}
static uint8_t DS1339_GetTime(struct tm *time)
{
	uint8_t status,rData[7];

	IIC3_Start();
	IIC3_SendByte(DS1339_ADDR);
	status = IIC3_WaitAck();
	IIC3_SendByte(REGADDR_SECONDS);
	status |= IIC3_WaitAck();
	
	IIC3_Start();
	IIC3_SendByte(DS1339_ADDR|0x01);
	status |= IIC3_WaitAck();
	for(uint8_t i=0;i<7;i++)
	{
		if(i!=0)IIC3_Ack();
		rData[i]=IIC3_ReadByte();
	}
	IIC3_NAck();
	IIC3_Stop();
	
	for(uint8_t i=0;i<7;i++)
	{
		ConvertDataToGet(rData[i],time,REGADDR_SECONDS+i);
	}
	if(status)
	{
		return 1;
	}else
	{	
		return  0;
	}
}


static uint8_t ConvertDataToSet(struct tm *time,uint8_t regType)
{
	uint8_t sendData = 0;
	switch(regType)
	{
		case REGADDR_SECONDS:
			if(time->tm_sec >= 10)
			{
				sendData=((time->tm_sec/10)<<4)&0x7F;
				sendData|=(time->tm_sec)%10;
			}else
			{
				sendData=time->tm_sec;
			}
		break;	
		case REGADDR_MINUTES:
			if(time->tm_min >= 10)
			{
				sendData=((time->tm_min/10)<<4)&0x7F;
				sendData|=(time->tm_min)%10;
			}else
			{
				sendData=time->tm_min;
			}
		break;	
		case REGADDR_HOURS:
			if(time->tm_hour >= 10)
			{
				sendData=((time->tm_hour/10)<<4)&0x1F;
				sendData|=(time->tm_hour)%10;
			}else
			{
				sendData=time->tm_hour;
			}
		break;	
		case REGADDR_DAY:
			sendData=(time->tm_wday) & 0X0E;
		break;	
		case REGADDR_DATA:
			if(time->tm_mday >= 10)
			{
				sendData=((time->tm_mday/10)<<4)&0x3F;
				sendData|=(time->tm_mday)%10;
			}else
			{
				sendData=time->tm_mday;
			}
		break;	
		case REGADDR_MONTH_CENTURY:
			if(time->tm_mon >= 10)
			{
				sendData=((time->tm_mon/10)<<4)&0x1F;
				sendData|=(time->tm_mon)%10;
			}else
			{
				sendData=time->tm_mon;
			}
		break;	
		case REGADDR_YEAR:
			if(time->tm_year%100 >= 10)
			{
				sendData=(((time->tm_year%100)/10)<<4)&0xF0;
				sendData|= (time->tm_year)%10;
			}else
			{
				sendData=time->tm_year%100;
			}
		break;	
		default :
		break;			
	}
	return sendData;
}
static void ConvertDataToGet(uint8_t rData,struct tm *time,uint8_t regType)
{
	switch(regType)
	{
		case REGADDR_SECONDS:
			time->tm_sec=(rData>>4 & 0x07)*10+(rData & 0x0F);
		break;	
		case REGADDR_MINUTES:
			time->tm_min=(rData>>4 & 0x07)*10+(rData & 0x0F);
		break;	
		case REGADDR_HOURS:
			time->tm_hour=(rData>>4 & 0x01)*10+(rData & 0x0F);
		break;	
		case REGADDR_DAY:
			time->tm_wday =rData & 0x07;
		break;	
		case REGADDR_DATA:
			time->tm_mday=(rData>>4 & 0x03)*10+(rData & 0x0F);
		break;	
		case REGADDR_MONTH_CENTURY:
			time->tm_mon=(rData>>4 & 0x01)*10+(rData & 0x0F);
		break;	
		case REGADDR_YEAR:
			time->tm_year=2000+(rData>>4 & 0x0F)*10+(rData & 0x0F);
		break;	
		default :
		break;			
	}
}	
//*****************************************************************************
//* END
//*****************************************************************************

