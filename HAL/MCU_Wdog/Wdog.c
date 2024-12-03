/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Wdog.c
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-10-07
 * ****************************************************************************
 */

#include "IF_Board.h"
#include "IF_Wdog.h"
#include "IF_Gpio.h"
#include "Wdog.h"


/* CONST & MACROS */

/* DATA STRUCTURES */

/* LOCAL VARIABLES */


/* GLOBAL VARIABLES */

/* FUNCTION PROTOTYPES */
static void Wdog_Init(void);
static void Wdog_Feed(void);
/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/

/**
 * ************************************************************************
 * @Brief :      
 * ************************************************************************
 */
void IF_WdogInit(void)
{
   Wdog_Init();

}


void IF_WdogFeed(void)
{
	Wdog_Feed();
}

/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/
static void Wdog_Init(void)
{
	
	gpio_init_type gpio_init_struct;
	
    /* set default parameter */
    gpio_default_para_init(&gpio_init_struct);

    /* configure the normal MCU OUTPUT gpio */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	
	gpio_init_struct.gpio_pins = 0;
    gpio_init_struct.gpio_pins = HD_WDI_PIN;
    gpio_init(HD_WDI_GPIO, &gpio_init_struct);
	IF_GpioOutPut(HD_WDI_GPIO, HD_WDI_PIN, 1, LOGIC_POUT);
}
/**
 * ************************************************************************
 * @Brief :     Wdog_Feed 
 * ************************************************************************
 */
static void Wdog_Feed(void)
{
	IF_GpioToggle(HD_WDI_GPIO,HD_WDI_PIN); 
}







//*****************************************************************************
//* END
//*****************************************************************************

