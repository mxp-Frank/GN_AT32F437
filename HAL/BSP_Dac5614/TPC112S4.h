/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Tpc112S4.h--->internal use
 * @Brief :      ref from bsp_dac_tlv5614.h(AMP/AME svn324) Author: Huxian
 * @Author :     Chenghanbing (iwanthaha@sina.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */
#ifndef _TPC112S4_H_
#define _TPC112S4_H_

#include "IF_Board.h"
#include "IF_Gpio.h"


#if(config_ExDACVREF==1)
	#define EXDAC_VREF		 	 2049 	//旧版硬件兼容版
#else
	#define EXDAC_VREF           3300  //降成本版硬件
#endif
#define HIGH                 1
#define LOW                  0

#define FAST_MODE            0x1000
#define POWER_DOWN           0x2000

#if(config_DeviceFirst==1)
	#define DAC_PD_PORT				 GPIOB
	#define DAC_PD_PIN				 DAC_PD_MCU136_PB6
		
	#define DAC_LDAC_PORT			 GPIOB
	#define DAC_LDAC_PIN			 DAC_LDAC_MCU137_PB7	

	#define DAC_CS_PORT				 GPIOE
	#define DAC_CS_PIN				 DAC_CS_MCU141_PE0	
		
	#define DAC_DIN_PORT			 GPIOB
	#define DAC_DIN_PIN			 	 DAC_DIN_MCU139_PB8	

		
	#define DAC_FS_PORT				 GPIOE
	#define DAC_FS_PIN			 	 DAC_FS_MCU142_PE1	
		
	#define DAC_SCLK_PORT			 GPIOB
	#define DAC_SCLK_PIN			 DAC_SCK_MCU140_PB9
#else	
	
	#if(config_ExDAC_TLV5614 ==1)
			
		#define DAC_PD_PORT				 GPIOE
		#define DAC_PD_PIN				 DAC_PD_MCU142_PE1
			
		#define DAC_LDAC_PORT			 GPIOE
		#define DAC_LDAC_PIN			 DAC_LDAC_MCU141_PE0	

		#define DAC_CS_PORT				 GPIOD
		#define DAC_CS_PIN				 DAC_CS_MCU114_PD0	
			
		#define DAC_DIN_PORT			 GPIOB
		#define DAC_DIN_PIN			 	 DAC_DIN_MCU069_PB10	

			
		#define DAC_FS_PORT				 GPIOD
		#define DAC_FS_PIN			 	 DAC_FS_MCU115_PD1	
			
		#define DAC_SCLK_PORT			 GPIOB
		#define DAC_SCLK_PIN			 DAC_SCK_MCU070_PB11
		
	#else	  //TPC112S4  DAC

		#define DAC_LDAC_PORT			 GPIOE
		#define DAC_LDAC_PIN			 DAC_LDAC_MCU141_PE0

		#define DAC_CS_PORT				 0//GPIOD
		#define DAC_CS_PIN				 0//DAC_CS_MCU114_PD0		

		#define DAC_PD_PORT				 0//GPIOD
		#define DAC_PD_PIN				 0//DAC_CS_MCU114_PD0	

		#define DAC_DIN_PORT			 GPIOB
		#define DAC_DIN_PIN			 	 DAC_DIN_MCU069_PB10	
			
		#define DAC_FS_PORT				 GPIOD
		#define DAC_FS_PIN			 	 DAC_FS_MCU115_PD1	
			
		#define DAC_SCLK_PORT			 GPIOB
		#define DAC_SCLK_PIN			 DAC_SCK_MCU070_PB11

	#endif
#endif 
	
#define DAC_DIN_LOW  	         IF_GpioOutPut(DAC_DIN_PORT, DAC_DIN_PIN, 0, LOGIC_POUT)
#define DAC_DIN_HIGH 	         IF_GpioOutPut(DAC_DIN_PORT, DAC_DIN_PIN, 1, LOGIC_POUT)
	
#define DAC_LDAC_LOW  	         IF_GpioOutPut(DAC_LDAC_PORT, DAC_LDAC_PIN, 0, LOGIC_POUT)
#define DAC_LDAC_HIGH 	         IF_GpioOutPut(DAC_LDAC_PORT, DAC_LDAC_PIN, 1, LOGIC_POUT)

#define DAC_CS_LOW  	         IF_GpioOutPut(DAC_CS_PORT, DAC_CS_PIN, 0, LOGIC_POUT)
#define DAC_CS_HIGH 	         IF_GpioOutPut(DAC_CS_PORT, DAC_CS_PIN, 1, LOGIC_POUT)	

#define DAC_SCLK_LOW  	         IF_GpioOutPut(DAC_SCLK_PORT, DAC_SCLK_PIN, 0, LOGIC_POUT)
#define DAC_SCLK_HIGH 	         IF_GpioOutPut(DAC_SCLK_PORT, DAC_SCLK_PIN, 1, LOGIC_POUT)

#define DAC_FS_LOW  	         IF_GpioOutPut(DAC_FS_PORT, DAC_FS_PIN, 0, LOGIC_POUT)
#define DAC_FS_HIGH 	         IF_GpioOutPut(DAC_FS_PORT, DAC_FS_PIN, 1, LOGIC_POUT)

#define DAC_PD_LOW  	         IF_GpioOutPut(DAC_PD_PORT, DAC_PD_PIN, 0, LOGIC_POUT)
#define DAC_PD_HIGH 	         IF_GpioOutPut(DAC_PD_PORT, DAC_PD_PIN, 1, LOGIC_POUT)
#endif
//*****************************************************************************
//* END
//*****************************************************************************

