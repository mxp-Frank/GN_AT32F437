/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Tpc112S4.c
 * @Brief :      ref from bsp_dac_tlv5614.c(AMP/AME svn324) Author: Huxian
 * @Author :     Chenghanbing (iwanthaha@sina.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#include "IF_TPC112S4.h"
#include "Tpc112S4.h"
#include "configDevice.h"

/* CONST & MACROS */

/* DATA STRUCTURES */

/* LOCAL VARIABLES */

/* GLOBAL VARIABLES */

/* FUNCTION PROTOTYPES */

static void ExDAC_PortInit(void);
#if(config_ExDAC_TLV5614 == 1)
	static void Dac5614WriteDA(uint16_t channel, uint16_t codeValue);
#else
	static void Tpc112S4WriteDA(uint16_t channel, uint16_t codeValue);
#endif
/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/

/**
 * ************************************************************************
 * @Brief :      
 * ************************************************************************
 */
void IF_ExDAC_Init(void)
{
	ExDAC_PortInit();
	
   
}
/* FUNCTION *********************************************************************************
 * Function Name : IF_ExDAC_WriteDA
 * Description   : Load电机驱动参考电压设置
 * Parameter     : dacIndex ----电容序号  refVolt---参考电平
 * END ***************************************************************************************/
void IF_ExDAC_WriteDA(uint16_t channel,uint16_t refVolt)
{
	uint32_t codeValue;
	#if(config_ExDAC_TLV5614 == 1)
		/*************************************************
		Vout = REF * CODE / (2^n) (REF = 3.3V, n = 12)
		****************************************************/
		codeValue = (uint32_t) (refVolt * 4096)/(2*EXDAC_VREF);
		Dac5614WriteDA(channel,(uint16_t)codeValue);
	#else
		/*************************************************
		Vout = REF * CODE / (2^n) (REF = 3.3V, n = 12)
		****************************************************/
		codeValue = (uint32_t)(refVolt * 4096)/EXDAC_VREF;
		Tpc112S4WriteDA(channel,(uint16_t)codeValue);
	#endif
}

/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/
static void ExDAC_PortInit(void)
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
	gpio_init_struct.gpio_pins = DAC_DIN_PIN ;
	gpio_init(DAC_DIN_PORT, &gpio_init_struct);
	
	gpio_init_struct.gpio_pins = 0;
	gpio_init_struct.gpio_pins = DAC_SCLK_PIN;
	gpio_init(DAC_SCLK_PORT, &gpio_init_struct);
	
	gpio_init_struct.gpio_pins = 0;
	gpio_init_struct.gpio_pins = DAC_FS_PIN;
	gpio_init(DAC_FS_PORT, &gpio_init_struct);
	
	gpio_init_struct.gpio_pins = 0;
	gpio_init_struct.gpio_pins = DAC_CS_PIN;
	gpio_init(DAC_CS_PORT, &gpio_init_struct);
	
	gpio_init_struct.gpio_pins = 0;
	gpio_init_struct.gpio_pins = DAC_LDAC_PIN ;
	gpio_init(DAC_LDAC_PORT, &gpio_init_struct);

	gpio_init_struct.gpio_pins = 0;
	gpio_init_struct.gpio_pins = DAC_PD_PIN;
	gpio_init(DAC_PD_PORT, &gpio_init_struct);	
}
#if(config_ExDAC_TLV5614 == 1)
static void Dac5614WriteDA(uint16_t channel, uint16_t codeValue)
{
	uint8_t i;
	uint16_t data;

	DAC_PD_HIGH;
	DAC_FS_HIGH;
	DAC_SCLK_HIGH;
	DAC_CS_HIGH;
	DAC_LDAC_HIGH;
	DAC_DIN_HIGH;
	DAC_LDAC_LOW;
	data = (channel << 14) | (codeValue & 0x0FFF);

	DAC_FS_HIGH;
	DAC_CS_LOW;
	DAC_FS_LOW;

	for (i = 0; i < 16; i++)
	{
		DAC_SCLK_HIGH;

		if (data & 0x8000)
		{
			DAC_DIN_HIGH;
		}
		else
		{
			DAC_DIN_LOW;
		}

		DAC_SCLK_LOW;

		data <<= 1;
	}

	DAC_SCLK_HIGH;
	DAC_FS_HIGH;
	DAC_CS_HIGH;
}

#else
static void Tpc112S4WriteDA(uint16_t channel, uint16_t codeValue)
{
	uint8_t i;
	uint16_t data;

	DAC_FS_HIGH;
	DAC_SCLK_HIGH;
	DAC_LDAC_HIGH;
	DAC_DIN_HIGH;
	DAC_LDAC_LOW;

	data = (channel << 12) | (codeValue & 0x0FFF);

	DAC_FS_HIGH;
	DAC_FS_LOW;

	for (i = 0; i < 16; i++)
	{
		DAC_SCLK_HIGH;

		if (data & 0x8000)
		{
			DAC_DIN_HIGH;
		}
		else
		{
			DAC_DIN_LOW;
		}

		DAC_SCLK_LOW;

		data <<= 1;
	}

	DAC_SCLK_HIGH;
	DAC_FS_HIGH;
}

#endif
//*****************************************************************************
//* END
//*****************************************************************************

