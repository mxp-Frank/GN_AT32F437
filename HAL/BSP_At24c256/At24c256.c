/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  At24c256.c
 * @Brief :      ref from bsp_eeprom.c(AMP/AME svn324) Author: Huxian
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#include "IF_At24c256.h"
#include "At24c256.h"
#include "IF_Tmr.h"

/* CONST & MACROS */
#define  SDA_IN() 		SDA_PortInit(SDA_INPUT)
#define  SDA_OUT() 		SDA_PortInit(SDA_OUTPUT)
/* DATA STRUCTURES */

/* LOCAL VARIABLES */

/* GLOBAL VARIABLES */

/* FUNCTION PROTOTYPES */
static void At24c256_PortInit(void);
static void SCL_PortInit(void);
static void SDA_PortInit(uint8_t para);
/***************i2c Interface*************************/
static void I2C_Start(void);
static void I2C_Stop(void);
static void I2C_Ack(void);
static void I2C_NAck(void);
static uint8_t I2C_WaitAck(void);
static void I2C_SendByte(uint8_t byte);
static uint8_t I2C_ReadByte(void);
static uint8_t I2C_CheckDevice(uint8_t address);
/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/

/**
 * ************************************************************************
 * @Brief :    IF_At24c256Init
 * ************************************************************************
 */
void IF_At24c256Init(void)
{
    At24c256_PortInit();
}
/* FUNCTION **********************************************************************
* Function Name : IF_At24c256_CheckOk
* Description   : 判断串行EERPOM是否正常
* Parameter	 :
* return        : 1 表示正常， 0 表示不正常
* END ************************************************************************/
uint8_t IF_At24c256_CheckOk(void)
{
    uint8_t status;
    if(I2C_CheckDevice(EEPROM_DEV_ADDR) == 0)
    {
        status= 1;
    }
    else
    {
        I2C_Stop(); /* 失败后，切记发送I2C总线停止信号 */
        status= 0;
    }

    return status;
}
/* FUNCTION **********************************************************************
* Function Name : IF_At24c256_ReadBytes
* Description   : EEPROM驱动读取数据
* Parameter	 : address:读地址
* Parameter	 : readBuf：读数据
* Parameter	 : size: 读数据的长度
* return        : 0 表示失败，1表示成功
* END ************************************************************************/
uint8_t IF_At24c256_ReadBytes(uint16_t address,uint8_t *readBuf, uint16_t size)
{
    uint16_t i;
    /* 第1步：发起I2C总线启动信号 */
    I2C_Start();

    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    I2C_SendByte(EEPROM_DEV_ADDR | I2C_WR);	/* 此处是写指令 */

    /* 第3步：等待ACK */
    if (I2C_WaitAck() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }

    /* 第4步：发送2个字节地址 */
    I2C_SendByte((uint8_t)((address >> 8) & 0x7F));
    /* 第5步：等待ACK */
    if (I2C_WaitAck() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }
    I2C_SendByte((uint8_t)(address & 0xFF));
    if (I2C_WaitAck() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }

    /* 第6步：重新启动I2C总线。前面的代码的目的向EEPROM传送地址，下面开始读取数据 */
    I2C_Start();

    /* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    I2C_SendByte(EEPROM_DEV_ADDR | I2C_RD);	/* 此处是读指令 */

    /* 第8步：发送ACK */
    if (I2C_WaitAck() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }

    /* 第9步：循环读取数据 */
    for (i = 0; i < size; i++)
    {
        readBuf[i] = I2C_ReadByte();	/* 读1个字节 */

        /* 每读完1个字节后，需要发送Ack， 最后一个字节不需要Ack，发Nack */
        if (i != size - 1)
        {
            I2C_Ack();	/* 中间字节读完后，CPU产生ACK信号(驱动SDA = 0) */
        }
        else
        {
            I2C_NAck();	/* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
        }
    }
    /* 发送I2C总线停止信号 */
    I2C_Stop();
    return 1;	/* 执行成功 */
	

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
    /* 发送I2C总线停止信号 */
    I2C_Stop();
    return 0;
}

/* FUNCTION **********************************************************************
* Function Name : IF_At24c256_WriteBytes
* Description   : EEPROM驱动写数据
* Parameter	 : address:写地址
* Parameter	 : writeBuf：写数据
* Parameter	 : size: 写数据的长度
* return        : 0 表示失败，1表示成功
* END ************************************************************************/
uint8_t IF_At24c256_WriteBytes(uint16_t address, uint8_t *writeBuf, uint16_t size)
{
    uint16_t  m,usAddr;
    usAddr = address;
    for (uint16_t i = 0; i < size; i++)
    {
        /* 当发送第1个字节或是页面首地址时，需要重新发起启动信号和地址 */
        if ((i == 0) || (usAddr & (EEPROM_PAGE_SIZE - 1)) == 0)
        {
            /*　第０步：发停止信号，启动内部写操作　*/
            I2C_Stop();

            /* 通过检查器件应答的方式，判断内部写操作是否完成, 一般小于 10ms
                CLK频率为200KHz时，查询次数为30次左右
            */
            for (m = 0; m < 1000; m++)
            {
                /* 第1步：发起I2C总线启动信号 */
                I2C_Start();

                /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
                I2C_SendByte(EEPROM_DEV_ADDR | I2C_WR);	/* 此处是写指令 */

                /* 第3步：发送一个时钟，判断器件是否正确应答 */
                if (I2C_WaitAck() == 0)
                {
                    break;
                }
            }
            if (m  == 1000)
            {
                goto cmd_fail;	/* EEPROM器件写超时 */
            }

            /* 第4步：发送字节地址 */
            I2C_SendByte((uint8_t)((usAddr >> 8) & 0x7F));

            /* 第5步：等待ACK */
            if (I2C_WaitAck() != 0)
            {
                goto cmd_fail;	/* EEPROM器件无应答 */
            }

            /* 第6步：发送字节地址 */
            I2C_SendByte((uint8_t)(usAddr & 0xFF));
            /* 第7步：等待ACK */
            if (I2C_WaitAck() != 0)
            {
                goto cmd_fail;	/* EEPROM器件无应答 */
            }
        }

        /* 第8步：开始写入数据 */
        I2C_SendByte(writeBuf[i]);

        /* 第9步：发送ACK */
        if (I2C_WaitAck() != 0)
        {
            goto cmd_fail;	/* EEPROM器件无应答 */
        }

        usAddr++;	/* 地址增1 */
    }

    /* 命令执行成功，发送I2C总线停止信号 */
    I2C_Stop();
	delay_ms(5);  //等待写完成
    return 1;

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
    /* 发送I2C总线停止信号 */
    I2C_Stop();
    return 0;
}



/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/

static void At24c256_PortInit(void)
{
    SCL_PortInit();	
    SDA_PortInit(SDA_OUTPUT);
}

static void SCL_PortInit(void)
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
	gpio_init_struct.gpio_pins = IIC2_SCL_PIN;
	gpio_init(IIC2_SCL_GPIO, &gpio_init_struct);
}

static void SDA_PortInit(uint8_t para)
{
    gpio_init_type gpio_init_struct;

    /* set default parameter */
    gpio_default_para_init(&gpio_init_struct);

    if(para == SDA_INPUT)
    {
        /* configure the normal MCU INPUT gpio */
        gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE; 

  
    }
    else if(para == SDA_OUTPUT)
    {
        /* configure the normal MCU OUTPUT gpio */
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	}
	//SDA output
	gpio_init_struct.gpio_pins = 0;
	gpio_init_struct.gpio_pins = IIC2_SDA_PIN;
	gpio_init(IIC2_SDA_GPIO, &gpio_init_struct);

}

/* FUNCTION **********************************************************************
* Function Name : I2C_Start
* Description   : CPU发起I2C总线启动信号
* Parameter	 :
* return        :
* END ************************************************************************/
static void I2C_Start(void)
{
    /* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
    SDA_OUT();
    SDA_HIGH();
    SCL_HIGH();
    delay_us(4);
    SDA_LOW();
    delay_us(4);
    SCL_LOW();
    delay_us(4);
}

/* FUNCTION **********************************************************************
* Function Name : I2C_Stop
* Description   : CPU发起I2C总线停止信号
* Parameter	 :
* return        :
* END ************************************************************************/
static void I2C_Stop(void)
{
    /* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
    SDA_OUT();
    SDA_LOW();
    SCL_HIGH();
    delay_us(4);
    SDA_HIGH();
}

/* FUNCTION **********************************************************************
* Function Name : I2C_Ack
* Description   : CPU产生一个ACK信号
* Parameter	 :
* return        :
* END ************************************************************************/
static void I2C_Ack(void)
{
    SDA_OUT();   //设置SDA为输出
    SDA_LOW();   /* CPU驱动SDA = 0 */
    delay_us(4);
    SCL_HIGH();	 /* CPU驱动SCL = 0 */
    delay_us(4);
    SCL_LOW();	 /* CPU产生1个时钟 */
    delay_us(4);
    SDA_HIGH();  //CPU释放SDA总线
}

/* FUNCTION **********************************************************************
* Function Name : I2C_NAck
* Description   : CPU产生1个NACK信号
* Parameter	 :
* return        :
* END ************************************************************************/
static void I2C_NAck(void)
{
    SDA_OUT();    //设置SDA为输出
    SDA_HIGH();   /* CPU驱动SDA = 1 */
    delay_us(4);
    SCL_HIGH();	  /* CPU产生1个时钟 */
    delay_us(4);
    SCL_LOW();
    delay_us(4);
}

/* FUNCTION **********************************************************************
* Function Name : I2C_WaitAck
* Description   : CPU产生一个时钟，并读取器件的ACK应答信号
* Parameter	 :
* return        : 0表示正确应答，1表示无器件响应
* END ************************************************************************/
static uint8_t I2C_WaitAck(void)
{
    uint8_t re=0;

    SDA_OUT();      /*SDA设置成输出*/
    SDA_HIGH();  	/*CPU释放SDA总线 */
    delay_us(4);    /*延时函数*/
    SDA_IN();       /*SDA设置成输入*/
    SCL_HIGH();	    /* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
    delay_us(4);
    re = ((SDA_READ() == 1) ? 1:0);  /* CPU读取SDA口线状态 */
    SCL_LOW();
    delay_us(4);

    return re;
}

/* FUNCTION **********************************************************************
* Function Name :void I2C_SendByte(uint8_t Byte)
* Description   : CPU从I2C总线设备读取8bit数据
* Parameter	 :
* return        :
* END ************************************************************************/
static void I2C_SendByte(uint8_t byte)
{
    SDA_OUT();
    /* 先发送字节的高位bit7 */
    for (uint8_t i = 0; i < 8; i++)
    {
        if (byte & 0x80)
        {
            SDA_HIGH();
        }
        else
        {
            SDA_LOW();
        }
        delay_us(4);
        SCL_HIGH();
        delay_us(4);
        SCL_LOW();
        if (i == 7)
        {
            SDA_HIGH(); // 释放总线
        }
        byte <<= 1;	      /* 左移一个bit */
        delay_us(4);
    }
}

/* FUNCTION **********************************************************************
* Function Name :uint8_t I2C_ReadByte(void)
* Description   : CPU从I2C总线设备读取8bit数据
* Parameter	 :
* return        : 读到的数据
* END ************************************************************************/
static uint8_t I2C_ReadByte(void)
{
    uint8_t value = 0;
    SDA_IN();   //SDA设置为输入

    /* 读到第1个bit为数据的bit7 */
    for (uint8_t i = 0; i < 8; i++)
    {
        value <<= 1;
        SCL_HIGH();
        delay_us(4);
        if (SDA_READ())
        {
			value++;
        }
        SCL_LOW();
        delay_us(4);
    }
    return value;
}

/* FUNCTION **********************************************************************
* Function Name : I2C_CheckDevice
* Description   : 检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
* Parameter	 : address：设备的I2C总线地址
* return        : 返回值 0 表示正确， 返回1表示未探测到
* END ************************************************************************/
static uint8_t I2C_CheckDevice(uint8_t address)
{
    uint8_t ucAck = 0;

    SCL_PortInit();
    SDA_PortInit(SDA_OUTPUT);

    I2C_Stop();         /* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
    I2C_Start();		/* 发送启动信号 */

    /* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
    I2C_SendByte(address | I2C_WR);

    ucAck = I2C_WaitAck();	/* 检测设备的ACK应答 */
    I2C_Stop();			    /* 发送停止信号 */

    return ucAck;
}

//*****************************************************************************
//* END
//*****************************************************************************

