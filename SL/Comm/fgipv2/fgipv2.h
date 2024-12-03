
#ifndef _FGIPV2_H_
#define _FGIPV2_H_

#include "stdint.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#define MAKE_UINT16(a,b)         ((uint16_t)(((a) << 8) | (b)))
#define MAKE_UINT32(a,b,c,d)     ((uint32_t)(((a) << 24) | ((b) << 16) | ((c) << 8) | (d)))

#define Byte0_UINT16(x)          ((uint8_t)((x) & 0xFF))
#define Byte1_UINT16(x)          ((uint8_t)(((x) >> 8) & 0xFF))

#define Byte0_UINT32(x)          (uint8_t)((x) & 0xFF)
#define Byte1_UINT32(x)          (uint8_t)(((x) >> 8) & 0xFF)
#define Byte2_UINT32(x)          (uint8_t)(((x) >> 16) & 0xFF)
#define Byte3_UINT32(x)          (uint8_t)(((x) >> 24) & 0xFF)

#define Word0_UINT32(x)          ((uint16_t)((x) & 0xFFFF))
#define Word1_UINT32(x)          ((uint16_t)(((x) >> 16) & 0xFFFF))

#define SET_BIT(x,bit)			  (x|(0x01<<bit))	
#define GET_BIT(x,bit)			  (x&(0x01<<bit))
#define GET_UINT16(x)             (uint16_t)(x)

#define FloatToUINT16(x)		     ((x-(uint16_t)x>0)? (x+1):(x))

#define BUFFER_SIZE            	520   // ????: (255 + 4) * 2 + 2

typedef union _Control_t
{
	uint8_t byte;
	struct
	{
		uint8_t dir      : 1;    // 0: master->slave, 1: slave->master
		uint8_t reserved : 7;
	} bits;

}Control_t;


//User defined type for message queue
typedef struct _CommMsg_t
{
	uint8_t port;                //?????
	uint16_t len;                //????
	uint16_t index;				 //????g'i't
	uint8_t flag_status;		 //???
	uint8_t data[BUFFER_SIZE];   //????
}CommMsg_t, *pCommMsg_t;


typedef struct _FGIPv2Frame_t
{
	uint8_t port;
	uint8_t address;
	Control_t control;
	uint16_t infoLen;
	uint8_t* info;
	uint16_t crc16;

}FGIPv2Frame_t;



#if defined(__cplusplus)
};
#endif /* __cplusplus */
#endif /* _FGIPV2_H_ */

