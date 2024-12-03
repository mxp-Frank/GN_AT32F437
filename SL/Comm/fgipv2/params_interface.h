//move to IF_Nvm.h
// /*
//  * params_interface.h
//  *
//  *  Created on: 2022年3月12日
//  *      Author: Huxian
//  */

// #ifndef _PARAMS_INTERFACE_H_
// #define _PARAMS_INTERFACE_H_

// #ifdef FLASH_SECTOR_SIZE
// #define FLASH_BUF_SIZE                           FLASH_SECTOR_SIZE
// #else
// #define FLASH_BUF_SIZE                           4096
// #endif

// #define BOOTLOADER_VERSION                       "BOOT_V1.0.7"
// #define BOOTLOADER_VERSION_LEN                   16

// #define SOFTWARE_VERSION                         "APP_V1.0.7"
// #define SOFTWARE_VERSION_LEN                     16

// #define APP_FILE_SIGN                            "FG_AM016"
// #define APP_FILE_SIGN_LEN                        8
// #define APP_FILE_SIGN_OFFSET                     16

// #ifndef MAX_APP_FILE_SIZE
// #define MAX_APP_FILE_SIZE                        0x32000   //200K
// #endif

// ///////////////////////////////////////////////////////////////////////////////////////////////////
// //-------------------------------------------------------------------------------------------------
// // Identification parameter Operation
// void Set_DeviceType(uint8_t value);
// uint8_t Get_DeviceType(void);

// void Set_VendorCode(uint8_t value);
// uint8_t Get_VendorCode(void);

// void Set_HardwareVersion(uint8_t* pBuf, uint8_t len);
// uint8_t Get_HardwareVersion(uint8_t* pBuf);

// uint8_t Get_SoftwareVersion(uint8_t* pBuf);

// uint8_t Get_BootloaderVersion(uint8_t* pBuf);

// void Set_ProductModel(uint8_t* pBuf, uint8_t len);
// uint8_t Get_ProductModel(uint8_t* pBuf);

// void Set_SerialNumber(uint8_t* pBuf, uint8_t len);
// uint8_t Get_SerialNumber(uint8_t* pBuf);

// void Set_TrackingNumber(uint8_t* pBuf, uint8_t len);
// uint8_t Get_TrackingNumber(uint8_t* pBuf);

// uint32_t Get_CheckSum(void);

// //-------------------------------------------------------------------------------------------------
// // Communication parameter Operation
// void Set_DeviceAddress(uint8_t value);
// uint8_t Get_DeviceAddress(void);

// void Set_Net_MAC(uint8_t* pBuf);
// uint8_t Get_Net_MAC(uint8_t* pBuf);

// void Set_Net_IP(uint32_t value);
// uint32_t Get_Net_IP(void);

// void Set_Net_Mask(uint32_t value);
// uint32_t Get_Net_Mask(void);

// void Set_Net_Gateway(uint32_t value);
// uint32_t Get_Net_Gateway(void);

// void Set_DataRate(uint8_t value);
// uint8_t Get_DataRate(void);
// uint32_t Get_BaudRateValue(void);

// //-------------------------------------------------------------------------------------------------
// //Executable Action
// void Cmd_Reset(void);
// void Cmd_ResumeFactorySettings(void);
// void Cmd_SaveConfigTofFlah(void);

// //-------------------------------------------------------------------------------------------------
// //AM Configurable User Parameters
// void Set_MotorCanMove(uint8_t capIndex, uint8_t value);
// uint8_t Get_MotorCanMove(uint8_t capIndex);

// void Set_WorkMode(uint8_t value);
// uint8_t Get_WorkMode(void);

// void Set_AnalogControl(uint8_t value);
// uint8_t Get_AnalogControl(void);

// void Set_AnalogVoltRange(uint16_t value);
// uint16_t Get_AnalogVoltRange(void);

// void Set_PosLowLimit(uint8_t capIndex, uint16_t value);
// uint16_t Get_PosLowLimit(uint8_t capIndex);

// void Set_PosHighLimit(uint8_t capIndex, uint16_t value);
// uint16_t Get_PosHighLimit(uint8_t capIndex);

// void Set_PresetSource(uint8_t value);
// uint8_t Get_PresetSource(void);

// void Set_RFOffPresetMode(uint8_t value);
// uint8_t Get_RFOffPresetMode(void);

// void Set_RFOnPresetMode(uint8_t value);
// uint8_t Get_RFOnPresetMode(void);

// void Set_RFOffPresetDelay(uint16_t value);
// uint16_t Get_RFOffPresetDelay(void);

// void Set_RFOnPresetDelay(uint16_t value);
// uint16_t Get_RFOnPresetDelay(void);

// void Set_RFOffPresetNumber(uint8_t value);
// uint8_t Get_RFOffPresetNumber(void);

// void Set_RFOnPresetNumber(uint8_t value);
// uint8_t Get_RFOnPresetNumber(void);

// void Set_Preset(uint8_t presetIndex, uint8_t capIndex, uint16_t value);
// uint16_t Get_Preset(uint8_t presetIndex, uint8_t capIndex);

// //-------------------------------------------------------------------------------------------------
// //AM Configurable Internal Parameters
// void Set_RFOffDelay(uint16_t value);
// uint16_t Get_RFOffDelay(void);

// void Set_RFOnThrPhaseErr(uint16_t value);
// uint16_t Get_RFOnThrPhaseErr(void);

// void Set_RFOnThrMagErr(uint16_t value);
// uint16_t Get_RFOnThrMagErr(void);

// void Set_RFOnThrPower(uint16_t value);
// uint16_t Get_RFOnThrPower(void);

// void Set_RFOnThrRFV(uint16_t value);
// uint16_t Get_RFOnThrRFV(void);

// void Set_DCBiasAtten(uint16_t value);
// uint16_t Get_DCBiasAtten(void);

// void Set_RFProbeAtten(uint16_t value);
// uint16_t Get_RFProbeAtten(void);

// void Set_CurrentAtten(uint16_t value);
// uint16_t Get_CurrentAtten(void);

// void Set_VppAtten(uint16_t value);
// uint16_t Get_VppAtten(void);

// void Set_Deadband(uint8_t capIndex, uint16_t value);
// uint16_t Get_Deadband(uint8_t capIndex);

// void Set_HighSpeedThr(uint8_t capIndex, uint16_t value);
// uint16_t Get_HighSpeedThr(uint8_t capIndex);

// void Set_MinSpeed(uint8_t capIndex, uint16_t value);
// uint16_t Get_MinSpeed(uint8_t capIndex);

// void Set_MaxSpeed(uint8_t capIndex, uint16_t value);
// uint16_t Get_MaxSpeed(uint8_t capIndex);

// void Set_MaxBreakSpeed(uint8_t capIndex, uint16_t value);
// uint16_t Get_MaxBreakSpeed(uint8_t capIndex);

// void Set_MaxAutoMatchTime(uint16_t value);
// uint16_t Get_MaxAutoMatchTime(void);

// void Set_DiagStep(uint8_t capIndex, uint16_t value);
// uint16_t Get_DiagStep(uint8_t capIndex);

// //-------------------------------------------------------------------------------------------------
// //AM Configurable Factory Parameters
// void Set_MinToZeroPercent(uint8_t capIndex, uint16_t value);
// uint16_t Get_MinToZeroPercent(uint8_t capIndex);

// void Set_ZeroTo100Percent(uint8_t capIndex, uint16_t value);
// uint16_t Get_ZeroTo100Percent(uint8_t capIndex);

// void Set_MotorPlusesPerRound(uint8_t capIndex, uint16_t value);
// uint16_t Get_MotorPlusesPerRound(uint8_t capIndex);

// void Set_EncoderPlusesPerRound(uint8_t capIndex, uint16_t value);
// uint16_t Get_EncoderPlusesPerRound(uint8_t capIndex);

// //-------------------------------------------------------------------------------------------------
// //AM Read only Monitor Parameters
// // uint16_t Get_ActualPos(uint8_t capIndex);

// // int16_t Get_MagnitudeErr(void);
// // int16_t Get_PhaseErr(void);

// // uint16_t Get_InputPower(void);
// // uint16_t Get_InputVoltage(void);
// // uint16_t Get_InputCurrent(void);

// // uint16_t Get_DCBias(void);
// // uint16_t Get_Vpp(void);

// // int16_t Get_Temperature(void);
// // uint16_t Get_FanCurrent(void);
// // uint16_t Get_PowerSupply(void);

// // uint16_t Get_LastTuneTime(void);
// // uint16_t Get_PowerUpTime(void);
// // uint16_t Get_RFOnTime(void);

// uint16_t Get_StatusWord(void);
// uint16_t Get_FaultWord(void);

// //-------------------------------------------------------------------------------------------------
// //AM Control and Other Parameters
// void Cmd_MoveToPos(uint8_t capIndex, uint16_t value);
// void Cmd_Goto(uint8_t presetNumber);
// void Cmd_Regrease(void);
// void Cmd_Index(void);

// void Set_DiagnosticMode(uint8_t value);

// uint8_t Get_TraSel(void);
// void Set_TraSel(uint8_t value);
// uint8_t Get_CapSel(void);
// void Set_CapSel(uint8_t value);

// void Cmd_ClearFaultWord(void);
// void Cmd_ResetLastTuneTime(void);
// void Cmd_ResumeUserParams(void);
// void Cmd_ResumeInternalParams(void);

// void Set_FactoryMode(uint8_t value);
// uint8_t Get_FactoryMode(void);

// //-------------------------------------------------------------------------------------------------
// //AM Parameters only for debugging and testing
// uint8_t Get_SystemResetTimes(void);


// //-------------------------------------------------------------------------------------------------
// //AM Data block Parameters
// uint8_t Set_FactoryParams(uint8_t *pBuf, uint16_t len);
// void Get_FactoryParams(uint8_t *pBuf, uint16_t len);

// uint8_t Set_InternalParams(uint8_t *pBuf, uint16_t len);
// void Get_InternalParams(uint8_t *pBuf, uint16_t len);

// uint8_t Set_UserParams(uint8_t *pBuf, uint16_t len);
// void Get_UserParams(uint8_t *pBuf, uint16_t len);

// uint32_t GetTotalAMPDLength(void);
// uint16_t GetAMPD(uint16_t frameNo, uint8_t* pBuf);

// uint32_t GetTotalDiagDataLength(void);
// uint16_t GetDiagnoseData(uint16_t frameNo, uint8_t* pBuf);

// ///////////////////////////////////////////////////////////////////////////////////////////////////
// //软件升级使用的外部函数，函数须在外部实现
// uint8_t IF_Nvm_WriteDownloadFile(uint32_t destAddrOffset, uint16_t* data, uint32_t size);
// uint32_t IF_Nvm_ReadDownloadFileLen(void);
// uint8_t IF_Nvm_CheckDownloadFile(void);
// uint8_t IF_Nvm_WriteDownloadFlag(void);

// //uint32_t Read_EncriptionFlag(void);
// //void IF_Nvm_EncryptDownloadFile(uint32_t encryptionFlag);
// //uint8_t IF_Nvm_CheckRawDownloadFile(void);

// //外部函数
// void DelayMs(uint32_t ms);
// void ResetDevice(void);

// void ExecuteActionsAfterRsp(void);


// #endif /* _FGIPV2_PARAMS_INTERFACE_H_ */
