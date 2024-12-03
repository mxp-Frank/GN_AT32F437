/*
 * stm32_crc32.h
 *
 *  Created on: 2022年3月4日
 *      Author: Huxian
 */

#ifndef _STM32_CRC32_H_
#define _STM32_CRC32_H_

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* private defines -----------------------------------------------------------*/
#define CRC32_INIT       0xFFFFFFFF

/* Exported functions prototypes ---------------------------------------------*/
uint32_t STM32_CrcBitByBitFast(uint32_t crc, uint8_t* p, uint32_t len);
uint16_t Decapsulate(uint8_t* data, uint16_t dataLen);
uint16_t Binary_Search(uint32_t *a,uint16_t n,uint16_t key);
#endif /* _STM32_CRC32_H_ */
