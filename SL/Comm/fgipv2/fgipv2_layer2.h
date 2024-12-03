
#ifndef _FGIPV2_LAYER2_H_
#define _FGIPV2_LAYER2_H_

#include "fgipv2.h"
#include "Common.h"
#include "IF_Comm.h"
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
	
#define PORT_NUM               	4   //User port, Debug port, Fpga port, Modbus port



#define MIN_FRAME_LEN          	6
#define MAX_INFO_LEN           	255

#define DEFAULT_ADDRESS        	0x00

typedef union _GlobeStatus_t
{
	uint8_t byte;
	struct
	{
		uint8_t alarm    : 1;   // 0: no alarm;   1: have alarms
		uint8_t ready    : 1;   // 0: not ready;  1: ready, boot process complete
		uint8_t active   : 1;   // 0: not active; 1: Interface is currently active
		uint8_t reserved : 5;
	} bits;

} GlobeStatus_t;

typedef enum
{
  upgrade_Frame_Unknow          = 0x00,
  upgrade_Frame_SOF           	= 0x01,
  upgrade_Frame_COF        		= 0x02,
  upgrade_Frame_EOF        		= 0x03, 
}UpgradeFrameType;


typedef struct _UpgradeFrame_struct
{ 
    UpgradeFrameType frame_type;
    uint16_t		 frame_number;
	uint8_t 		 Data[MAX_INFO_LEN];
    uint16_t 		 len;

}UpgradeFrame_t;

extern GlobeStatus_t g_GS[PORT_NUM];

extern void FGIPv2_Init(void);
extern void ReceiveChar(uint8_t port, uint8_t ch);
extern void TransmitChar(uint8_t port);
extern void SendLayer2Frame(uint8_t port,uint8_t *layer2Buf, uint16_t layer2Len);
extern void Dealwith_FGIPv2_Layer2(uint8_t port,pCommMsg_t pRxMsg);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _FGIPV2_LAYER2_H_ */
