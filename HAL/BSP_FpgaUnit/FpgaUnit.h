/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  FpgaUnit.h--->internal use
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#ifndef _FPGAUNIT_H_
#define _FPGAUNIT_H_

#include "IF_Board.h"
#include "IF_Gpio.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
#define DATA_INPUT          	0
#define DATA_OUTPUT         	1

 //reg addr
 #define DATA_REG00_ADDR      0x00
 #define DATA_REG01_ADDR      0x01
 #define DATA_REG02_ADDR      0x02
 #define DATA_REG03_ADDR      0x03
 #define DATA_REG04_ADDR      0x04
 #define DATA_REG05_ADDR      0x05
 #define DATA_REG06_ADDR      0x06
 #define DATA_REG07_ADDR      0x07

#define DATA_RE_PORT			GPIOB
#define DATA_RE_PIN				DATA_RE_Pin73_PB12

#define DATA_WE_PORT			GPIOB
#define DATA_WE_PIN				DATA_WE_Pin74_PB13

#define DATA_CLK_PORT			GPIOB
#define DATA_CLK_PIN			DATA_CLK_Pin75_PB14

#define DATA_START_PORT			GPIOC
#define DATA_START_PIN			FPGA_IO1_Pin113_PC12

#define DATA_READ_PORT			GPIOA
#define DATA_READ_PIN			FPGA_IO2_Pin110_PA15

#define DATA_RESET_PORT			GPIOH
#define DATA_RESET_PIN			FPGA_IO3_Pin106_PH2

#define DATA_A0_PORT			GPIOC
#define DATA_A0_PIN				DATA_A0_Pin26_PC0

#define DATA_A1_PORT			GPIOC
#define DATA_A1_PIN				DATA_A1_Pin27_PC1

#define DATA_A2_PORT			GPIOC
#define DATA_A2_PIN				DATA_A2_Pin28_PC2

#define DATA_A3_PORT			GPIOC
#define DATA_A3_PIN				DATA_A3_Pin29_PC3

#define DATA_A4_PORT			GPIOA
#define DATA_A4_PIN				DATA_A4_Pin34_PA0

#define DATA_A5_PORT			GPIOA
#define DATA_A5_PIN				DATA_A5_Pin35_PA1

#define DATA_A6_PORT			GPIOA
#define DATA_A6_PIN				DATA_A6_Pin36_PA2

#define DATA_A7_PORT			GPIOA
#define DATA_A7_PIN				DATA_A7_Pin37_PA3

#define DATA_A8_PORT			GPIOD
#define DATA_A8_PIN				DATA_A8_Pin114_PD0

#define DATA_D0_PORT 			GPIOE
#define DATA_D0_PIN				DATA_D0_Pin68_PE15

#define DATA_D1_PORT 			GPIOE
#define DATA_D1_PIN				DATA_D1_Pin67_PE14

#define DATA_D2_PORT 			GPIOE
#define DATA_D2_PIN				DATA_D2_Pin66_PE13

#define DATA_D3_PORT 			GPIOE
#define DATA_D3_PIN				DATA_D3_Pin65_PE12

#define DATA_D4_PORT 			GPIOE
#define DATA_D4_PIN				DATA_D4_Pin64_PE11

#define DATA_D5_PORT 			GPIOE
#define DATA_D5_PIN				DATA_D5_Pin63_PE10

#define DATA_D6_PORT 			GPIOE
#define DATA_D6_PIN				DATA_D6_Pin60_PE9

#define DATA_D7_PORT 			GPIOE
#define DATA_D7_PIN				DATA_D7_Pin59_PE8
 //control signal
 #define DATA_RE_LOW          	IF_GpioOutPut(DATA_RE_PORT, DATA_RE_PIN, 0, LOGIC_POUT)
 #define DATA_RE_HIGH         	IF_GpioOutPut(DATA_RE_PORT, DATA_RE_PIN, 1, LOGIC_POUT)

 #define DATA_WE_LOW   	      	IF_GpioOutPut(DATA_WE_PORT, DATA_WE_PIN, 0, LOGIC_POUT)
 #define DATA_WE_HIGH 	      	IF_GpioOutPut(DATA_WE_PORT, DATA_WE_PIN, 1, LOGIC_POUT)

 #define DATA_CLK_LOW   	  	IF_GpioOutPut(DATA_CLK_PORT, DATA_CLK_PIN, 0, LOGIC_POUT)
 #define DATA_CLK_HIGH 	      	IF_GpioOutPut(DATA_CLK_PORT, DATA_CLK_PIN, 1, LOGIC_POUT)

 #define DATA_START_LOW       	IF_GpioOutPut(DATA_START_PORT, DATA_START_PIN, 0, LOGIC_POUT)
 #define DATA_START_HIGH      	IF_GpioOutPut(DATA_START_PORT, DATA_START_PIN, 1, LOGIC_POUT)

 #define FPGA_RESET_LOW       	IF_GpioOutPut(DATA_RESET_PORT, DATA_RESET_PIN, 0, LOGIC_POUT)
 #define FPGA_RESET_HIGH      	IF_GpioOutPut(DATA_RESET_PORT, DATA_RESET_PIN, 1, LOGIC_POUT)

 //data(input) signal
 #define DATA_FPGA_READ   	 	IF_GpioInPut(DATA_READ_PORT, DATA_READ_PIN, ACTIVE_HIGH)

 //addr signal
 #define DATA_A0_LOW   	      	IF_GpioOutPut(DATA_A0_PORT, DATA_A0_PIN, 0, LOGIC_POUT)
 #define DATA_A0_HIGH 	      	IF_GpioOutPut(DATA_A0_PORT, DATA_A0_PIN, 1, LOGIC_POUT)

 #define DATA_A1_LOW   	      	IF_GpioOutPut(DATA_A1_PORT, DATA_A1_PIN, 0, LOGIC_POUT)
 #define DATA_A1_HIGH 	      	IF_GpioOutPut(DATA_A1_PORT, DATA_A1_PIN, 1, LOGIC_POUT)

 #define DATA_A2_LOW   	      	IF_GpioOutPut(DATA_A2_PORT, DATA_A2_PIN, 0, LOGIC_POUT)
 #define DATA_A2_HIGH 	     	IF_GpioOutPut(DATA_A2_PORT, DATA_A2_PIN, 1, LOGIC_POUT)

 #define DATA_A3_LOW   	     	IF_GpioOutPut(DATA_A3_PORT, DATA_A3_PIN, 0, LOGIC_POUT)
 #define DATA_A3_HIGH 	     	IF_GpioOutPut(DATA_A3_PORT, DATA_A3_PIN, 1, LOGIC_POUT)

 #define DATA_A4_LOW   	     	IF_GpioOutPut(DATA_A4_PORT, DATA_A4_PIN, 0, LOGIC_POUT)
 #define DATA_A4_HIGH 	     	IF_GpioOutPut(DATA_A4_PORT, DATA_A4_PIN, 1, LOGIC_POUT)

 #define DATA_A5_LOW   	     	IF_GpioOutPut(DATA_A5_PORT, DATA_A5_PIN, 0, LOGIC_POUT)
 #define DATA_A5_HIGH 	     	IF_GpioOutPut(DATA_A5_PORT, DATA_A5_PIN, 1, LOGIC_POUT)

 #define DATA_A6_LOW   	     	IF_GpioOutPut(DATA_A6_PORT, DATA_A6_PIN, 0, LOGIC_POUT)
 #define DATA_A6_HIGH 	     	IF_GpioOutPut(DATA_A6_PORT, DATA_A6_PIN, 1, LOGIC_POUT)

 #define DATA_A7_LOW   	     	IF_GpioOutPut(DATA_A7_PORT, DATA_A7_PIN, 0, LOGIC_POUT)
 #define DATA_A7_HIGH 	     	IF_GpioOutPut(DATA_A7_PORT, DATA_A7_PIN, 1, LOGIC_POUT)

 #define DATA_A8_LOW   	     	IF_GpioOutPut(DATA_A8_PORT, DATA_A8_PIN, 0, LOGIC_POUT)
 #define DATA_A8_HIGH 	     	IF_GpioOutPut(DATA_A8_PORT, DATA_A8_PIN, 1, LOGIC_POUT)
 
 //data(output) signal
 #define DATA_D0_LOW   	     	IF_GpioOutPut(DATA_D0_PORT, DATA_D0_PIN, 0, LOGIC_POUT)
 #define DATA_D0_HIGH 	     	IF_GpioOutPut(DATA_D0_PORT, DATA_D0_PIN, 1, LOGIC_POUT)

 #define DATA_D1_LOW   	     	IF_GpioOutPut(DATA_D1_PORT, DATA_D1_PIN, 0, LOGIC_POUT)
 #define DATA_D1_HIGH 	     	IF_GpioOutPut(DATA_D1_PORT, DATA_D1_PIN, 1, LOGIC_POUT)

 #define DATA_D2_LOW   	     	IF_GpioOutPut(DATA_D2_PORT, DATA_D2_PIN, 0, LOGIC_POUT)
 #define DATA_D2_HIGH 	     	IF_GpioOutPut(DATA_D2_PORT, DATA_D2_PIN, 1, LOGIC_POUT)

 #define DATA_D3_LOW   	     	IF_GpioOutPut(DATA_D3_PORT, DATA_D3_PIN, 0, LOGIC_POUT)
 #define DATA_D3_HIGH 	     	IF_GpioOutPut(DATA_D3_PORT, DATA_D3_PIN, 1, LOGIC_POUT)

 #define DATA_D4_LOW   	     	IF_GpioOutPut(DATA_D4_PORT, DATA_D4_PIN, 0, LOGIC_POUT)
 #define DATA_D4_HIGH 	     	IF_GpioOutPut(DATA_D4_PORT, DATA_D4_PIN, 1, LOGIC_POUT)

 #define DATA_D5_LOW   	     	IF_GpioOutPut(DATA_D5_PORT, DATA_D5_PIN, 0, LOGIC_POUT)
 #define DATA_D5_HIGH 	     	IF_GpioOutPut(DATA_D5_PORT, DATA_D5_PIN, 1, LOGIC_POUT)

 #define DATA_D6_LOW   	     	IF_GpioOutPut(DATA_D6_PORT, DATA_D6_PIN, 0, LOGIC_POUT)
 #define DATA_D6_HIGH 	     	IF_GpioOutPut(DATA_D6_PORT, DATA_D6_PIN, 1, LOGIC_POUT)

 #define DATA_D7_LOW   	     	IF_GpioOutPut(DATA_D7_PORT, DATA_D7_PIN, 0, LOGIC_POUT)
 #define DATA_D7_HIGH 	     	IF_GpioOutPut(DATA_D7_PORT, DATA_D7_PIN, 1, LOGIC_POUT)


 #define DATA_D0_READ   	     IF_GpioInPut(DATA_D0_PORT, DATA_D0_PIN, ACTIVE_HIGH)
 #define DATA_D1_READ   	     IF_GpioInPut(DATA_D1_PORT, DATA_D1_PIN, ACTIVE_HIGH)
 #define DATA_D2_READ   	     IF_GpioInPut(DATA_D2_PORT, DATA_D2_PIN, ACTIVE_HIGH)
 #define DATA_D3_READ   	     IF_GpioInPut(DATA_D3_PORT, DATA_D3_PIN, ACTIVE_HIGH)
 #define DATA_D4_READ   	     IF_GpioInPut(DATA_D4_PORT, DATA_D4_PIN, ACTIVE_HIGH)
 #define DATA_D5_READ   	     IF_GpioInPut(DATA_D5_PORT, DATA_D5_PIN, ACTIVE_HIGH)
 #define DATA_D6_READ   	     IF_GpioInPut(DATA_D6_PORT, DATA_D6_PIN, ACTIVE_HIGH)
 #define DATA_D7_READ   	     IF_GpioInPut(DATA_D7_PORT, DATA_D7_PIN, ACTIVE_HIGH)

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif


//*****************************************************************************
//* END
//*****************************************************************************

