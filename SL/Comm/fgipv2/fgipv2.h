
#ifndef _FGIPV2_H_
#define _FGIPV2_H_

#include "stdint.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

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
	uint8_t port;
	uint16_t len;                
	uint16_t index;				 
	uint8_t flag;		 			
	uint8_t data[BUFFER_SIZE];   
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
typedef struct _Layer2Data_t
{
	
	uint8_t  Buf[BUFFER_SIZE];
	uint16_t Len;
}Layer2Data_t,*pLayer2Data_t;

#if defined(__cplusplus)
};
#endif /* __cplusplus */
#endif /* _FGIPV2_H_ */

