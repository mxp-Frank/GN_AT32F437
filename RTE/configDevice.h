/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  configdevice.h
 * @Brief :      
 * @Author :     xingping.meng (mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2023-02-01
 * ****************************************************************************
 */
#ifndef _CONFIGDEVICE_H_
#define _CONFIGDEVICE_H_

#define BOOTLOADER_VERSION                       "CB_0.1.4"
#define BOOTLOADER_VERSION_LEN                   16

#define SOFTWARE_VERSION                         "CA_0.1.4"
#define SOFTWARE_VERSION_LEN                     16

#define MODBUS_FILE_SIGN                        "acdc_017"
#define FPGA_FILE_SIGN							"fpga_boa"
#define APP_FILE_SIGN                           "CY_GN017"

#define APP_FILE_SIGN_LEN                        8
#define APP_FILE_SIGN_OFFSET                     16

#define MANUAL								0
#define AUTO                                1

#define	FWDREGULATION   					0
#define LOADREGULATION  					1
#define	EXTREGULATION   					2 

#define UART_MODE							0
#define ANLG_MODE                           1
#define KEY_MODE							2

/*****************设置值范围控制************************/ 
#define MAX_MAP_POWER							8000000   	//射频电源最大映射功率(mW)
#define MAX_FPGA_PHASE							164500		//射频电源最大输出相位值(1/1000)  
#define POWER_RATIO							    1         	//射频电源输出功率分辨率(W)
#define MAX_POWER								18000     	//射频电源最大功率(W)
#define MAX_VDCATTEN							4000      	//射频电源最大VDC衰减因子
#define MAX_MATCHPOS							10000     	//射频电源最大匹配器位置
#define MAX_REFLECTDELAY						255       	//射频电源关断反射功率延时时间(s) 
#define TUNED_VSWR								1.06F     	//射频电源调整驻波比门限      
#define MAX_SLOWDELAY							10000     	//射频电源最大功率斜率上升时间(ms)
#define MAX_POWEROFFSET							2000      	//射频电源线损补偿因子/1000 
#define CENTER_FREQUENTY						2000000		//射频电源中心工作频率(Hz)
#define MAX_PULSEFREQ							30000     	//射频电源最大脉冲频率(Hz)
#define MAX_PULSEDUTY							100        	//射频电源最大脉冲占空比(%)
#define MAX_COMTYPE								2		  	//通信类型  1：模拟口   0:串口      2：显示接口
#define MAX_REGUALATION							2		  	//功率类型  1：前向功率 0:反射功率  2： VDC功率
#define MAX_ENCODER								30000     	//编码器最大值
#define MAX_ACDC_VOLT							160000     	//AC-DC电源最大输出电压值(mv)
#define MAX_ACDC_CURRENT						90000    	//AC-DC电源最大输出电流值(mA) 
#define INIT_ACDC_VOLTAGE						100000	  	//AC-DC电源初始化输出电压值(mv)
#define INIT_ACDC_PHASE							164000	 	//AC-DC电源初始化输出相位值(1/1000)


/******************匹配器电容设置参数***************************/
typedef enum
{
    LOAD = 0,						//自动匹配器C1电容Nvm参数				
    TUNE = 1,						//自动匹配器C2电容Nvm参数
    CAP_NUM = 2
} CAPACITOR_INDEX;



#endif
//*****************************************************************************
//* END
//*****************************************************************************


