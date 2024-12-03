/*
 * stm32_crc32.c
 *
 *  Created on: 2022年3月4日
 *      Author: Huxian
 */

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "stm32_crc32.h"

const int order = 32;
uint32_t polynom = 0x04c11db7;

uint32_t crchighbit = (uint32_t)1 << (order - 1);

uint32_t STM32_CrcBitByBitFast(uint32_t crc, uint8_t* p, uint32_t len)
{
	// fast bit by bit algorithm without augmented zero bytes.
	// does not use lookup table, suited for polynom orders between 1...32.

	uint32_t i, j, c, bit;

	for (i = 0; i < len; i++)
	{
		c = (uint32_t)*p++;

		for (j = 0x80; j; j >>= 1)
		{
			bit = crc & crchighbit;
			crc <<= 1;
			if (c & j) bit ^= crchighbit;
			if (bit) crc ^= polynom;
		}
	}

	return (crc);
}
/* FUNCTION *********************************************************************************
 * Function Name : Decapsulate
 * Description   : 将数据包内的7D 5D转化成7D, 7D 5E转化成7E
 * Parameter     : data: 数据的首地址
 * Parameter     : dataLen: 转化前的数据长度
 * Return        : 返回转化后的数据长度
 * END ***************************************************************************************/
uint16_t Decapsulate(uint8_t* data, uint16_t dataLen)
{
    uint16_t i;
    uint16_t len = 0;

    for (i = 0; i < dataLen; i++)
    {
		if (data[i] == 0x7D)
		{
			i++;
			if (i < dataLen)
			{
				data[len++] = data[i] ^ 0x20;
			}
		}
		else
		{
			data[len++] = data[i];
		}
    }

    return len;
}
//折半查找法
uint16_t Binary_Search(uint32_t *a,uint16_t n,uint16_t key)
{
	uint16_t low = 0,high = 0,mid = 0;
	high = n;
	while(low <= high)
	{
		mid =(low+high)/2;//折半
		if(key < a[mid])
		{
			high = mid-1;
		}
		else if(key > a[mid])
		{
			low = mid +1;
		}else
		{ 
			return mid;
		}
	}
	return 0;
}


