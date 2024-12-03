/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  HAL_Cfg.c
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#include "IF_HAL_Cfg.h"
#include "HAL_Cfg.h"

/* CONST & MACROS */

/* DATA STRUCTURES */

/* LOCAL VARIABLES */

/* GLOBAL VARIABLES */

/* FUNCTION PROTOTYPES */


//*************************************************************************
//* Global Functions Definitions                                          
//*************************************************************************

/**
 * ************************************************************************
 * @Brief :    IF_HAL_CfgPwrOnInit
 * Hal configration when power up...
 * ************************************************************************
 */

void IF_HAL_CfgInit(void)
{
    // Initialize and configure clocks, power management
     IF_BoardInit();

    // Initialize MCU Peripheral
     IF_TmrInit();
     IF_GpioInit();
     IF_AdcInit();
	 IF_FlashInit();
   
     IF_WdogInit();

    // Initialize BSP Peripheral
     IF_FpgaUnitInit();
	 IF_At24c256Init();
	 IF_Dac5614Init();
	 IF_AD7689Init();
	 IF_IOGpioInit();
	 IF_UartInit();
	 IF_DS1339Init();	
	 IF_SpiflashInit();  
	
}

//*********************************
//SL Interface define. TBD...
//*********************************

//PART 1 : EEPROM for NVM_SL
uint8_t IF_HAL_EEPROM_CheckOk(void)
{
    return IF_At24c256_CheckOk();
}


uint8_t IF_HAL_EEPROM_ReadBytes(uint16_t address, uint8_t *readBuf, uint16_t size)
{
    return IF_At24c256_ReadBytes(address, readBuf, size);
}

uint8_t IF_HAL_EEPROM_WriteBytes(uint16_t address, uint8_t *writeBuf, uint16_t size)
{
    return IF_At24c256_WriteBytes(address, writeBuf, size);
}

void IF_HAL_Flash_read(uint32_t read_addr, uint8_t *p_buffer, uint16_t num_read)
{
    IF_Flash_ReadByte(read_addr, p_buffer, num_read);
}

error_status IF_HAL_Flash_write(uint32_t write_addr,uint16_t *p_Buffer, uint16_t num_write)
{
    return IF_Flash_write(write_addr, p_Buffer, num_write);
}

/********************模拟接口端口设置***********************************/
void IF_HAL_PCBDebugLed_Switch(uint8_t para)
{
	IF_PCBDebugLed_Switch(para);
}

void IF_HAL_Power12VOn_Switch(uint8_t para)
{
	IF_Power12VOn_Switch(para);
}
void IF_HAL_DC5VOnEnable_Switch(uint8_t para)
{
	IF_DC5VOn_Switch(para);
}
void IF_HAL_IntLock_Enable_Switch(uint8_t para)
{
	IF_IntLock_En_Switch(para);
}
void IF_HAL_PwrOnOffLed_Switch(uint8_t para)
{
	IF_PowerOnOffLed_Switch(para);
}
void IF_HAL_FaultLed_Switch(uint8_t para)
{
	IF_FaultLed_Switch(para);
}
void IF_HAL_WarningLed_Switch(uint8_t para)
{
	IF_WarningLed_Switch(para);
}
void IF_HAL_InterLockLed_Switch(uint8_t para)
{
	IF_IntLockLed_Switch(para);
}
void IF_HAL_RFOnOffLed_Switch(uint8_t para)
{
	IF_RFOnOffLed_Switch(para);
}
uint8_t IF_HAL_InterLockDect_SigRead(void)
{
	return IF_InterLockDect_SigRead();
}


//PART 3 : DAC and Encoder 
void IF_HAL_EXDAC_SET(uint16_t channel, float refVolt)
{
   uint16_t codeValue = (uint16_t)(refVolt * 4096/(2*EXDAC_VREF));
   IF_Dac5614WriteDAC(channel, codeValue);
}

//PART 4 : FPGA for SENSOR_SL
void IF_HAL_Sensor_MCU_Start(void)
{
    IF_AdcStart();
}

uint8_t IF_HAL_FpgaReg_ReadStart(void)
{
	return IF_FpgaReg_ReadStart();
}

void IF_HAL_FpgaReg_ReadEnd(void)
{
	IF_FpgaReg_ReadEnd();
}

uint8_t IF_HAL_FpgaReg_Read(uint16_t regAddr)
{
	return  IF_FpgaReg_Read(regAddr);
}
void IF_HAL_FpgaReg_Write(uint16_t regAddr,uint8_t value)
{
	IF_FpgaReg_Write(regAddr, value);
}
uint16_t IF_HAL_GetEXADCValue(uint8_t channel)
{
	return  IF_SPI_ReadADCValue(channel);
}
/***************Internal ADC  HAL **************/
uint16_t IF_HAL_Temperature_ReadAdc(void)
{
	return  IF_AdcResult_Read(ADCn1, ADC_CHANNEL_6);
}
uint16_t IF_HAL_Power_ReadAdc(void)
{
	return  IF_AdcResult_Read(ADCn1, ADC_CHANNEL_7);
}

//PART 5 : UART for COMM_SL
UARTBAUD_TypeDef IF_HAL_GetBaudRateValue(uint8_t DataRate)
{
	return IF_Uart_GetBaudRateValue(DataRate);
}
uint8_t IF_HAL_GetUartBaudRateKey(UARTBAUD_TypeDef DataRate)
{
	return  IF_Uart_GetBaudRateKey(DataRate);
}

void IF_HAL_UartSetBaudRate(uint8_t port, UARTBAUD_TypeDef baudrate)
{
    IF_UartSetBaudRate(port, baudrate);
}

void IF_HAL_UartEnableTx(uint8_t port)
{
    IF_UartEnableTx(port);
}

void IF_HAL_UartDisableTx(uint8_t port)
{
    IF_UartDisableTx(port);
}

void IF_HAL_UartSendByte(uint8_t port, uint8_t byte)
{
    IF_UartSendByte(port, byte);
}

uint8_t IF_HAL_UartGetFlag_TDBE(uint8_t port)
{
	return IF_UartGetFlag(port, USART_TDBE_FLAG);
}
void IF_HAL_UartClearFlag_TDBE(uint8_t port)
{
	IF_UartClearFlag(port,USART_TDBE_FLAG);	
}
uint8_t IF_HAL_UartGetFlag_RDBF(uint8_t port)
{
	return IF_UartGetFlag(port, USART_RDBF_FLAG);
}
void IF_HAL_UartClearFlag_RDBF(uint8_t port)
{
	IF_UartClearFlag(port,USART_RDBF_FLAG);	
}


uint8_t IF_HAL_UartReadByte(uint8_t port)
{
	return  IF_UartReadByte(port); 
}

void IF_HAL_SetBaudRate(uint8_t value)
{
	UARTBAUD_TypeDef  baudRateValue = IF_Uart_GetBaudRateValue(value);
	IF_UartSetBaudRate(COM1,  baudRateValue);  //User Interface
	IF_UartSetBaudRate(COM2,  baudRateValue);  //Debug Interface
	IF_UartSetBaudRate(COM3,  BAUD115200);	   //Fpga Interface
}


//PART 6 : Wdog for All_SL
void IF_HAL_WDOG_FEED(void)
{
    IF_WdogFeed();
}

void IF_HAL_DS1339_GetTimer(struct tm *timer)
{
	IF_DS1339_GetTimer(timer);
}
void IF_HAL_DS1339_SetTimer(time_t localtime)
{
	IF_DS1339_SetTimer(localtime);
}


/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/



//*****************************************************************************
//* END
//*****************************************************************************

