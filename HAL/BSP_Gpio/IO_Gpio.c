/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IO_Gpio.c
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-10-07
 * ****************************************************************************
 */

#include "IF_IO_Gpio.h"
#include "IO_Gpio.h"

/* CONST & MACROS */

/* DATA STRUCTURES */

/* LOCAL VARIABLES */

/* GLOBAL VARIABLES */

/* FUNCTION PROTOTYPES */
static void RFPowerPort_Init(void);


/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/

/**
 * ************************************************************************
 * @Brief :      
 * ************************************************************************
 */
void IF_IOGpioInit(void)
{
	RFPowerPort_Init();
}

/***************DC-12V  Output Interface ****************************/
void IF_Power12VOn_Switch(uint8_t para)
{
	IF_OutPutGpio_Write(PW12V_ON_GPIO, PW12V_ON_PIN, para, LOGIC_POUT);	 //高电平有效
}
/***************DC-5V  Output Interface ****************************/
void IF_DC5VOn_Switch(uint8_t para)
{
	IF_OutPutGpio_Write(DC5V_ON_GPIO, DC5V_ON_PIN, para, LOGIC_POUT);	//高电平有效
	
}
/***************InterLock EN  Output Interface ****************************/
void IF_IntLock_En_Switch(uint8_t para)					 //高电平有效
{
    IF_OutPutGpio_Write(IntLock_EN_GPIO, IntLock_EN_PIN, para, LOGIC_POUT);
}
uint8_t IF_InterLockEn_SigRead(void)
{
	 return IF_OutPutGpio_Read(IntLock_EN_GPIO,IntLock_EN_PIN,ACTIVE_HIGH);
}
/***************PCB Debug Led  Output Interface ****************************/
void IF_PCBDebugLed_Switch(uint8_t para)
{
	IF_OutPutGpio_Write(PCBDebug_LED_GPIO, PCBDebug_LED_PIN, para, LOGIC_POUT);	 //高电平有效
}
/***************PWR-LED ON/OFF  Output Interface ****************************/
void IF_PowerOnOffLed_Switch(uint8_t para)						//低电平有效
{
	IF_OutPutGpio_Write(PWR_ON_LED_GPIO, PWR_ON_LED_PIN, para, LOGIC_NOUT);
}
/***************Fault-LED ON/OFF  Output Interface ****************************/
void IF_FaultLed_Switch(uint8_t para)
{
	IF_OutPutGpio_Write(Fault_LED_GPIO, Fault_LED_PIN, para, LOGIC_NOUT);   //低电平有效
}

/***************Warning-LED ON/OFF  Output Interface ****************************/
void IF_WarningLed_Switch(uint8_t para)
{
	IF_OutPutGpio_Write(Warning_LED_GPIO, Warning_LED_PIN, para, LOGIC_NOUT);   //低电平有效
}
/***************InterLock-LED ON/OFF  Output Interface ****************************/
void IF_IntLockLed_Switch(uint8_t para)
{
	IF_OutPutGpio_Write(IntLock_LED_GPIO, IntLock_LED_PIN, para, LOGIC_NOUT);   //低电平有效
}
/***************RF-LED ON/OFF  Output Interface ****************************/
void IF_RFOnOffLed_Switch(uint8_t para)
{
	IF_OutPutGpio_Write(RF_ON_LED_GPIO, RF_ON_LED_PIN, para, LOGIC_NOUT);   //低电平有效
}
/***************InterLock Detect  Input Interface ****************************/
uint8_t IF_InterLockDect_SigRead(void)
{
    return IF_InPutGpio_Read(IntLock_Detect_GPIO, IntLock_Detect_PIN, ACTIVE_LOW); //低电平有效,高电平正常
}
	

/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/

static void RFPowerPort_Init(void)
{
	gpio_init_type gpio_init_struct;
    /* set default parameter */
	gpio_default_para_init(&gpio_init_struct);
	
    /* configure the normal output gpio */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	
	
	/*PWR_12v on gpio output structure config*/
    gpio_init_struct.gpio_pins = 0;
    gpio_init_struct.gpio_pins = PW12V_ON_PIN;
    gpio_init(PW12V_ON_GPIO, &gpio_init_struct);
	IF_OutPutGpio_Write(PW12V_ON_GPIO, PW12V_ON_PIN, 0, LOGIC_POUT);  
	
	/*DC_5v on gpio output structure config*/
    gpio_init_struct.gpio_pins = 0;
    gpio_init_struct.gpio_pins = DC5V_ON_PIN;
    gpio_init(DC5V_ON_GPIO, &gpio_init_struct);
	IF_OutPutGpio_Write(DC5V_ON_GPIO, DC5V_ON_PIN, 0, LOGIC_POUT);  
	
	/*InterLock EN on gpio output structure config*/
    gpio_init_struct.gpio_pins = 0;
    gpio_init_struct.gpio_pins = IntLock_EN_PIN;
    gpio_init(IntLock_EN_GPIO, &gpio_init_struct);
	IF_OutPutGpio_Write(IntLock_EN_GPIO, IntLock_EN_PIN, 0, LOGIC_POUT);  
	
	/*PCB Debug LED gpio output structure config*/
    gpio_init_struct.gpio_pins = 0;
    gpio_init_struct.gpio_pins = PCBDebug_LED_PIN;
    gpio_init(PCBDebug_LED_GPIO, &gpio_init_struct);
	IF_OutPutGpio_Write(PCBDebug_LED_GPIO, PCBDebug_LED_PIN, 0, LOGIC_NOUT);  
	
	/*pwr On LED gpio output structure config*/
    gpio_init_struct.gpio_pins = 0;
    gpio_init_struct.gpio_pins = PWR_ON_LED_PIN;
    gpio_init(PWR_ON_LED_GPIO, &gpio_init_struct);
	IF_OutPutGpio_Write(PWR_ON_LED_GPIO, PWR_ON_LED_PIN, 0, LOGIC_NOUT); 

	/*Warning LED gpio output structure config*/
    gpio_init_struct.gpio_pins = 0;
    gpio_init_struct.gpio_pins = Warning_LED_PIN;
    gpio_init(Warning_LED_GPIO, &gpio_init_struct);
	IF_OutPutGpio_Write(Warning_LED_GPIO, Warning_LED_PIN, 0, LOGIC_NOUT); 
	
	/*InterLock LED gpio output structure config*/
    gpio_init_struct.gpio_pins = 0;
    gpio_init_struct.gpio_pins = IntLock_LED_PIN;
    gpio_init(IntLock_LED_GPIO, &gpio_init_struct);
	IF_OutPutGpio_Write(IntLock_LED_GPIO, IntLock_LED_PIN, 0, LOGIC_NOUT); 
	
	/*RFOn LED gpio output structure config*/
    gpio_init_struct.gpio_pins = 0;
    gpio_init_struct.gpio_pins = RF_ON_LED_PIN;
    gpio_init(RF_ON_LED_GPIO, &gpio_init_struct);
	IF_OutPutGpio_Write(RF_ON_LED_GPIO, RF_ON_LED_PIN, 0, LOGIC_NOUT); 
	
	/* configure the normal INPUT gpio */
    gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
    gpio_init_struct.gpio_pull = GPIO_PULL_UP;
	
	/*InterLock Detect  gpio input structure config*/
	gpio_init_struct.gpio_pins = 0;
    gpio_init_struct.gpio_pins = IntLock_Detect_PIN;
    gpio_init(IntLock_Detect_GPIO, &gpio_init_struct);
	
}


//*****************************************************************************
//* END
//*****************************************************************************

