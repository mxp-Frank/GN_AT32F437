/*
 * pppfcs.h
 *
 *  Created on: 2022年3月4日
 *      Author: Huxian
 */

#ifndef _PPPFCS_H_
#define _PPPFCS_H_

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* private defines -----------------------------------------------------------*/
#define PPPINITFCS16 0xffff   /* Initial FCS value */
#define PPPGOODFCS16 0xf0b8   /* Good final FCS value */

/* Exported functions prototypes ---------------------------------------------*/
extern uint16_t CalCRC16_X25(uint8_t* data, uint16_t dataLen);
#endif /* _PPPFCS_H_ */
