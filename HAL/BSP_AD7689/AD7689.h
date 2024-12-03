/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Ltc1865.h
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-19
 * ****************************************************************************
 */

#ifndef _AD7689_H_
#define _AD7689_H_

#include "IF_Board.h"
#include "IF_Gpio.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
/***********************************************************************
CFG   :配置更新
		0-->保持当前的配置  
		1-->覆盖寄存器的内容 
***************************************************************************/
/****************************************************************************
INCC   BIT12    BIT11    BIT10
		 0		 0         X          双极性差分对：   INx-以Vref/2+-0.1为参考
		 0		 1         0          双极性：   	   INx-以COM=Vref/2+-0.1为参考
		 0		 1         1          温度传感器：   	   
		 1		 0         x          单极性差分对：   INx-以GND+-0.1为参考
		 1		 1         0          单极性：   	   INx-以COM=GND+-0.1为参考
		 1		 1         0          单极性：   	   INx-以GND为参考
		 
**********************************************************************/
/**********************************************************************
INx   BIT9    	BIT8    BIT7
		 0		 0         0          IN0
		 0		 0         1          IN1
		 0		 1         0          IN2
		...		...		  ...		  ...
		 1		 1         1          IN7		 
**********************************************************************/
/**********************************************************************
BW   	选择低通滤波器的带宽
		0=1/4带宽，最高速率也降至1/4
		1=全带宽
**********************************************************************/
/**********************************************************************
REF   	基准电源源
		BIT5     BIT4    BIT3
		 0		 0         0          内部基准源  REF=2.5V输出
		 0		 0         1          内部基准源  REF=4.096V输出
		 0		 1         0          外部基准源  温度传感器输出
		 0		 1		   1		  外部基准源  内部缓冲器，温度传感器使能
		 1		 1         0          外部基准源  温度传感器禁用
		 1		 1         1          外部基准源  内部缓冲器，温度传感器禁用		 
**********************************************************************/
/**********************************************************************
SEQ   	通道序列器。允许以IN0到IN[7：0]的方式扫描通道
		BIT2     BIT1    
		 0		 0                   禁用序列器
		 0		 1                   序列扫描期间更新配置
		 1		 0                   扫描IN0到IN[7：0](由CFG[9:7]设置)，然后扫描温度
		 1		 1		   		  	 扫描IN0到IN[7：0](由CFG[9:7]设置)	 
**********************************************************************/
/***********************************************************************
RB   :	回读CFG寄存器
		0-->数据结束时回读当前的配置  
		1-->不回读配置内容 
***************************************************************************/
#define AD7689_RB				1  //不回读CFG寄存器配置内容
#define AD7689_SEQ				0  //禁止序列器    
#define AD7689_REF				1  //内部基准电压源 ref =4.096V
#define AD7689_BW				1  //全带宽低通滤波器
#define AD7689_INCC				7  //单极性，以GND为参考   
#define AD7689_CFG				1  //覆盖寄存器内容 

#define AD7689_CNV_ADC_GPIO		GPIOE
#define AD7689_CNV_ADC_PIN		CNV_ADC_Pin3_PE4

#define AD7689_SCK_ADC_GPIO		GPIOE
#define AD7689_SCK_ADC_PIN		SCK_ADC_Pin1_PE2

#define AD7689_DIN_ADC_GPIO		GPIOE
#define AD7689_DIN_ADC_PIN		DIN_ADC_Pin5_PE6

#define AD7689_SDO_ADC_GPIO		GPIOE
#define AD7689_SDO_ADC_PIN		SDO_ADC_Pin4_PE5	

#define AD7689_CNV_LOW         	IF_GpioOutPut(AD7689_CNV_ADC_GPIO, AD7689_CNV_ADC_PIN, 0, LOGIC_POUT)
#define AD7689_CNV_HIGH        	IF_GpioOutPut(AD7689_CNV_ADC_GPIO, AD7689_CNV_ADC_PIN, 1, LOGIC_POUT)

#define AD7689_SCK_LOW 	     	IF_GpioOutPut(AD7689_SCK_ADC_GPIO, AD7689_SCK_ADC_PIN, 0, LOGIC_POUT)
#define AD7689_SCK_HIGH 	    IF_GpioOutPut(AD7689_SCK_ADC_GPIO, AD7689_SCK_ADC_PIN, 1, LOGIC_POUT)

#define AD7689_DIN_LOW 	     	IF_GpioOutPut(AD7689_DIN_ADC_GPIO, AD7689_DIN_ADC_PIN, 0, LOGIC_POUT)
#define AD7689_DIN_HIGH 	    IF_GpioOutPut(AD7689_DIN_ADC_GPIO, AD7689_DIN_ADC_PIN, 1, LOGIC_POUT)

#define AD7689_SDO_VALUE    	IF_GpioInPut(AD7689_SDO_ADC_GPIO,  AD7689_SDO_ADC_PIN, ACTIVE_HIGH)
#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif


//*****************************************************************************
//* END
//*****************************************************************************

