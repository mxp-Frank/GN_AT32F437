/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Dac.c
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#include "IF_Dac.h"
#include "Dac.h"

/* CONST & MACROS */

/* DATA STRUCTURES */

/* LOCAL VARIABLES */

/* GLOBAL VARIABLES */

/* FUNCTION PROTOTYPES */

/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/
static void Dac_PortInit(void);
static void Dac_Config(void);

/**
 * ************************************************************************
 * @Brief :      
 * ************************************************************************
 */
void IF_DacInit(void)
{
    Dac_PortInit();
    Dac_Config();

}

void IF_Dac_SetValue(dac_select_type dacChn, uint16_t dacValue)
{
    uint16_t tmp = 0;

    tmp =  0xFFF & dacValue;

    if(dacChn == DAC1)
    {
        dac_1_data_set(DAC1_12BIT_RIGHT, tmp);//note:4096=0v;4095=max Vout
        dac_software_trigger_generate(DAC1_SELECT);
    }
    else if (dacChn == DAC2)
    {
        dac_2_data_set(DAC2_12BIT_RIGHT, tmp);//note:4096=0v;4095=max Vout
        dac_software_trigger_generate(DAC2_SELECT);
    }

}

/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/

static void Dac_PortInit(void)
{
    gpio_init_type gpio_initstructure;

    gpio_default_para_init(&gpio_initstructure);

    /* config adc pin as analog input mode */
    gpio_initstructure.gpio_mode = GPIO_MODE_ANALOG;
    gpio_initstructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_initstructure.gpio_pull = GPIO_PULL_NONE;
    gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	
}


static void Dac_Config(void)
{
    crm_periph_clock_enable(CRM_DAC_PERIPH_CLOCK, TRUE);

    /* dac1 configuration */
    dac_trigger_select(DAC1_SELECT, DAC_SOFTWARE_TRIGGER);
    dac_trigger_enable(DAC1_SELECT, TRUE);
    dac_output_buffer_enable(DAC1_SELECT, TRUE);

    /* dac2 configuration */
    dac_trigger_select(DAC2_SELECT, DAC_SOFTWARE_TRIGGER);
    dac_trigger_enable(DAC2_SELECT, TRUE);
    dac_output_buffer_enable(DAC2_SELECT, TRUE);
	
#if(config_AnalogInterface == 1)
    /* enable dac */
    dac_enable(DAC1_SELECT, TRUE);
    dac_enable(DAC2_SELECT, TRUE);
#endif
	
}
//*****************************************************************************
//* END
//*****************************************************************************

