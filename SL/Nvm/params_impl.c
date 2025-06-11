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

static uint8_t 	ActionsRSP[ID_SET_ALLNUM];		//控制激活参数
//结构体变量
static PartsParam_t 			Fs_PartsParam;
static CommonParam_t			Fs_CommonParam;			
static InternalParam_t 			Fs_InternalParam;
static UserParam_t 				Fs_UserParam;
 
static PartsParam_t 			PartsParam;
static CommonParam_t			CommonParam;
static InternalParam_t 			InternalParam;
static UserParam_t 				UserParam;
static VoltMapParam_t           VoltMapParam;
static PhaseMapParam_t          PhaseMapParam;	

static CmdCtrlParam_t 			DevCmdParam;

/* GLOBAL VARIABLES */
FaultWord_t 			g_FaultWord;
StatusWord_t 			g_StatusWord;
ParamFactor_t 			g_FactorData;
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
static void Check_VoltMapParam(void);
static void Check_PhaseMapParam(void);
static void ConvertUnit_InternalParam(InternalParam_t *pInternalParam);
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
	memset(&DevCmdParam,0,sizeof(CmdCtrlParam_t));
	
	//DevCmdParam.PowerWorkMode = 1;
	for(uint8_t capIndex = 0;capIndex < CAP_NUM;capIndex++)
	{
		DevCmdParam.TargetPos[capIndex] = 5000;
		DevCmdParam.CurrentPos[capIndex]= 5000;	
	}
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
	Read_VoltageMapParam((uint8_t*)&VoltMapParam);
	Read_PhaseMapParam((uint8_t*)&PhaseMapParam);
	
	Check_PartsParam();
	Check_CommonParam();
	Check_InternalParam();
	Check_UserParam(); 
    Check_VoltMapParam();
    Check_PhaseMapParam();	
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
		Fs_InternalParam.AnalogVoltRange = 10000;
		Fs_InternalParam.AnalogRFPowerFactor = 10000;
		Fs_InternalParam.ACDCCurrent = MAX_ACDC_CURRENT;
		Fs_InternalParam.WorkCenterFreq = CENTER_FREQUENTY;	
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
	if(Fs_InternalParam.DDSChannelNo > 2)
	{
		Fs_InternalParam.DDSChannelNo = 0;
	}
	if(Fs_InternalParam.ACDCCurrent > MAX_ACDC_CURRENT)
	{
		Fs_InternalParam.ACDCCurrent = MAX_ACDC_CURRENT;
	}
	if(Fs_InternalParam.WorkCenterFreq > 2500000)
	{
		Fs_InternalParam.WorkCenterFreq = CENTER_FREQUENTY;
	}	
   ConvertUnit_InternalParam(&Fs_InternalParam);
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
		InternalParam.FeedPreMask = 40;		
		InternalParam.ACDCCurrent = MAX_ACDC_CURRENT;
		InternalParam.WorkCenterFreq = CENTER_FREQUENTY;
		
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
		InternalParam.AnalogRFPowerFactor = 10000;
   }
   if(InternalParam.StartPoint >= MAX_POWER)
   {
		InternalParam.StartPoint = 0;
   }
   if(InternalParam.DDSChannelNo > 2)
	{
		InternalParam.DDSChannelNo = 0;
	}
	if(InternalParam.ACDCCurrent > MAX_ACDC_CURRENT)
	{
		InternalParam.ACDCCurrent = MAX_ACDC_CURRENT;
	}
	if(InternalParam.WorkCenterFreq > 2500000)
	{
		InternalParam.WorkCenterFreq = CENTER_FREQUENTY;
	}	
   ConvertUnit_InternalParam(&InternalParam);
}

static void ConvertUnit_InternalParam(InternalParam_t *pInternalParam)
{
	g_FactorData.VrmsFactor  = pInternalParam->VrmsFactor/(1000.0F);
	g_FactorData.IrmsFactor  = pInternalParam->IrmsFactor/(1000.0F);
	g_FactorData.PhaseFactor = pInternalParam->PhaseFactor/1000.0F;
	
	g_FactorData.VrmsOffset  = pInternalParam->VrmsOffset/(1000.0F);
	g_FactorData.IrmsOffset  = pInternalParam->IrmsOffset/(1000.0F);
	g_FactorData.PhaseOffset = pInternalParam->PhaseOffset/1000.0F;
	
	g_FactorData.DrainGain   = (0.9877F *pInternalParam->DrainVoltGain)/(1000.0F*2000.0F);
	g_FactorData.DrainOffset = pInternalParam->DrainVoltOffset/1000.0F;
	
	g_FactorData.ACDCVoltGain = pInternalParam->ACDCVoltGain/6941.25F;
	g_FactorData.ACDCVoltOffset = (1000*pInternalParam->ACDCVoltOffset)/1000.0F;
	
	g_FactorData.ACDCCurrentGain   = 11252.1977F/pInternalParam->ACDCCurrentGain;
	g_FactorData.ACDCCurrentOffset = pInternalParam->ACDCCurrentOffset/1000.0F;
	
	g_FactorData.VSWRLimit = pInternalParam->VSWRLimit/1000.0F;
	
	g_FactorData.PhaseKp  = pInternalParam->PhasePIDProportion/1000.0F;
	g_FactorData.PhaseKi  = pInternalParam->PhasePIDIntegral/1000.0F;
	g_FactorData.PhaseKd  = pInternalParam->PhasePIDDerivatice/1000.0F;
	g_FactorData.PhaseEThr= pInternalParam->PhasePIDErrorThr/1000.0F;
	
	g_FactorData.VoltKp  = pInternalParam->VoltPIDProportion/1000.0F;
	g_FactorData.VoltKi  = pInternalParam->VoltPIDIntegral/1000.0F;
	g_FactorData.VoltKd  = pInternalParam->VoltPIDDerivatice/1000.0F;
	g_FactorData.VoltEThr= pInternalParam->VoltPIDErrorThr/1000.0F;
	/*********************************************************************************/	
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
		Fs_UserParam.SlowRFOnDelay = 10;
		Fs_UserParam.SlowRFOffDelay = 10;
		Fs_UserParam.VDCFactor = MAX_VDCATTEN;
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
	if(Fs_UserParam.PrefMode > 1) 
	{
		Fs_UserParam.PrefMode = 0;
	}		
	if(Fs_UserParam.PrefDelayOff > MAX_REFLECTDELAY) 
	{
		Fs_UserParam.PrefDelayOff = 15;
	}
	if(Fs_UserParam.SlowRFOnDelay > MAX_SLOWDELAY) 
	{
		Fs_UserParam.SlowRFOnDelay = 10;
	}
	if(Fs_UserParam.SlowRFOffDelay > MAX_SLOWDELAY) 
	{
		Fs_UserParam.SlowRFOffDelay = 10;
	}
	if(Fs_UserParam.VDCFactor > MAX_VDCATTEN)
	{
		Fs_UserParam.VDCFactor = MAX_VDCATTEN;
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
		UserParam.SlowRFOnDelay = 10;
		UserParam.SlowRFOffDelay = 10;
		UserParam.VDCFactor = MAX_VDCATTEN;
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

	if(UserParam.PrefMode > 1) 
	{
		UserParam.PrefMode = 0;
	}		
	if(UserParam.PrefDelayOff > MAX_REFLECTDELAY) 
	{
		UserParam.PrefDelayOff = 15;
	}
	if(UserParam.SlowRFOnDelay > MAX_SLOWDELAY) 
	{
		UserParam.SlowRFOnDelay = 10;
	}
	if(UserParam.SlowRFOffDelay > MAX_SLOWDELAY) 
	{
		UserParam.SlowRFOffDelay = 10;
	}
	if(UserParam.VDCFactor > MAX_VDCATTEN)
	{
		UserParam.VDCFactor = MAX_VDCATTEN;
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
/* FUNCTION ***********************************************************************
 * Function Name : Check_VoltMapParam
 * Description   : 检查电压映射参数
 * Parameter     : 
 * return        :               
 * END ***************************************************************************/
static void Check_VoltMapParam(void)
{	
    if(VoltMapParam.len >= MAX_RFPWRVOL_NUM)
    {
		VoltMapParam.len = MAX_RFPWRVOL_NUM;
	}	
	for(uint16_t i = 0;i< MAX_RFPWRVOL_NUM;i++)
	{
		if(VoltMapParam.Volt[i] > MAX_ACDC_VOLT)
		{
			VoltMapParam.Volt[i] = 0;
		}
	}
}
/* FUNCTION ***********************************************************************
 * Function Name : Check_PhaseMapParam
 * Description   : 检查相位映射参数
 * Parameter     : 
 * return        :               
 * END ***************************************************************************/
static void Check_PhaseMapParam(void)
{	
	if(PhaseMapParam.len >= MAX_RFPWRPHS_NUM)
    {
		PhaseMapParam.len = MAX_RFPWRPHS_NUM;
	}
	for(uint16_t i = 0;i < MAX_RFPWRPHS_NUM;i++)
	{
		if(PhaseMapParam.power[i] > MAX_MAP_POWER)
		{
			PhaseMapParam.power[i] = MAX_MAP_POWER;
		}
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
		ActionsRSP[ID_SAVE_FS_PARTS_PARAM]= ON;	
	}
	PartsParam.DeviceType = value;
	ActionsRSP[ID_SAVE_PARTS_PARAM]= ON;			
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
		ActionsRSP[ID_SAVE_FS_PARTS_PARAM]= ON;	
	}	
	PartsParam.VendorCode = value;
	ActionsRSP[ID_SAVE_PARTS_PARAM]= ON;	
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
		ActionsRSP[ID_SAVE_FS_PARTS_PARAM]= ON;	
	}	
	memset(PartsParam.HardwareVersion, 0, HARDWARE_VERSION_LEN);
	memcpy(PartsParam.HardwareVersion, pBuf,len);
	ActionsRSP[ID_SAVE_PARTS_PARAM]= ON;	
}

uint8_t IF_IdentParam_GetHardwareVersion(uint8_t* pBuf)
{
	uint8_t i;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		for (i = 0; i < HARDWARE_VERSION_LEN; i++)
		{
			if (Fs_PartsParam.HardwareVersion[i] == 0)break;
			pBuf[i] = Fs_PartsParam.HardwareVersion[i];
		}
	}else
	{
		for (i = 0; i < HARDWARE_VERSION_LEN; i++)
		{
			if (PartsParam.HardwareVersion[i] == 0)break;
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
	if(len >= PRODUCT_MODEL_LEN)  len = PRODUCT_MODEL_LEN;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		memset(Fs_PartsParam.ProductModel, 0, PRODUCT_MODEL_LEN);
		memcpy(Fs_PartsParam.ProductModel, pBuf, len);
		ActionsRSP[ID_SAVE_FS_PARTS_PARAM]=ON;	
	}
	memset(PartsParam.ProductModel, 0, PRODUCT_MODEL_LEN);
	memcpy(PartsParam.ProductModel, pBuf,len);
	ActionsRSP[ID_SAVE_PARTS_PARAM]=ON;		
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
	if(len >= SERIAL_NUMBER_LEN)len = SERIAL_NUMBER_LEN;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		memset(Fs_PartsParam.SerialNumber, 0, SERIAL_NUMBER_LEN);
		memcpy(Fs_PartsParam.SerialNumber, pBuf,len);
		ActionsRSP[ID_SAVE_FS_PARTS_PARAM]= ON;	
	}
	memset(PartsParam.SerialNumber, 0, SERIAL_NUMBER_LEN);
	memcpy(PartsParam.SerialNumber, pBuf,len);
	ActionsRSP[ID_SAVE_PARTS_PARAM]= ON;		
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
	if(len >= TRACKING_NUMBER_LEN)len = TRACKING_NUMBER_LEN;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		memset(Fs_PartsParam.TrackingNumber, 0, TRACKING_NUMBER_LEN);
		memcpy(Fs_PartsParam.TrackingNumber, pBuf,len );
		ActionsRSP[ID_SAVE_FS_PARTS_PARAM]= ON;	
	}
	memset(PartsParam.TrackingNumber, 0, TRACKING_NUMBER_LEN);
	memcpy(PartsParam.TrackingNumber, pBuf,len);
	ActionsRSP[ID_SAVE_PARTS_PARAM]= ON;	
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
		ActionsRSP[ID_SAVE_FS_COMMON_PARAM]= ON;		
	}
	CommonParam.CommunicateType = value;
	ActionsRSP[ID_SAVE_COMMON_PARAM] = ON;
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
		ActionsRSP[ID_SAVE_FS_COMMON_PARAM]=ON;	
	}
	CommonParam.DeviceAddress = value;
	ActionsRSP[ID_SAVE_COMMON_PARAM]= ON;		
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
		ActionsRSP[ID_SAVE_FS_COMMON_PARAM]= 1;	
	}		
	memcpy(CommonParam.MAC, pBuf, 6);
	ActionsRSP[ID_SAVE_COMMON_PARAM]= 1;			
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
		ActionsRSP[ID_SAVE_FS_COMMON_PARAM]= ON;	
	}	
	CommonParam.IP = value;
	ActionsRSP[ID_SAVE_COMMON_PARAM]= ON;		
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
		
		ActionsRSP[ID_SAVE_FS_COMMON_PARAM]= ON;	
	}
	CommonParam.Mask = value;
	ActionsRSP[ID_SAVE_COMMON_PARAM]= ON;			
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
		ActionsRSP[ID_SAVE_FS_COMMON_PARAM]=ON;	
	}
	CommonParam.Gateway = value;
	ActionsRSP[ID_SAVE_COMMON_PARAM]=ON;			
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
		ActionsRSP[ID_SAVE_FS_COMMON_PARAM]= ON;	
	}
	CommonParam.DataRate = value;
	ActionsRSP[ID_SET_BAUDRATE]= ON;		
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
		ActionsRSP[ID_SAVE_FS_COMMON_PARAM]= ON;	
	}
	CommonParam.EtherCatAddress = value;
	ActionsRSP[ID_SAVE_COMMON_PARAM]= ON;		
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
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.VrmsFactor = value; 
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;			
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
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.IrmsFactor = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;		
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
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.PhaseFactor = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;			
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

void IF_InternalParam_SetSensorVrmsOffset(int32_t value)
{
	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.VrmsOffset = value; 	
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.VrmsOffset = value; 
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;			
}

int32_t IF_InternalParam_GetSensorVrmsOffset(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.VrmsOffset;
	}else
	{
		value = InternalParam.VrmsOffset;
	}
	return value;
}

void IF_InternalParam_SetSensorIrmsOffset(int32_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.IrmsOffset = value;
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.IrmsOffset = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;		
}

int32_t IF_InternalParam_GetSensorIrmsOffset(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.IrmsOffset;
	}else
	{
		value = InternalParam.IrmsOffset;
	}
	return value;
}

void IF_InternalParam_SetSensorPhaseOffset(int32_t value)
{	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.PhaseOffset = value;
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.PhaseOffset = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;			
}

int32_t IF_InternalParam_GetSensorPhaseOffset(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.PhaseOffset;
	}else
	{
		value = InternalParam.PhaseOffset;
	}
	return value;
}
void IF_InternalParam_SetAnalogVoltRange(uint16_t value)
{
	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.AnalogVoltRange = value;
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]=ON;	
	}
	InternalParam.AnalogVoltRange = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;			
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
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.AnalogRFPowerFactor = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;		
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
void IF_InternalParam_SetPhasePIDProportion(int32_t value)
{
	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.PhasePIDProportion = value;
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.PhasePIDProportion = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;		
}
int32_t IF_InternalParam_GetPhasePIDProportion(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.PhasePIDProportion;
	}else
	{
		value = InternalParam.PhasePIDProportion;
	}
	return value;
}
void IF_InternalParam_SetPhasePIDIntegral(int32_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.PhasePIDIntegral = value;
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.PhasePIDIntegral = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;		
}
int32_t IF_InternalParam_GetPhasePIDIntegral(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.PhasePIDIntegral;
	}else
	{
		value = InternalParam.PhasePIDIntegral;
	}
	return value;
}
void IF_InternalParam_SetPhasePIDDerivatice(int32_t value)
{	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.PhasePIDDerivatice = value;
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.PhasePIDDerivatice = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;		
}
int32_t IF_InternalParam_GetPhasePIDDerivatice(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.PhasePIDDerivatice;
	}else
	{
		value = InternalParam.PhasePIDDerivatice;
	}
	return value;
}
void IF_InternalParam_SetPhasePIDErrorThr(int32_t value)
{	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.PhasePIDErrorThr = value;
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.PhasePIDErrorThr = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;
}
int32_t IF_InternalParam_GetPhasePIDErrorThr(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.PhasePIDErrorThr;
	}else
	{
		value = InternalParam.PhasePIDErrorThr;
	}
	return value;
}

void IF_InternalParam_SetVoltPIDProportion(int32_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.VoltPIDProportion = value;
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.VoltPIDProportion = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;		
}
int32_t IF_InternalParam_GetVoltPIDProportion(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.VoltPIDProportion;
	}else
	{
		value = InternalParam.VoltPIDProportion;
	}
	return value;
}
void IF_InternalParam_SetVoltPIDIntegral(int32_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.VoltPIDIntegral = value;
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.VoltPIDIntegral = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;		
}
int32_t IF_InternalParam_GetVoltPIDIntegral(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.VoltPIDIntegral;
	}else
	{
		value = InternalParam.VoltPIDIntegral;
	}
	return value;
}
void IF_InternalParam_SetVoltPIDDerivatice(int32_t value)
{	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.VoltPIDDerivatice = value;
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.VoltPIDDerivatice = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;		
}
int32_t IF_InternalParam_GetVoltPIDDerivatice(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.VoltPIDDerivatice;
	}else
	{
		value = InternalParam.VoltPIDDerivatice;
	}
	return value;
}
void IF_InternalParam_SetVoltPIDErrorThr(int32_t value)
{	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.VoltPIDErrorThr = value;
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.VoltPIDErrorThr = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;
}
int32_t IF_InternalParam_GetVoltPIDErrorThr(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.VoltPIDErrorThr;
	}else
	{
		value = InternalParam.VoltPIDErrorThr;
	}
	return value;
}
//Internal ACDCVoltGain  param
void IF_InternalParam_SetACDCVoltGain(int32_t value)
{	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.ACDCVoltGain = value;
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.ACDCVoltGain = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;
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
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.ACDCVoltOffset = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;
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
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.ACDCCurrentGain = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;
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
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.ACDCCurrentOffset = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;
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
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.DrainVoltGain = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;
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
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.DrainVoltOffset = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;
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
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]=ON;	
	}
	InternalParam.VSWRLimit = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;	
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
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.StartPoint = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;		
}

int32_t IF_InternalParam_GetPhasePoint(void)
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
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.FpgaPulsePowerThr = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM] = ON;		
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
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.FpgaPulseSyncDelay = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM] = ON;		
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
//---------------------------------------------------------------
uint8_t IF_InternalParam_GetDDSChannelNo(void)
{
	uint8_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.DDSChannelNo;
	}else
	{
		value = InternalParam.DDSChannelNo;
	}
	return value; 
}
void IF_InternalParam_SetDDSChannelNo(uint8_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.DDSChannelNo = value;
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.DDSChannelNo = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM] = ON;
}
//---------------------------------------------------------------
uint32_t IF_InternalParam_GetDDSCenterFreq(void)
{
	uint32_t value = 0;
	
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.WorkCenterFreq;
	}else
	{
		value = InternalParam.WorkCenterFreq;
	}
	return value;  
}
void IF_InternalParam_SetWorkCenterFreq(uint32_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.WorkCenterFreq = value;
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.WorkCenterFreq = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM] = ON;	
}

//---------------------------------------------------------------
uint32_t IF_InternalParam_GetACDCCurrent(void)
{
	uint32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.ACDCCurrent;
	}else
	{
		value = InternalParam.ACDCCurrent;
	}
	return value;  
}
void IF_InternalParam_SetACDCCurrent(uint32_t value)
{	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.ACDCCurrent  = value;
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.ACDCCurrent  = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM] = ON;	
}

//---------------------------------------------------------------
uint8_t IF_InternalParam_GetFeedCollectionMode(void)
{
	uint8_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.FeedCollectionMode;
	}else
	{
		value = InternalParam.FeedCollectionMode;
	}
	return value;  
}
void IF_InternalParam_SetFeedCollectionMode(uint8_t value)
{	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.FeedCollectionMode  = value;
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.FeedCollectionMode  = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM] = ON;	
}
//---------------------------------------------------------------
uint32_t IF_InternalParam_GetFeedPreMask(void)
{
	uint32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.FeedPreMask;
	}else
	{
		value = InternalParam.FeedPreMask;
	}
	return value;  
}
void IF_InternalParam_SetFeedPreMask(uint32_t value)
{	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.FeedPreMask  = value;
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.FeedPreMask  = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM] = ON;	
}
//---------------------------------------------------------------
uint32_t IF_InternalParam_GetFeedPostMask(void)
{
	uint32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.FeedPostMask;
	}else
	{
		value = InternalParam.FeedPostMask;
	}
	return value;  
}
void IF_InternalParam_SetFeedPostMask(uint32_t value)
{	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.FeedPostMask  = value;
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.FeedPostMask  = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM] = ON;	
}

//---------------------------------------------------------------
int32_t IF_InternalParam_GetPhaseState2(void)
{
	uint32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.PhaseState2;
	}else
	{
		value = InternalParam.PhaseState2;
	}
	return value;  
}
void IF_InternalParam_SetPhaseState2(int32_t value)
{	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.PhaseState2  = value;
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.PhaseState2  = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM] = ON;	
}
//---------------------------------------------------------------
uint32_t IF_InternalParam_GetPhaseStepSpeed(void)
{
	uint32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.PhaseStepSpeed;
	}else
	{
		value = InternalParam.PhaseStepSpeed;
	}
	return value;  
}
void IF_InternalParam_SetPhaseStepSpeed(uint32_t value)
{	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.PhaseStepSpeed  = value;
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.PhaseStepSpeed  = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM] = ON;	
}
//---------------------------------------------------------------
uint32_t IF_InternalParam_GetPhaseStepTimer(void)
{
	uint32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_InternalParam.PhaseStepTimer;
	}else
	{
		value = InternalParam.PhaseStepTimer;
	}
	return value;  
}
void IF_InternalParam_SetPhaseStepTimer(uint32_t value)
{	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_InternalParam.PhaseStepTimer  = value;
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM]= ON;	
	}
	InternalParam.PhaseStepTimer  = value;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM] = ON;	
}
//-------------------------------------------------------------------------------------------------
//PT Configurable User Parameters
//-----------------------------------------------------------------------------
void IF_UserParam_SetRegulationMode(uint8_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_UserParam.RegulationMode = value;
		ActionsRSP[ID_SAVE_FS_USER_PARAM]= ON;	
	}
	UserParam.RegulationMode = value;
	ActionsRSP[ID_SAVE_USER_PARAM] = ON;
	
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
		ActionsRSP[ID_SAVE_FS_USER_PARAM]= ON;		
	}
	UserParam.PfwdLimit = value;
	ActionsRSP[ID_SAVE_USER_PARAM] = ON;		
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
		ActionsRSP[ID_SAVE_FS_USER_PARAM]= ON;		
	}
	UserParam.PrefLimit = value;
	ActionsRSP[ID_SAVE_USER_PARAM] = ON;
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
		ActionsRSP[ID_SAVE_FS_USER_PARAM]= ON;	
	}
	UserParam.PrefMode = value;
	ActionsRSP[ID_SAVE_USER_PARAM] = ON;	
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
		ActionsRSP[ID_SAVE_FS_USER_PARAM]= ON;	
	}
	UserParam.PrefThr = value;
	ActionsRSP[ID_SAVE_USER_PARAM] = ON;
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
//ReflectPowerDelayOff
/**********************************************************/
void IF_UserParam_SetReflectPowerDelayOff(uint8_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_UserParam.PrefDelayOff = value;
		ActionsRSP[ID_SAVE_FS_USER_PARAM]= ON;	
	}
	UserParam.PrefDelayOff = value;
	ActionsRSP[ID_SAVE_USER_PARAM] = ON;	
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
void IF_UserParam_SetVDCFactor(uint32_t value)
{
	
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_UserParam.VDCFactor = value;
		ActionsRSP[ID_SAVE_FS_USER_PARAM]= ON;	
	}
	UserParam.VDCFactor = value;
	ActionsRSP[ID_SAVE_USER_PARAM] = ON;	
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
		ActionsRSP[ID_SAVE_FS_USER_PARAM]= ON;		
	}
	UserParam.MatchMode = value;
	ActionsRSP[ID_SAVE_USER_PARAM] = ON;	
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
		ActionsRSP[ID_SAVE_FS_USER_PARAM]= ON;		
	}
	UserParam.PvdcLimit = value;
	ActionsRSP[ID_SAVE_USER_PARAM] = ON;	
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
		ActionsRSP[ID_SAVE_FS_USER_PARAM]= ON;		
	}
	UserParam.SlowMode = value;
	ActionsRSP[ID_SAVE_USER_PARAM] = ON;	
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
		ActionsRSP[ID_SAVE_FS_USER_PARAM]= ON;
	}
	UserParam.SlowRFOnDelay =value;
	ActionsRSP[ID_SAVE_USER_PARAM]= ON;	
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
		ActionsRSP[ID_SAVE_FS_USER_PARAM]= ON;		
	}
	UserParam.SlowRFOffDelay = value;
	ActionsRSP[ID_SAVE_USER_PARAM] = ON;	
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
void IF_UserParam_SetVDCOffset(int32_t value)
{
	if(ON==IF_CmdParam_GetFactoryMode())
	{	
		Fs_UserParam.VDCOffset = value;
		ActionsRSP[ID_SAVE_FS_USER_PARAM]= ON;	
	}
	UserParam.VDCOffset = value;
	ActionsRSP[ID_SAVE_USER_PARAM] = ON;	
}

int32_t IF_UserParam_GetVDCOffset(void)
{
	int32_t value = 0;
	if(ON==IF_CmdParam_GetFactoryMode())
	{
		value = Fs_UserParam.VDCOffset;
	}else
	{
		value = UserParam.VDCOffset;
	}
	return value;
}	
//-------------------------------------------------------------------------------------------------
//Executable Action

/***********************State Function **********************************/
void IF_CmdParam_SetFactoryMode(uint8_t value)
{
	DevCmdParam.FactoryMode = value;
}
uint8_t IF_CmdParam_GetFactoryMode(void)
{
	uint8_t value = 0;
	value = DevCmdParam.FactoryMode;
	return value;
}
//-----------------------------------------------------------------------------
void IF_CmdParam_SetResetDevice(void)
{
	ActionsRSP[ID_SET_DEVICERESET]= ON;	
}
//---------------------------------------------------------------
void IF_CmdParam_ResumeFactorySettings(void)
{
	memcpy(&PartsParam,   &Fs_PartsParam,   PARTS_PARAM_LEN);	
	memcpy(&CommonParam,  &Fs_CommonParam,  COMMUN_PARAM_LEN);	
	memcpy(&InternalParam,&Fs_InternalParam,INTERNAL_PARAM_LEN);	
	memcpy(&UserParam,    &Fs_UserParam,    USER_PARAM_LEN);
	ActionsRSP[ID_SAVE_PARTS_PARAM]= ON;
	ActionsRSP[ID_SAVE_COMMON_PARAM]= ON;
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]= ON;
	ActionsRSP[ID_SAVE_USER_PARAM]= ON;		
}
//---------------------------------------------------------------
uint8_t IF_CmdParam_GetRFPowerSwitch(void)
{
	return DevCmdParam.RFPowerState;
}
void IF_CmdParam_SetRFPowerSwitch(uint8_t value)
{
	DevCmdParam.RFPowerState = value;
}
//---------------------------------------------------------------
uint8_t IF_CmdParam_GetDDSDriverSwitch(void)
{
	return DevCmdParam.DDSSignState;		
}	
void IF_CmdParam_SetDDSDriverSwitch(uint8_t value)
{
	DevCmdParam.DDSSignState = value;
}
//---------------------------------------------------------------
uint32_t IF_CmdParam_GetACDCDriverSwitch(void)
{
	return DevCmdParam.SetACDCState;  
}
void IF_CmdParam_SetACDCDriverSwitch(uint32_t value)
{
	DevCmdParam.SetACDCState = value;
}
//---------------------------------------------------------------
void IF_CmdParam_SetPowerWorkMode(uint8_t value)
{
	DevCmdParam.PowerWorkMode = value;
}
uint8_t IF_CmdParam_GetPowerWorkMode(void)
{
	return DevCmdParam.PowerWorkMode;
}
//---------------------------------------------------------------
uint32_t IF_CmdParam_GetACDCVoltage(void)
{
	return DevCmdParam.SetACDCVolt;  
}
void IF_CmdParam_SetACDCVoltage(uint32_t value)
{
	DevCmdParam.SetACDCVolt  = value;
}
//---------------------------------------------------------------
uint32_t IF_CmdParam_GetDDSWorkPhase(void)
{
	return DevCmdParam.DDSWorkPhase;  
}
void IF_CmdParam_SetDDSWorkPhase(uint32_t value)
{
	DevCmdParam.DDSWorkPhase  = value;	
}
//PulseMode
/**********************************************************/
void IF_CmdParam_SetPulseMode(uint8_t value)
{
	DevCmdParam.PulseMode = value;
}
uint8_t IF_CmdParam_GetPulseMode(void)
{
	return  DevCmdParam.PulseMode;
}
/**********************************************************/
void IF_CmdParam_SetPulseFrequency(uint32_t value)
{
	DevCmdParam.PulseFreq = value;				
}
uint32_t IF_CmdParam_GetPulseFrequency(void)
{
	return DevCmdParam.PulseFreq;;
}
/**********************************************************/
void IF_CmdParam_SetPulseDutyCircle(uint8_t value)
{ 
	DevCmdParam.PulseDuty = value;	
}

uint8_t IF_CmdParam_GetPulseDutyCircle(void)
{
	return DevCmdParam.PulseDuty;
}
//SyncMode
void IF_CmdParam_SetSyncSource(uint8_t value)
{
	DevCmdParam.SyncMode = value;
}
uint8_t IF_CmdParam_GetSyncSource(void)
{
	return  DevCmdParam.SyncMode;
}
/**********************************************************/

void IF_CmdParam_SetSyncOutDelay(uint32_t value)
{
	DevCmdParam.SyncOutDelay = value;				
}
uint32_t IF_CmdParam_GetSyncOutDelay(void)
{
	return DevCmdParam.SyncOutDelay;
}
/**********************************************************/
void IF_CmdParam_SetSyncOutEnable(uint8_t value)
{ 
	DevCmdParam.SyncOutEnable = value;	
}

uint8_t IF_CmdParam_GetSyncOutEnable(void)
{
	return DevCmdParam.SyncOutEnable;
}
//---------------------------------------------------------------
void IF_CmdParam_SetPwrPoint(uint16_t value)
{
	DevCmdParam.SetPointValue = value;
}
uint16_t IF_CmdParam_GetPwrPoint(void)
{
	return DevCmdParam.SetPointValue;
}
//-----------------------------------------------------------------
uint16_t IF_CmdParam_GetMatchCurrentPos(uint8_t capIndex)
{
	return DevCmdParam.CurrentPos[capIndex];
}
//-----------------------------------------------------------------
uint16_t IF_CmdParam_GetMatchMoveToPos(uint8_t capIndex)
{
	return DevCmdParam.TargetPos[capIndex];
}
void IF_CmdParam_SetMatchMoveToPos(uint8_t capIndex, uint16_t value)
{
	DevCmdParam.TargetPos[capIndex] =value;
	switch(capIndex)
	{
		case LOAD:ActionsRSP[ID_MOVE_LOADTOPOS]= ON;break;
		case TUNE:ActionsRSP[ID_MOVE_TUNETOPOS]= ON;break;
	}	
}
/**************************NvmFlash  Parameters****************************/
void IF_NvmParam_SetVoltMapMap(int32_t value,uint16_t index)
{	
	VoltMapParam.len = index;	
	VoltMapParam.Volt[index] = value;
	ActionsRSP[ID_SAVE_VOLTMAP_PARAM]= ON;	
}
int32_t IF_NvmParam_GetVoltMapTable(uint16_t index)
{
	uint32_t value = 0;	
    if(index <= VoltMapParam.len)	
	{
		value = VoltMapParam.Volt[index];
	}
	return value;
}
void IF_NvmParam_SetPhaseMapMap(int32_t value,uint16_t index)
{	
	PhaseMapParam.len = index;	
	PhaseMapParam.power[index] = value;
	ActionsRSP[ID_SAVE_PHASEMAP_PARAM]= ON;	
}
int32_t IF_NvmParam_GetPhaseMapTable(int32_t power)
{
	uint32_t PhaseValue = 0;
	uint16_t tabIndex = 0;	
	uint32_t powerMW = power*1000;  //参数功率单位(mw)
	while(tabIndex <= PhaseMapParam.len)
    {
		if(powerMW <= PhaseMapParam.power[tabIndex])break;	
		tabIndex++;
	}
	if(tabIndex > 0)PhaseValue  = (tabIndex-1)*1000;//参数相位单位(1/1000)
	if(PhaseValue >= MAX_FPGA_PHASE)PhaseValue = MAX_FPGA_PHASE;
	return PhaseValue;
}
/*************************Process Data Function************************************************/
ProcessData_t g_ProcessData;
static uint16_t lenOneFrame = PD_ONE_RECORD_LEN * PD_RECORD_NUM_EACH_FRAME;
/* FUNCTION *******************************************************************
 *
 * Function Name : IF_UpdateRFPwrPIDProcessData
 * Description   : 
 *
 * END ***********************************************************************/
void IF_UpdateRFPwrPIDProcessData(void)
{	 
	 if (g_ProcessData.RecordNum < MAX_PD_RECORD_NUM)
	 {	
		if(IF_CmdParam_GetPwrPoint() < IF_InternalParam_GetPhasePoint())
		{
			g_ProcessData.Records[g_ProcessData.RecordNum].ACDCVolt = IF_CmdParam_GetDDSWorkPhase();	
		}else
		{
			g_ProcessData.Records[g_ProcessData.RecordNum].ACDCVolt = IF_CmdParam_GetACDCVoltage();
		}
		g_ProcessData.Records[g_ProcessData.RecordNum].Pfwd = 1000*IF_Fpga_GetSensor(ChnN_Pfwd);
		g_ProcessData.Records[g_ProcessData.RecordNum].Pref = 1000*IF_Fpga_GetSensor(ChnN_Pref);
		g_ProcessData.Records[g_ProcessData.RecordNum].vswr = 1000*IF_Fpga_GetSensor(ChnN_VSWR);
		g_ProcessData.RecordNum++;
	 }
}

void IF_ClearRFPwrPIDProcessData(void)
{
    memset(&g_ProcessData, 0, PROCESS_DATA_LEN);
}
uint32_t IF_GetTotaltRFPwrPIDProcessDataLength(void)
{
	return (2 + g_ProcessData.RecordNum * PD_ONE_RECORD_LEN);
}
uint16_t IF_GetRFPwrPIDProcessDataRecordNum(void)
{
    return g_ProcessData.RecordNum;
}
//pBuf: 一帧AMPD数据的首地址
//返回: 一帧AMPD数据的长度（不带帧号）
uint16_t IF_GetRFPwrPIDProcessData(uint16_t frameNo, uint8_t* pBuf)
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
		ActionsRSP[ID_SAVE_FS_PARTS_PARAM] = ON;		
	}
	memcpy((uint8_t*)&PartsParam,pBuf,len); 
	ActionsRSP[ID_SAVE_PARTS_PARAM]	=ON;		
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
		ActionsRSP[ID_SAVE_FS_INTERNAL_PARAM] = ON;		
	}
	memcpy((uint8_t*)&InternalParam,pBuf,len);  
	ActionsRSP[ID_SAVE_INTERNAL_PARAM]	=ON;
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
		ActionsRSP[ID_SAVE_FS_USER_PARAM] = ON;		
	}
	memcpy((uint8_t*)&UserParam,pBuf,len);   
	ActionsRSP[ID_SAVE_USER_PARAM] = ON;
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
   for(uint8_t Index = 0;Index < ID_SET_ALLNUM;Index++)
   {
	   if(ActionsRSP[Index] == ON)
	   {
		   switch(Index)
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
			case ID_SAVE_VOLTMAP_PARAM: 
				Check_VoltMapParam();
				DeviceCmd.cmdId =  CMD_SAVE_VOLTMAPPARAM;	
				DeviceCmd.cmdData.Val = 0;			
			break;	
			case ID_SAVE_PHASEMAP_PARAM: 
				Check_PhaseMapParam();
				DeviceCmd.cmdId =  CMD_SAVE_PHASEMAPPARAM;	
				DeviceCmd.cmdData.Val = 0;			
			break;			
			case ID_SET_DEVICERESET:  		
				DeviceCmd.cmdId = CMD_DEVICERESET;
				DeviceCmd.cmdData.Val = 0;				
			break;
			case ID_SET_BAUDRATE:			
				DeviceCmd.cmdId = CMD_SETBAUDRATE;
				DeviceCmd.cmdData.Val = IF_CommParam_GetDataRate();			
			break;
			default:break;
			}
			xQueueSend(CmdQueue, &DeviceCmd, (TickType_t)0);
			ActionsRSP[Index] = OFF;
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
		/***************工厂模式写参数*****************************/
		case Write_Fs_PartsMask: 	Write_FS_PartsParam((uint8_t *)&Fs_PartsParam);			break;	
		case Write_FS_CommonMask: 	Write_FS_CommonParam((uint8_t *)&Fs_CommonParam);		break;
		case Write_FS_InternalMask:	Write_FS_InternalParam((uint8_t *)&Fs_InternalParam);	break;
		case Write_FS_UserMask: 	Write_UserParam((uint8_t *)&Fs_UserParam);				break;	

		/***************正常模式写参数************************/
		case Write_PartsMask:   	Write_PartsParam((uint8_t *)&PartsParam);      			break;
		case Write_CommonMask:  	Write_CommonParam((uint8_t *)&CommonParam);	 			break;
		case Write_InternalMask:	Write_InternalParam((uint8_t *)&InternalParam);			break;
		case Write_UserMask:    	Write_UserParam((uint8_t *)&UserParam);					break;
		
		/***************映射表写参数************************/
		case Write_VoltMapMask:     Write_VoltageMapParam((uint8_t*)&VoltMapParam);  		break;
		case Write_PhaseMapMask:    Write_PhaseMapParam((uint8_t*)&PhaseMapParam);   		break;
	}  
}

