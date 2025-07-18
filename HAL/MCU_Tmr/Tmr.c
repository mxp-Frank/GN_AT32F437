/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Tmr.c
 * @Brief :      Total 3 timers: 1ms timer, TUNE motor timer, LOAD motor timer;
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#include "IF_Tmr.h"
#include "Tmr.h"
#include "FreeRTOS.h"
#include "task.h"



/* CONST & MACROS */
static uint32_t fac_us=0;							//us延时倍乘数
static uint16_t fac_ms=0;							//ms延时倍乘数,在FreeRTOS下,代表每个节拍的ms数

extern void xPortSysTickHandler(void);
/* GLOBAL VARIABLES */

/* FUNCTION PROTOTYPES */
static void TIMER1_HAL_Init(void);

static void TIMER2_PWM_Port_Init(void);
static void TIMER2_PWM_Init(void);

static void TIMER3_HAL_Init(void);
static void delay_init(void);


/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/

/**
 * ************************************************************************
 * @Brief :      
 * ************************************************************************
 */

__weak void TMR_1ms_Callback(void)
{

}
__weak void TMR_Modbus_ISR_Callback(void)
{

}
__weak void FAN_PWM_ISR_Callback(void)
{
	
}

void IF_FAN_PWM_Switch(uint8_t para)
{
	IF_OutPutGpio_Write(FAN_PWM_PORT, FAN_PWM_PIN, para, LOGIC_POUT);	//高电平有效	
}	

/* FUNCTION *******************************************************************
 * Function Name : IF_TmrInit
 * Description   : 定时器初始化
 * Parameter	 : 
 * Return        :  
 * END ***********************************************************************/
void IF_TmrInit(void)
{
	//系统时钟初始化
	delay_init();
	//1ms硬件时钟
    TIMER1_HAL_Init();
	//Fan硬件PWM时钟
	TIMER2_PWM_Port_Init();
	TIMER2_PWM_Init();
	//Modbus接收时钟
	TIMER3_HAL_Init();
}

/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/
/* FUNCTION *******************************************************************
 * Function Name : TIMER1_HAL_Init
 * Description   : 定时器初始化 TMR6 for OSTICK 1ms
 * Parameter	 : 
 * Return        :  
 * END ***********************************************************************/
static void TIMER1_HAL_Init(void)
{
    crm_clocks_freq_type crm_clocks_freq_struct = {0};
    tmr_cfg_type tmrCfg;

    /* get system clock */
    crm_clocks_freq_get(&crm_clocks_freq_struct);
    tmrCfg.tmrDiv = ((crm_clocks_freq_struct.apb1_freq * 2) / TMRCLK_28M) - 1;
    tmrCfg.tmrPeriod = TMRPRD_1MS;  //1ms period
	
	/* enable tmr6 clock */
	crm_periph_clock_enable(CRM_TMR6_PERIPH_CLOCK, TRUE);
    /* tmr6 configuration */
    tmr_base_init(TMR6, tmrCfg.tmrPeriod, tmrCfg.tmrDiv);
    tmr_cnt_dir_set(TMR6, TMR_COUNT_UP);

    /* overflow interrupt enable */
    tmr_interrupt_enable(TMR6, TMR_OVF_INT, TRUE);

    /* tmr6 hall interrupt nvic init */
    nvic_irq_enable(TMR6_DAC_GLOBAL_IRQn, 1, 0);

    /* enable tmr6 */
#ifdef DEBUG_ON    
    debug_apb1_periph_mode_set(DEBUG_TMR6_PAUSE, TRUE);
#endif

    tmr_counter_enable(TMR6, TRUE);

}

/****************************************************************
  * @brief  this function  timer1 handler overflow.
  *         TMR6 for OSTICK 1ms
  * @param  none 
  * @param  none
  * @retval none
  *****************************************************************/
void TMR6_DAC_GLOBAL_IRQHandler(void)
{

    if(tmr_flag_get(TMR6, TMR_OVF_FLAG) == SET)
    {
        TMR_1ms_Callback();
        tmr_flag_clear(TMR6, TMR_OVF_FLAG);
    }
}
/* FUNCTION *******************************************************************
 * Function Name : TIMER2_PWM_Port_Init
 * Description   : PWM输出管脚Init
 * Parameter	 : 
 * Return        :  
 * END ***********************************************************************/
static void TIMER2_PWM_Port_Init(void)
{
	gpio_init_type gpio_init_struct;

	gpio_default_para_init(&gpio_init_struct);
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	
	gpio_init_struct.gpio_pins = FAN_PWM_PIN;
	gpio_init(FAN_PWM_PORT, &gpio_init_struct);
	IF_OutPutGpio_Write(FAN_PWM_PORT, FAN_PWM_PIN, 1, LOGIC_POUT);	//高电平有效
	
}
/* FUNCTION *******************************************************************
 * Function Name : TIMER2_PWM_Init
 * Description   : 定时器初始化 TMR6 for PWM周期 1ms
 * Parameter	 : duty------占空比
 * Return        :  
 * END ***********************************************************************/
static void TIMER2_PWM_Init(void)
{
	crm_clocks_freq_type crm_clocks_freq_struct = {0};
    tmr_cfg_type tmrCfg;

    /* get system clock */
    crm_clocks_freq_get(&crm_clocks_freq_struct);
    tmrCfg.tmrDiv = ((crm_clocks_freq_struct.apb2_freq * 2) / TMRCLK_28M) - 1;
    tmrCfg.tmrPeriod = TMRPRD_25US;  //25us period

	/* enable tmr1 clock */
	crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);
    /* tmr5 configuration */
   
	/* time base configuration */
    tmr_base_init(TMR1, tmrCfg.tmrPeriod, tmrCfg.tmrDiv);
    tmr_cnt_dir_set(TMR1, TMR_COUNT_UP);

    /* overflow interrupt enable */
    tmr_interrupt_enable(TMR1, TMR_OVF_INT, TRUE);

    /* tmr1 hall interrupt nvic init */
    nvic_irq_enable(TMR1_OVF_TMR10_IRQn, 3, 0);
	/* enable tmr1 */
	#ifdef DEBUG_ON    
		debug_apb2_periph_mode_set(DEBUG_TMR1_PAUSE, TRUE);
	#endif
    tmr_counter_enable(TMR1, TRUE);
}
/****************************************************************
  * @brief  this function handles timer3 overflow handler.
  *         TMR1 for LOAD1_MOTOR is 25us timer  
  * @param  none
  * @retval none
  *****************************************************************/
void TMR1_OVF_TMR10_IRQHandler(void)
{
    if(tmr_flag_get(TMR1, TMR_OVF_FLAG) == SET)
    {
        //user's callback
        FAN_PWM_ISR_Callback();
        tmr_flag_clear(TMR1, TMR_OVF_FLAG);
    }
}
/* FUNCTION *******************************************************************
 * Function Name : TIMER3_HAL_Init
 * Description   : 定时器初始化 TMR5 for OSTICK 10us
 * Parameter	 : 
 * Return        :  
 * END ***********************************************************************/
static void TIMER3_HAL_Init(void)
{
    crm_clocks_freq_type crm_clocks_freq_struct = {0};
    tmr_cfg_type tmrCfg;

    /* get system clock */
    crm_clocks_freq_get(&crm_clocks_freq_struct);
    tmrCfg.tmrDiv = ((crm_clocks_freq_struct.apb1_freq * 2) / TMRCLK_28M) - 1;
    tmrCfg.tmrPeriod = TMRPRD_1MS;  //1ms period

	/* enable tmr1 clock */
	crm_periph_clock_enable(CRM_TMR5_PERIPH_CLOCK, TRUE);
    /* tmr5 configuration */
   
	/* time base configuration */
    tmr_base_init(TMR5, tmrCfg.tmrPeriod, tmrCfg.tmrDiv);
    tmr_cnt_dir_set(TMR5, TMR_COUNT_UP);

    /* overflow interrupt enable */
    tmr_interrupt_enable(TMR5, TMR_OVF_INT, TRUE);

    /* tmr1 hall interrupt nvic init */
    nvic_irq_enable(TMR5_GLOBAL_IRQn, 8, 0);
	/* enable tmr5 */
	#ifdef DEBUG_ON    
		debug_apb1_periph_mode_set(DEBUG_TMR5_PAUSE, TRUE);
	#endif
    tmr_counter_enable(TMR5, TRUE);
}

/****************************************************************
  * @brief  this function handles timer3 overflow handler.
  *         TMR1 for LOAD1_MOTOR is 25us timer  
  * @param  none
  * @retval none
  *****************************************************************/
void TMR5_GLOBAL_IRQHandler(void)
{
    if(tmr_flag_get(TMR5, TMR_OVF_FLAG) == SET)
    {
        //user's callback
        TMR_Modbus_ISR_Callback();
		
        tmr_flag_clear(TMR5, TMR_OVF_FLAG);
    }
}

/* FUNCTION *********************************************************************************
 * Function Name : delay_init
 * Description   : 初始化延迟函数，这里为了兼容FreeRTOS, SystemCoreClock:系统时钟频率
 * Parameter     :
 * Return        :
 * END ***************************************************************************************/
void delay_init(void)
{
	uint32_t reload;

	fac_us = SystemCoreClock / 1000000;				     // 不论是否使用OS,fac_us都需要使用
	reload = SystemCoreClock / 1000000;				     // 每秒钟的计数次数 单位为M
	reload *= 1000000 / configTICK_RATE_HZ;			     // 根据configTICK_RATE_HZ设定溢出时间												    //reload为24位寄存器,最大值:16777216,在72M下,约合0.233s左右
	fac_ms = 1000 / configTICK_RATE_HZ;				     // 代表OS可以延时的最少单位

	SysTick->LOAD = reload;                              // Set reload register to max value
	SysTick->VAL = 0;                                    // As per ARM reference initialization, set initial value to 0
	                                                     // interrupts are only triggered when going from 1 to 0
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |         // Set timer to core clock frequency
	                SysTick_CTRL_TICKINT_Msk   |           // Enable interrupts on timeout
	                SysTick_CTRL_ENABLE_Msk;             // Enable SysTick IRQ and SysTick Timer
}


void delay_SuspendTick(void)
{
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}	

void delay_ResumeTick(void)
{
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
}
/* FUNCTION *********************************************************************************
 * Function Name : delay_us
 * Description   : 延时nus,引起任务调度  0~204522252(最大值即2^32/fac_us@fac_us=120)
 * Parameter     : nus:  要延时的us数.
 * Return        :
 * END ***************************************************************************************/
void delay_us(uint32_t nus)
{
	uint32_t ticks, told, tnow, tcnt = 0;
	uint32_t reload = SysTick->LOAD;			//LOAD的值
	ticks = nus * fac_us; 						//需要的节拍数
	told = SysTick->VAL;        				//刚进入时的计数器值

	while(1)
	{
		tnow = SysTick->VAL;

		if(tnow != told)
		{
			if (tnow < told)
			{
				tcnt += told - tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			}
			else
			{
				tcnt += reload - tnow + told;
			}

			told = tnow;

			if (tcnt >= ticks)
			{
				break;			       //时间超过/等于要延迟的时间,则退出.
			}
		}
	}
}

/* FUNCTION *********************************************************************************
 * Function Name : delay_ms
 * Description   : 延时nms,引起任务调度  0~204522252(最大值即2^32/fac_us@fac_us=120)
 * Parameter     : nms:  要延时的ms数.
 * Return        :
 * END ***************************************************************************************/
void delay_ms(uint32_t nms)
{
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)//系统已经运行
	{
		if (nms >= fac_ms)
		{
			vTaskDelay(nms / fac_ms);   //延时的时间大于OS的最少时间周期
		}

		nms %= fac_ms;					//OS已经无法提供这么小的延时了,采用普通方式延时
	}
	
	delay_us((uint32_t)(nms*1000));		//普通方式延时
	
}

/* FUNCTION *********************************************************************************
 * Function Name : delay_xms
 * Description   : 延时nms,不会引起任务调度  0~204522252(最大值即2^32/fac_us@fac_us=120)
 * Parameter     : nms:  要延时的ms数.
 * Return        :
 * END ***************************************************************************************/
void delay_xms(uint32_t nms)
{
	uint32_t i;

	for(i = 0; i < nms; i++)
	{
		delay_us(1000);
	}
}

/* FUNCTION *********************************************************************************
 * Function Name : delay_xns
 * Description   : 延时ns,不会引起任务调度  0~204522252(最大值即2^32/fac_us@fac_us=120)
 * Parameter     : ns:  要延时的ns数.
 * Return        :
 * END ***************************************************************************************/
void delay_xns(uint32_t ns)
{
	uint32_t i;

	for(i = 0; i < ns; i++)
	{
		__NOP();
	}
}
//*****************************************************************************
//* END
//*****************************************************************************

