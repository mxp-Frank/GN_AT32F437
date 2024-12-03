/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Board.c
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#include "IF_Board.h"
#include "Board.h"

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
void IF_BoardInit(void)
{
    //Vector offset
    #ifndef BOOT
    nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x32200);
    #endif

    //clock and interrup
    system_clock_config();

    /* enable the all gpio clock, this code must be placed aboved the GPIO config code*/ 
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOD_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOE_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOF_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOG_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOH_PERIPH_CLOCK, TRUE);
	
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);//!!!!!其他位置已经去掉

	/* enable pwc clock */
	crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);
	
	
}


/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/



//*****************************************************************************
//* END
//*****************************************************************************

