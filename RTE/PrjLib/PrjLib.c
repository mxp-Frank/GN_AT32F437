/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  PrjLib.c
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-18
 * ****************************************************************************
 */
#include "IF_PrjLib.h"
#include "PrjLib.h"

/* CONST & MACROS */

/* DATA STRUCTURES */

/* LOCAL VARIABLES */

/* GLOBAL VARIABLES */

/* FUNCTION PROTOTYPES */

/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/

//===由4个字节拼成一个双字===//
uint32_t GetData_32Bit(uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4)
{
    uint32_t tmp1, tmp2, tmp3, tmp4, tmp;

    tmp1 = data1;
    tmp2 = data2;
    tmp3 = data3;
    tmp4 = data4;
    tmp = ((tmp1 << 24) & 0xff000000) | ((tmp2 << 16) & 0x00ff0000) | ((tmp3 << 8) & 0x0000ff00) | (tmp4 & 0x000000ff);
    
    return tmp;
}

//===由两个字节拼成一个字===//
uint16_t GetData_16Bit(uint8_t data1, uint8_t data2)
{
    uint16_t tmp1,tmp2,tmp;
    tmp1 = data1;
    tmp2 = data2;
    tmp = ((tmp1 << 8) & 0xff00) | (tmp2 & 0x00ff);

    return tmp;
}


/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/






//*****************************************************************************
//* END
//*****************************************************************************


