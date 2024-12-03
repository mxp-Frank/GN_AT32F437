/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_PrjLib.h
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-17
 * ****************************************************************************
 */

#ifndef _IF_PRJLIB_H_
#define _IF_PRJLIB_H_
#include <math.h>
#include <stdio.h> 
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <assert.h>
#include "IF_Board.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#define ABS(x)   				((x<0)? (-x):(x))

typedef union
{
    uint32_t lvalue;
    int16_t  ivalue;
	int8_t   value;
}U32ToInt_Union;

extern uint32_t GetData_32Bit(uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4);
extern uint16_t GetData_16Bit(uint8_t data1, uint8_t data2);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

 #endif /*_IF_LIBRARY_H_*/
 /*
*******************************************************************************
* END
*******************************************************************************
*/
