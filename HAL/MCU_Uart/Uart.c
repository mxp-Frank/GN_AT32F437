/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Uart.c
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#include "IF_Uart.h"
#include "Uart.h"

/* CONST & MACROS */

/* DATA STRUCTURES */

/* LOCAL VARIABLES */

/* GLOBAL VARIABLES */
usart_type * COM_USART[COMn] = {COM1_UART, COM2_UART, COM3_UART, COM4_UART, COM5_UART};

/* FUNCTION PROTOTYPES */
/* support printf function, usemicrolib is unnecessary */
#if (__ARMCC_VERSION > 6000000)
  __asm (".global __use_no_semihosting\n\t");
  void _sys_exit(int x)
  {
    x = x;
  }
  /* __use_no_semihosting was requested, but _ttywrch was */
  void _ttywrch(int ch)
  {
    ch = ch;
  }
  FILE __stdout;
#else
 #ifdef __CC_ARM
  #pragma import(__use_no_semihosting)
  struct __FILE
  {
    int handle;
  };
  FILE __stdout;
  void _sys_exit(int x)
  {
    x = x;
  }
  /* __use_no_semihosting was requested, but _ttywrch was */
  void _ttywrch(int ch)
  {
    ch = ch;
  }
 #endif
#endif

#if defined (__GNUC__) && !defined (__clang__)
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

/**
  * @brief  retargets the c library printf function to the usart.
  * @param  none
  * @retval none
  */
PUTCHAR_PROTOTYPE
{
  while(usart_flag_get(COM5_UART, USART_TDBE_FLAG) == RESET);
  usart_data_transmit(COM5_UART, ch);
  return ch;
}

static void COM1_UART_PortInit(void);
static void COM1_UART_config(void);
static void COM2_UART_PortInit(void);
static void COM2_UART_config(void);
static void COM3_UART_PortInit(void);
static void COM3_UART_config(void);
static void COM4_UART_PortInit(void);
static void COM4_UART_config(void);
static void COM5_UART_PortInit(void);
static void COM5_UART_config(void);
/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/

/**
 * ************************************************************************
 * @Brief :      
 * ************************************************************************
 */
void IF_UartInit(void)
{
    COM1_UART_PortInit();
    COM1_UART_config();

    COM2_UART_PortInit();
    COM2_UART_config();

    COM3_UART_PortInit();
    COM3_UART_config();

    COM4_UART_PortInit();
    COM4_UART_config();

    COM5_UART_PortInit();
    COM5_UART_config();
	
}

void IF_UartSetBaudRate(uint8_t port, UARTBAUD_TypeDef baudrate)
{
    usart_init(COM_USART[port], baudrate, USART_DATA_8BITS, USART_STOP_1_BIT);
}

//打开UART发送中断
void IF_UartEnableTx(uint8_t port)
{
    usart_interrupt_enable(COM_USART[port], USART_TDBE_INT, TRUE);
}

//关闭UART发送中断
void IF_UartDisableTx(uint8_t port)
{
    usart_interrupt_enable(COM_USART[port], USART_TDBE_INT, FALSE);
}

void IF_UartSendByte(uint8_t port, uint8_t byte)
{
    usart_data_transmit(COM_USART[port], byte);
}


uint8_t IF_UartGetFlag(uint8_t port, uint32_t flag)
{
    uint8_t tmp = 0;

    if((flag == USART_TDBE_FLAG) || (flag == USART_RDBF_FLAG))
    {
        tmp = usart_flag_get(COM_USART[port], flag);
    }

    return tmp;
    
}

void IF_UartClearFlag(uint8_t port,uint32_t flag)
{
	 if((flag == USART_TDBE_FLAG) || (flag == USART_RDBF_FLAG))
	 {
		 usart_flag_clear(COM_USART[port], flag);
	 }
}

uint8_t IF_UartReadByte(uint8_t port)
{
    return usart_data_receive(COM_USART[port]);
}


UARTBAUD_TypeDef IF_Uart_GetBaudRateValue(uint8_t dataRateKey)
{
	UARTBAUD_TypeDef baudRateValue = BAUD38400;

	switch(dataRateKey)
	{
	case 0:
		baudRateValue = BAUD9600;
		break;
	case 1:
		baudRateValue = BAUD19200;
		break;
	case 2:
		baudRateValue = BAUD38400;
		break;
	case 3:
		baudRateValue = BAUD57600;
		break;
	case 4:
		baudRateValue = BAUD115200;
		break;
	default:
		baudRateValue = BAUD38400;
		break;
	} 
	return baudRateValue; 	
}   
uint8_t IF_Uart_GetBaudRateKey(UARTBAUD_TypeDef baudRateValue)
{
	uint8_t datarate = 5;	
	switch(baudRateValue)
	{
		case BAUD9600:   datarate =0;break;
		case BAUD19200:  datarate =1;break;
		case BAUD38400:  datarate =2;break;
		case BAUD57600:  datarate =3;break;
		case BAUD115200: datarate =4;break;
		default:  		 datarate =5;break;
	}
	return datarate;
}	
/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/

/*******************************************************
@Port	:		 COM1_UART: uart6
@description :   TTL电平与其他PCB通信
******************************************************/
static void COM1_UART_PortInit(void)
{	
     gpio_init_type gpio_initstructure;

    gpio_default_para_init(&gpio_initstructure);

    /* configure the usart tx, rx pin */
    gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_initstructure.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
    gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
    
	
	gpio_initstructure.gpio_pins = 0;
    gpio_initstructure.gpio_pins = COM1_TX_PIN;
    gpio_initstructure.gpio_pull = GPIO_PULL_NONE;
    gpio_init(COM1_TX_PORT, &gpio_initstructure);
	
	gpio_pin_mux_config(COM1_TX_PORT, COM1_TX_PINS_SOURCE, COM1_TX_MUX);
	
    gpio_initstructure.gpio_pins = 0;
    gpio_initstructure.gpio_pins = COM1_RX_PIN;
    gpio_initstructure.gpio_pull = GPIO_PULL_NONE;
    gpio_init(COM1_RX_PORT, &gpio_initstructure);
	
    gpio_pin_mux_config(COM1_RX_PORT, COM1_RX_PINS_SOURCE, COM1_RX_MUX);
}

static void COM1_UART_config(void)
{
	 /* enable the usart and gpio clock */
    crm_periph_clock_enable(COM1_UART_CLK, TRUE);

    /* config usart nvic interrupt */
    nvic_irq_enable(COM1_UART_IRQn, 5, 0);

    /* configure usart param */
#if(config_UserInterface == 2)	
    usart_init(COM1_UART, 115200, USART_DATA_8BITS, USART_STOP_1_BIT);
#else
	 usart_init(COM1_UART, COM1_UART_BAUD, USART_DATA_8BITS, USART_STOP_1_BIT);
#endif
    usart_transmitter_enable(COM1_UART, TRUE);
    usart_receiver_enable(COM1_UART, TRUE);

    /* enable usart interrupt */
    usart_interrupt_enable(COM1_UART, USART_RDBF_INT, TRUE);
    usart_enable(COM1_UART, TRUE); //for back using...
    

}

__weak void USER_UART_Callback(void)
{
  
}

void COM1_UART_IRQHandler(void)
{
     USER_UART_Callback();
}


/*******************************************************
@Port	:		 COM2_UART: uart2
@description	: 连接到RS232电平
******************************************************/
static void COM2_UART_PortInit(void)
{
    gpio_init_type gpio_initstructure;

    gpio_default_para_init(&gpio_initstructure);

    /* configure the usart tx, rx pin */
    gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_initstructure.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
    gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
    
 	gpio_initstructure.gpio_pins = 0;
    gpio_initstructure.gpio_pins = COM2_TX_PIN;
    gpio_initstructure.gpio_pull = GPIO_PULL_NONE;
    gpio_init(COM2_TX_PORT, &gpio_initstructure);	
	gpio_pin_mux_config(COM2_TX_PORT, COM2_TX_PINS_SOURCE, COM2_TX_MUX);
	
    gpio_initstructure.gpio_pins = 0;
    gpio_initstructure.gpio_pins = COM2_RX_PIN;
    gpio_initstructure.gpio_pull = GPIO_PULL_NONE;
    gpio_init(COM2_RX_PORT, &gpio_initstructure);	
    gpio_pin_mux_config(COM2_RX_PORT, COM2_RX_PINS_SOURCE, COM2_RX_MUX);

}

static void COM2_UART_config(void)
{
    /* enable the usart and gpio clock */
    crm_periph_clock_enable(COM2_UART_CLK, TRUE);

    /* config usart nvic interrupt */
    nvic_irq_enable(COM2_UART_IRQn, 5, 0);

    /* configure usart param */
    usart_init(COM2_UART, COM2_UART_BAUD, USART_DATA_8BITS, USART_STOP_1_BIT);
    usart_transmitter_enable(COM2_UART, TRUE);
    usart_receiver_enable(COM2_UART, TRUE);

    /* enable usart interrupt */
    usart_interrupt_enable(COM2_UART, USART_RDBF_INT, TRUE);
    usart_enable(COM2_UART, TRUE);

}

__weak void DEBUG_UART_Callback(void)
{

}

void COM2_UART_IRQHandler(void)
{
    DEBUG_UART_Callback();
}
/*****************************************************************
 @Port	:		 COM3_UART: uart5
 @description :  COM3_UART ------>fpga uart
*****************************************************************/
static void COM3_UART_PortInit(void)
{	//Am016 bak use uart.
	gpio_init_type gpio_initstructure;
    gpio_default_para_init(&gpio_initstructure);

    /* configure the usart tx, rx pin */
    gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_initstructure.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
    gpio_initstructure.gpio_mode = GPIO_MODE_MUX;

	 /* configure the usart2 tx, rx pin */
 	gpio_initstructure.gpio_pins = 0;
    gpio_initstructure.gpio_pins = COM3_TX_PIN;
    gpio_initstructure.gpio_pull = GPIO_PULL_NONE;
    gpio_init(COM3_TX_PORT, &gpio_initstructure);	
	gpio_pin_mux_config(COM3_TX_PORT, COM3_TX_PINS_SOURCE, COM3_TX_MUX);
	
    gpio_initstructure.gpio_pins = 0;
    gpio_initstructure.gpio_pins = COM3_RX_PIN;
    gpio_initstructure.gpio_pull = GPIO_PULL_NONE;
    gpio_init(COM3_RX_PORT, &gpio_initstructure);	
    gpio_pin_mux_config(COM3_RX_PORT, COM3_RX_PINS_SOURCE, COM3_RX_MUX);

}

static void COM3_UART_config(void)
{
    /* enable the usart and gpio clock */
     crm_periph_clock_enable(COM3_UART_CLK, TRUE);

     /* config usart nvic interrupt */
     nvic_irq_enable(COM3_UART_IRQn, 9, 0);

     /* configure usart param */
     usart_init(COM3_UART, COM3_UART_BAUD, USART_DATA_8BITS, USART_STOP_1_BIT);
     usart_transmitter_enable(COM3_UART, TRUE);
     usart_receiver_enable(COM3_UART, TRUE);

     /* enable usart interrupt */
     usart_interrupt_enable(COM3_UART, USART_RDBF_INT, TRUE);
     usart_enable(COM3_UART, TRUE);

}

__weak void FPGA_UART_Callback(void)
{

}

void COM3_UART_IRQHandler(void)
{
    FPGA_UART_Callback(); 	
}
/*******************************************************
@Port	:		 COM4_UART: uart7
@description	: 连接到RS485通信串口上
******************************************************/
static void COM4_UART_PortInit(void)
{
    gpio_init_type gpio_initstructure;

    gpio_default_para_init(&gpio_initstructure);
	
    gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_initstructure.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
    gpio_initstructure.gpio_mode = GPIO_MODE_OUTPUT;

	
	/* configure the usart2 en pin */
 	gpio_initstructure.gpio_pins = 0;
    gpio_initstructure.gpio_pins = COM4_EN_PIN;
    gpio_initstructure.gpio_pull = GPIO_PULL_NONE;	
    gpio_init(COM4_EN_PORT, &gpio_initstructure);
		
    /* configure the usart tx, rx pin */
    gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_initstructure.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
    gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
		
 	gpio_initstructure.gpio_pins = 0;
    gpio_initstructure.gpio_pins = COM4_TX_PIN;
	gpio_initstructure.gpio_pull = GPIO_PULL_NONE;
    gpio_init(COM4_TX_PORT, &gpio_initstructure);	
	gpio_pin_mux_config(COM4_TX_PORT, COM4_TX_PINS_SOURCE, COM4_TX_MUX);
	
    gpio_initstructure.gpio_pins = 0;
    gpio_initstructure.gpio_pins = COM4_RX_PIN;
    gpio_initstructure.gpio_pull = GPIO_PULL_NONE;
    gpio_init(COM4_RX_PORT, &gpio_initstructure);	
    gpio_pin_mux_config(COM4_RX_PORT, COM4_RX_PINS_SOURCE, COM4_RX_MUX);
  
}

static void COM4_UART_config(void)
{
    /* enable the usart and gpio clock */
    crm_periph_clock_enable(COM4_UART_CLK, TRUE);

    /* config usart nvic interrupt */
    nvic_irq_enable(COM4_UART_IRQn, 10, 0);

    /* configure usart param */
    usart_init(COM4_UART, COM4_UART_BAUD, USART_DATA_8BITS, USART_STOP_1_BIT);
    usart_transmitter_enable(COM4_UART, TRUE);
    usart_receiver_enable(COM4_UART, TRUE);

    /* enable usart interrupt */
    usart_interrupt_enable(COM4_UART, USART_RDBF_INT, TRUE);
    usart_enable(COM4_UART, TRUE);
	
	/*控制485芯片进入接受模式*/
   IF_GpioOutPut(COM4_EN_PORT,COM4_EN_PIN,0,LOGIC_POUT);
}

__weak void RS485_UART_Callback(void)
{

}

void COM4_UART_IRQHandler(void)
{
    RS485_UART_Callback();
}
//***************************************************
// COM5_UART: Debug port uart1
// @description:  Debug Port 连接到SWD接插件上

//***************************************************
static void COM5_UART_PortInit(void)
{
    gpio_init_type gpio_initstructure;

    gpio_default_para_init(&gpio_initstructure);

    /* configure the usart tx, rx pin */
    gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_initstructure.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
    gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
    
   	 /* configure the usart2 tx, rx pin */
 	gpio_initstructure.gpio_pins = 0;
    gpio_initstructure.gpio_pins = COM5_TX_PIN;
    gpio_initstructure.gpio_pull = GPIO_PULL_NONE;
    gpio_init(COM5_TX_PORT, &gpio_initstructure);	
	gpio_pin_mux_config(COM5_TX_PORT, COM5_TX_PINS_SOURCE, COM5_TX_MUX);
	
    gpio_initstructure.gpio_pins = 0;
    gpio_initstructure.gpio_pins = COM5_RX_PIN;
    gpio_initstructure.gpio_pull = GPIO_PULL_NONE;
    gpio_init(COM5_RX_PORT, &gpio_initstructure);	
    gpio_pin_mux_config(COM5_RX_PORT, COM5_RX_PINS_SOURCE, COM5_RX_MUX);

}

static void COM5_UART_config(void)
{
    /* enable the usart6 and gpio clock */
    crm_periph_clock_enable(COM5_UART_CLK, TRUE);

    /* config usart nvic interrupt */
    nvic_irq_enable(COM5_UART_IRQn, 10, 0);

    /* configure usart param */
    usart_init(COM5_UART, COM5_UART_BAUD, USART_DATA_8BITS, USART_STOP_1_BIT);
    usart_transmitter_enable(COM5_UART, TRUE);
    //usart_receiver_enable(COM5_UART, TRUE);

    /* enable usart interrupt */
    usart_interrupt_enable(COM5_UART, USART_RDBF_INT, TRUE);
    usart_enable(COM5_UART, TRUE);

}

__weak void COM5_UART_Callback(void)
{

}

void COM5_UART_IRQHandler(void)
{
    COM5_UART_Callback();
}

//*****************************************************************************
//* END
//*****************************************************************************

