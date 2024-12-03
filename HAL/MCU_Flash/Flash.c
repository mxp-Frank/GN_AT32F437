/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  Flash.c
 * @Brief :      ref from bsp_motor.c(AMP/AME svn324) Author: Huxian
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

#include "IF_Board.h"
#include "IF_Flash.h"
#include "Flash.h"

/* CONST & MACROS */

/* DATA STRUCTURES */

/* LOCAL VARIABLES */
uint16_t flash_buf[SECTOR_SIZE / 2];

/* GLOBAL VARIABLES */

/* FUNCTION PROTOTYPES */
static error_status IF_Flash_write_nocheck(uint32_t write_addr, uint16_t *p_buffer, uint16_t num_write);

/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/

/**
 * ************************************************************************
 * @Brief :      
 * ************************************************************************
 */
void IF_FlashInit(void)
{

}


/**
  * @brief  read data using halfword mode
  * @param  read_addr: the address of reading
  * @param  p_buffer: the buffer of reading data
  * @param  num_read: the number of reading data
  * @retval none
  */
void IF_Flash_ReadByte(uint32_t read_addr, uint8_t *p_buffer, uint16_t num_read)
{
    uint16_t i;
    for(i = 0; i < num_read; i++)
    {
        p_buffer[i] = *(volatile uint8_t*)(read_addr);
        read_addr++;
    }

}

void IF_Flash_ReadWord(uint32_t read_addr, uint16_t *p_buffer, uint16_t num_read)
{
    uint16_t i;
    for(i = 0; i < num_read; i++)
    {
        p_buffer[i] = *(volatile uint16_t*)(read_addr);
        read_addr++;
    }

}


error_status IF_Flash_SectorErase(uint32_t sectorAddr)
{//erase and program need to unlock & lock flash...

    flash_status_type status = FLASH_OPERATE_DONE;

    if((sectorAddr % SECTOR_SIZE) != 0)
        return ERROR;
    
    flash_unlock();

    /* wait for operation to be completed */
    status = flash_operation_wait_for(ERASE_TIMEOUT);
    
    if((status == FLASH_PROGRAM_ERROR) || (status == FLASH_EPP_ERROR))
        flash_flag_clear(FLASH_PRGMERR_FLAG | FLASH_EPPERR_FLAG);
    else if(status == FLASH_OPERATE_TIMEOUT)
        return ERROR;
    
    status = flash_sector_erase(sectorAddr);
    
    if(status != FLASH_OPERATE_DONE)
        return ERROR;

    flash_lock();
    return SUCCESS;
}


/**
  * @brief  write data using halfword mode without checking
  * @param  write_addr: the address of writing
  * @param  p_buffer: the buffer of writing data
  * @param  num_write: the number of writing data
  * @retval result
  */
static error_status IF_Flash_write_nocheck(uint32_t write_addr, uint16_t *p_buffer, uint16_t num_write)
{
    uint16_t i;
    flash_status_type status = FLASH_OPERATE_DONE; 
    for(i = 0; i < num_write; i++)
    {
        status = flash_halfword_program(write_addr, p_buffer[i]);
        if(status != FLASH_OPERATE_DONE)
        return ERROR;
        write_addr += 2;
    }
    return SUCCESS;
}

/**
  * @brief  write data using halfword mode with checking
  * @param  write_addr: the address of writing
  * @param  p_buffer: the buffer of writing data
  * @param  num_write: the number of writing data
  * @retval result
  */
error_status IF_Flash_write(uint32_t write_addr, uint16_t *p_buffer, uint16_t num_write)
{
    uint32_t offset_addr;
    uint32_t sector_position;
    uint16_t sector_offset;
    uint16_t sector_remain;
    uint16_t i;
    flash_status_type status = FLASH_OPERATE_DONE;
    
    flash_unlock();
    offset_addr = write_addr - FLASH_BASE;
    sector_position = offset_addr / SECTOR_SIZE;
    sector_offset = (offset_addr % SECTOR_SIZE) / 2;
    sector_remain = SECTOR_SIZE / 2 - sector_offset;
    if(num_write <= sector_remain)
        sector_remain = num_write;
    while(1)
    {
        IF_Flash_ReadWord(sector_position * SECTOR_SIZE + FLASH_BASE, flash_buf, SECTOR_SIZE/2); //read by words
        for(i = 0; i < sector_remain; i++)
        {
            if(flash_buf[sector_offset + i] != 0xFFFF)
                break;
        }
        if(i < sector_remain)
        {
            /* wait for operation to be completed */
            status = flash_operation_wait_for(ERASE_TIMEOUT);
            
            if((status == FLASH_PROGRAM_ERROR) || (status == FLASH_EPP_ERROR))
                flash_flag_clear(FLASH_PRGMERR_FLAG | FLASH_EPPERR_FLAG);
            else if(status == FLASH_OPERATE_TIMEOUT)
                return ERROR;
            
            status = flash_sector_erase(sector_position * SECTOR_SIZE + FLASH_BASE);
            
            if(status != FLASH_OPERATE_DONE)
                return ERROR;

            for(i = 0; i < sector_remain; i++)
            {
                flash_buf[i + sector_offset] = p_buffer[i];
            }

            if(IF_Flash_write_nocheck(sector_position * SECTOR_SIZE + FLASH_BASE, flash_buf, SECTOR_SIZE / 2) != SUCCESS)
                return ERROR;
        }
        else
        {
            if(IF_Flash_write_nocheck(write_addr, p_buffer, sector_remain) != SUCCESS)
                return ERROR;
        }
        if(num_write == sector_remain)
        break;
        else
        {
            sector_position++;
            sector_offset = 0;
            p_buffer += sector_remain;
            write_addr += (sector_remain * 2);
            num_write -= sector_remain;
            if(num_write > (SECTOR_SIZE / 2))
                sector_remain = SECTOR_SIZE / 2;
            else
                sector_remain = num_write;
        }
    }
    flash_lock();
    return SUCCESS;
}


//*****************************************************************************
//* END
//*****************************************************************************

