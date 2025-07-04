/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_Nvm.h
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-09
 * ****************************************************************************
 */
 
#ifndef _IF_NVM_H_
#define _IF_NVM_H_

#include "IF_PrjLib.h"
#include "IF_Board.h"

#include "IF_HAL_Cfg.h"


#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
///////////////////////////////////////////////////////////////////////////////////////////////////
//片内Flash存储相关的宏定义

// 代码相关定义
#define BOOTLOADER_START_ADDR                     FLASH_SECTOR_ADDR(0)     // 0x00000

#define RUNNING_APP_START_ADDR                    FLASH_SECTOR_ADDR(100)    // 0x32000
#define APP_HEADER_LEN                            0x200
#define RUNNING_APP_VECTOR_ADDR                   (RUNNING_APP_START_ADDR + APP_HEADER_LEN)   // 0x32200

#define DOWNLOAD_ENCRYPTED_APP_START_ADDR         FLASH_SECTOR_ADDR(200)   // 0x64000
#define DOWNLOAD_RAW_APP_START_ADDR               FLASH_SECTOR_ADDR(300)   // 0x96000
#define DOWNLOAD_RAW_APP_VECTOR_ADDR              (DOWNLOAD_RAW_APP_START_ADDR + APP_HEADER_LEN)

#define DOWNLOAD_REQUEST_FLAG_ADDR                FLASH_SECTOR_ADDR(508)   // 0xFE000
#define DOWNLOAD_REQUEST_FLAG                     0x01
#define READ_DOWNLOAD_REQUEST_FLAG                (*(__IO uint8_t*)DOWNLOAD_REQUEST_FLAG_ADDR)

#define DOWNLOAD_APP_FLAG_ADDR                    FLASH_SECTOR_ADDR(510)   // 0xFF000
#define DOWNLOAD_APP_FLAG                         0x11223344
#define READ_DOWNLOAD_APP_FLAG                    (*(__IO uint32_t*)DOWNLOAD_APP_FLAG_ADDR)

#define MAX_APP_FILE_SIZE                         0x32000   // 0x32000 = 204800 Bytes = 200 KBytes

#define RUNNING_APP_CRC                           (*(__IO uint32_t*)(RUNNING_APP_START_ADDR + 4))
#define RUNNING_APP_FILE_LEN                      (*(__IO uint32_t*)(RUNNING_APP_START_ADDR + 8))
#define RUNNING_APP_LEN                           (RUNNING_APP_FILE_LEN - APP_HEADER_LEN)

#define DOWNLOAD_ENCRYPTED_FILE_CRC               (*(__IO uint32_t*)DOWNLOAD_ENCRYPTED_APP_START_ADDR)
#define DOWNLOAD_ENCRYPTED_FILE_LEN               (*(__IO uint32_t*)(DOWNLOAD_ENCRYPTED_APP_START_ADDR + 8))
#define DOWNLOAD_ENCRYPTED_FLAG                   (*(__IO uint32_t*)(DOWNLOAD_ENCRYPTED_APP_START_ADDR + 12))

#define DOWNLOAD_RAW_APP_CRC                      (*(__IO uint32_t*)(DOWNLOAD_RAW_APP_START_ADDR + 4))
#define DOWNLOAD_RAW_FILE_LEN                     (*(__IO uint32_t*)(DOWNLOAD_RAW_APP_START_ADDR + 8))
#define DOWNLOAD_RAW_APP_LEN                      (DOWNLOAD_RAW_FILE_LEN - APP_HEADER_LEN)

//-------------------------------------------------------------------------------------------------
// 用户数据相关定义
#define DIAGNOSE_DATA_BASE_ADDR                  FLASH_SECTOR_ADDR(400)  //诊断数据的首地址
#define DIAGNOSE_RECORD_NUM_EACH_SECTOR          (FLASH_SECTOR_SIZE / sizeof(ProcessDataRecord_t))
#define MAX_DIAGNOSE_DATA_SECTOR_NUM             100                      //诊断数据最多保存50个Sector,即200K大小

///////////////////////////////////////////////////////////////////////////////////////////////////
//片外EEPROM存储相关的宏定义

#define EEPROM_ADDR(x)                           	(EEPROM_PAGE_SIZE * (x))      // x = 0 ~ 511

#define SYSTEM_RESET_TIMES_ADDR                  	EEPROM_ADDR(1)      //系统复位次数的保存地址
#define SYSTEM_RESET_MAX_TIMES                   	10

#define PRESETPARAM_NUM_ADDR						EEPROM_ADDR(2)      //系统预设参数
#define SYSTEM_TOTALTIMES_ADDR						EEPROM_ADDR(3)      //系统运行总时间

#define FACTORY_PARAM_FS_ADDR                    	EEPROM_ADDR(10)     //工厂参数保存地址
#define COMMON_PARAM_FS_ADDR						EEPROM_ADDR(20)     //工厂模式下的通信参数保存地址
#define INTERNAL_PARAM_FS_ADDR                   	EEPROM_ADDR(30)     //工厂模式下的内部参数保存地址
#define USER_PARAM_FS_ADDR                          EEPROM_ADDR(50)     //工厂模式下的用户参数保存地址

#define FACTORY_PARAM_ADDR                    		EEPROM_ADDR(40)     //工厂参数保存地址

#define COMMON_PARAM_ADDR							EEPROM_ADDR(60)     //通信参数保存地址
#define INTERNAL_PARAM_ADDR                         EEPROM_ADDR(70)     //内部参数保存地址

#define USER_PARAM_ADDR                         	EEPROM_ADDR(80)     //用户参数1保存地址

#define MAPVSPOWER_ADDR								EEPROM_ADDR(120)    //电压与功率映射表地址0~1000W
#define MAPPHPOWER_ADDR								EEPROM_ADDR(200)    //相位与功率映射表地址0~1000W
#define TEMP_PARAM_ADDR								EEPROM_ADDR(250)    //温度映射表
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
	uint16_t Index;
	uint8_t  Buf[FLASH_SECTOR_SIZE]; 
}NvmBuffer_t;

extern NvmBuffer_t NvmFlash;
uint8_t Read_BootloaderVersion(uint8_t* pBuf);

uint8_t IF_NvmParam_EraseDownloadFlag(void);
uint8_t IF_NvmParam_WriteDownloadFlag(void);

void IF_NvmParam_CopyRawFileToRunningArea(void);
void IF_NvmParam_EncryptDownloadFile(uint32_t encryptionFlag);

uint8_t IF_NvmParam_WriteDownloadFile(uint32_t destAddrOffset, uint16_t* data, uint32_t size);

uint8_t IF_NvmParam_WriteDiagnoseData(uint8_t sectorIndex);
void Read_DiagnoseData(uint32_t addrOffset, uint8_t* pBuf, uint16_t len);

uint32_t IF_NvmParam_ReadDownloadFileLen(void);
uint32_t Read_EncriptionFlag(void);

uint8_t IF_NvmParam_CheckRunningFile(void);
uint8_t IF_NvmParam_CheckDownloadFile(void);
uint8_t IF_NvmParam_CheckRawDownloadFile(void);

///////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t Read_SystemResetTimes(void);
uint8_t IF_NvmParam_CheckSystemResetTimes(void);

uint8_t Load_ConfigParam(uint8_t value);
uint8_t Write_ConfigParam(uint8_t* pBuf);
/********************工程参数*************************/


uint8_t Read_FS_PartsParam(uint8_t* pBuf);
uint8_t Write_FS_PartsParam(uint8_t* pBuf);
uint8_t Read_FS_CommonParam(uint8_t* pBuf);
uint8_t Write_FS_CommonParam(uint8_t* pBuf);
uint8_t Read_FS_InternalParam(uint8_t* pBuf);
uint8_t Write_FS_InternalParam(uint8_t* pBuf);
uint8_t Read_FS_UserParam(uint8_t* pBuf);
uint8_t Write_FS_UserParam(uint8_t* pBuf);
/********************普通参数*************************/
uint8_t Read_PartsParam(uint8_t* pBuf);
uint8_t Write_PartsParam(uint8_t* pBuf);
uint8_t Read_CommonParam(uint8_t* pBuf);
uint8_t Write_CommonParam(uint8_t* pBuf);
uint8_t Read_InternalParam(uint8_t* pBuf);
uint8_t Write_InternalParam(uint8_t* pBuf);

uint8_t Read_UserParam(uint8_t* pBuf);
uint8_t Write_UserParam(uint8_t* pBuf);

uint8_t Read_VoltageMapParam(uint8_t* pBuf);
uint8_t Write_VoltageMapParam(uint8_t* pBuf);

uint8_t Read_PhaseMapParam(uint8_t* pBuf);
uint8_t Write_PhaseMapParam(uint8_t* pBuf);

uint8_t Read_SystemTotalTimesParam(uint8_t *pBuf);
uint8_t Write_SystemTotalTimesParam(uint8_t *pBuf);

/******************************************************************************
* Interface functions                                                         *
******************************************************************************/
extern void IF_NvmInit(void);
extern void IF_NvmParam_WriteSystemResetTimes(uint8_t resetTimes);
extern uint16_t IF_NvmParam_ReadSystemTotalTimes(void);
extern void IF_NvmParam_WriteSystemTotalTimes(uint32_t powerupTimer);
#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif

 /*
*******************************************************************************
* END
*******************************************************************************
*/

