/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Nvm.c
 * @Brief :      ref from bsp_motor.c(AMP/AME svn324) Author: Huxian
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-09
 * ****************************************************************************
 */

#include "Nvm.h"
#include "IF_HAL_Cfg.h"
#include "IF_SL_Cfg.h"

/* CONST & MACROS */

/* DATA STRUCTURES */

/* LOCAL VARIABLES */

NvmBuffer_t NvmFlash;
/* GLOBAL VARIABLES */

/* FUNCTION PROTOTYPES */
static uint8_t Write_SystemTotalTimesParam(uint8_t *pBuf);
static uint8_t Read_SystemTotalTimesParam(uint8_t *pBuf);
/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/

/**
 * ************************************************************************
 * @Brief :      
 * ************************************************************************
 */
void IF_NvmInit(void)
{
	Params_Init();
}

/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/

//AM_Flash data saving...

#ifndef BOOT
uint8_t Read_BootloaderVersion(uint8_t* pBuf)
{
	uint8_t i;
	uint8_t data;
	volatile uint32_t addr = 0x08000410;  //BootloaderVersion在Flash中的保存地址

	for (i = 0; i < BOOTLOADER_VERSION_LEN; i++)
	{
		data = *(volatile uint8_t *)(addr + i);
		if (data == 0)
		{
			break;
		}

		pBuf[i] = data;
	}

	return i;
}
#endif

/* FUNCTION **********************************************************************
 * Function Name : IF_NvmParam_EraseDownloadFlag
 * Description   : 擦除Download升级标识
 * Parameter	 :
 * Return        : 1返回成功，0返回失败
 * END ************************************************************************/
uint8_t IF_NvmParam_EraseDownloadFlag(void)
{
    error_status status;

    __disable_irq();

    status = IF_Flash_SectorErase(DOWNLOAD_APP_FLAG_ADDR);

    __enable_irq();

    if (SUCCESS == status)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/* FUNCTION **********************************************************************
 * Function Name : IF_NvmParam_WriteDownloadFlag
 * Description   : 设置Download升级标识为0x11223344
 * Parameter	 :
 * Return        : 1返回成功，0返回失败
 * END ************************************************************************/
uint8_t IF_NvmParam_WriteDownloadFlag(void)
{
    error_status status;

    memset(&NvmFlash, 0,sizeof(NvmBuffer_t));

    NvmFlash.Buf[0] = 0x44;
    NvmFlash.Buf[1] = 0x33;
    NvmFlash.Buf[2] = 0x22;
    NvmFlash.Buf[3] = 0x11;

    __disable_irq();

    status = IF_Flash_write(DOWNLOAD_APP_FLAG_ADDR, (uint16_t *)NvmFlash.Buf, FLASH_SECTOR_SIZE/2);

    __enable_irq();

    if (SUCCESS == status)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//将Download的解密后的文件copy到运行区
void IF_NvmParam_CopyRawFileToRunningArea(void)
{
    uint32_t i;
    uint32_t sectorNum;
    uint32_t codeLen = DOWNLOAD_RAW_FILE_LEN;

    sectorNum = codeLen / FLASH_SECTOR_SIZE;

    if((codeLen % FLASH_SECTOR_SIZE) != 0)
    {
        sectorNum = sectorNum + 1;
    }

    __disable_irq();

    for (i = 0; i < sectorNum; i++)
    {
        memset(&NvmFlash, 0,sizeof(NvmBuffer_t));

        IF_HAL_Flash_read(DOWNLOAD_RAW_APP_START_ADDR + i * FLASH_SECTOR_SIZE, NvmFlash.Buf, FLASH_SECTOR_SIZE);

        IF_HAL_Flash_write(RUNNING_APP_START_ADDR + i * FLASH_SECTOR_SIZE, (uint16_t *)&NvmFlash.Buf, FLASH_SECTOR_SIZE/2);

        IF_HAL_WDOG_FEED();
    }

    __enable_irq();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* FUNCTION *******************************************************************
 *
 * Function Name : IF_NvmParam_EncryptDownloadFile
 * Description   : 解密加密的Download文件，并保存到解密区
 *
 * END ***********************************************************************/
void IF_NvmParam_EncryptDownloadFile(uint32_t encryptionFlag)
{
    uint16_t i, j;
    uint16_t sectorNum;
    uint32_t codeLen = DOWNLOAD_ENCRYPTED_FILE_LEN;
    uint8_t encryptionCode[4] = {0x00};

    sectorNum = codeLen / FLASH_SECTOR_SIZE;

    if((codeLen % FLASH_SECTOR_SIZE) != 0)
    {
        sectorNum = sectorNum + 1;
    }

    switch (encryptionFlag)
    {
    case 0x00000000:
        encryptionCode[0] = 0x00;
        encryptionCode[1] = 0x00;
        encryptionCode[2] = 0x00;
        encryptionCode[3] = 0x00;
        break;
    case 0x00000001:
        encryptionCode[0] = 0x19;
        encryptionCode[1] = 0x04;
        encryptionCode[2] = 0x22;
        encryptionCode[3] = 0x20;
        break;
    default:
        break;
    }

    __disable_irq();

    for (i = 0; i < sectorNum; i++)
    {
        memset(&NvmFlash, 0,sizeof(NvmBuffer_t));

        IF_HAL_Flash_read(DOWNLOAD_ENCRYPTED_APP_START_ADDR + i * FLASH_SECTOR_SIZE,
                NvmFlash.Buf, FLASH_SECTOR_SIZE);

        j = (i == 0) ? 0x200 : 0;

        for (; j < FLASH_SECTOR_SIZE; j++)
        {
            NvmFlash.Buf[j] ^= encryptionCode[j % 4];
        }

        IF_HAL_Flash_write(DOWNLOAD_RAW_APP_START_ADDR + i * FLASH_SECTOR_SIZE,
                (uint16_t *)&NvmFlash.Buf, FLASH_SECTOR_SIZE/2);

        IF_HAL_WDOG_FEED();
    }

    __enable_irq();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* FUNCTION *******************************************************************
 *
 * Function Name : IF_NvmParam_EncryptDownloadFile
 * Description   : Download文件写入到Flash
 *
 * END ***********************************************************************/
uint8_t IF_NvmParam_WriteDownloadFile(uint32_t destAddrOffset, uint16_t* data, uint32_t size)
{
    error_status status;

    __disable_irq();

    status = IF_HAL_Flash_write(DOWNLOAD_ENCRYPTED_APP_START_ADDR + destAddrOffset, data, size);

    __enable_irq();

    if (SUCCESS == status)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t IF_NvmParam_ReadDownloadFileLen(void)
{
	return DOWNLOAD_ENCRYPTED_FILE_LEN;
}

uint32_t Read_EncriptionFlag(void)
{
	return DOWNLOAD_ENCRYPTED_FLAG;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t IF_NvmParam_CheckRunningFile(void)
{
    uint32_t crc32;

    crc32 = STM32_CrcBitByBitFast(CRC32_INIT,
            (uint8_t*)RUNNING_APP_VECTOR_ADDR,
            RUNNING_APP_LEN);

    if (crc32 == RUNNING_APP_CRC)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t IF_NvmParam_CheckDownloadFile(void)
{
    uint32_t crc32;

    crc32 = STM32_CrcBitByBitFast(CRC32_INIT,
            (uint8_t*)(DOWNLOAD_ENCRYPTED_APP_START_ADDR + 4),
            DOWNLOAD_ENCRYPTED_FILE_LEN - 4);

    if (crc32 == DOWNLOAD_ENCRYPTED_FILE_CRC)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t IF_NvmParam_CheckRawDownloadFile(void)
{
    uint32_t crc32;

    crc32 = STM32_CrcBitByBitFast(CRC32_INIT,
            (uint8_t*)DOWNLOAD_RAW_APP_VECTOR_ADDR,
            DOWNLOAD_RAW_APP_LEN);

    if (crc32 == DOWNLOAD_RAW_APP_CRC)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


//AM_Params data saving...
///////////////////////////////////////////////////////////////////////////////////////////////////
/* FUNCTION *******************************************************************
 *
 * Function Name : Write_SystemResetTimes
 * Description   : 写系统复位次数到Flash中
 *
 * END ***********************************************************************/
void Write_SystemResetTimes(uint8_t resetTimes)
{
	uint8_t buf = resetTimes;
	IF_HAL_EEPROM_WriteBytes(SYSTEM_RESET_TIMES_ADDR, &buf, 1);
}

void IF_NvmParam_WriteSystemResetTimes(uint8_t resetTimes)
{
    Write_SystemResetTimes(resetTimes);
}
void IF_NvmParam_WriteSystemTotalTimes(uint32_t deviceTimer)
{
	Write_SystemTotalTimesParam((uint8_t*)&deviceTimer);
}	
uint16_t IF_NvmParam_ReadSystemTotalTimes(void)
{
	uint16_t value = 0;
	Read_SystemTotalTimesParam((uint8_t*)&value);
	if(value==0xFFFF)value= 0;
	return value;
}

uint8_t Read_SystemResetTimes(void)
{
	uint8_t buf;

	IF_HAL_EEPROM_ReadBytes(SYSTEM_RESET_TIMES_ADDR, &buf, 1);

	if (buf == 0xFF)
	{
		buf = 0;
	}

    return buf;
}

/* FUNCTION *******************************************************************
 *
 * Function Name : IF_NvmParam_CheckSystemResetTimes
 * Description   : 检查系统复位次数，连续复位10次则返回1，否则返回0
 *
 * END ***********************************************************************/
uint8_t IF_NvmParam_CheckSystemResetTimes(void)
{
	uint8_t systemResetTimes;

	systemResetTimes = Read_SystemResetTimes();

	systemResetTimes++;
	Write_SystemResetTimes(systemResetTimes);
    delay_ms(10);
	if (systemResetTimes >= SYSTEM_RESET_MAX_TIMES)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t Load_ConfigParam(uint8_t value)
{
	return IF_HAL_EEPROM_ReadBytes(PRESETPARAM_NUM_ADDR, &value, sizeof(uint8_t));
}

uint8_t Write_ConfigParam(uint8_t* pBuf)
{
	return  IF_HAL_EEPROM_WriteBytes(PRESETPARAM_NUM_ADDR, pBuf, sizeof(char));
}

uint8_t Read_FS_PartsParam(uint8_t* pBuf)
{
	return IF_HAL_EEPROM_ReadBytes(FACTORY_PARAM_FS_ADDR, pBuf, PARTS_PARAM_LEN);	
}

uint8_t Write_FS_PartsParam(uint8_t* pBuf)
{
	return IF_HAL_EEPROM_WriteBytes(FACTORY_PARAM_FS_ADDR, pBuf, PARTS_PARAM_LEN);
}
uint8_t Read_FS_CommonParam(uint8_t* pBuf)
{
	
	return IF_HAL_EEPROM_ReadBytes(COMMON_PARAM_FS_ADDR, pBuf, PARTS_PARAM_LEN);	
}

uint8_t Write_FS_CommonParam(uint8_t* pBuf)
{
	return IF_HAL_EEPROM_WriteBytes(COMMON_PARAM_FS_ADDR, pBuf, PARTS_PARAM_LEN);
}
//-----------------------------------------------------------------------------
uint8_t Read_FS_InternalParam(uint8_t* pBuf)
{
	return IF_HAL_EEPROM_ReadBytes(INTERNAL_PARAM_FS_ADDR, pBuf, INTERNAL_PARAM_LEN);	
}

uint8_t Write_FS_InternalParam(uint8_t* pBuf)
{
	return IF_HAL_EEPROM_WriteBytes(INTERNAL_PARAM_FS_ADDR, pBuf, INTERNAL_PARAM_LEN);
}

//-----------------------------------------------------------------------------
uint8_t Read_PartsParam(uint8_t* pBuf)
{
	return IF_HAL_EEPROM_ReadBytes(FACTORY_PARAM_ADDR, pBuf, PARTS_PARAM_LEN);	
}

uint8_t Write_PartsParam(uint8_t* pBuf)
{
	return IF_HAL_EEPROM_WriteBytes(FACTORY_PARAM_ADDR, pBuf, PARTS_PARAM_LEN);
}
//--------------------------------------------------------------------------------

uint8_t Read_CommonParam(uint8_t* pBuf)
{	
	return IF_HAL_EEPROM_ReadBytes(COMMON_PARAM_ADDR, pBuf, INTERNAL_PARAM_LEN);	
}

uint8_t Write_CommonParam(uint8_t* pBuf)
{
	return IF_HAL_EEPROM_WriteBytes(COMMON_PARAM_ADDR, pBuf, INTERNAL_PARAM_LEN);
}
//-----------------------------------------------------------------------------
uint8_t Read_InternalParam(uint8_t* pBuf)
{
		
	return IF_HAL_EEPROM_ReadBytes(INTERNAL_PARAM_ADDR, pBuf, INTERNAL_PARAM_LEN);	
}

uint8_t Write_InternalParam(uint8_t* pBuf)
{
	return IF_HAL_EEPROM_WriteBytes(INTERNAL_PARAM_ADDR, pBuf, INTERNAL_PARAM_LEN);
}
//-----------------------------------------------------------------------------
uint8_t Read_FS_UserParam(uint8_t* pBuf)
{
	return IF_HAL_EEPROM_ReadBytes(USER_PARAM_FS_ADDR, pBuf, USER_PARAM_LEN);;
}

uint8_t Write_FS_UserParam(uint8_t* pBuf)
{
	return IF_HAL_EEPROM_WriteBytes(USER_PARAM_FS_ADDR, pBuf, USER_PARAM_LEN);	
}
//-----------------------------------------------------------------------------
uint8_t Read_UserParam(uint8_t* pBuf)
{
	return IF_HAL_EEPROM_ReadBytes(USER_PARAM_ADDR, pBuf, USER_PARAM_LEN);;
}

uint8_t Write_UserParam(uint8_t* pBuf)
{
	return IF_HAL_EEPROM_WriteBytes(USER_PARAM_ADDR, pBuf, USER_PARAM_LEN);	
}
uint8_t Read_VoltageMapParam(uint8_t* pBuf)
{	
	return IF_HAL_EEPROM_ReadBytes(MAPVSPOWER_ADDR, pBuf,  VOLTMAP_PARAM_LEN);	
}
uint8_t Write_VoltageMapParam(uint8_t* pBuf)
{
	return IF_HAL_EEPROM_WriteBytes(MAPVSPOWER_ADDR, pBuf, VOLTMAP_PARAM_LEN);
}
uint8_t Read_PhaseMapParam(uint8_t* pBuf)
{	
	return IF_HAL_EEPROM_ReadBytes(MAPPHPOWER_ADDR, pBuf,  PHASEMAP_PARAM_LEN);	
}
uint8_t Write_PhaseMapParam(uint8_t* pBuf)
{
	return IF_HAL_EEPROM_WriteBytes(MAPPHPOWER_ADDR, pBuf, PHASEMAP_PARAM_LEN);
}
static uint8_t Read_SystemTotalTimesParam(uint8_t *pBuf)
{	
	return IF_HAL_EEPROM_ReadBytes(SYSTEM_TOTALTIMES_ADDR, pBuf, sizeof(uint32_t));	
}

static uint8_t Write_SystemTotalTimesParam(uint8_t *pBuf)
{	
	return IF_HAL_EEPROM_WriteBytes(SYSTEM_TOTALTIMES_ADDR, pBuf, sizeof(uint32_t));	
}
//*****************************************************************************
//* END
//*****************************************************************************

