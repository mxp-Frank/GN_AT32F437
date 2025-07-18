/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  bsip_layer7.h
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2023-05-19
 * ****************************************************************************
 */

#ifndef _BSIP_LAYER7_H_
#define _BSIP_LAYER7_H_

#include "bsip_layer2.h"
#include "bsip_layer7.h"
#include "IF_SL_Cfg.h"
#include "IF_HAL_Cfg.h"

#define MAX_QUEUE_SIZE                          5
#define BSIP_TASK_PERIOD						5
typedef enum
{
    //Core Board ---> Interface Board
    ID_Param_InitPartsParams                     = 0x10,
	ID_Param_InitFactoryParams                   = 0x20,
	ID_Param_InitUserParams                      = 0x30,
	ID_Param_InitInternalParams                  = 0x40,
	ID_Param_InitMobusParams                     = 0x50,
	
    ID_Status_RunningInfo                        = 0x80,
	
    ID_Debug_ProcessDataStart                    = 0x90,
	ID_Debug_ProcessData                         = 0x91,
    
    //Interface Board ---> Core Board
    ID_Param_SetPartsParams                      = 0xA1,
	ID_Param_SetFactoryParams                    = 0xA2,
	ID_Param_SetUserParams                       = 0xA3,
	ID_Param_SetPresetParams                     = 0xA4,
	ID_Param_SetInternalParams                   = 0xA5,
	ID_Param_SetModbusParams                     = 0xA6,

    ID_Cmd_Reset                                 = 0xB0,
    ID_Cmd_RFPowerState                          = 0xB1,
    ID_Cmd_RFPowerPoint                          = 0xB2,
    ID_Cmd_PulseFreq                             = 0xB3,
    ID_Cmd_PulseDuty                             = 0xB4,
    ID_Cmd_SyncSource                            = 0xB5,
    ID_Cmd_SyncOutDelay                          = 0xB6,
    ID_Cmd_SyncOutEnable                         = 0xB7,
 	ID_Cmd_LoadTarget							 = 0xB8,
	ID_Cmd_TuneTarget							 = 0xB9,
    ID_Cmd_Mute                                  = 0xBA,
    ID_Cmd_Sync                                  = 0xBB,
	ID_Cmd_FactorMode                            = 0xBC,
	ID_Cmd_ResumeDefaultParam                    = 0xBD,
	
	ID_Cmd_PowerWorkMode                         = 0xC1,
	ID_Cmd_SetACDCState							 = 0xC2,
	ID_Cmd_SetACDCVolt                           = 0xC3,
	ID_Cmd_DDSSignState                          = 0xC4,
	ID_Cmd_DDSPhase								 = 0xC5,
	
    ID_Debug_GetProcessData                      = 0xE0,
	
	
    ID_Debug_FirmwareUpdate                      = 0xF1,
	ID_Debug_LoadScaleUpdate					 = 0xF2,
	ID_Debug_TuneScaleUpdate					 = 0xF3,
	ID_Debug_AuxiScaleUpdate					 = 0xF4
	
} MsgIdEnum;

typedef enum
{//uinit: ms
    DEBUGDATA_NOT_FINISH                           = 0,
    DEBUGDATA_FINISH                               = 1,
    DEBUGDATA_ERROR                                = 2

} DebugDataResultEnum;

typedef enum
{//uinit: ms
    TIME_INTERVAL_ACK_MSG                        = 5,
    TIME_INTERVAL_INIT_MSG                       = 50,
    TIME_INTERVAL_STATUS_MSG                     = 50, //30 normaly, 50 is only for test!
    TIME_INTERVAL_DEBUG_MSG                      = 100

} MsgTimeIntervalEnum;

typedef enum
{//uinit: ms
    FIRMWARE_NOF                                 = 0,  //no frame
    FIRMWARE_SOF                                 = 1,  //start frame
    FIRMWARE_COF                                 = 2,  //continious frame
    FIRMWARE_EOF                                 = 3   //end frame

} FirmwareFrameTypeEnum;

typedef enum
{//uinit: ms
    DEVICE_COREBOARD                             = 0,
    DEVICE_FPGA                                  = 1,
	DEVICE_MODBUS                                 = 2

} FirmwareDeviceTypeEnum;

typedef struct _FirmwareDataMsg_t
{
    FirmwareDeviceTypeEnum TargetDevice;
    uint16_t ExpectedFrameNo;
    uint32_t DownFileLen;
    uint8_t FileSign[APP_FILE_SIGN_LEN];
    uint8_t FpgaSign[APP_FILE_SIGN_LEN];
	uint8_t ModbusSign[APP_FILE_SIGN_LEN];
    uint8_t DownFileSign[APP_FILE_SIGN_LEN];
    uint16_t FlashBufIndex;
    uint32_t DownloadFileIndex;

}FirmwareDataMsg_t, *pFirmwareDataMsg_t;

typedef struct _DebugDataMsg_t
{
    uint16_t FrameNo;
    uint16_t ExpectedFrameNo;
    uint32_t TxDataRemainLen;
    uint8_t CurrentBufLen;

}DebugDataMsg_t, *pDebugDataMsg_t;


///////////////////////////////////////////////////////////////////////////////////////////////////

extern void Dealwith_BSIP_Layer7(void);
extern void BSIP_TxTimeManagment(uint8_t port);
extern void BSIP_TxBufManagment(uint8_t port);
extern void BSIP_FpgaFirmwareUpdate(uint8_t port);
extern void BSIP_ModbusFirmwareUpdate(uint8_t port,uint8_t Modbbus_Address);


#endif /* _BSIP_LAYER7_H_ */
