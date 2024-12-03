/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_Ltc1865.h
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-19
 * ****************************************************************************
 */

#ifndef _IF_AD7689_H_
#define _IF_AD7689_H_

#include "IF_PrjLib.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#define EXT_ADC_VREF       		4096    //内部基准源
#define EX_AD_REF_VOLT       	5000	//外部基准源
#define EXT_ADC_VRES            0xFFFF   //2^16

typedef enum
{
  ADC_CHNL1  	  = 0,       //using CHANNEL 0:  Power_value_adc
  ADC_CHNL2    	  = 1,       //using CHANNEL 1:  plasma_vdc_adc
  ADC_CHNL3    	  = 2,       //using CHANNEL 2:  match_vdc_adc			
  ADC_CHNL4       = 3,       //using CHANNEL 3:  tune_position_adc
  ADC_CHNL5  	  = 4,       //using CHANNEL 0:  Power_value_adc
  ADC_CHNL6    	  = 5,       //using CHANNEL 1:  plasma_vdc_adc
  ADC_CHNL7    	  = 6,       //using CHANNEL 2:  match_vdc_adc			
  ADC_CHNL8       = 7,       //using CHANNEL 3:  tune_position_adc
  EXT_ADC_CHNL_NUM   = 8
}CHANNEL_TypeDef;

/******************************************************************************
 * Interface functions                                                         *
 ******************************************************************************/
extern void IF_AD7689Init(void);
extern uint16_t IF_SPI_ReadADCValue(uint8_t channel);

#if defined(__cplusplus)
}
#endif /* __cplusplus */


#endif

/*
*******************************************************************************
* END
*******************************************************************************
*/

