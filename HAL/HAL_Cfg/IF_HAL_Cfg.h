/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_HAL_Cfg.h
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */
#ifndef _IF_HAL_CFG_H_
#define _IF_HAL_CFG_H_

#include "IF_PrjLib.h"
#include "IF_Board.h"
#include "IF_IO_Gpio.h"
#include "IF_Uart.h"
#include "IF_Tmr.h"
#include "IF_Wdog.h"
#include "IF_Flash.h"

#include "IF_Uart.h"
#include "IF_At24c256.h"
#include "IF_Dac5614.h"
#include "IF_Ad7689.h"
#include "IF_DS1339.h"
#include "IF_MX25L128.h"
#include "configDevice.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

typedef enum
{
  TESTPOINT7      = 7U,
  TESTPOINT23     = 23U
} TestPoint_type;

#define EXT_ADC_VREF       		4096    //内部基准源
#define EX_AD_REF_VOLT       	5000	//外部基准源
#define EXT_ADC_VRES            0xFFFF   //2^16


#define ADC_VREF            3300
#define ADC_VRES            4096

//*****************************************************************************
//* Interface functions                                                         
//*****************************************************************************
extern void IF_HAL_CfgInit(void);

//*********************************
//SL Interface define.
//*********************************
extern uint8_t IF_HAL_EEPROM_CheckOk(void);
extern uint8_t IF_HAL_EEPROM_ReadBytes(uint16_t address, uint8_t *readBuf, uint16_t size);
extern uint8_t IF_HAL_EEPROM_WriteBytes(uint16_t address, uint8_t *writeBuf, uint16_t size);
extern void IF_HAL_Flash_read(uint32_t read_addr, uint8_t *p_buffer, uint16_t num_read);
extern error_status IF_HAL_Flash_write(uint32_t write_addr,uint16_t *p_Buffer, uint16_t num_write);

//External ADC for Interface SL
extern uint16_t IF_HAL_GetEXADCValue(uint8_t channel);

//Internal ADC for Interface SL
extern uint16_t IF_HAL_Temperature_ReadAdc(void);
extern uint16_t IF_HAL_Power_ReadAdc(void);

//External DAC for Interface SL
extern void IF_HAL_EXDAC_SET(uint16_t channel, float refValue);

//PART 4 : FPGA for SENSOR_SL
extern void IF_HAL_Sensor_MCU_Start(void);
extern void IF_HAL_FpgaReg_ReadStart(void);
extern void IF_HAL_FpgaReg_ReadEnd(void);
extern uint8_t IF_HAL_FpgaReg_Read(uint16_t regAddr);
extern void IF_HAL_FpgaReg_Write(uint16_t regAddr, uint8_t regData);

//PART 5: Input/Output Interface 
extern void IF_HAL_PCBDebugLed_Switch(uint8_t para);
extern void IF_HAL_Power12VOn_Switch(uint8_t para);
extern void IF_HAL_DC5VOnEnable_Switch(uint8_t para);
extern void IF_HAL_IntLock_Enable_Switch(uint8_t para);					
extern void IF_HAL_PwrOnOffLed_Switch(uint8_t para);	
extern void IF_HAL_FaultLed_Switch(uint8_t para);
extern void IF_HAL_WarningLed_Switch(uint8_t para);
extern void IF_HAL_InterLockLed_Switch(uint8_t para);
extern void IF_HAL_RFOnOffLed_Switch(uint8_t para);
extern void IF_HAL_Fan_PWM_Switch(uint8_t para);

extern uint8_t IF_HAL_InterLockDect_SigRead(void);
extern uint8_t IF_HAL_InterLockEnable_SigRead(void);
// UART for COMM_SL
extern UARTBAUD_TypeDef IF_HAL_GetBaudRateValue(uint8_t DataRate);
extern uint8_t IF_HAL_GetUartBaudRateKey(UARTBAUD_TypeDef DataRate);
extern void IF_HAL_UartSetBaudRate(uint8_t port, UARTBAUD_TypeDef baudrate);
extern void IF_HAL_UartEnableTx(uint8_t port);
extern void IF_HAL_UartDisableTx(uint8_t port);
extern void IF_HAL_UartSendByte(uint8_t port, uint8_t byte);
extern uint8_t IF_HAL_UartGetFlag_TDBE(uint8_t port);
extern void IF_HAL_UartClearFlag_TDBE(uint8_t port);
extern uint8_t IF_HAL_UartGetFlag_RDBF(uint8_t port);
extern void IF_HAL_UartClearFlag_RDBF(uint8_t port);
extern uint8_t IF_HAL_UartReadByte(uint8_t port);
extern void IF_HAL_SetBaudRate(uint8_t value);
//PART 6 : Wdog for All_SL
extern void IF_HAL_WDOG_FEED(void);

extern void IF_HAL_DS1339_GetTimer(struct tm *timer);
extern void IF_HAL_DS1339_SetTimer(time_t localtimer);


#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif
