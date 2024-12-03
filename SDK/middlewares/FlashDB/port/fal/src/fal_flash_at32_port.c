/*
 * Copyright (c) 2020, Armink, <armink.ztl@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <fal.h>
#include "at32f435_437.h"
#include "IF_HAL_cfg.h"
#include "Common.h"
static int init(void)
{
    /* do nothing now */
    return 1;
}


static int ef_err_port_cnt = 0;
int on_ic_read_cnt  = 0;
int on_ic_write_cnt = 0;

void feed_dog(void)
{
	IF_WdogFeed();
}

static int read(long offset, uint8_t *buf, size_t size)
{

    size_t i;
    uint32_t addr = stm32_onchip_flash.addr + offset;
    if( addr%4 != 0)
        ef_err_port_cnt++;

    for (i = 0; i < size; i++, addr++, buf++)
    {
        *buf = *(uint8_t *) addr;
    }
    on_ic_read_cnt++;
    return size;
}


static int write(long offset, const uint8_t *buf, size_t size)
{
    size_t   i;
    uint32_t addr = stm32_onchip_flash.addr + offset; 
	uint32_t write_data;
    uint32_t read_data;  
	
    if(addr%4 != 0)ef_err_port_cnt++;

    flash_unlock();
    for (i = 0; i < size; i += 4, buf+=4, addr += 4) 
	{
        memcpy(&write_data, buf, 4); //用以保证HAL_FLASH_Program的第三个参数是内存首地址对齐
        flash_word_program(addr, write_data);
        read_data = *(uint32_t *)addr;
        /* You can add your code under here. */
        if (read_data != write_data) 
		{
            flash_lock(); 
            return -1;
        }else
		{	
           feed_dog(); //FLash操作可能非常耗时，如果有看门狗需要喂狗，以下代码由用户实现
        }
    }
    flash_lock(); 
    on_ic_write_cnt++;
    return size;
}


static int erase(long offset, size_t size)
{
	flash_status_type status = FLASH_OPERATE_DONE;
    uint32_t addr = stm32_onchip_flash.addr + offset;
    size_t erase_sectors, i;
    erase_sectors = size / SECTOR_SIZE;
    if (size % SECTOR_SIZE != 0) {
        erase_sectors++;
    }
    flash_unlock();
	for(i =0;i<erase_sectors;i++)
    {	
		uint32_t SectorAddress = addr +(SECTOR_SIZE*i);
		/* wait for operation to be completed */
		status = flash_operation_wait_for(ERASE_TIMEOUT);		
		if((status == FLASH_PROGRAM_ERROR) || (status == FLASH_EPP_ERROR))
		{
			flash_flag_clear(FLASH_PRGMERR_FLAG | FLASH_EPPERR_FLAG);
		}else if(status == FLASH_OPERATE_TIMEOUT)
		{  
			return ERROR;    
		}
		status = flash_sector_erase(SectorAddress);
		if(status != FLASH_OPERATE_DONE)
		{
			return ERROR;
		}
	}
    flash_lock();
    return size;
}


/*
  "stm32_onchip" : Flash 设备的名字。
  0x08000000: 	  对 Flash 操作的起始地址。
  1024*1024：Flash 的总大小（1MB）。
  128*1024：Flash 块/扇区大小（因为 STM32F2 各块大小不均匀，所以擦除粒度为最大块的大小：128K）。
  {init, read, write, erase} ：Flash 的操作函数。 如果没有 init 初始化过程，第一个操作函数位置可以置空。
  8 : 设置写粒度，单位 bit， 0 表示未生效（默认值为 0 ），该成员是 fal 版本大于 0.4.0 的新增成员。各个 flash 写入粒度不尽相同，可通过该成员进行设置，以下列举几种常见 Flash 写粒度：
  nor flash:  1 bit
  stm32f2/f4: 8 bit
  stm32f1:    32 bit
  stm32l4:    64 bit
 */

//1.定义 flash 设备

const struct fal_flash_dev stm32_onchip_flash =
{
    .name       = "stm32_onchip",
    .addr       = 0x08000000,
    .len        = 1024*1024,
    .blk_size   = 2*1024,
    .ops        = {init, read, write, erase},
    .write_gran = 32
};

