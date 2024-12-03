/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Boot.c
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-10-31
 * ****************************************************************************
 */

/*
 * bootloader.c
 *
 *  Created on: 2022年3月4日
 *      Author: Huxian
 */

#include "IF_Board.h"
#include "IF_Nvm.h"

#pragma pack(push)
#pragma O0
typedef void (*pFun_TypeDef) (void);
/* FUNCTION *********************************************************************************
 * Function Name : app_load
 * Description   : load app函数
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void app_load(uint32_t app_addr)
{
    register pFun_TypeDef pJump2App;
        
    /* check the address of stack */
    if(((*(uint32_t*)app_addr) - 0x20000000) < (384 * 1024))
    {

       /* 先关闭全局中断 */
       __disable_irq() ; 

       for (uint8_t i = 0; i < 8; i++)
       {
           NVIC->ICER[i]=0xFFFFFFFF;
           NVIC->ICPR[i]=0xFFFFFFFF;
       }
   
        pJump2App = (pFun_TypeDef)*(uint32_t*)(app_addr + 4);        /* code second word is reset address */
        __set_MSP(*(uint32_t*)app_addr);                        /* init app stack pointer(code first word is stack address) */
        pJump2App();                                          /* jump to user app */
    }
}
/* FUNCTION *********************************************************************************
 * Function Name : JumpToUserApplication
 * Description   : 跳转到app函数
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void JumpToUserApplication(uint32_t appStartAddr)
{
    if(((*(uint32_t*)(appStartAddr + 4)) & 0xFF000000) == 0x08000000)
        app_load(appStartAddr);
}
/* FUNCTION *********************************************************************************
 * Function Name : CrcCheckForAppAndRun
 * Description   : CRC校验app函数
 * Parameter     : 
 * return        :                
 * END ***************************************************************************************/
static void CrcCheckForAppAndRun(uint8_t bEraseDownloadFlag)
{
	if (RUNNING_APP_FILE_LEN > MAX_APP_FILE_SIZE)
	{
        //do nothing...
	}
    else
    {
        if (IF_NvmParam_CheckRunningFile())
        {
            if (bEraseDownloadFlag)
            {
                IF_NvmParam_EraseDownloadFlag();
            }

            JumpToUserApplication(RUNNING_APP_VECTOR_ADDR);
        }
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************接口函数**********************************************************/
void IF_Bootloader(void)
{
	uint32_t downloadFlag = READ_DOWNLOAD_APP_FLAG;

	//系统复位次数达到10次直接运行bootloader不跳转到app
    if (IF_NvmParam_CheckSystemResetTimes())
    {
        //do nothing...
    }
    else
    {
        if(DOWNLOAD_APP_FLAG == downloadFlag)
        {
            uint32_t encryptionFlag = DOWNLOAD_ENCRYPTED_FLAG;

            IF_NvmParam_EncryptDownloadFile(encryptionFlag);

            if (IF_NvmParam_CheckRawDownloadFile())
            {
                IF_NvmParam_CopyRawFileToRunningArea();
            }

            CrcCheckForAppAndRun(1);
        }
        else
        {
            CrcCheckForAppAndRun(0);
        }
    }
}

#pragma pack(pop)
/************************ (C) COPYRIGHT CHENGYUAN *****END OF FILE***********************/
