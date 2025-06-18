/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  FpgaUnit.c
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */
//Am016 not used.
#include "IF_FpgaUnit.h"
#include "IF_Tmr.h"
#include "FpgaUnit.h"

/* CONST & MACROS */

/* DATA STRUCTURES */

/* LOCAL VARIABLES */

/* GLOBAL VARIABLES */

/* FUNCTION PROTOTYPES */
static void FpgaAddr_PortInit(void);
static void FpgaCtrl_PortInit(void);
static void FpgaData_PortInit(uint8_t para);//daul direction
static void FpgaAddr_Write(uint16_t regAddr);
static void FpgaUnit_Reset(void);

/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/

/**
 * ************************************************************************
 * @Brief :      
 * ************************************************************************
 */
void IF_FpgaUnitInit(void)
{
    FpgaAddr_PortInit();
    FpgaCtrl_PortInit();
    FpgaData_PortInit(DATA_OUTPUT); 
	FpgaUnit_Reset();
}

/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/
static void FpgaAddr_PortInit(void)
{
     gpio_init_type gpio_init_struct;

     /* set default parameter */
     gpio_default_para_init(&gpio_init_struct);

     /* configure the normal MCU OUTPUT gpio */
     gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
     gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
     gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
     gpio_init_struct.gpio_pull = GPIO_PULL_NONE;

     //FPGA reg Addr
	 gpio_init_struct.gpio_pins = 0;
     gpio_init_struct.gpio_pins = DATA_A0_PIN;
     gpio_init(DATA_A0_PORT, &gpio_init_struct);

	 gpio_init_struct.gpio_pins = 0;
     gpio_init_struct.gpio_pins = DATA_A1_PIN;
     gpio_init(DATA_A1_PORT, &gpio_init_struct);


	 gpio_init_struct.gpio_pins = 0;
     gpio_init_struct.gpio_pins = DATA_A2_PIN;
     gpio_init(DATA_A2_PORT, &gpio_init_struct);
	 
	 gpio_init_struct.gpio_pins = 0;
     gpio_init_struct.gpio_pins = DATA_A3_PIN;
     gpio_init(DATA_A3_PORT, &gpio_init_struct);
	 
	 gpio_init_struct.gpio_pins = 0;
     gpio_init_struct.gpio_pins = DATA_A4_PIN;
     gpio_init(DATA_A4_PORT, &gpio_init_struct);
	 
	 gpio_init_struct.gpio_pins = 0;
     gpio_init_struct.gpio_pins = DATA_A5_PIN;
     gpio_init(DATA_A5_PORT, &gpio_init_struct);
	 
     gpio_init_struct.gpio_pins = 0;
     gpio_init_struct.gpio_pins = DATA_A6_PIN;
     gpio_init(DATA_A6_PORT, &gpio_init_struct);
	 
     gpio_init_struct.gpio_pins = 0;
     gpio_init_struct.gpio_pins = DATA_A7_PIN;
     gpio_init(DATA_A7_PORT, &gpio_init_struct);
	 
	 gpio_init_struct.gpio_pins = 0;
	 gpio_init_struct.gpio_pins = DATA_A8_PIN; 								
     gpio_init(DATA_A8_PORT, &gpio_init_struct);

}

static void FpgaCtrl_PortInit(void)
{
	//FPGA_IOx is for bak using...
     gpio_init_type gpio_init_struct;

     /* set default parameter */
     gpio_default_para_init(&gpio_init_struct);

     /* configure the normal MCU INPUT gpio */
     gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
     gpio_init_struct.gpio_pull = GPIO_PULL_NONE; 

     //FPGA Data ready signal input for MCU
     gpio_init_struct.gpio_pins = 0;
     gpio_init_struct.gpio_pins = DATA_READ_PIN;
     gpio_init(DATA_READ_PORT, &gpio_init_struct);    

     /* configure the normal MCU OUTPUT gpio */
     gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
     gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
     gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
     gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	
	//for Fpga start
     gpio_init_struct.gpio_pins = 0;
     gpio_init_struct.gpio_pins = DATA_START_PIN ;
     gpio_init(DATA_START_PORT, &gpio_init_struct);
	 
	 //for Fpga  reset
	 gpio_init_struct.gpio_pins = 0;
     gpio_init_struct.gpio_pins = DATA_RESET_PIN;						  
     gpio_init(DATA_RESET_PORT, &gpio_init_struct);
	 
	     
     //RE: Reg Enable;
	 gpio_init_struct.gpio_pins = 0;
     gpio_init_struct.gpio_pins = DATA_RE_PIN;
     gpio_init(DATA_RE_PORT, &gpio_init_struct);
	  //FPGA reg Ctrl
    
     // WE: Write Enable; CLK;
	 gpio_init_struct.gpio_pins = 0;
     gpio_init_struct.gpio_pins = DATA_WE_PIN;
     gpio_init(DATA_WE_PORT, &gpio_init_struct);

    
     //CLK: Read/Write Clk;
	 gpio_init_struct.gpio_pins = 0;
     gpio_init_struct.gpio_pins = DATA_CLK_PIN;
     gpio_init(DATA_CLK_PORT, &gpio_init_struct);
	
}

static void FpgaData_PortInit(uint8_t para)
{
     gpio_init_type gpio_init_struct;

    // /* set default parameter */
     gpio_default_para_init(&gpio_init_struct);

     if(para == DATA_INPUT)
     {
         /* configure the normal MCU INPUT gpio */
         gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
         gpio_init_struct.gpio_pull = GPIO_PULL_NONE; 
     }
     else if(para == DATA_OUTPUT)
     {
         /* configure the normal MCU OUTPUT gpio */
         gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
         gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
         gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
         gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	 }
	 //MCU data ouput to FPGA
	 gpio_init_struct.gpio_pins = 0;
	 gpio_init_struct.gpio_pins = DATA_D7_PIN; 							  
	 gpio_init(DATA_D7_PORT, &gpio_init_struct);
	 
	 gpio_init_struct.gpio_pins = 0;
	 gpio_init_struct.gpio_pins = DATA_D6_PIN; 							  
	 gpio_init(DATA_D6_PORT, &gpio_init_struct);
	 
	 gpio_init_struct.gpio_pins = 0;
	 gpio_init_struct.gpio_pins = DATA_D5_PIN; 							  
	 gpio_init(DATA_D5_PORT, &gpio_init_struct);
	 	 
	 gpio_init_struct.gpio_pins = 0;
	 gpio_init_struct.gpio_pins = DATA_D4_PIN; 							  
	 gpio_init(DATA_D4_PORT, &gpio_init_struct);
	 	 
	 gpio_init_struct.gpio_pins = 0;
	 gpio_init_struct.gpio_pins = DATA_D3_PIN; 							  
	 gpio_init(DATA_D3_PORT, &gpio_init_struct);
	 	 
	 gpio_init_struct.gpio_pins = 0;
	 gpio_init_struct.gpio_pins = DATA_D2_PIN; 							  
	 gpio_init(DATA_D2_PORT, &gpio_init_struct);
	 	 
	 gpio_init_struct.gpio_pins = 0;
	 gpio_init_struct.gpio_pins = DATA_D1_PIN; 							  
	 gpio_init(DATA_D1_PORT, &gpio_init_struct);
	 
	 gpio_init_struct.gpio_pins = 0;
	 gpio_init_struct.gpio_pins = DATA_D0_PIN; 							  
	 gpio_init(DATA_D0_PORT, &gpio_init_struct);

}


static void FpgaUnit_Reset(void)
{
     FPGA_RESET_LOW;
     delay_us(1);
     FPGA_RESET_HIGH;

}

 static void FpgaAddr_Write(uint16_t regAddr)
 {
   
     if(regAddr & (0x1<<0))
     {
         DATA_A0_HIGH;
     }
     else
     {
         DATA_A0_LOW;
     }
    
     if(regAddr & (0x1<<1))
     {
         DATA_A1_HIGH;
     }
     else
     {
         DATA_A1_LOW;
     }
    
     if(regAddr & (0x1<<2))
     {
         DATA_A2_HIGH;
     }
     else
     {
         DATA_A2_LOW;
     }
    
     if(regAddr & (0x1<<3))
     {
         DATA_A3_HIGH;
     }
     else
     {
         DATA_A3_LOW;
     }
    
     if(regAddr & (0x1<<4))
     {
         DATA_A4_HIGH;
     }
     else
     {
         DATA_A4_LOW;
     }
    
     if(regAddr & (0x1<<5))
     {
         DATA_A5_HIGH;
     }
     else
     {
         DATA_A5_LOW;
     }
    
     if(regAddr & (0x1<<6))
     {
         DATA_A6_HIGH;
     }
     else
     {
         DATA_A6_LOW;
     }

     if(regAddr & (0x1<<7))
     {
         DATA_A7_HIGH;
     }
     else
     {
         DATA_A7_LOW;
     }    
	if(regAddr & (0x1<<8))
     {
         DATA_A8_HIGH;
     }
     else
     {
         DATA_A8_LOW;
     }   
 }

//read or write a singal reg of FPGA
uint8_t IF_FpgaReg_ReadStart(void)
{
	uint8_t error_flag = 0;
	uint8_t timeOutCnt = 0;
	FpgaData_PortInit(DATA_INPUT); //Setup Data port Input
	DATA_START_HIGH; 
	while(DATA_FPGA_READ == 0)//normal operation time is less 100us	
	{
		timeOutCnt++;
		if(timeOutCnt > 100)
		{
			error_flag = 1;
			break;  
		} 
		delay_us(1);
	}
	return error_flag;
}

void IF_FpgaReg_ReadEnd(void)
{
     DATA_START_LOW;
     FpgaData_PortInit(DATA_OUTPUT);
}

uint8_t IF_FpgaReg_Read(uint16_t regAddr)
{
    uint8_t result = 0;

     //step1: set ctrl singal
     DATA_RE_HIGH; //Reg Enable
     DATA_WE_LOW;  //WE=0 means read enable

     //step2: set addr and latch the addr
     FpgaAddr_Write(regAddr);
     DATA_CLK_LOW;
     DATA_CLK_HIGH;
     DATA_CLK_LOW;
	
     DATA_CLK_LOW;
     DATA_CLK_HIGH;
     DATA_CLK_LOW;
	
	 DATA_CLK_LOW;
     DATA_CLK_HIGH;
     DATA_CLK_LOW;
     //step5: get data
//     if(SET == DATA_D0_READ)
//     {
//         result |= 0x01;
//     }

//     if(SET == DATA_D1_READ)
//     {
//         result |= 0x02;
//     }

//     if(SET == DATA_D2_READ)
//     {
//         result |= 0x04;
//     }

//     if(SET == DATA_D3_READ)
//     {
//         result |= 0x08;
//     }

//     if(SET == DATA_D4_READ)
//     {
//         result |= 0x10;
//     }

//     if(SET == DATA_D5_READ)
//     {
//         result |= 0x20;
//     }

//     if(SET == DATA_D6_READ)
//     {
//         result |= 0x40;
//     }

//     if(SET == DATA_D7_READ)
//     {
//         result |= 0x80;
//     }
	 result = READ_DATA_D0D7_PORT;
     //step6: set ctrl singal
     DATA_RE_LOW; //Reg Disable

    return result;
}

void IF_FpgaReg_Write(uint16_t regAddr, uint8_t regData)
{

   // step2: set ctrl singal
     DATA_RE_HIGH; //Reg Enable
     DATA_WE_HIGH;//WE=1 means write enable

     //step1: set addr
     FpgaAddr_Write(regAddr);

     //step3: set data direction
//   FpgaData_PortInit(DATA_OUTPUT);

     //step4: prepare data
     if(regData & 0x01)
     {
         DATA_D0_HIGH;
     }
     else
     {
         DATA_D0_LOW;
     }
    
     if(regData & 0x02)
     {
         DATA_D1_HIGH;
     }
     else
     {
         DATA_D1_LOW;
     }
    
     if(regData & 0x04)
     {
         DATA_D2_HIGH;
     }
     else
     {
         DATA_D2_LOW;
     }
    
     if(regData & 0x08)
     {
         DATA_D3_HIGH;
     }
     else
     {
         DATA_D3_LOW;
     }
    
     if(regData & 0x10)
     {
         DATA_D4_HIGH;
     }
     else
     {
         DATA_D4_LOW;
     }
    
     if(regData & 0x20)
     {
         DATA_D5_HIGH;
     }
     else
     {
         DATA_D5_LOW;
     }
    
     if(regData & 0x40)
     {
         DATA_D6_HIGH;
     }
     else
     {
         DATA_D6_LOW;
     }

     if(regData & 0x80)
     {
         DATA_D7_HIGH;
     }
     else
     {
         DATA_D7_LOW;
     }    

     //step5: set clk
     DATA_CLK_LOW;
     DATA_CLK_HIGH;
     DATA_CLK_LOW;

     //step6: set ctrl singal
     DATA_RE_LOW; //Reg Disable
     DATA_WE_LOW;
}


//*****************************************************************************
//* END
//*****************************************************************************

