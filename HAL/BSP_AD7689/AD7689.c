/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Ltc1865.c
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2023-05-15
 * ****************************************************************************
 */

#include "IF_AD7689.h"
#include "AD7689.h"
#include "IF_Tmr.h"
/* CONST & MACROS */

/* DATA STRUCTURES */

/* LOCAL VARIABLES */

/* GLOBAL VARIABLES */

/* FUNCTION PROTOTYPES */

static void AD7689_PortInit(void);
static uint16_t SPI_AD7689_ReadADC(uint8_t channel);
/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/
/*** ************************************************************************
 * @Brief :    IF_A3979Init  
 **************************************************************************/
void IF_AD7689Init(void)
{ 
    /*control port init*/
    AD7689_PortInit();
}
/*** ************************************************************************
 * @Brief :    IF_SPI_ReadADCValue  
 **************************************************************************/
uint16_t IF_SPI_ReadADCValue(uint8_t channel)
{
	uint16_t value = 0;
	value = SPI_AD7689_ReadADC(channel);
	return value;
}
/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/
/*** ************************************************************************
 * @Brief :    AD7689_PortInit  
 **************************************************************************/
static void AD7689_PortInit(void)
{
     gpio_init_type gpio_init_struct;

    /* set default parameter */
    gpio_default_para_init(&gpio_init_struct);

    /* configure the normal MCU OUTPUT gpio */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;

    //DAC pin configuartion
    gpio_init_struct.gpio_pins = 0;
    gpio_init_struct.gpio_pins = AD7689_CNV_ADC_PIN; 

    gpio_init(AD7689_CNV_ADC_GPIO, &gpio_init_struct);
	//DAC pin configuartion
    gpio_init_struct.gpio_pins = 0;
    gpio_init_struct.gpio_pins = AD7689_SCK_ADC_PIN; 
    gpio_init(AD7689_SCK_ADC_GPIO, &gpio_init_struct);
	//DAC pin configuartion
    gpio_init_struct.gpio_pins = 0;
    gpio_init_struct.gpio_pins = AD7689_DIN_ADC_PIN; 
    gpio_init(AD7689_DIN_ADC_GPIO, &gpio_init_struct);
	/*Configure the normal MCU INPUT gpio */
    gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;

	gpio_init_struct.gpio_pins = 0;
    gpio_init_struct.gpio_pins = AD7689_SDO_ADC_PIN; 
	gpio_init(AD7689_SDO_ADC_GPIO, &gpio_init_struct);
}
/*** ************************************************************************
 * @Brief :    SPI_AD7689_ReadADC  
 **************************************************************************/
static uint16_t SPI_AD7689_ReadADC(uint8_t AD7689_INx)
{
	uint8_t i = 0;
	uint16_t AD7689_Config = 0;
	uint16_t AD7689_Data;
	AD7689_Config = (AD7689_CFG<<13)|(AD7689_INCC<<10)|(AD7689_INx<<7)|(AD7689_BW<<6)|(AD7689_REF<<3)|(AD7689_SEQ<<1)|(AD7689_RB<<0);
	AD7689_Config <<= 2;  			//左移2位末尾补0
	
	AD7689_CNV_HIGH;   			 	//CNV =1
	delay_us(10);
	AD7689_CNV_LOW;   			 	//CNV =0  
	delay_us(10);
	AD7689_CNV_HIGH;   			 	//CNV =1
	delay_us(10);
	AD7689_CNV_LOW;   			 	//CNV =0  
	delay_us(10);
	for(i=0;i<16;i++)
	{
		if(AD7689_Config&0x8000)  	//
		{
			AD7689_DIN_HIGH;     	//DIN =1  
		}else
		{
			AD7689_DIN_LOW;		 	//DIN =0 
		}
		delay_xns(50);
		AD7689_SCK_LOW;             //在SCK下降沿将ODD/SIGN发送
		delay_xns(50);
		AD7689_Config <<= 1;        //获得config值
		delay_xns(50);
		AD7689_SCK_HIGH;            //在SCK上升沿可以获取值
		delay_xns(50);
		AD7689_Data <<= 1;
		delay_xns(50);
		if(AD7689_SDO_VALUE)
		{
			AD7689_Data|= 1;
		}
		delay_xns(50);
	}
	delay_us(5);
	AD7689_SCK_LOW;			 	//SCK=0
	delay_us(5);
	AD7689_CNV_HIGH;			//CNV=1
	return (AD7689_Data&0xFFFF);
}

//*****************************************************************************
//* END
//*****************************************************************************

