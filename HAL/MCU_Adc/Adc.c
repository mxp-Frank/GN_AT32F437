/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Adc.c
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */
//提醒硬件工程师将ADC资源集中在一起，避免过多使用系统DMA资源(DMA资源个数有限)
#include "IF_Adc.h"
#include "Adc.h"

/* CONST & MACROS */

/* DATA STRUCTURES */

/* LOCAL VARIABLES */
static __IO uint16_t adc1_ordinary_valuetab[ADC1_CHNNUM] = {0};
static __IO uint16_t adc3_ordinary_valuetab[ADC3_CHNNUM] = {0};

/* GLOBAL VARIABLES */
adc_type * ADC_MCU[ADCn] = {ADC1_MCU, ADC3_MCU};

/* FUNCTION PROTOTYPES */
static void Adc1_PortInit(void);
static void Dma1_config(void);
static void Adc1_config(void);
//static void Adc3_PortInit(void);
//static void Adc3_config(void);

/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/

/**
 * ************************************************************************
 * @Brief :      
 * ************************************************************************
 */
void IF_AdcInit(void)
{
    Adc1_PortInit();
    

    Dma1_config();
    
    Adc1_config();
	
//	Adc3_PortInit();
//  Adc3_config();

}

void IF_AdcStart(void)
{
	/**************************************************
	call this func to start sensor_mcu in IF_SL_CfgInit, 
	because DMA-ADC starting before bootloader will cause APPJUMP error!!!!
	**************************************************/
    dma_channel_enable(DMA1_CHANNEL1, TRUE);  //for adc1
    //dma_channel_enable(DMA1_CHANNEL2, TRUE);  //for adc3

    adc_ordinary_software_trigger_enable(ADC1, TRUE);
    //adc_ordinary_software_trigger_enable(ADC3, TRUE);

}

uint16_t IF_AdcResult_Read(ADC_TypeDef adcnx, adc_channel_select_type adc_channel)
{
    uint16_t tmp;
    if(adcnx == ADCn1)
    {
        switch (adc_channel)
        {
        case ADC_CHANNEL_6:				//temp_adc
            tmp =  adc1_ordinary_valuetab[0];
            break;
		
		case ADC_CHANNEL_7:				//power_adc
            tmp =  adc1_ordinary_valuetab[1];
            break;

        case ADC_CHANNEL_14:			//Fan1_adc
            tmp = adc1_ordinary_valuetab[3];
            break; 
   
        case ADC_CHANNEL_15:			//Fan2_adc
            tmp = adc1_ordinary_valuetab[4];
            break; 
		
        default:
            tmp = 0;
            break;
        }

    }
	return tmp;
}

/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/

static void Adc1_PortInit(void)
{
    gpio_init_type gpio_initstructure;

    gpio_default_para_init(&gpio_initstructure);

    /* config adc pin as analog input mode */
    gpio_initstructure.gpio_mode = GPIO_MODE_ANALOG;
    gpio_initstructure.gpio_pins = 0;
    gpio_initstructure.gpio_pins = TEMP_ADC_PIN ;
    gpio_init(TEMP_ADC_GPIO, &gpio_initstructure);
	
    gpio_initstructure.gpio_pins = 0;
    gpio_initstructure.gpio_pins =  POWER_ADC_PIN;
    gpio_init(POWER_ADC_GPIO, &gpio_initstructure);
}


static void Dma1_config(void)
{
    dma_init_type dma_init_struct;
    crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
    nvic_irq_enable(DMA1_Channel1_IRQn, 0, 0);  //? not needed ?

    dma_reset(DMA1_CHANNEL1);
    dma_default_para_init(&dma_init_struct);
    dma_init_struct.buffer_size = ADC1_CHNNUM;
    dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_base_addr = (uint32_t)adc1_ordinary_valuetab;
    dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
    dma_init_struct.memory_inc_enable = TRUE;
    dma_init_struct.peripheral_base_addr = (uint32_t)&(ADC1->odt);
    dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
    dma_init_struct.peripheral_inc_enable = FALSE;//fixed value, don't need +1
    dma_init_struct.priority = DMA_PRIORITY_HIGH;
    dma_init_struct.loop_mode_enable = TRUE;//change to loop mode;
    dma_init(DMA1_CHANNEL1, &dma_init_struct);

    dmamux_enable(DMA1, TRUE);
    dmamux_init(DMA1MUX_CHANNEL1, DMAMUX_DMAREQ_ID_ADC1);

    dma_reset(DMA1_CHANNEL2);
    dma_default_para_init(&dma_init_struct);
    dma_init_struct.buffer_size = ADC3_CHNNUM;
    dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_base_addr = (uint32_t)adc3_ordinary_valuetab;
    dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
    dma_init_struct.memory_inc_enable = TRUE;
    dma_init_struct.peripheral_base_addr = (uint32_t)&(ADC3->odt);
    dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
    dma_init_struct.peripheral_inc_enable = FALSE;//fixed value, don't need +1
    dma_init_struct.priority = DMA_PRIORITY_HIGH;
    dma_init_struct.loop_mode_enable = TRUE;//change to loop mode;
    dma_init(DMA1_CHANNEL2, &dma_init_struct);

    dmamux_enable(DMA1, TRUE);
    dmamux_init(DMA1MUX_CHANNEL2, DMAMUX_DMAREQ_ID_ADC3);

}

static void Adc1_config(void)
{
    adc_common_config_type adc_common_struct;
    adc_base_config_type adc_base_struct;
    crm_periph_clock_enable(CRM_ADC1_PERIPH_CLOCK, TRUE);
    nvic_irq_enable(ADC1_2_3_IRQn, 0, 0);  //? not needed ?

    adc_common_default_para_init(&adc_common_struct);

    /* config combine mode */
    adc_common_struct.combine_mode = ADC_INDEPENDENT_MODE;//not Master & Slaver Combine...

    /* config division,adcclk is division by hclk */
    adc_common_struct.div = ADC_HCLK_DIV_4;

    /* config common dma mode,it's not useful in independent mode */
    adc_common_struct.common_dma_mode = ADC_COMMON_DMAMODE_DISABLE;

    /* config common dma request repeat,it's not useful in independent mode */
    adc_common_struct.common_dma_request_repeat_state = FALSE;

    /* config adjacent adc sampling interval,it's useful for ordinary shifting mode */
    adc_common_struct.sampling_interval = ADC_SAMPLING_INTERVAL_5CYCLES;

    /* config inner temperature sensor and vintrv */
    adc_common_struct.tempervintrv_state = FALSE;

    /* config voltage battery */
    adc_common_struct.vbat_state = FALSE;
    adc_common_config(&adc_common_struct);

    adc_base_default_para_init(&adc_base_struct);

    adc_base_struct.sequence_mode = TRUE;
    adc_base_struct.repeat_mode = TRUE;
    adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
    adc_base_struct.ordinary_channel_length = ADC1_CHNNUM;
    adc_base_config(ADC1, &adc_base_struct);
    adc_resolution_set(ADC1, ADC_RESOLUTION_12B);

    /* config ordinary channel */  //use dma1-ch1
    adc_ordinary_channel_set(ADC1, ADC_CHANNEL_6,  1, ADC_SAMPLETIME_47_5);    //TEMP_ADC
	adc_ordinary_channel_set(ADC1, ADC_CHANNEL_7,  2, ADC_SAMPLETIME_47_5);    //PWR_ADC  

    /* config ordinary trigger source and trigger edge */
    adc_ordinary_conversion_trigger_set(ADC1, ADC_ORDINARY_TRIG_TMR1CH1, ADC_ORDINARY_TRIG_EDGE_NONE);

    /* config dma mode,it's not useful when common dma mode is use */
    adc_dma_mode_enable(ADC1, TRUE);//when ADC_ODT is update, then send a DMA request
                                    //should move after the oversample settings;
    /* config dma request repeat,it's not useful when common dma mode is use */
    adc_dma_request_repeat_enable(ADC1, TRUE);  //repeat dma transmit, needed by test!
    //Needed! not redundant after "adc_dma_mode_enable(ADC1, TRUE);"
												//when DMA transfer num=0, then repeat
    adc_oversample_ratio_shift_set(ADC1, ADC_OVERSAMPLE_RATIO_32, ADC_OVERSAMPLE_SHIFT_5);

    /* disable ordinary oversampling trigger mode */
    adc_ordinary_oversample_trig_enable(ADC1, FALSE);//not need re-trigger...

    /* set ordinary oversample restart mode */
    adc_ordinary_oversample_restart_set(ADC1, ADC_OVERSAMPLE_CONTINUE); //被抢占通道打断后，不清零之前的累积值，而是继续。。。

    /* enable ordinary oversampling */
    adc_ordinary_oversample_enable(ADC1, TRUE);

    /* adc enable */
    adc_enable(ADC1, TRUE);
    while(adc_flag_get(ADC1, ADC_RDY_FLAG) == RESET);

    /* adc calibration */
    adc_calibration_init(ADC1);
    while(adc_calibration_init_status_get(ADC1));
    adc_calibration_start(ADC1);
    while(adc_calibration_status_get(ADC1));

}


//*****************************************************************************
//* END
//*****************************************************************************

