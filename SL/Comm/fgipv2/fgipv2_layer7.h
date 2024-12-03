
#ifndef _FGIPV2_LAYER7_H_
#define _FGIPV2_LAYER7_H_

#include "stdint.h"
#include "fgipv2.h"
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#define MAX_PRESET_NUM             50

typedef enum _CommandCode_t
{
	TelegramError   = 0xFE,    //Telegram error (as response of the slave, if the telegram structure is not correct).
	ParameterRead   = 0x01,
	ParameterWrite  = 0x02,
	DataBlockRead   = 0x03,
	DataBlockWrite  = 0x04,
	InterfaceActive = 0x05,    // Control of interface activity.

} CommandCode_t;

typedef enum _ErrorCode_t
{
	PacketCRCError     = 0x01,  // CRC check of the data packet error.
	UnknownCommand     = 0x02,  // The unknown command is not supported.
	SystemBusy         = 0x03,  // The system is busy.
	DeviceStatusError  = 0x04,  // Device status error.
	FrameFormatError   = 0x05,  // Frame Format error.
	CSR2RFONError      = 0x06,  // The cmd is allowed  only when RF is off.
	DataOutRanageError = 0x07,

} ErrorCode_t;

typedef enum _ParamterIndex_t
{
	//------  Common Parameters  ------
	// Identification parameters
	ID_DeviceType                  = 0x0001,
	ID_VendorCode                  = 0x0002,
	ID_HardwareVersion             = 0x0003,
	ID_SoftwareVersion             = 0x0004,
	ID_BootloaderVersion           = 0x0005,
	ID_ProductModel                = 0x0006,
	ID_SerialNumber                = 0x0007,
	ID_TrackingNumber              = 0x0008,
	ID_CheckSum                    = 0x0009,
	ID_FpgawareVersion             = 0x000A,
	
	// Communication parameters
	ID_DeviceAddress               = 0x0101,
	ID_NET_MAC                     = 0x0102,
	ID_NET_IP                      = 0x0103,
	ID_NET_MASK                    = 0x0104,
	ID_NET_Gateway                 = 0x0105,
	ID_DataRate                    = 0x0106,
    ID_EtherCATAddess			   = 0x0107,
		
	// Executable Action
	ID_Reset                       = 0x0201,
	ID_ResumeFactorySettings       = 0x0202,

	 // Configurable Internal parameter                                        
	ID_InputVrmsFactor			   = 0x4200,
	ID_InputIrmsFactor			   = 0x4201,
	ID_InputPhaseFactor			   = 0x4202,
	ID_PIDProportion 			   = 0x4203,
	ID_PIDIntegral 			   	   = 0x4204,
	ID_PIDDerivatice 			   = 0x4205,
	ID_PIDErrorThr 			       = 0x4206,
	ID_VoltPowerMap				   = 0x4207,
	ID_SetVoltage				   = 0x4208,
	ID_SetWorkMode                 = 0x4209,
	ID_VSWRLimit				   = 0x420A,
	ID_StartPower				   = 0x420B,
	ID_SlowMode					   = 0x420C,
	ID_SlowStartDelay			   = 0x420D,
	ID_SlowStopDelay			   = 0x420E,	
	ID_AnalogVoltageRange          = 0x420F,  // 0 means no analog interface;
                                      
	ID_CmdDDSState                 = 0x4210,	
	ID_SensorChannelNo             = 0x4211,
	ID_CmdWorkFreq		           = 0x4212,
	ID_CmdWorkPhase                = 0x4213,
	ID_CmdACDCVoltage              = 0x4214,
	ID_CmdACDCCurrent              = 0x4215,
	ID_CmdACDCDriveState           = 0x4216,
	ID_RTCStamps              	   = 0x4217,
	
	ID_ACDCVoltGain				   = 0x4218,
	ID_ACDCVoltOffset			   = 0x4219,
	ID_ACDCCurrentGain			   = 0x421A,
	ID_ACDCCurrentOffset		   = 0x421B,
	ID_DrainVoltGain			   = 0x421C,
	ID_DrainVoltOffset		   	   = 0x421D,
	//Configurable User parameters 
    ID_RegulationMode              = 0x4000,  // work mode, 0: Pfwd, 1: Pdlv 2:Vdc
	ID_PfwdLimit				   = 0x4001,  //Forward power Limit
	ID_PrefLimit				   = 0x4002,  //Reflect power Limit
	ID_PrefThreshold			   = 0x4003,  //Reflect power Limit
	ID_DelayReachTime   		   = 0x4004,  //Delay time after reaching Reflect Power Threshold until is turned off.
	ID_AnalogContorl               = 0x4005,  // Analog Control, 0: Disable, 1: Enable
	ID_PulseMode               	   = 0x4006,  // pulseMode
	ID_PulseFreq               	   = 0x4007,  // pulse Frequency
	ID_PulseDuty               	   = 0x4008,  // pulse Frequency
	ID_InputVDCFactor              = 0x4009,
	ID_SetPoint                    = 0x400A,  // [1, 4095]
	ID_CommunicateType			   = 0x400B,  //[0,1,2]
	ID_PvdcLimit				   = 0x400C,  
	ID_FpgaPulsePowerThr		   = 0x400D,
	ID_FpgaPulseSyncDelay		   = 0x400E,
	ID_PrefThrMode				   = 0x400F,
	
	//Readonly Monitor Parameter
	ID_SensorFreq                  = 0x4600,
	ID_SensorPfwd                  = 0x4601,
	ID_SensorPref				   = 0x4602,
	ID_SensorPdlv				   = 0x4603,
	ID_RFOnTime					   = 0x4604,
	ID_PowerUpTime				   = 0x4605,
	ID_PCBTemperature			   = 0x4606,
	ID_SensorDrain				   = 0x4607,
	ID_FanCurrent				   = 0x4608,
	ID_MatchDCBias				   = 0x4609,
	
	ID_SensorVrms                  = 0x4610,
	ID_SensorIrms                  = 0x4611,
	ID_SensorPhase                 = 0x4612,
	ID_SensorZ                     = 0x4613,
	ID_SensorR                     = 0x4614,
	ID_SensorX                     = 0x4615,
	ID_SensorVSWR                  = 0x4616,
	ID_SensorTemp                  = 0x4617,
	
	ID_ACInputVolt1				   = 0x4618,
	ID_ACInputVolt2				   = 0x4619,
	ID_ACInputVolt3				   = 0x461A,
	ID_ACInputCurrent1			   = 0x461B,
	ID_ACInputCurrent2			   = 0x461C,
	ID_ACInputCurrent3			   = 0x461D,
	ID_PAVoltage1				   = 0x461E,
	ID_PAVoltage2				   = 0x461F,
	ID_PACurrent1				   = 0x4620,
	ID_PACurrent2				   = 0x4621,
	ID_PACurrent3				   = 0x4622,
	ID_PACurrent4				   = 0x4623,
	ID_PATemperature1			   = 0x4624,
	ID_PATemperature2			   = 0x4625,
	ID_PATemperature3			   = 0x4626,
	ID_PATemperature4			   = 0x4627,
	ID_ACDCTemperature1			   = 0x4628,
	ID_ACDCTemperature2			   = 0x4629,
	
    ID_Load_ActualCapPos           = 0x460A,
	ID_Tune_ActualCapPos           = 0x460B,
	ID_StatusWord                  = 0x460C,
    ID_FaultWord                   = 0x460D,
	ID_24VPowerSupply              = 0x460E,
 
	 /*Control and Other Parameters*/
	ID_RFState           		   = 0x4800,  // RF Power work State  0:RF Off  1:RF On
	ID_MatchMode                   = 0x4801, 
	ID_Load_MoveToCapPos           = 0x4802,  // Move the load capacitor
	ID_Tune_MoveToCapPos           = 0x4803,  // Move the tune capacitor
	ID_FactoryMode                 = 0x4804,  // 0x01 enter the factory mode, 0x00 exits the factory mode.
    ID_SaveConfig                  = 0x4805,                                	      
	ID_LoadConfig                  = 0x4806,
	ID_RFDriveState                = 0x4807,
} ParamterIndex_t;

typedef enum _Status_t
{
	NoErrorHaveData        = 0x00, // No error, data follows.
	UnknownParam           = 0x01, // Unknown parameter index/impermissible parameter index.
	NotAcceptedCommand     = 0x02, /* This command is not accepted.
									  Example: writing parameters/data but interface is not "active".
									  Example: activating the interface when interface is already active.  */
	NotPermittedParam      = 0x03, // The parameter reading or writing is not permitted.
    NotInitializedParam    = 0x04, // The parameter is not initialized correctly.
	CheckFailedParam       = 0x05, // A parameter specific check has failed.
	InsufficientSpace      = 0x06, // Insufficient space available in the response telegram.
	UnknownDatablockID     = 0x07, // Unknown/wrong datablock ID
	IllegalDatablock       = 0x08, /* Illegal datablock attribute (e.g. attempt to write/read
	                                  a datablock when the datablock may only be read/written to;
	                                  or write access if maximum datablock size is exceeded).   */
	UnknownSubIndex        = 0x09, // Unknown sub-index, e.g. in the case of "Parameter write to sub-index 0".
	NotPossibleActiveation = 0x10, // Activation of the interface not possible.
	NoErrorNoData          = 0x7F, // No data, no error.

	DataBlockLengthError   = 0x81,
	DataBlockCRCError      = 0x82,
	InvalidFileContent     = 0x83,
	HardwareError          = 0x84,

} Status_t;

typedef enum _DataType_t
{
	DataType_BOOL     = 0x01,  // 1 byte, bit value LSB on | off [0x01|0x00]
	DataType_SINT8    = 0x02,  // 1 byte, signed integer [-128 to 127]
	DataType_UINT8    = 0x03,  // 1 byte, unsigned integer value [0 to 255]
	DataType_SINT16   = 0x04,  // 2 bytes, signed integer value [-32 768 to 32 767]
	DataType_UINT16   = 0x05,  // 2 bytes, unsigned integer [0 to 65 535]
	DataType_SINT32   = 0x06,  // 4 bytes, signed integer value [-2 147 483 648 to 2 147 483 647]
	DataType_UINT32   = 0x07,  // 4 bytes, unsigned integer value [0 to 4 294 967 295]
	DataType_FLT32    = 0x08,  // 4 bytes, floating point IEEE
	DataType_FLT64    = 0x09,  // 8 bytes, floating point IEEE
	DataType_VECT     = 0x0A,  // 1 to 248 bytes of vector data; data field with length of 1 byte followed by up to 247 bytes of data

} DataType_t;

typedef enum _DataBlockID_t
{
	ID_FactoryParams        = 0x01,
	ID_InternalParams       = 0x02,
	ID_UserParams           = 0x03,

	ID_ProcessData    		= 0x0E,
	ID_Firmware             = 0x10,
} DataBlockID_t;

typedef enum _DataBlockFlag_t
{
	NotUsed     = 0x00,
	MoreBlock   = 0x01,
	LastBlock   = 0x02,

} DataBlockFlag_t;


#define DOB_LEN_NO_DATA               4    //一个不带参数的DOB长度为4
#define DOB_MIN_LEN_WITH_DATA         6    //一个uint8_t参数的DOB长度为6
#define DOB_MAX_LEN                   253  //一个VECT参数的DOB最大为253
#define MAX_NUM_OF_DOBs               (DOB_MAX_LEN / DOB_LEN_NO_DATA)   // 253/4 = 63
#define MAX_VECT_LEN                  (DOB_MAX_LEN - 5)
#define MAX_DATABLOCK_TX_LEN          240

typedef struct _DataObject_t
{
	uint16_t Index;
	uint8_t  SubIndex;  
	uint8_t  Status;
	uint8_t  DataType;
	uint8_t  DataLen;
	uint8_t* pData;

} DataObject_t;

typedef struct _InfoFiled_t
{
	uint8_t port;
	uint8_t GS;
	uint8_t Cmd;
	uint8_t NumOfDOBs;
	DataObject_t DOBs[MAX_NUM_OF_DOBs];

} InfoFiled_t, *PInfoFiled_t;


///////////////////////////////////////////////////////////////////////////////////////////////////
extern void SendTelegramError(uint8_t errorCode);
extern void Dealwith_FGIPv2_Layer7(FGIPv2Frame_t *pRxFrame);


#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _FGIPV2_LAYER7_H_ */
