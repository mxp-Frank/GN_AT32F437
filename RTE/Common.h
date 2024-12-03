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

#endif

