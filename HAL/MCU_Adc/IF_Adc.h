/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_Adc.h--->external use
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#ifndef _IF_ADC_H_
#define _IF_ADC_H_

#include "IF_PrjLib.h"
#include "IF_Board.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
typedef enum
{
  ADCn1 = 0,
  ADCn3 = 1,
  ADCn
}ADC_TypeDef;


/******************************************************************************
 * Interface functions                                                         *
 ******************************************************************************/
extern void IF_AdcInit(void);
extern void IF_AdcStart(void);
uint16_t IF_AdcResult_Read(ADC_TypeDef adcnx, adc_channel_select_type adc_channel);
#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif

/*
*******************************************************************************
* END
*******************************************************************************
*/

