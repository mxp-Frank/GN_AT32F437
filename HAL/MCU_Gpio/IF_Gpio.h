/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_Gpio.h--->external use
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#ifndef _IF_GPIO_H_
#define _IF_GPIO_H_

#include "IF_PrjLib.h"
#include "IF_Board.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#define    LOGIC_NOUT 0
#define    LOGIC_POUT 1
#define    ACTIVE_LOW 0
#define    ACTIVE_HIGH 1

//check pass！20230220

/*Exit ADC Pin*/
	
#define SCK_ADC_Pin1_PE2          	    GPIO_PINS_2
#define TBD_Pin2_PE3           		    GPIO_PINS_3		//待定义
#define CNV_ADC_Pin3_PE4			    GPIO_PINS_4	      
#define SDO_ADC_Pin4_PE5    		    GPIO_PINS_5
#define DIN_ADC_Pin5_PE6			    GPIO_PINS_6
//VBAT
#define TBD_Pin7_PC13					GPIO_PINS_13
#define TBD_Pin8_PC14					GPIO_PINS_14
#define TBD_Pin9_PC15					GPIO_PINS_15
#define IIC2_SDA_Pin10_PF0				GPIO_PINS_0
#define IIC2_SCL_Pin11_PF1				GPIO_PINS_1
#define TBD_Pin12_PF2					GPIO_PINS_2
#define TBD_Pin13_PF3					GPIO_PINS_3
#define TBD_Pin14_PF4					GPIO_PINS_4
#define TBD_Pin15_PF5					GPIO_PINS_5
//Pin16-----VSS1
//Pin17-----VDD1
#define UART7_RX_Pin18_PF6				GPIO_PINS_6
#define UART7_TX_Pin19_PF7				GPIO_PINS_7
#define TBD_Pin20_PF8					GPIO_PINS_8
#define TBD_Pin21_PF9					GPIO_PINS_9
#define TBD_Pin22_PF10					GPIO_PINS_10
//Pin23------HEXT_IN
//Pin24------HEXT_OUT
//Pin25------NRST
#define DATA_A0_Pin26_PC0				GPIO_PINS_0
#define DATA_A1_Pin27_PC1				GPIO_PINS_1
#define DATA_A2_Pin28_PC2				GPIO_PINS_2
#define DATA_A3_Pin29_PC3				GPIO_PINS_3
//Pin30------NC1
//Pin31------VSSA
//Pin32------VREF+
//Pin33------VDDA1
#define DATA_A4_Pin34_PA0				GPIO_PINS_0
#define DATA_A5_Pin35_PA1				GPIO_PINS_1
#define DATA_A6_Pin36_PA2				GPIO_PINS_2
#define DATA_A7_Pin37_PA3				GPIO_PINS_3
//Pin38------VSS2
//Pin39------VDD2
#define TBD_Pin40_PA4					GPIO_PINS_4
#define TBD_Pin41_PA5					GPIO_PINS_5
#define TEMP_ADC_Pin42_PA6				GPIO_PINS_6		//ADC12_IN6
#define POWER_ADC_Pin43_PA7				GPIO_PINS_7		//ADC12_IN7
#define TBD_Pin44_PC4					GPIO_PINS_4		//ADC12_IN14
#define TBD_Pin45_PC5					GPIO_PINS_5		//ADC12_IN15
#define TBD_Pin46_PB0					GPIO_PINS_0		//ADC12_IN8
#define TBD_Pin47_PB1					GPIO_PINS_1		//ADC12_IN9
//Pin48------BOOT1
#define TBD_Pin49_PF11					GPIO_PINS_11
#define TBD_Pin50_PF12					GPIO_PINS_12	
//Pin51-------VSS
//Pin52-------VDD
#define WLD_NC_Pin53_PF13				GPIO_PINS_13
#define WLD_NO_Pin54_PF14				GPIO_PINS_14
#define DC5V_ON_Pin55_PF15				GPIO_PINS_15		//TMR20_CH4
#define TBD_Pin56_PG0					GPIO_PINS_0
#define TBD_Pin57_PG1					GPIO_PINS_1
#define TBD_Pin58_PE7					GPIO_PINS_7
#define DATA_D7_Pin59_PE8				GPIO_PINS_8
#define DATA_D6_Pin60_PE9				GPIO_PINS_9
//Pin61-----VSS
//Pin62-----VDD
#define DATA_D5_Pin63_PE10				GPIO_PINS_10
#define DATA_D4_Pin64_PE11				GPIO_PINS_11
#define DATA_D3_Pin65_PE12				GPIO_PINS_12
#define DATA_D2_Pin66_PE13				GPIO_PINS_13
#define DATA_D1_Pin67_PE14				GPIO_PINS_14
#define DATA_D0_Pin68_PE15				GPIO_PINS_15
#define TBD_Pin69_PB10					GPIO_PINS_10	
#define TBD_Pin70_PB11					GPIO_PINS_11	
#define PW12V_ON_Pin71_PH3				GPIO_PINS_3
//Pin72------VDD
#define DATA_RE_Pin73_PB12				GPIO_PINS_12
#define DATA_WE_Pin74_PB13				GPIO_PINS_13
#define	DATA_CLK_Pin75_PB14				GPIO_PINS_14
#define LED_Pin76_PB15					GPIO_PINS_15
#define UART3_TX_Pin77_PD8				GPIO_PINS_8
#define UART3_RX_Pin78_PD9				GPIO_PINS_9
#define TBD_Pin79_PD10					GPIO_PINS_10
#define WDI_Pin80_PD11					GPIO_PINS_11
#define TBD_Pin81_PD12					GPIO_PINS_12	//TMR4_CH1
#define RS485_EN_Pin82_PD13				GPIO_PINS_13	//TMR4_CH2
//Pin83-----VSS
//Pin84-----VDD
#define IIC3_SCL_Pin85_PD14				GPIO_PINS_14
#define IIC3_SDA_Pin86_PD15				GPIO_PINS_15
#define FPGA_IO4_Pin87_PG2				GPIO_PINS_2
#define IntLock_Detect_Pin88_PG3		GPIO_PINS_3
#define Fault_LED_Pin89_PG4				GPIO_PINS_4
#define Warning_LED_Pin90_PG5			GPIO_PINS_5
#define IntLock_LED_Pin91_PG6			GPIO_PINS_6
#define RF_ON_LED_Pin92_PG7				GPIO_PINS_7
#define PWR_ON_LED_Pin93_PG8		   GPIO_PINS_8
//Pin94-----VSS
//Pin95-----VDD
#define Fan4_FG_RS_Pin96_PC6			GPIO_PINS_6		//TMR3_CH1
#define Fan3_FG_RS_Pin97_PC7			GPIO_PINS_7		//TMR3_CH2
#define Fan2_FG_RS_Pin98_PC8			GPIO_PINS_8		//TMR3_CH3
#define Fan1_FG_RS_Pin99_PC9			GPIO_PINS_9		//TMR8_CH4
#define Fan_PWM_Pin100_PA8				GPIO_PINS_8		//TMR1_CH1
#define UART1_TX_Pin101_PA9				GPIO_PINS_9
#define UART1_RX_Pin102_PA10			GPIO_PINS_10
#define TBD_Pin103_PA11					GPIO_PINS_11
#define TBD_Pin104_PA12					GPIO_PINS_12
//Pin105------SWDIO
#define FPGA_IO3_Pin106_PH2				GPIO_PINS_2
//Pin107-----VSS
//Pin108-----VDD
//Pin109------SWCLK
#define FPGA_IO2_Pin110_PA15			GPIO_PINS_15
#define UART4_TX_Pin111_PC10			GPIO_PINS_10	//TMR5_CH2
#define UART4_RX_Pin112_PC11			GPIO_PINS_11	//TMR5_CH3
#define FPGA_IO1_Pin113_PC12			GPIO_PINS_12
#define DATA_A8_Pin114_PD0				GPIO_PINS_0
#define TBD_Pin115_PD1					GPIO_PINS_1
#define IntLock_EN_Pin116_PD2			GPIO_PINS_2
#define Flash_Reset_Pin117_PD3			GPIO_PINS_3			//
#define TBD_Pin118_PD4					GPIO_PINS_4
#define UART2_TX_Pin119_PD5				GPIO_PINS_5			//RS232
//Pin120-----VSS
//Pin121-----VDD
#define UART2_RX_Pin122_PD6				GPIO_PINS_6			//RS232
#define TBD_Pin123_PD7					GPIO_PINS_7
#define TBD_Pin124_PG9					GPIO_PINS_9			
#define Flash_WP_Pin125_PG10			GPIO_PINS_10		//SPI FLASH
#define SPI4_SCK_Pin126_PG11			GPIO_PINS_11		//SPI FLASH
#define SPI4_MISO_Pin127_PG12			GPIO_PINS_12		//SPI FLASH
#define SPI4_MOSI_Pin128_PG13			GPIO_PINS_13		//SPI FLASH
#define SPI4_CS_Pin129_PG14				GPIO_PINS_14		//SPI FLASH
//Pin130------VSS
//Pin131------VDD
#define TBD_Pin132_PG15					GPIO_PINS_15
//Pin133-------SWO
#define TBD_Pin134_PB4					GPIO_PINS_4
#define UART5_RX_Pin135_PB5				GPIO_PINS_5
#define UART5_TX_Pin136_PB6				GPIO_PINS_6
#define DAC_LDAC_Pin137_PB7				GPIO_PINS_7
//Pin138------BOOT0
#define DAC_DIN_Pin139_PB8				GPIO_PINS_8
#define DAC_SCK_Pin140_PB9				GPIO_PINS_9
#define TBD_Pin141_PE0					GPIO_PINS_0
#define DAC_FS_Pin142_PE1				GPIO_PINS_1
//Pin143--------VSS
//Pin144--------VDD


/******************************************************************************
 * Interface functions                                                         *
 ******************************************************************************/
extern void IF_GpioInit(void);
extern void IF_OutPutGpio_Write(gpio_type *gpio_x, uint16_t pins, uint8_t outPutValue, uint8_t outLogic);
extern flag_status IF_OutPutGpio_Read(gpio_type *gpio_x, uint16_t pins, uint8_t inLogic);
extern flag_status IF_InPutGpio_Read(gpio_type *gpio_x, uint16_t pins, uint8_t inLogic);
extern void IF_GpioToggle(gpio_type *gpio_x, uint16_t gpio_pin);

#if defined(__cplusplus)
}
#endif /* __cplusplus */	
#endif

/*
*******************************************************************************
* END
*******************************************************************************
*/

