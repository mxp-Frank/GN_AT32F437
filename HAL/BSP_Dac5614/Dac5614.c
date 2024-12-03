/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Dac5614.c
 * @Brief :      ref from bsp_dac_tlv5614.c(AMP/AME svn324) Author: Huxian
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#include "IF_Dac5614.h"
#include "Dac5614.h"

/* CONST & MACROS */

/* DATA STRUCTURES */

/* LOCAL VARIABLES */

/* GLOBAL VARIABLES */

/* FUNCTION PROTOTYPES */
static void Dac5614_PortInit(void);

/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/

/**
 * ************************************************************************
 * @Brief :      
 * ************************************************************************
 */
void IF_Dac5614Init(void)
{
    Dac5614_PortInit();
}


void IF_Dac5614WriteDAC(uint16_t channel, uint16_t codeValue)
{
	uint8_t i;
	uint16_t data;
	
	FS_HIGH;
	SCLK_HIGH;

	LDAC_HIGH;
	DIN_HIGH;
	LDAC_LOW;

	data = (channel << 14) | (codeValue & 0x0FFF);

	FS_HIGH;
	FS_LOW;

	for (i = 0; i < 16; i++)
	{
		SCLK_HIGH;

		if (data & 0x8000)
		{
			DIN_HIGH;
		}
		else
		{
			DIN_LOW;
		}

		SCLK_LOW;

		data <<= 1;
	}

	SCLK_HIGH;
	FS_HIGH;
}

/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/

static void Dac5614_PortInit(void)
{//all pins are output...
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
    gpio_init_struct.gpio_pins = DAC_LDAC_PIN;							
    gpio_init(DAC_LDAC_GPIO, &gpio_init_struct);
	
    //DAC pin configuartion
    gpio_init_struct.gpio_pins = 0;
    gpio_init_struct.gpio_pins = DAC_DIN_PIN; 
    gpio_init(DAC_DIN_GPIO, &gpio_init_struct);
	
	 //DAC pin configuartion
    gpio_init_struct.gpio_pins = 0;
    gpio_init_struct.gpio_pins = DAC_SCLK_PIN;							 
    gpio_init(DAC_SCLK_GPIO, &gpio_init_struct);
		
	gpio_init_struct.gpio_pins = 0;
	gpio_init_struct.gpio_pins = DAC_FS_PIN;
	gpio_init(DAC_FS_GPIO, &gpio_init_struct);
	
}

//*****************************************************************************
//* END
//*****************************************************************************

