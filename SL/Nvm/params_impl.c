/*
 * params_impl.c
 *
 *  Created on: 2022年3月11日
 *      Author: Huxian
 */
//Note by mengxingping@chengyuan-tech.com 2022-12-16
//This document defined all parameters(including Configurable parameters and running parameters) used
//by the AM device, and it implemented the API of these parameters for SL layer and APP layer...

#define USE_GLOBE

//#include "board.h"
#include "params_impl.h"
#include "IF_SL_Cfg.h"
#include "IF_HAL_Cfg.h"

/* CONST & MACROS */

/* DATA STRUCTURES */

/* LOCAL VARIABLES */


static uint8_t 					cmdActionID[ID_SET_ALLNUM];		//控制激活参数
//结构体变量
static PartsParam_t 			Fs_PartsParam;
static CommonParam_t			Fs_CommonParam;			
static InternalParam_t 			Fs_InternalParam;
static UserParam_t 				Fs_UserParam;
 
static PartsParam_t 			PartsParam;
static CommonParam_t			CommonParam;
static InternalParam_t 			InternalParam;
static UserParam_t 				UserParam;



	
static DefaultParam_t DeviceParam =
{
	.FactoryMode  = 0,         //工厂模式	
  	.RFPowerState = 0,	 	  //射频电源工作状态    	  0：RF_OFF        1：RF_ON
  	.DDSSignState = 0,	      //射频电源驱动DDS工作状态   0：RF_OFF        1：RF_ON
    .SetACDCState = 0,        //设置射频电源AC-DC工作状态   0：RF_OFF        1：RF_ON
  	.PowerWorkMode = 0,	      //设置电源功率工作模式   0：正常模式      1：调试模式(开环模式)
  	.SetChannelNo = 0,	      //设置
    .SetPointValue = 0,       //设置射频电源功率  0~1000
  	.SetACDCVolt = 0,	      //设置射频电源AC-DC电压值  0~50000
    .SetACDCCurrent = 0,      //设置射频电源AC-DC电流值，0-200
    .SetWorkFreq = 0,         //设置射频电源输出频率值，0-2000
    .SetWorkPhase = 0,        //设置射频电源输出相位值，0-2000
	.TargetPos[LOAD]=5000,
	.TargetPos[TUNE]=5000,
	.CurrentPos[LOAD]= 5000,
	.CurrentPos[TUNE]= 5000,
};
/* GLOBAL VARIABLES */
FaultWord_t 			g_FaultWord;
StatusWord_t 			g_StatusWord;
///////////////////////////////////////////////////////////////////////////////////////////////////
// 以下是应用中使用到的变量或参数



#ifdef BOOT
const uint8_t BootloaderVersion[BOOTLOADER_VERSION_LEN] __attribute__((at(0x08000410)))  = BOOTLOADER_VERSION;
#else

const uint8_t BootloaderVersion[BOOTLOADER_VERSION_LEN]  = BOOTLOADER_VERSION;
const uint8_t SoftwareVersion[SOFTWARE_VERSION_LEN]  = SOFTWARE_VERSION;
#endif
static void Check_Fs_PartsParam(void);
static void Check_Fs_CommonParam(void);
static void Check_Fs_InternalParam(void);
static void Check_Fs_UserParam(void);
static void Check_PartsParam(void);
static void Check_CommonParam(void);
static void Check_InternalParam(void);
static void Check_UserParam(void);


/* FUNCTION ***********************************************************************
 * Function Name : Params_Init
 * Description   : 初始化参数
 * Parameter     : 
 * return        :               
 * END ***************************************************************************/
void Params_Init(void)
{
	/*清除各个状态字*/
	memset(&g_FaultWord,0,sizeof(FaultWord_t));
	memset(&g_StatusWord,0,sizeof(StatusWord_t));
	
	Read_FS_PartsParam((uint8_t*)&Fs_PartsParam);
	Read_FS_CommonParam((uint8_t*)&Fs_CommonParam);
	Read_FS_InternalParam((uint8_t*)&Fs_InternalParam);
	Read_FS_UserParam((uint8_t*)&Fs_UserParam);
	Check_Fs_PartsParam();
	Check_Fs_CommonParam();
	Check_Fs_InternalParam();
	Check_Fs_UserParam(); 
	
	Read_PartsParam((uint8_t*)&PartsParam);
	Read_CommonParam((uint8_t*)&CommonParam);
	Read_InternalParam((uint8_t*)&InternalParam);
	Read_UserParam((uint8_t*)&UserParam);
	Check_PartsParam();
	Check_CommonParam();
	Check_InternalParam();
	Check_UserParam(); 
	
}

/* FUNCTION ***********************************************************************
 * Function Name : Check_PartsParam
 * Description   : 检查配置信息参数
 * Parameter     : 
 * return        :               
 * END ***************************************************************************/
static void Check_Fs_PartsParam(void)
{
	uint16_t i;
	//内容全部为0xFF则将参数值全部初始化
	uint8_t *pCheckParam = (uint8_t*)&Fs_PartsParam;
	for (i = 0; i < PARTS_PARAM_LEN; i++)
	{
		if (pCheckParam[i] != 0xFF)break;
		
	}
	if (i >= PARTS_PARAM_LEN/3)
	{
		memset(&Fs_PartsParam, 0, PARTS_PARAM_LEN);
	}
}
static void Check_PartsParam(void)
{
	uint16_t i;
	//内容全部为0xFF则将参数值全部初始化
	uint8_t *pCheckParam = (uint8_t*)&PartsParam;
	for (i = 0; i < PARTS_PARAM_LEN; i++)
	{
		if (pCheckParam[i] != 0xFF)break;		
	}
	
	if (i >= PARTS_PARAM_LEN/3)
	{
		memset(&PartsParam, 0, PARTS_PARAM_LEN);
	}
	
}
/* FUNCTION ***********************************************************************
 * Function Name : Check_CommonParam
 * Description   : 检查内部通信参数
 * Parameter     : 
 * return        :               
 * END ***************************************************************************/
static void Check_Fs_CommonParam(void)
{
	uint16_t i;
	//内容全部为0xFF则将参数值全部初始化
	uint8_t *pCheckParam = (uint8_t*)&Fs_CommonParam;
	for (i = 0; i < COMMUN_PARAM_LEN; i++)
	{
		if (pCheckParam[i] != 0xFF)break;
	}
	if (i >= COMMUN_PARAM_LEN/3)
	{
		memset(&Fs_CommonParam, 0, PARTS_PARAM_LEN);
		Fs_CommonParam.DataRate = 2;
		Fs_CommonParam.CommunicateType = 2;
	}
	//检查所有参数范围(参数有范围值)
	if(Fs_CommonParam.DataRate > 4)
	{
		Fs_CommonParam.DataRate = 2;
	}
	if(Fs_CommonParam.CommunicateType > 2)
	{
		Fs_CommonParam.CommunicateType = 2;
	}
}
static void Check_CommonParam(void)
{
	uint16_t i;
	//内容全部为0xFF则将参数值全部初始化
	uint8_t *pCheckParam = (uint8_t*)&CommonParam;
	for (i = 0; i < COMMUN_PARAM_LEN; i++)
	{
		if (pCheckParam[i] != 0xFF)break;
	}
	if (i >= COMMUN_PARAM_LEN/3)
	{
		memset(&PartsParam, 0, PARTS_PARAM_LEN);
		CommonParam.DataRate = 2;
		CommonParam.CommunicateType = 2;
	}
	//检查所有参数范围(参数有范围值)
	if(CommonParam.DataRate > 4)
	{
		CommonParam.DataRate = 2;
	}
	if(CommonParam.CommunicateType > 2)
	{
		CommonParam.CommunicateType = 2;
	}
}
/* FUNCTION ***********************************************************************
 * Function Name : Check_InternalParam
 * Description   : 检查内部参数
 * Parameter     : 
 * return        :               
 * END ***************************************************************************/
static void Check_Fs_InternalParam(void)
{
	uint16_t i;
	//内容全部为0xFF则将参数值全部初始化
	uint8_t *pCheckParam = (uint8_t*)&Fs_InternalParam;
	for (i = 0; i < INTERNAL_PARAM_LEN; i++)
	{
		if (pCheckParam[i] != 0xFF)break;
	}
	if(i>INTERNAL_PARAM_LEN/3)
	{
		memset(&Fs_InternalParam, 0, INTERNAL_PARAM_LEN);
		Fs_InternalParam.VSWRLimit= 1500;
		Fs_InternalParam.AnalogVoltRange= 10000;
		Fs_InternalParam.AnalogRFPowerFactor= 10000;
	}	
	//检查所有参数范围(参数有范围值)
   if(Fs_InternalParam.VSWRLimit < 1000)
   {
		Fs_InternalParam.VSWRLimit= 1500;
   }
   if(Fs_InternalParam.AnalogVoltRange > 15000)
   {
		Fs_InternalParam.AnalogVoltRange= 10000;
   }
   if(Fs_InternalParam.AnalogRFPowerFactor > 15000)
   {
		Fs_InternalParam.AnalogRFPowerFactor= 10000;
   }
    if(Fs_InternalParam.StartPoint >= MAX_POWER)
   {
		Fs_InternalParam.StartPoint = 0;
   }
}

static void Check_InternalParam(void)
{
	uint16_t i;
	//内容全部为0xFF则将参数值全部初始化
	uint8_t *pCheckParam = (uint8_t*)&InternalParam;
	for (i = 0; i < INTERNAL_PARAM_LEN; i++)
	{
		if (pCheckParam[i] != 0xFF)break;
	}
	if(i>INTERNAL_PARAM_LEN/3)
	{
		memset(&InternalParam, 0, INTERNAL_PARAM_LEN);
		InternalParam.VSWRLimit= 1500;
		InternalParam.AnalogVoltRange= 10000;
		InternalParam.AnalogRFPowerFactor= 10000;
	}	
	//检查所有参数范围(参数有范围值)
   if(InternalParam.VSWRLimit < 1000)
   {
		InternalParam.VSWRLimit= 1500;
   }
   if(InternalParam.AnalogVoltRange > 15000)
   {
		InternalParam.AnalogVoltRange= 10000;
   }
   if(InternalParam.AnalogRFPowerFactor > 15000)
   {
		InternalParam.AnalogRFPowerFactor= 10000;
   }
    if(InternalParam.StartPoint >= MAX_POWER)
   {
		InternalParam.StartPoint = 0;
   }
}

/* FUNCTION ***********************************************************************
 * Function Name : Check_UserParam
 * Description   : 检查用户参数
 * Parameter     : 
 * return        :               
 * END ***************************************************************************/
static void Check_Fs_UserParam(void)
{	
	uint16_t i;
	//内容全部为0xFF则将参数值全部初始化
	uint8_t *pCheckParam = (uint8_t*)&Fs_UserParam;
	for (i = 0; i < USER_PARAM_LEN; i++)
	{
		if (pCheckParam[i] != 0xFF)break;
	}
	if(i>USER_PARAM_LEN/3)
	{
		memset(&Fs_UserParam, 0, USER_PARAM_LEN);
		Fs_UserParam.PrefDelayOff = 15;	
		Fs_UserParam.PulseDuty = 50;
		Fs_UserParam.VDCFactor = MAX_VDCATTEN;
		Fs_UserParam.PowerOffsetFactor = 1000;
		Fs_UserParam.PrefThr = MAX_POWER;
		Fs_UserParam.PfwdLimit = MAX_POWER;
		Fs_UserParam.PrefLimit = MAX_POWER;
		Fs_UserParam.PvdcLimit = MAX_POWER;
		
	}	
	//检查所有参数范围(参数有范围值)
	if(Fs_UserParam.RegulationMode > MAX_REGUALATION) 
	{
		Fs_UserParam.RegulationMode = 0;
	}
	if(Fs_UserParam.SlowMode > 1) 
	{
		Fs_UserParam.SlowMode = 0;
	}
	if(Fs_UserParam.MatchMode > 1)
	{
		Fs_UserParam.MatchMode = 0;
	}
	if(Fs_UserParam.PulseMode > 1) 
	{
		Fs_UserParam.PulseMode= 0;
	}
	if(Fs_UserParam.PrefMode > 1) 
	{
		Fs_UserParam.PrefMode = 0;
	}		
	if(Fs_UserParam.PrefDelayOff > MAX_REFLECTDELAY) 
	{
		Fs_UserParam.PrefDelayOff = 15;
	}
	if(Fs_UserParam.PulseFreq > 1000000) 
	{
		Fs_UserParam.PulseFreq = 0;
	}
	if(Fs_UserParam.PulseDuty > 100) 
	{
		Fs_UserParam.PulseDuty = 50;
	}
	if(Fs_UserParam.SlowRFOnDelay > MAX_SLOWDELAY) 
	{
		Fs_UserParam.SlowRFOnDelay = 0;
	}
	if(Fs_UserParam.SlowRFOffDelay > MAX_SLOWDELAY) 
	{
		Fs_UserParam.SlowRFOffDelay = 0;
	}
	if(Fs_UserParam.VDCFactor > MAX_VDCATTEN)
	{
		Fs_UserParam.VDCFactor = MAX_VDCATTEN;
	}
	if(Fs_UserParam.PowerOffsetFactor > MAX_POWEROFFSET) 
	{
		Fs_UserParam.PowerOffsetFactor = 1000;
	}
	if(Fs_UserParam.PrefThr > MAX_POWER)
	{
		Fs_UserParam.PrefThr = 500;
	}
	if(Fs_UserParam.PfwdLimit > MAX_POWER) 
	{
		Fs_UserParam.PfwdLimit = MAX_POWER;
	}
	if(Fs_UserParam.PrefLimit > MAX_POWER)
	{
		Fs_UserParam.PrefLimit = MAX_POWER;
	}
	if(Fs_UserParam.PvdcLimit > MAX_POWER)
	{
		Fs_UserParam.PvdcLimit = MAX_POWER;
	}	
}
static void Check_UserParam(void)
{	
	uint16_t i;
	//内容全部为0xFF则将参数值全部初始化
	uint8_t *pCheckParam = (uint8_t*)&UserParam;
	for (i = 0; i < USER_PARAM_LEN; i++)
	{
		if (pCheckParam[i] != 0xFF)break;
	}
	if(i>USER_PARAM_LEN/3)
	{
		memset(&UserParam, 0, USER_PARAM_LEN);
		UserParam.PrefDelayOff = 15;	
		UserParam.PulseDuty = 50;
		UserParam.VDCFactor = MAX_VDCATTEN;
		UserParam.PowerOffsetFactor = 1000;
		UserParam.PrefThr = MAX_POWER;
		UserParam.PfwdLimit = MAX_POWER;
		UserParam.PrefLimit = MAX_POWER;
		UserParam.PvdcLimit = MAX_POWER;
		
	}	
	//检查所有参数范围(参数有范围值)
	if(UserParam.RegulationMode > MAX_REGUALATION) 
	{
		UserParam.RegulationMode = 0;
	}
	if(UserParam.SlowMode > 1) 
	{
		UserParam.SlowMode = 0;
	}
	if(UserParam.MatchMode > 1)
	{
		UserParam.MatchMode = 0;
	}
	if(UserParam.PulseMode > 1) 
	{
		UserParam.PulseMode= 0;
	}
	if(UserParam.PrefMode > 1) 
	{
		UserParam.PrefMode = 0;
	}		
	if(UserParam.PrefDelayOff > MAX_REFLECTDELAY) 
	{
		UserParam.PrefDelayOff = 15;
	}
	if(UserParam.PulseFreq > 1000000) 
	{
		UserParam.PulseFreq = 0;
	}
	if(UserParam.PulseDuty > 100) 
	{
		UserParam.PulseDuty = 50;
	}
	if(UserParam.SlowRFOnDelay > MAX_SLOWDELAY) 
	{
		UserParam.SlowRFOnDelay = 0;
	}
	if(UserParam.SlowRFOffDelay > MAX_SLOWDELAY) 
	{
		UserParam.SlowRFOffDelay = 0;
	}
	if(UserParam.VDCFactor > MAX_VDCATTEN)
	{
		UserParam.VDCFactor = MAX_VDCATTEN;
	}
	if(UserParam.PowerOffsetFactor > MAX_POWEROFFSET) 
	{
		UserParam.PowerOffsetFactor = 1000;
	}
	if(UserParam.PrefThr > MAX_POWER)
	{
		UserParam.PrefThr = 500;
	}
	if(UserParam.PfwdLimit > MAX_POWER) 
	{
		UserParam.PfwdLimit = MAX_POWER;
	}
	if(UserParam.PrefLimit > MAX_POWER)
	{
		UserParam.PrefLimit = MAX_POWER;
	}
	if(UserParam.PvdcLimit > MAX_POWER)
	{
		UserParam.PvdcLimit = MAX_POWER;
	}	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// 以下是对params_interface.h中定义的接口的实现
//------------------------------------------------------------------------------------------------
// Identification parameter Operation
void IF_IdentParam_SetDeviceType(uint8_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		Fs_PartsParam.DeviceType = value;
		cmdActionID[ID_SAVE_FS_PARTS_PARAM]= ON;	
	}
	PartsParam.DeviceType = value;
	cmdActionID[ID_SAVE_PARTS_PARAM]= ON;		
}

uint8_t IF_IdentParam_GetDeviceType(void)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		return Fs_PartsParam.DeviceType;
	}else
	{
		return PartsParam.DeviceType;
	}
}
//-----------------------------------------------------------------------------
void IF_IdentParam_SetVendorCode(uint8_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		PartsParam.VendorCode = value;
		cmdActionID[ID_SAVE_FS_PARTS_PARAM]= ON;	
	}	
	PartsParam.VendorCode = value;
	cmdActionID[ID_SAVE_PARTS_PARAM]= ON;	
}

uint8_t IF_IdentParam_GetVendorCode(void)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		return Fs_PartsParam.VendorCode;
	}else
	{
		return PartsParam.VendorCode;
	}
}
//-----------------------------------------------------------------------------
void IF_IdentParam_SetHardwareVersion(uint8_t* pBuf, uint8_t len)
{
	if(len >= HARDWARE_VERSION_LEN)len = HARDWARE_VERSION_LEN;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		memset(Fs_PartsParam.HardwareVersion, 0, HARDWARE_VERSION_LEN);
		memcpy(Fs_PartsParam.HardwareVersion, pBuf,len);
		cmdActionID[ID_SAVE_FS_PARTS_PARAM]= ON;	
	}	
	memset(PartsParam.HardwareVersion, 0, HARDWARE_VERSION_LEN);
	memcpy(PartsParam.HardwareVersion, pBuf,len);
	cmdActionID[ID_SAVE_PARTS_PARAM]= ON;	
}

uint8_t IF_IdentParam_GetHardwareVersion(uint8_t* pBuf)
{
	uint8_t i;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		for (i = 0; i < HARDWARE_VERSION_LEN; i++)
		{
			if (Fs_PartsParam.HardwareVersion[i] == 0)
			{
				break;
			}

			pBuf[i] = Fs_PartsParam.HardwareVersion[i];
		}
	}else
	{
		for (i = 0; i < HARDWARE_VERSION_LEN; i++)
		{
			if (PartsParam.HardwareVersion[i] == 0)
			{
				break;
			}

			pBuf[i] = PartsParam.HardwareVersion[i];
		}
	}
	return i;	
}

//-----------------------------------------------------------------------------
uint8_t IF_IdentParam_GetSoftwareVersion(uint8_t* pBuf)
{
#ifdef BOOT
	uint8_t i;

	for (i = 0; i < BOOTLOADER_VERSION_LEN; i++)
	{
		if (BootloaderVersion[i] == 0)
		{
			break;
		}

		pBuf[i] = BootloaderVersion[i];
	}

	return i;
#else
	uint8_t i;

	for (i = 0; i < SOFTWARE_VERSION_LEN; i++)
	{
		if (SoftwareVersion[i] == 0)
		{
			break;
		}

		pBuf[i] = SoftwareVersion[i];
	}
	return i;
#endif
}

//-----------------------------------------------------------------------------
uint8_t IF_IdentParam_GetBootloaderVersion(uint8_t* pBuf)
{
#ifdef BOOT
	return IF_IdentParam_GetSoftwareVersion(pBuf);
#else
	return Read_BootloaderVersion(pBuf);
#endif
}
//-----------------------------------------------------------------------------
void IF_IdentParam_SetProductModel(uint8_t* pBuf, uint8_t len)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		memset(Fs_PartsParam.ProductModel, 0, PRODUCT_MODEL_LEN);
		memcpy(Fs_PartsParam.ProductModel, pBuf,
			(len < PRODUCT_MODEL_LEN) ? len : PRODUCT_MODEL_LEN);
		cmdActionID[ID_SAVE_FS_PARTS_PARAM]=ON;	
	}
	memset(PartsParam.ProductModel, 0, PRODUCT_MODEL_LEN);
	memcpy(PartsParam.ProductModel, pBuf,
			(len < PRODUCT_MODEL_LEN) ? len : PRODUCT_MODEL_LEN);
	cmdActionID[ID_SAVE_PARTS_PARAM]=ON;		
}

uint8_t IF_IdentParam_GetProductModel(uint8_t* pBuf)
{
	uint8_t i;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		for (i = 0; i < PRODUCT_MODEL_LEN; i++)
		{
			if (Fs_PartsParam.ProductModel[i] == 0)
			{
				break;
			}

			pBuf[i] = Fs_PartsParam.ProductModel[i];
		}
	}else
	{
		for (i = 0; i < PRODUCT_MODEL_LEN; i++)
		{
			if (PartsParam.ProductModel[i] == 0)
			{
				break;
			}

			pBuf[i] = PartsParam.ProductModel[i];
		}
	}
	return i;
}
//-----------------------------------------------------------------------------
void IF_IdentParam_SetSerialNumber(uint8_t* pBuf, uint8_t len)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		memset(Fs_PartsParam.SerialNumber, 0, SERIAL_NUMBER_LEN);
		memcpy(Fs_PartsParam.SerialNumber, pBuf,
			(len < SERIAL_NUMBER_LEN) ? len : SERIAL_NUMBER_LEN);
		cmdActionID[ID_SAVE_FS_PARTS_PARAM]= ON;	
	}
	memset(PartsParam.SerialNumber, 0, SERIAL_NUMBER_LEN);
	memcpy(PartsParam.SerialNumber, pBuf,
			(len < SERIAL_NUMBER_LEN) ? len : SERIAL_NUMBER_LEN);
	cmdActionID[ID_SAVE_PARTS_PARAM]= ON;		
}

uint8_t IF_IdentParam_GetSerialNumber(uint8_t* pBuf)
{
	uint8_t i;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
	
		for (i = 0; i < SERIAL_NUMBER_LEN; i++)
		{
			if (Fs_PartsParam.SerialNumber[i] == 0)
			{
				break;
			}
			pBuf[i] = Fs_PartsParam.SerialNumber[i];
		}
	}else
	{
		for (i = 0; i < SERIAL_NUMBER_LEN; i++)
		{
			if (PartsParam.SerialNumber[i] == 0)
			{
				break;
			}
			pBuf[i] = PartsParam.SerialNumber[i];
		}
	}

	return i;
}
//-----------------------------------------------------------------------------
void IF_IdentParam_SetTrackingNumber(uint8_t* pBuf, uint8_t len)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		memset(Fs_PartsParam.TrackingNumber, 0, TRACKING_NUMBER_LEN);
		memcpy(Fs_PartsParam.TrackingNumber, pBuf,
			(len < TRACKING_NUMBER_LEN) ? len : TRACKING_NUMBER_LEN);
		cmdActionID[ID_SAVE_FS_PARTS_PARAM]= ON;	
	}
	memset(PartsParam.TrackingNumber, 0, TRACKING_NUMBER_LEN);
	memcpy(PartsParam.TrackingNumber, pBuf,
			(len < TRACKING_NUMBER_LEN) ? len : TRACKING_NUMBER_LEN);
	cmdActionID[ID_SAVE_PARTS_PARAM]= ON;	
}

uint8_t IF_IdentParam_GetTrackingNumber(uint8_t* pBuf)
{
	uint8_t i;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		for (i = 0; i < TRACKING_NUMBER_LEN; i++)
		{
			if (Fs_PartsParam.TrackingNumber[i] == 0)
			{
				break;
			}

			pBuf[i] = Fs_PartsParam.TrackingNumber[i];
		}
	}else
	{
		for (i = 0; i < TRACKING_NUMBER_LEN; i++)
		{
			if (PartsParam.TrackingNumber[i] == 0)
			{
				break;
			}

			pBuf[i] = PartsParam.TrackingNumber[i];
		}
	}
	return i;
}
//-----------------------------------------------------------------------------
uint32_t IF_IdentParam_GetCheckSum(void)
{
#ifdef BOOT
	return 0;
#else
	return RUNNING_APP_CRC;
#endif
}

//---------------------------------Communication parameter Operation---------------------------------------------------------
// Communication parameter Operation
void IF_CommParam_SetCommunicateType(uint8_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_CommonParam.CommunicateType = value;
		cmdActionID[ID_SAVE_FS_COMMON_PARAM]= ON;		
	}
	CommonParam.CommunicateType = value;
	cmdActionID[ID_SAVE_COMMON_PARAM] = ON;
}

uint8_t IF_CommParam_GetCommunicateType(void)
{
	uint8_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_CommonParam.CommunicateType;
	}else
	{
		value = CommonParam.CommunicateType;
	}
	return value;
}


void IF_CommParam_SetDeviceAddress(uint8_t value)
{
	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_CommonParam.DeviceAddress = value;
		cmdActionID[ID_SAVE_FS_COMMON_PARAM]=ON;	
	}
	CommonParam.DeviceAddress = value;
	cmdActionID[ID_SAVE_COMMON_PARAM]= ON;		
}

uint8_t IF_CommParam_GetDeviceAddress(void)
{
	uint8_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_CommonParam.DeviceAddress;
	}else
	{
		value = CommonParam.DeviceAddress;
	}
	return value;
	
}

void IF_CommParam_SetNetMAC(uint8_t* pBuf)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		memcpy(Fs_CommonParam.MAC, pBuf, 6);
		cmdActionID[ID_SAVE_FS_COMMON_PARAM]= 1;	
	}		
	memcpy(CommonParam.MAC, pBuf, 6);
	cmdActionID[ID_SAVE_COMMON_PARAM]= 1;			
}

uint8_t IF_CommParam_GetNetMAC(uint8_t* pBuf)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		memcpy(pBuf, Fs_CommonParam.MAC, 6);
	}else
	{
		memcpy(pBuf, CommonParam.MAC, 6);
	}
	return 6;
}
//-----------------------------------------------------------------------------

void IF_CommParam_SetNetIP(uint32_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_CommonParam.IP = value;
		cmdActionID[ID_SAVE_FS_COMMON_PARAM]=ON;	
	}	
	CommonParam.IP = value;
	cmdActionID[ID_SAVE_COMMON_PARAM]= ON;		
}

uint32_t IF_CommParam_GetNetIP(void)
{
	uint32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_CommonParam.IP;
	}else
	{
		value = CommonParam.IP;
	}
	return value;
}
//-----------------------------------------------------------------------------

void IF_CommParam_SetNetMask(uint32_t value)
{
	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_CommonParam.Mask = value;
		
		cmdActionID[ID_SAVE_FS_COMMON_PARAM]= ON;	
	}
	CommonParam.Mask = value;
	cmdActionID[ID_SAVE_COMMON_PARAM]= ON;			
}

uint32_t IF_CommParam_GetNetMask(void)
{
	uint32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_CommonParam.Mask;
	}else
	{
		value = CommonParam.Mask;
	}
	return value;
}
//-----------------------------------------------------------------------------

void IF_CommParam_SetNetGateway(uint32_t value)
{
	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_CommonParam.Gateway = value;
		cmdActionID[ID_SAVE_FS_COMMON_PARAM]=ON;	
	}
	CommonParam.Gateway = value;
	cmdActionID[ID_SAVE_COMMON_PARAM]=ON;			
}

uint32_t IF_CommParam_GetNetGateway(void)
{
	uint32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_CommonParam.Gateway;
	}else
	{
		value = CommonParam.Gateway;
	}
	return value;
}

//-----------------------------------------------------------------------------

void IF_CommParam_SetDataRate(uint8_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_CommonParam.DataRate = value;	
		cmdActionID[ID_SAVE_FS_COMMON_PARAM]= ON;	
	}
	CommonParam.DataRate = value;
	cmdActionID[ID_SET_BAUDRATE]= ON;		
}

uint8_t IF_CommParam_GetDataRate(void)
{	
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		return Fs_CommonParam.DataRate;
	}else
	{
		return CommonParam.DataRate;
	}
}
//------------------------------------------------------------------
void IF_CommParam_SetEtherCatAddress(uint16_t value)
{
	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_CommonParam.EtherCatAddress = value;
		cmdActionID[ID_SAVE_FS_COMMON_PARAM]= ON;	
	}
	CommonParam.EtherCatAddress = value;
	cmdActionID[ID_SAVE_COMMON_PARAM]= ON;		
}

uint16_t IF_CommParam_GetEtherCatAddress(void)
{
	uint16_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_CommonParam.EtherCatAddress;
	}else
	{
		value = CommonParam.EtherCatAddress;
	}
	return value;
}
//-----------------------------------------------------------------------------
//Configurable Internal parameters
//-----------------------------------------------------------------------------
void IF_InternalParam_SetSensorVrmsFactor(int32_t value)
{
	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.VrmsFactor = value; 	
		cmdActionID[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.VrmsFactor = value; 
	cmdActionID[ID_SAVE_INTERNAL_PARAM]= ON;			
}

int32_t IF_InternalParam_GetSensorVrmsFactor(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.VrmsFactor;
	}else
	{
		value = InternalParam.VrmsFactor;
	}
	return value;
}

void IF_InternalParam_SetSensorIrmsFactor(int32_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.IrmsFactor = value;
		cmdActionID[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.IrmsFactor = value;
	cmdActionID[ID_SAVE_INTERNAL_PARAM]= ON;		
}

int32_t IF_InternalParam_GetSensorIrmsFactor(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.IrmsFactor;
	}else
	{
		value = InternalParam.IrmsFactor;
	}
	return value;
}

void IF_InternalParam_SetSensorPhaseFactor(int32_t value)
{	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.PhaseFactor = value;
		cmdActionID[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.PhaseFactor = value;
	cmdActionID[ID_SAVE_INTERNAL_PARAM]= ON;			
}

int32_t IF_InternalParam_GetSensorPhaseFactor(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.PhaseFactor;
	}else
	{
		value = InternalParam.PhaseFactor;
	}
	return value;
}
void IF_InternalParam_SetAnalogVoltRange(uint16_t value)
{
	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.AnalogVoltRange = value;
		cmdActionID[ID_SAVE_FS_INTERNAL_PARAM]=ON;	
	}
	InternalParam.AnalogVoltRange = value;
	cmdActionID[ID_SAVE_INTERNAL_PARAM]= ON;			
}

uint16_t IF_InternalParam_GetAnalogVoltRange(void)
{
	uint16_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.AnalogVoltRange;
	}else
	{
		value = InternalParam.AnalogVoltRange;
	}
	return value;
}

void IF_InternalParam_SetAnalogRFPowerFactor(uint16_t value)
{
	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.AnalogRFPowerFactor = value;
		cmdActionID[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.AnalogRFPowerFactor = value;
	cmdActionID[ID_SAVE_INTERNAL_PARAM]= ON;		
}
uint16_t IF_InternalParam_GetAnalogRFPowerFactor(void)
{
	uint16_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.AnalogRFPowerFactor;
	}else
	{
		value = InternalParam.AnalogRFPowerFactor;
	}
	return value;
}
void IF_InternalParam_SetProportion(int32_t value)
{
	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.PIDProportion = value;
		cmdActionID[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.PIDProportion = value;
	cmdActionID[ID_SAVE_INTERNAL_PARAM]= ON;		
}
int32_t IF_InternalParam_GetProportion(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.PIDProportion;
	}else
	{
		value = InternalParam.PIDProportion;
	}
	return value;
}
void IF_InternalParam_SetIntegral(int32_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.PIDIntegral = value;
		cmdActionID[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.PIDIntegral = value;
	cmdActionID[ID_SAVE_INTERNAL_PARAM]= ON;		
}
int32_t IF_InternalParam_GetIntegral(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.PIDIntegral;
	}else
	{
		value = InternalParam.PIDIntegral;
	}
	return value;
}
void IF_InternalParam_SetDerivatice(int32_t value)
{	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.PIDDerivatice = value;
		cmdActionID[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.PIDDerivatice = value;
	cmdActionID[ID_SAVE_INTERNAL_PARAM]= ON;		
}
int32_t IF_InternalParam_GetDerivatice(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.PIDDerivatice;
	}else
	{
		value = InternalParam.PIDDerivatice;
	}
	return value;
}
void IF_InternalParam_SetPIDErrorThr(int32_t value)
{	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.PIDErrorThr = value;
		cmdActionID[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.PIDErrorThr = value;
	cmdActionID[ID_SAVE_INTERNAL_PARAM]= ON;
}
int32_t IF_InternalParam_GetPIDErrorThr(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.PIDErrorThr;
	}else
	{
		value = InternalParam.PIDErrorThr;
	}
	return value;
}
//Internal ACDCVoltGain  param
void IF_InternalParam_SetACDCVoltGain(int32_t value)
{	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.ACDCVoltGain = value;
		cmdActionID[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.ACDCVoltGain = value;
	cmdActionID[ID_SAVE_INTERNAL_PARAM]= ON;
}
int32_t IF_InternalParam_GetACDCVoltGain(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.ACDCVoltGain;
	}else
	{
		value = InternalParam.ACDCVoltGain;
	}
	return value;
}
//Internal ACDCVoltOffset  param
void IF_InternalParam_SetACDCVoltOffset(int32_t value)
{	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.ACDCVoltOffset = value;
		cmdActionID[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.ACDCVoltOffset = value;
	cmdActionID[ID_SAVE_INTERNAL_PARAM]= ON;
}
int32_t IF_InternalParam_GetACDCVoltOffset(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.ACDCVoltOffset;
	}else
	{
		value = InternalParam.ACDCVoltOffset;
	}
	return value;
}

//Internal ACDCCurrentGain  param
void IF_InternalParam_SetACDCCurrentGain(int32_t value)
{	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.ACDCCurrentGain = value;
		cmdActionID[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.ACDCCurrentGain = value;
	cmdActionID[ID_SAVE_INTERNAL_PARAM]= ON;
}
int32_t IF_InternalParam_GetACDCCurrentGain(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.ACDCCurrentGain;
	}else
	{
		value = InternalParam.ACDCCurrentGain;
	}
	return value;
}
//Internal ACDCCurrentOffset  param
void IF_InternalParam_SetACDCCurrentOffset(int32_t value)
{	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.ACDCCurrentOffset = value;
		cmdActionID[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.ACDCCurrentOffset = value;
	cmdActionID[ID_SAVE_INTERNAL_PARAM]= ON;
}
int32_t IF_InternalParam_GetACDCCurrentOffset(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.ACDCCurrentOffset;
	}else
	{
		value = InternalParam.ACDCCurrentOffset;
	}
	return value;
}
//Internal DrainVoltGain  param
void IF_InternalParam_SetDrainVoltGain(int32_t value)
{	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.DrainVoltGain = value;
		cmdActionID[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.DrainVoltGain = value;
	cmdActionID[ID_SAVE_INTERNAL_PARAM]= ON;
}
int32_t IF_InternalParam_GetDrainVoltGain(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.DrainVoltGain;
	}else
	{
		value = InternalParam.DrainVoltGain;
	}
	return value;
}

//Internal DrainVoltOffset  param
void IF_InternalParam_SetDrainVoltOffset(int32_t value)
{	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.DrainVoltOffset = value;
		cmdActionID[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.DrainVoltOffset = value;
	cmdActionID[ID_SAVE_INTERNAL_PARAM]= ON;
}
int32_t IF_InternalParam_GetDrainVoltOffset(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.DrainVoltOffset;
	}else
	{
		value = InternalParam.DrainVoltOffset;
	}
	return value;
}
//Internal vswr limit param
void IF_InternalParam_SetVSWRLimit(int32_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		Fs_InternalParam.VSWRLimit = value;
		cmdActionID[ID_SAVE_FS_INTERNAL_PARAM]=ON;	
	}
	InternalParam.VSWRLimit = value;
	cmdActionID[ID_SAVE_INTERNAL_PARAM]= ON;	
}

int32_t IF_InternalParam_GetVSWRLimit(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.VSWRLimit;
	}else
	{
		value = InternalParam.VSWRLimit;
	}
	return value;
}
//Internal Init SetPoint param
void IF_InternalParam_StartPoint(int32_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		Fs_InternalParam.StartPoint = value;	
		cmdActionID[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.StartPoint = value;
	cmdActionID[ID_SAVE_INTERNAL_PARAM]= ON;		
}

int32_t IF_InternalParam_GetInitPoint(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.StartPoint;
	}else
	{
		value = InternalParam.StartPoint;
	}
	return value;
}

void IF_InternaleParam_SetFpgaPulsePowerThr(uint32_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.FpgaPulsePowerThr = value;
		cmdActionID[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.FpgaPulsePowerThr = value;
	cmdActionID[ID_SAVE_INTERNAL_PARAM] = ON;		
}

uint32_t IF_InternalParam_GetFpgaPulsePowerThr(void)
{
	uint32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		value = Fs_InternalParam.FpgaPulsePowerThr;
	}else
	{
		value = InternalParam.FpgaPulsePowerThr;	
	}
	return value;
}

void IF_InternalParam_SetFpgaPulseSyncDelay(uint16_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.FpgaPulseSyncDelay = value;
		cmdActionID[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.FpgaPulseSyncDelay = value;
	cmdActionID[ID_SAVE_INTERNAL_PARAM] = ON;		
}

uint16_t IF_InternalParam_GetFpgaPulseSyncDelay(void)
{
	uint16_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		value = Fs_InternalParam.FpgaPulseSyncDelay;
	}else
	{
		value = InternalParam.FpgaPulseSyncDelay;	
	}
	return value;
}
//-------------------------------------------------------------------------------------------------
//PT Configurable User Parameters
//-----------------------------------------------------------------------------
void IF_UserParam_SetRegulationMode(uint8_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_UserParam.RegulationMode = value;
		cmdActionID[ID_SAVE_FS_USER_PARAM]= ON;	
	}
	UserParam.RegulationMode = value;
	cmdActionID[ID_SAVE_USER_PARAM] = ON;
	
}

uint8_t IF_UserParam_GetRegulationMode(void)
{
	uint8_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_UserParam.RegulationMode;
	}else
	{
		value = UserParam.RegulationMode;
	}
	return value;
}

void IF_UserParam_SetForwardPowerLimit(uint16_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_UserParam.PfwdLimit = value;
		cmdActionID[ID_SAVE_FS_USER_PARAM]= ON;		
	}
	UserParam.PfwdLimit = value;
	cmdActionID[ID_SAVE_USER_PARAM] = ON;		
}

uint16_t IF_UserParam_GetForwardPowerLimit(void)
{
	uint16_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_UserParam.PfwdLimit;
	}else
	{
		value = UserParam.PfwdLimit;
	}
	return value;
}

void IF_UserParam_SetReflectPowerLimit(uint16_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_UserParam.PrefLimit = value;
		cmdActionID[ID_SAVE_FS_USER_PARAM]= ON;		
	}
	UserParam.PrefLimit = value;
	cmdActionID[ID_SAVE_USER_PARAM] = ON;
}

uint16_t IF_UserParam_GetReflectPowerLimit(void)
{
	uint16_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_UserParam.PrefLimit;
	}else
	{
		value = UserParam.PrefLimit;
	}
	return value;
}
//ReflectPowerSwitchOff
/**********************************************************/
void IF_UserParam_SetReflectPowerSwitchOff(uint8_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_UserParam.PrefMode = value;
		cmdActionID[ID_SAVE_FS_USER_PARAM]= ON;	
	}
	UserParam.PrefMode = value;
	cmdActionID[ID_SAVE_USER_PARAM] = ON;	
}

uint8_t IF_UserParam_GetReflectPowerSwitchOff(void)
{
	uint8_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_UserParam.PrefMode;
	}else
	{
		value = UserParam.PrefMode;
	}
	return value;
}
//ReflectPowerThreshold
/**********************************************************/
void IF_UserParam_SetReflectPowerThreshold(uint16_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_UserParam.PrefThr = value;
		cmdActionID[ID_SAVE_FS_USER_PARAM]= ON;	
	}
	UserParam.PrefThr = value;
	cmdActionID[ID_SAVE_USER_PARAM] = ON;
}

uint16_t IF_UserParam_GetReflectPowerThreshold(void)
{
	uint16_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_UserParam.PrefThr;
	}else
	{
		value = UserParam.PrefThr;
	}
	return value;
}
/**********************************************************/
void IF_UserParam_SetReflectPowerDelayOff(uint8_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_UserParam.PrefDelayOff = value;
		cmdActionID[ID_SAVE_FS_USER_PARAM]= ON;	
	}
	UserParam.PrefDelayOff = value;
	cmdActionID[ID_SAVE_USER_PARAM] = ON;	
}

uint8_t IF_UserParam_GetReflectPowerDelayOff(void)
{
	uint8_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_UserParam.PrefDelayOff;
	}else
	{
		value = UserParam.PrefDelayOff;
	}
	return value;
}
/**********************************************************/
void IF_UserParam_SetPulseMode(uint8_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_UserParam.PulseMode = value;
		cmdActionID[ID_SAVE_FS_USER_PARAM]= ON;		
	}
	UserParam.PulseMode = value;
	cmdActionID[ID_SAVE_USER_PARAM] = ON;	
}
uint8_t IF_UserParam_GetPulseMode(void)
{
	uint8_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		value = Fs_UserParam.PulseMode;
	}else
	{
		value = UserParam.PulseMode;		
	}
	return value;
}
/**********************************************************/
void IF_UserParam_SetPulseFrequency(uint32_t value)
{
	/*****检查占空比周期，高电平时间小于15us*******/
	if(10000*IF_UserParam_GetPulseDutyCircle()/value < 15)  
	{
		IF_UserParam_SetPulseDutyCircle(15*value/10000);	
	}
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_UserParam.PulseFreq = value;
		cmdActionID[ID_SAVE_FS_USER_PARAM]= ON;		
	}
	UserParam.PulseFreq = value;
	cmdActionID[ID_SAVE_USER_PARAM] = ON;			
}
uint32_t IF_UserParam_GetPulseFrequency(void)
{
	uint32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		value = Fs_UserParam.PulseFreq;
	}else
	{
		value = UserParam.PulseFreq;	
	}
	return value;
}
/**********************************************************/
void IF_UserParam_SetPulseDutyCircle(uint8_t value)
{ 
	/*****************检查占空比周期<15us***************************/
	if(10000*value/IF_UserParam_GetPulseFrequency()<15)
	{	
		value = FloatToUINT16(0.0015F*IF_UserParam_GetPulseFrequency());
	}
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_UserParam.PulseDuty = value;
		cmdActionID[ID_SAVE_FS_USER_PARAM]= ON;	
	}
	UserParam.PulseDuty = value;
	cmdActionID[ID_SAVE_USER_PARAM] = ON;		
}

uint8_t IF_UserParam_GetPulseDutyCircle(void)
{
	uint8_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		value = Fs_UserParam.PulseDuty;
	}else
	{
		value = UserParam.PulseDuty;	
	}
	return value;
}
/**********************************************************/
void IF_UserParam_SetVDCFactor(uint32_t value)
{
	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_UserParam.VDCFactor = value;
		cmdActionID[ID_SAVE_FS_USER_PARAM]= ON;	
	}
	UserParam.VDCFactor = value;
	cmdActionID[ID_SAVE_USER_PARAM] = ON;	
}

uint32_t IF_UserParam_GetVDCFactor(void)
{
	uint32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_UserParam.VDCFactor;
	}else
	{
		value = UserParam.VDCFactor;
	}
	return value;
}
/**********************************************************/
void IF_UserParam_SetMatchMode(uint8_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_UserParam.MatchMode = value;
		cmdActionID[ID_SAVE_FS_USER_PARAM]= ON;		
	}
	UserParam.MatchMode = value;
	cmdActionID[ID_SAVE_USER_PARAM] = ON;	
}

uint8_t IF_UserParam_GetMatchMode(void)
{
	uint8_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_UserParam.MatchMode;
	}else
	{
		value = UserParam.MatchMode;
	}
	return value;
}
/**********************************************************/
void IF_UserParam_SetDCBiasPowerLimit(uint16_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_UserParam.PvdcLimit = value;
		cmdActionID[ID_SAVE_FS_USER_PARAM]= ON;		
	}
	UserParam.PvdcLimit = value;
	cmdActionID[ID_SAVE_USER_PARAM] = ON;	
}
uint16_t IF_UserParam_GetDCBiasPowerLimit(void)
{
	uint16_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_UserParam.PvdcLimit;
	}else
	{
		value = UserParam.PvdcLimit;
	}
	return value;
}
/**********************************************************/
void IF_UserParam_SetSlowMode(uint8_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_UserParam.SlowMode = value;
		cmdActionID[ID_SAVE_FS_USER_PARAM]= ON;		
	}
	UserParam.SlowMode = value;
	cmdActionID[ID_SAVE_USER_PARAM] = ON;	
}

uint8_t IF_UserParam_GetSlowMode(void)
{
	uint8_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_UserParam.SlowMode;
	}else
	{
		value = UserParam.SlowMode;
	}
	return value;
}
/**********************************************************/
void IF_UserParam_SetSlowStartDelay(uint16_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		Fs_UserParam.SlowRFOnDelay =value;
		cmdActionID[ID_SAVE_FS_USER_PARAM]= ON;
	}
	UserParam.SlowRFOnDelay =value;
	cmdActionID[ID_SAVE_USER_PARAM]= ON;	
}
uint16_t IF_UserParam_GetSlowStartDelay(void)
{
	uint16_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_UserParam.SlowRFOnDelay;
	}else
	{
		value = UserParam.SlowRFOnDelay;
	}
	return value;
}

void IF_UserParam_SetSlowStopDelay(uint16_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_UserParam.SlowRFOffDelay = value;
		cmdActionID[ID_SAVE_FS_USER_PARAM]= ON;		
	}
	UserParam.SlowRFOffDelay = value;
	cmdActionID[ID_SAVE_USER_PARAM] = ON;	
}

uint16_t IF_UserParam_GetSlowStopDelay(void)
{
	uint16_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_UserParam.SlowRFOffDelay;
	}else
	{
		value = UserParam.SlowRFOffDelay;
	}
	return value;
}
void IF_UserParam_SetPowerOffsetFactor(int32_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_UserParam.PowerOffsetFactor = value;
		cmdActionID[ID_SAVE_FS_USER_PARAM]= ON;	
	}
	UserParam.PowerOffsetFactor = value;
	cmdActionID[ID_SAVE_USER_PARAM] = ON;	
}

int32_t IF_UserParam_GetPowerOffsetFactor(void)
{
	uint8_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_UserParam.PowerOffsetFactor;
	}else
	{
		value = UserParam.PowerOffsetFactor;
	}
	return value;
}	
//-------------------------------------------------------------------------------------------------
//Executable Action

/***********************State Function **********************************/
void IF_CmdParam_SetFactoryMode(uint8_t value)
{
	DeviceParam.FactoryMode = value;
}
uint8_t IF_CmdParam_GetFactoryMode(void)
{
	uint8_t value = 0;
	value = DeviceParam.FactoryMode;
	return value;
}
//-----------------------------------------------------------------------------
void IF_CmdParam_SetResetDevice(void)
{
	cmdActionID[ID_SET_DEVICERESET]= ON;	
}
//---------------------------------------------------------------
void IF_CmdParam_ResumeFactorySettings(void)
{
	memcpy(&PartsParam,   &Fs_PartsParam,   PARTS_PARAM_LEN);	
	memcpy(&CommonParam,  &Fs_CommonParam,  COMMUN_PARAM_LEN);	
	memcpy(&InternalParam,&Fs_InternalParam,INTERNAL_PARAM_LEN);	
	memcpy(&UserParam,    &Fs_UserParam,    USER_PARAM_LEN);
	cmdActionID[ID_RESUME_ALLPARAMS]= ON;	
}
//---------------------------------------------------------------
uint8_t IF_CmdParam_GetRFPowerState(void)
{
	return DeviceParam.RFPowerState;
}
void IF_CmdParam_SetRFPowerState(uint8_t value)
{
	DeviceParam.RFPowerState  = value;	
}
//---------------------------------------------------------------
uint8_t IF_CmdParam_GetDDSDriverState(void)
{
	return DeviceParam.DDSSignState;		
}	
void IF_CmdParam_SetDDSDriverState(uint8_t value)
{
	DeviceParam.DDSSignState  = value;
}
//---------------------------------------------------------------
uint32_t IF_CmdParam_GetACDCDriverState(void)
{
	return DeviceParam.SetACDCState;  
}
void IF_CmdParam_SetACDCDriverState(uint32_t value)
{
	DeviceParam.SetACDCState  = value;
}

//---------------------------------------------------------------
uint8_t IF_CmdParam_GetSensorChannelNo(void)
{
	return DeviceParam.SetChannelNo;  
}
void IF_CmdParam_SetSensorChannelNo(uint8_t value)
{
	DeviceParam.SetChannelNo  = value;
}
//---------------------------------------------------------------
void IF_CmdParam_SetPowerWorkMode(uint8_t value)
{
	DeviceParam.PowerWorkMode = value;
}
uint8_t IF_CmdParam_GetPowerWorkMode(void)
{
	return DeviceParam.PowerWorkMode;
}
//---------------------------------------------------------------
uint32_t IF_CmdParam_GetACDCVoltage(void)
{
	return DeviceParam.SetACDCVolt;  
}
void IF_CmdParam_SetACDCVoltage(uint32_t value)
{
	DeviceParam.SetACDCVolt  = value;
}
//---------------------------------------------------------------
uint32_t IF_CmdParam_GetACDCCurrent(void)
{
	return DeviceParam.SetACDCCurrent;  
}
void IF_CmdParam_SetACDCCurrent(uint32_t value)
{
	DeviceParam.SetACDCCurrent  = value;	
}
//---------------------------------------------------------------
uint32_t IF_CmdParam_GetWorkFrequency(void)
{
	return DeviceParam.SetWorkFreq;  
}
void IF_CmdParam_SetWorkFrequency(uint32_t value)
{
	DeviceParam.SetWorkFreq  = value;
}
//---------------------------------------------------------------
uint32_t IF_CmdParam_GetWorkPhase(void)
{
	return DeviceParam.SetWorkPhase;  
}
void IF_CmdParam_SetWorkPhase(uint32_t value)
{
	DeviceParam.SetWorkPhase  = value;	
}
//---------------------------------------------------------------
void IF_CmdParam_SetSetPoint(uint16_t value)
{
	DeviceParam.SetPointValue = value;		
}
uint16_t IF_CmdParam_GetSetPoint(void)
{
	return DeviceParam.SetPointValue;
}
//-----------------------------------------------------------------
uint16_t IF_CmdParam_GetMatchCurrentPos(uint8_t capIndex)
{
	return DeviceParam.CurrentPos[capIndex];
}
//-----------------------------------------------------------------
uint16_t IF_CmdParam_GetMatchMoveToPos(uint8_t capIndex)
{
	return DeviceParam.TargetPos[capIndex];
}
void IF_Cmd_SetMatchMoveToPos(uint8_t capIndex, uint16_t value)
{
	DeviceParam.TargetPos[capIndex] =value;
	switch(capIndex)
	{
		case LOAD:cmdActionID[ID_MOVE_LOADTOPOS]= ON;break;
		case TUNE:cmdActionID[ID_MOVE_TUNETOPOS]= ON;break;
	}	
}

/*************************Process Data Function************************************************/
ProcessData_t g_ProcessData;
static uint16_t lenOneFrame = PD_ONE_RECORD_LEN * PD_RECORD_NUM_EACH_FRAME;
/* FUNCTION *******************************************************************
 *
 * Function Name : IF_UpdateAMPD
 * Description   : 
 *
 * END ***********************************************************************/
void IF_UpdateAMPD(void)
{
	 if(g_ProcessData.RecordNum == 0)
	 {
		g_ProcessData.Records=(PDORecord_t *)malloc(sizeof(PDORecord_t)*MAX_PD_RECORD_NUM);
		if(g_ProcessData.Records==NULL)return;
	 }	 
	 if (g_ProcessData.RecordNum < MAX_PD_RECORD_NUM)
	 {	
		g_ProcessData.Records[g_ProcessData.RecordNum].ACDCVolt = IF_CmdParam_GetACDCVoltage();
		g_ProcessData.Records[g_ProcessData.RecordNum].Pfwd = 1000*IF_Fpga_GetSensor(ChnN_Pfwd);
		g_ProcessData.Records[g_ProcessData.RecordNum].Pref = 1000*IF_Fpga_GetSensor(ChnN_Pref);
		g_ProcessData.Records[g_ProcessData.RecordNum].vswr = 1000*IF_Fpga_GetSensor(ChnN_VSWR);
		g_ProcessData.RecordNum++;
	 }
}

void IF_ClearAMPD(void)
{
    memset(&g_ProcessData, 0, PROCESS_DATA_LEN);
	free(g_ProcessData.Records);
}
uint32_t IF_GetTotalAMPDLength(void)
{
	return (2 + g_ProcessData.RecordNum * PD_ONE_RECORD_LEN);
}
uint16_t IF_GetAMPDRecordNum(void)
{
    return g_ProcessData.RecordNum;
}
//pBuf: 一帧AMPD数据的首地址
//返回: 一帧AMPD数据的长度（不带帧号）
uint16_t IF_GetAMPD(uint16_t frameNo, uint8_t* pBuf)
{
	uint16_t len;
	uint16_t recordIndex;

	recordIndex = frameNo * PD_RECORD_NUM_EACH_FRAME;
	if (recordIndex > g_ProcessData.RecordNum)
	{
		return 0;
	}

	if (frameNo == 0)
	{	//首帧处理
		if (g_ProcessData.RecordNum >= PD_RECORD_NUM_EACH_FRAME)
		{//如果一帧传不完，那么首帧就传2字节记录数（RecordNum）
		 //外加PD_RECORD_NUM_EACH_FRAME条数据        
			len = lenOneFrame + 2;
		}
		else
		{//如果一帧就传完了，那么长度就等于实际的帧条数，外加2字节记录数（RecordNum）
			len = PD_ONE_RECORD_LEN * g_ProcessData.RecordNum + 2;
		}
        //返回值len也是要传递的数据长度
		memcpy(pBuf, (uint8_t*)&g_ProcessData, len);
	}
	else
	{
		if (g_ProcessData.RecordNum >= (recordIndex + PD_RECORD_NUM_EACH_FRAME))
		{	//中间帧数据，每次长度都是一个lenOneFrame = PD_ONE_RECORD_LEN * PD_RECORD_NUM_EACH_FRAME
			len = lenOneFrame;
		}
		else
		{	//最后一帧数据
			len = PD_ONE_RECORD_LEN * (g_ProcessData.RecordNum - recordIndex);
		}

		memcpy(pBuf, (uint8_t*)&g_ProcessData.Records[recordIndex], len);
	}

	return len;
}
//-------------------------------------------------------------------------------------------------
//DataBlock Parameters Operations
///////////////////////////////////////////////////////////////////////////////////////////
void IF_NvmParam_SetPartsParams(uint8_t  *pBuf, uint16_t len)
{
	if(len>PARTS_PARAM_LEN)len = PARTS_PARAM_LEN;
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		memcpy((uint8_t*)&Fs_PartsParam,pBuf,len); 
		cmdActionID[ID_SAVE_FS_PARTS_PARAM] = ON;		
	}
	memcpy((uint8_t*)&PartsParam,pBuf,len); 
	cmdActionID[ID_SAVE_PARTS_PARAM]	=ON;		
}
//-------------------------------------------------------------------------------------------------
void IF_NvmParam_GetPartsParams(uint8_t  *pBuf, uint16_t len)
{	
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		memcpy(pBuf,(uint8_t *)&Fs_PartsParam,len);	
	}
	else
	{
		memcpy(pBuf,(uint8_t *)&PartsParam,len);	
	}
}

//-------------------------------------------------------------------------------------------------
void IF_NvmParam_SetInternalParams(uint8_t  *pBuf, uint16_t len)
{
	if(len>INTERNAL_PARAM_LEN)len = INTERNAL_PARAM_LEN;
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		memcpy((uint8_t*)&Fs_InternalParam,pBuf,len); 
		cmdActionID[ID_SAVE_FS_INTERNAL_PARAM] = ON;		
	}
	memcpy((uint8_t*)&InternalParam,pBuf,len);  
	cmdActionID[ID_SAVE_INTERNAL_PARAM]	=ON;
}

void IF_NvmParam_GetInternalParams(uint8_t  *pBuf, uint16_t len)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		memcpy(pBuf,&Fs_InternalParam,len);
	}
	else
	{
		memcpy(pBuf,&InternalParam,len);
	}
}

//-------------------------------------------------------------------------------------------------
void IF_NvmParam_SetUserParams(uint8_t  *pBuf, uint16_t len)
{
	if(len>USER_PARAM_LEN)len = USER_PARAM_LEN;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		memcpy((uint8_t*)&Fs_UserParam,pBuf,len); 
		cmdActionID[ID_SAVE_FS_USER_PARAM] = ON;		
	}
	memcpy((uint8_t*)&UserParam,pBuf,len);   
	cmdActionID[ID_SAVE_USER_PARAM] = ON;
}

void IF_NvmParam_GetUserParams(uint8_t  *pBuf, uint16_t len)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		memcpy(pBuf,&Fs_UserParam,len);
	}
	else
	{
		memcpy(pBuf,&UserParam,len);
	}
}

/* FUNCTION *******************************************************************
 *
 * Function Name : IF_Param_ExecuteActionsAfterRsp
 * Description   : 命令控制参数的响应
 *
 * END ***********************************************************************/
void IF_Param_ExecuteActionsAfterRsp(void)
{
   DeviceCmdMsg_t DeviceCmd;
   for(uint8_t cmdID= 0;cmdID < ID_SET_ALLNUM;cmdID++)
   {
	   if(cmdActionID[cmdID] == ON)
	   {
		   switch(cmdID)
		   {
			case ID_SAVE_FS_PARTS_PARAM:	
				Check_Fs_PartsParam();
				DeviceCmd.cmdId = CMD_SAVE_FSPARTSPARAM;
				DeviceCmd.cmdData.Val = 0;			
			break;
			case ID_SAVE_FS_COMMON_PARAM: 
				Check_Fs_CommonParam();				
				DeviceCmd.cmdId = CMD_SAVE_FSCOMMONPARAM;
				DeviceCmd.cmdData.Val = 0;			
			break;
			case ID_SAVE_FS_INTERNAL_PARAM: 
				Check_Fs_InternalParam();
				DeviceCmd.cmdId = CMD_SAVE_FSINTERNALPARAM;
				DeviceCmd.cmdData.Val = 0;			
			break;
			case ID_SAVE_FS_USER_PARAM: 
				Check_Fs_UserParam();
				DeviceCmd.cmdId = CMD_SAVE_FSUSERPARAM;
				DeviceCmd.cmdData.Val = 0;			
			break;
			/*****************************************************/
			case ID_READ_FS_PARTS_PARAM:  	
				DeviceCmd.cmdId = CMD_READ_FSPARTSPARAM;
				DeviceCmd.cmdData.Val = 0;			
			break;
			case ID_READ_FS_COMMON_PARAM:	
				DeviceCmd.cmdId = CMD_READ_FSCOMMONPARAM;
				DeviceCmd.cmdData.Val = 0;			
			break;
			case ID_READ_FS_INTERNAL_PARAM: 
				DeviceCmd.cmdId = CMD_READ_FSINTERNALPARAM;
				DeviceCmd.cmdData.Val = 0;
			break;
			case ID_READ_FS_USER_PARAM:		
				DeviceCmd.cmdId = CMD_READ_FSUSERPARAM;	
				DeviceCmd.cmdData.Val = 0;			
			break;
			/*****************************************************/
			case ID_SAVE_PARTS_PARAM:	
				Check_PartsParam();				
				DeviceCmd.cmdId = CMD_SAVE_PARTSPARAM;
				DeviceCmd.cmdData.Val = 0;			
			break;
			case ID_SAVE_COMMON_PARAM:	
				Check_CommonParam();				
				DeviceCmd.cmdId = CMD_SAVE_COMMONPARAM;
				DeviceCmd.cmdData.Val = 0;			
			break;			
			
			case ID_SAVE_INTERNAL_PARAM:
				Check_InternalParam();					
				DeviceCmd.cmdId = CMD_SAVE_INTERNALPARAM;
				DeviceCmd.cmdData.Val = 0;			
			break;
			case ID_SAVE_USER_PARAM: 
				Check_UserParam();
				DeviceCmd.cmdId =  CMD_SAVE_USERPARAM;	
				DeviceCmd.cmdData.Val = 0;			
			break;			
           	/*****************************************************/
			case ID_READ_PARTS_PARAM:  	
				DeviceCmd.cmdId = CMD_READ_PARTSPARAM;
				DeviceCmd.cmdData.Val = 0;			
			break;		
			case ID_READ_COMMON_PARAM:		
				DeviceCmd.cmdId = CMD_READ_COMMONPARAM;
				DeviceCmd.cmdData.Val = 0;
			break;
			case ID_READ_INTERNAL_PARAM:	
				DeviceCmd.cmdId = CMD_READ_INTERNALPARAM;
				DeviceCmd.cmdData.Val = 0;			
			break;
			case ID_READ_USER_PARAM:		
				DeviceCmd.cmdId = CMD_READ_USERPARAM;
				DeviceCmd.cmdData.Val = 0;			
			break;	
			/*****************************************************/
			case ID_RESUME_ALLPARAMS:		
				DeviceCmd.cmdId = CMD_RESUME_ALLPARAM;
				DeviceCmd.cmdData.Val = 0;			
			break;
			/*****************************************************/
			case ID_SET_BAUDRATE:			
				DeviceCmd.cmdId = CMD_SETBAUDRATE;
				DeviceCmd.cmdData.Val = IF_CommParam_GetDataRate();			
			break;
			case ID_SET_DEVICERESET:  		
				DeviceCmd.cmdId = CMD_DEVICERESET;
				DeviceCmd.cmdData.Val = 0;				
			break;
			default:break;
			}
			xQueueSend(CmdQueue, &DeviceCmd, (TickType_t)0);
			cmdActionID[cmdID] = OFF;  //关闭状态
	   }
   }		
}

/* FUNCTION *******************************************************************
 *
 * Function Name : IF_NvmParamsRW
 * Description   : 函数实现对非易失存储器读写的功能实现
 * parameter     : NVMParam_Mask --保存NVM的标志掩码
 * END ***********************************************************************/
void IF_NvmParamsRW(NVMRWMask_Enum NVM_Mask)
{
	/***************工厂模式下************************/
	switch((uint32_t )NVM_Mask)
	{	
		/***************工厂模式下读参数*****************************/
		case Read_Fs_PartsMask:		Read_FS_PartsParam((uint8_t *)&Fs_PartsParam);			break;
		case Read_FS_CommonMask:	Read_FS_CommonParam((uint8_t *)&Fs_CommonParam);		break;
		case Read_FS_InternalMask:	Read_FS_InternalParam((uint8_t *)&Fs_InternalParam);	break;
		case Read_FS_UserMask: 		Read_UserParam((uint8_t *)&Fs_UserParam);				break;
		/***************工厂模式下写参数*****************************/
		case Write_Fs_PartsMask: 	Write_FS_PartsParam((uint8_t *)&Fs_PartsParam);			break;	
		case Write_FS_CommonMask: 	Write_FS_CommonParam((uint8_t *)&Fs_CommonParam);		break;
		case Write_FS_InternalMask:	Write_FS_InternalParam((uint8_t *)&Fs_InternalParam);	break;
		case Write_FS_UserMask: 	Write_UserParam((uint8_t *)&Fs_UserParam);				break;	
		/***************正常模式下读参数************************/
		case Read_PartsMask:   		Read_PartsParam((uint8_t *)&PartsParam);      			break;
		case Read_CommonMask:  		Read_CommonParam((uint8_t *)&CommonParam);    			break;
		case Read_InternalMask:		Read_InternalParam((uint8_t *)&InternalParam);			break;
		case Read_UserMask:    		Read_UserParam((uint8_t *)&UserParam);        			break;
		/***************正常模式下写参数************************/
		case Write_PartsMask:   	Write_PartsParam((uint8_t *)&PartsParam);      			break;
		case Write_CommonMask:  	Write_CommonParam((uint8_t *)&CommonParam);	 			break;
		case Write_InternalMask:	Write_InternalParam((uint8_t *)&InternalParam);			break;
		case Write_UserMask:    	Write_UserParam((uint8_t *)&UserParam);					break;
		case Write_AllResumeMask:
			Write_PartsParam((uint8_t *)&PartsParam);		
			Write_CommonParam((uint8_t *)&CommonParam);
			Write_InternalParam((uint8_t *)&InternalParam);
			Write_UserParam((uint8_t *)&UserParam);		
		break;
	}  
}

