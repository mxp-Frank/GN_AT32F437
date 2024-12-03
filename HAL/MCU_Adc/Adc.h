/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Adc.h--->internal use
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#ifndef _ADC_H_
#define _ADC_H_

#include "IF_Board.h"
#include "IF_Gpio.h"

//ADC pin portmap
// ADC1_CH6: TEMP_ADC_Pin42_PA6;
// ADC1_CH7: VDC_ADC Pin43_PA7
// ADC1_CH8: HF_Load_A_SLE_Pin46_PB0 
// ADC1_CH9: HF_Tune_B_SLE_Pin47_PB1;
// ADC1_CH14: FAN_ADC_Pin44_PC4
// ADC1_CH15: POWER_ADC_Pin45_PC5;

// ADC3_CH4: LF_TRA_SLE_Pin18_PF6
// ADC3_CH5: LF_CAP_SLE_Pin19_PF7;
#define TEMP_ADC_GPIO		GPIOA
#define TEMP_ADC_PIN    	TEMP_ADC_Pin42_PA6

#define POWER_ADC_GPIO		GPIOA
#define	POWER_ADC_PIN		POWER_ADC_Pin43_PA7
#define ADC1_MCU            ADC1
#define ADC1_CHNNUM         6                   // ADC1 Channel used num
#define ADC1_VREF           3300                // ADC1 Vref
#define ADC1_12BIT          4096                // ADC1 Resolution

#define ADC3_MCU            ADC3
#define ADC3_CHNNUM         2                   // ADC3 Channel used num
#define ADC3_VREF           3300                // ADC3 Vref
#define ADC3_12BIT          4096                // ADC3 Resolution

#endif


//*****************************************************************************
//* END
//*****************************************************************************

