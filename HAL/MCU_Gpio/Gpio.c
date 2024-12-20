/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Gpio.c
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#include "IF_Gpio.h"
#include "Gpio.h"

/* CONST & MACROS */

/* DATA STRUCTURES */

/* LOCAL VARIABLES */

/* GLOBAL VARIABLES */

/* FUNCTION PROTOTYPES */


/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/

/**
 * ************************************************************************
 * @Brief :      
 * ************************************************************************
 */
void IF_GpioInit(void)
{

}


flag_status IF_GpioInPut(gpio_type *gpio_x, uint16_t pins, uint8_t inLogic)
{
    flag_status inValue,result;
    inValue = gpio_input_data_bit_read(gpio_x, pins);
    
    if(RESET == inValue)
    {
        result=inLogic?RESET:SET;
    }
    else
    {
        result=inLogic?SET:RESET;
    }

    return result;
    
}

void IF_GpioOutPut(gpio_type *gpio_x, uint16_t pins, uint8_t outPutValue, uint8_t outLogic)
{
    if(outLogic == LOGIC_NOUT)
    {
        if(outPutValue == 1)
        {
            gpio_bits_reset(gpio_x, pins);
        }
        else
        {
            gpio_bits_set(gpio_x, pins);
        }
    }
    else if(outLogic == LOGIC_POUT)
    {
        if(outPutValue == 1)
        {
            gpio_bits_set(gpio_x, pins);
        }
        else
        {
            gpio_bits_reset(gpio_x, pins);
        }
    } 
}


void IF_GpioToggle(gpio_type* gpio_x, uint16_t gpio_pin)
{
	uint32_t odt;
	//get current output Data Register value;
	odt = gpio_x->odt;
	
	if(odt & gpio_pin)
	{
		gpio_x->clr = gpio_pin;   //clear the selected data port bits
		
	}else
	{
		gpio_x->scr = gpio_pin;  //set the selected data port bits.
	}
}

/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/


//*****************************************************************************
//* END
//*****************************************************************************

