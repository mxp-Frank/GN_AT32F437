/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  bsip.h
 * @Brief :      
 * @Author :     Chenghanbing (iwanthaha@sina.com)
 * @Version :    1.0
 * @Creat Date : 2023-06-08
 * ****************************************************************************
 */

#ifndef _BSIP_H_
#define _BSIP_H_

#include "stm32_crc32.h"
#include "bsip_layer2.h"
#include "params_impl.h"


#define MAKE_UINT16(a,b)         ((uint16_t)(((a) << 8) | (b)))
#define MAKE_UINT32(a,b,c,d)     ((uint32_t)(((a) << 24) | ((b) << 16) | ((c) << 8) | (d)))

#define Byte0_UINT16(x)          ((uint8_t)((x) & 0xFF))
#define Byte1_UINT16(x)          ((uint8_t)(((x) >> 8) & 0xFF))

#define Byte0_UINT32(x)          (uint8_t)((x) & 0xFF)
#define Byte1_UINT32(x)          (uint8_t)(((x) >> 8) & 0xFF)
#define Byte2_UINT32(x)          (uint8_t)(((x) >> 16) & 0xFF)
#define Byte3_UINT32(x)          (uint8_t)(((x) >> 24) & 0xFF)

#endif /* _BSIP_H_ */

