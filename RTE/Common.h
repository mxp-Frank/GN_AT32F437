/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Common.h
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-12
 * ****************************************************************************
 */

#ifndef __COMMON_H
#define __COMMON_H
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include "at32f435_437.h"
//#include "status.h"
#include "IF_PrjLib.h"
#include "configDevice.h"
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "event_groups.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/*---*/
#ifndef ZERO								
#define ZERO		(0U)
#endif
#if ZERO!=0
#error(Symbol "NO" is not 0!)
#endif

#ifndef YES
#define YES			(1U)		
#endif
#if YES!=1
#error(Symbol "YES" is not 1!)
#endif

#ifndef NO											
#define NO			(1U)
#endif
#if NO!=1
#error(Symbol "NO" is not 1!)
#endif

#ifndef OFF					
#define OFF			(0U)
#endif
#if OFF!=0
#error(Symbol "OFF" is not 0!)
#endif

#ifndef ON											
#define ON			(1U)
#endif
#if ON!=1
#error(Symbol "ON" is not 1!)
#endif


#ifndef INACTIVE						
#define INACTIVE	(0U)
#endif
#if INACTIVE!=0
#error(Symbol "INACTIVE" is not 0!)
#endif

#ifndef ACTIVE
#define ACTIVE		(1U)
#endif
#if ACTIVE!=1
#error(Symbol "ACTIVE" is not 1!)
#endif

#ifndef CLEAR				
#define CLEAR		(0U)
#endif
#if CLEAR!=0
#error(Symbol "CLEAR" is not 0!)
#endif


#ifndef IO_LOW					
#define IO_LOW		(0U)
#endif
#if IO_LOW!=0
#error(Symbol "IO_LOW" is not 0!)
#endif

#ifndef IO_HIGH
#define IO_HIGH		(1U)
#endif
#if IO_HIGH!=1
#error(Symbol "IO_HIGH" is not 1!)
#endif
    
#ifndef E_OK
#define E_OK 1u
#endif
#if E_OK!=1
    #error(Symbol "E_OK" is not 1!)
#endif
    
#ifndef E_NOT_OK
#define E_NOT_OK  0
#endif
 #if E_NOT_OK!=0
 #error(Symbol "E_NOT_OK" is not 0!)
#endif
    
#ifndef ENABLE
#define ENABLE 1u
#endif
#if ENABLE!=1
#error(Symbol "ENABLE" is not 1!)
#endif

#ifndef DISABLE
#define DISABLE  0
#endif
#if DISABLE!=0
#error(Symbol "DISABLE" is not 0!)
#endif

#define MAKE_UINT16(a,b)         ((uint16_t)(((a) << 8) | (b)))
#define MAKE_UINT32(a,b,c,d)     ((uint32_t)(((a) << 24) | ((b) << 16) | ((c) << 8) | (d)))

#define Byte0_UINT16(x)          ((uint8_t)((x) & 0xFF))
#define Byte1_UINT16(x)          ((uint8_t)(((x) >> 8) & 0xFF))

#define Byte0_UINT32(x)          (uint8_t)((x) & 0xFF)
#define Byte1_UINT32(x)          (uint8_t)(((x) >> 8) & 0xFF)
#define Byte2_UINT32(x)          (uint8_t)(((x) >> 16) & 0xFF)
#define Byte3_UINT32(x)          (uint8_t)(((x) >> 24) & 0xFF)

#define Word0_UINT32(x)          ((uint16_t)((x) & 0xFFFF))
#define Word1_UINT32(x)          ((uint16_t)(((x) >> 16) & 0xFFFF))
#define MAKE_UINT32_WORD(a,b)    ((uint32_t)(((a) << 16)| (b)))

#define SET_BIT(x,bit)			  (x|(0x01<<bit))	
#define GET_BIT(x,bit)			  (x&(0x01<<bit))
#define GET_UINT16(x)             (uint16_t)(x)

#define FloatToUINT16(x)		   ((x-(uint16_t)x>0)? (x+1):(x))
typedef union                                        
{
	float f_Buf;  			//小数
	u8  uc_Buf[4];  		//u8，用于存放IEEE754类型的小数
	u16 us_Buf[2];  		//u16，用于将IEEE754类型的小数存放到保持寄存器中
	u32 u_Buf;
}float_DtformConver; 

typedef union 
{
	uint16_t  Val ;
	uint16_t v[2] __attribute__((packed));
	struct __attribute__((packed))
	{
		uint8_t LB;
		uint8_t HB;
	} byte;
	struct __attribute__((packed))
	{
		 uint8_t b0:1;
		 uint8_t b1:1;
		 uint8_t b2:1;
		 uint8_t b3:1;
		 uint8_t b4:1;
		 uint8_t b5:1;
		 uint8_t b6:1;
		 uint8_t b7:1;
		 uint8_t b8:1;
		 uint8_t b9:1;
		 uint8_t b10:1;
		 uint8_t b11:1;
		 uint8_t b12:1;
		 uint8_t b13:1;
		 uint8_t b14:1;
		 uint8_t b15:1;
	}bits;

} UINT16_VAL;

typedef union 
{
		uint32_t Val;
		uint16_t w[2] __attribute__((packed));
		uint8_t  v[4];
		struct __attribute__((packed)) 
		{
			uint16_t LW;
			uint16_t HW;
		} word;
		struct __attribute__((packed)) 
		{
			uint8_t LB;
			uint8_t HB;
			uint8_t UB;
			uint8_t MB;
		} byte;
		struct __attribute__((packed))
		{
			 uint8_t b0:1;
			 uint8_t b1:1;
			 uint8_t b2:1;
			 uint8_t b3:1;
			 uint8_t b4:1;
			 uint8_t b5:1;
			 uint8_t b6:1;
			 uint8_t b7:1;
			 uint8_t b8:1;
			 uint8_t b9:1;
			 uint8_t b10:1;
			 uint8_t b11:1;
			 uint8_t b12:1;
			 uint8_t b13:1;
			 uint8_t b14:1;
			 uint8_t b15:1;
			 uint8_t b16:1;
			 uint8_t b17:1;
			 uint8_t b18:1;
			 uint8_t b19:1;
			 uint8_t b20:1;
			 uint8_t b21:1;
			 uint8_t b22:1;
			 uint8_t b23:1;
			 uint8_t b24:1;
			 uint8_t b25:1;
			 uint8_t b26:1;
			 uint8_t b27:1;
			 uint8_t b28:1;
			 uint8_t b29:1;
			 uint8_t b30:1;
			 uint8_t b31:1;
		 }bits;

} UINT32_VAL;

typedef union
{
    uint64_t Val;
    uint32_t d[2] __attribute__((packed));
    uint16_t w[4];
    uint8_t v[8];
    struct __attribute__((packed))
    {
        uint32_t LD;
        uint32_t HD;
    } dword;
    struct __attribute__((packed))
    {
        uint16_t LW;
        uint16_t HW;
        uint16_t UW;
        uint16_t MW;
    } word;
    struct __attribute__((packed))
    {
         uint8_t b0:1;
         uint8_t b1:1;
         uint8_t b2:1;
         uint8_t b3:1;
         uint8_t b4:1;
         uint8_t b5:1;
         uint8_t b6:1;
         uint8_t b7:1;
         uint8_t b8:1;
         uint8_t b9:1;
         uint8_t b10:1;
         uint8_t b11:1;
         uint8_t b12:1;
         uint8_t b13:1;
         uint8_t b14:1;
         uint8_t b15:1;
         uint8_t b16:1;
         uint8_t b17:1;
         uint8_t b18:1;
         uint8_t b19:1;
         uint8_t b20:1;
         uint8_t b21:1;
         uint8_t b22:1;
         uint8_t b23:1;
         uint8_t b24:1;
         uint8_t b25:1;
         uint8_t b26:1;
         uint8_t b27:1;
         uint8_t b28:1;
         uint8_t b29:1;
         uint8_t b30:1;
         uint8_t b31:1;
         uint8_t b32:1;
         uint8_t b33:1;
         uint8_t b34:1;
         uint8_t b35:1;
         uint8_t b36:1;
         uint8_t b37:1;
         uint8_t b38:1;
         uint8_t b39:1;
         uint8_t b40:1;
         uint8_t b41:1;
         uint8_t b42:1;
         uint8_t b43:1;
         uint8_t b44:1;
         uint8_t b45:1;
         uint8_t b46:1;
         uint8_t b47:1;
         uint8_t b48:1;
         uint8_t b49:1;
         uint8_t b50:1;
         uint8_t b51:1;
         uint8_t b52:1;
         uint8_t b53:1;
         uint8_t b54:1;
         uint8_t b55:1;
         uint8_t b56:1;
         uint8_t b57:1;
         uint8_t b58:1;
         uint8_t b59:1;
         uint8_t b60:1;
         uint8_t b61:1;
         uint8_t b62:1;
         uint8_t b63:1;
    } bits;
} UINT64_VAL;



#endif

