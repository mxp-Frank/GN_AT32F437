/*
 * This file is part of the Serial Flash Universal Driver Library.
 *
 * Copyright (c) 2016, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2016-04-23
 */

#include <sfud.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "at32f435_437_conf.h"
#include "MX25L128.h"
#include "IF_GPIO.h"
typedef struct 
{
    spi_type 	   *spix;
    spi_init_type  *spi_init_type;
    gpio_type 	   *cs_gpiox;
    uint16_t        cs_gpio_pin;
	gpio_type 	   *wp_gpiox;
    uint16_t        wp_gpio_pin;
	gpio_type 	   *reset_gpiox;
    uint16_t        reset_gpio_pin;
	
} spi_user_data,  *spi_user_data_t;
static char log_buf[256];
static spi_init_type spi_init_handle;
void sfud_log_debug(const char *file, const long line, const char *format, ...);



static void spi_configuration(spi_type *spix)
{	
  spi_default_para_init(&spi_init_handle);
  spi_init_handle.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;
  spi_init_handle.master_slave_mode = SPI_MODE_MASTER;
  spi_init_handle.mclk_freq_division = SPI_MCLK_DIV_8;
  spi_init_handle.first_bit_transmission = SPI_FIRST_BIT_MSB;
  spi_init_handle.frame_bit_num = SPI_FRAME_8BIT;
  spi_init_handle.clock_polarity = SPI_CLOCK_POLARITY_HIGH;
  spi_init_handle.clock_phase = SPI_CLOCK_PHASE_2EDGE;
  spi_init_handle.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
  spi_init(spix, &spi_init_handle);
  spi_enable(spix, TRUE);
}

static void spi_lock(const sfud_spi *spi) {
    __disable_irq();
}

static void spi_unlock(const sfud_spi *spi) {
    __enable_irq();
}

/**
 * SPI write data then read data
 */
static sfud_err spi_write_read(const sfud_spi *spi, const uint8_t *write_buf, size_t write_size, uint8_t *read_buf,size_t read_size) {
    sfud_err result = SFUD_SUCCESS;
    spi_user_data_t spi_dev = (spi_user_data_t) spi->user_data;
	
	volatile uint8_t dummy_data;
    if (write_size) 
	{
        SFUD_ASSERT(write_buf);
    }
    if (read_size) 
	{
        SFUD_ASSERT(read_buf);
    }
    gpio_bits_write(spi_dev->cs_gpiox, spi_dev->cs_gpio_pin, FALSE);

	while(write_size--)
	{
		while(spi_i2s_flag_get(spi_dev->spix, SPI_I2S_TDBE_FLAG) == RESET);
		spi_i2s_data_transmit(spi_dev->spix, *write_buf);		
		while(spi_i2s_flag_get(spi_dev->spix, SPI_I2S_RDBF_FLAG) == RESET);
		dummy_data = spi_i2s_data_receive(spi_dev->spix);
		write_buf++;
	}
	if(write_size)result = SFUD_ERR_WRITE;
	
	while(read_size--)
	{
		uint8_t write_value = FLASH_SPI_DUMMY_BYTE;		
		while(spi_i2s_flag_get(spi_dev->spix, SPI_I2S_TDBE_FLAG) == RESET);
		spi_i2s_data_transmit(spi_dev->spix, write_value);
		while(spi_i2s_flag_get(spi_dev->spix, SPI_I2S_RDBF_FLAG) == RESET);
		*read_buf = spi_i2s_data_receive(spi_dev->spix);
		read_buf++;
	}
	if(read_size)result = SFUD_ERR_READ;
	
    gpio_bits_write(spi_dev->cs_gpiox, spi_dev->cs_gpio_pin, TRUE);
	
    return result;
}

/* about 100 microsecond delay */
static void retry_delay_100us(void) {
    uint32_t delay = 120;
    while(delay--);
}
static spi_user_data spi4 = 
{ 
	.spix = SPI4, 
	.cs_gpiox = FLASH_CS_PORT, 
	.cs_gpio_pin = FLASH_CS_PIN, 
	.wp_gpiox = FLASH_WP_PORT, 
	.wp_gpio_pin = FLASH_WP_PIN, 
	.reset_gpiox = FLASH_RESET_PORT, 
	.reset_gpio_pin = FLASH_RESET_PIN, 
	.spi_init_type = &spi_init_handle
};

sfud_err sfud_spi_port_init(sfud_flash *flash) 
{
    sfud_err result = SFUD_SUCCESS;

    if (!strcmp(flash->spi.name, "SPI4"))
    {
		gpio_init_type gpio_initstructure;
		crm_periph_clock_enable(CRM_SPI4_PERIPH_CLOCK, TRUE);
				
		/* software cs, pd0 as a general io to control flash cs */
		gpio_initstructure.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;
		gpio_initstructure.gpio_pull           = GPIO_PULL_UP;
		gpio_initstructure.gpio_mode           = GPIO_MODE_OUTPUT;
		gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;

		gpio_initstructure.gpio_pins = spi4.cs_gpio_pin;
		gpio_init(spi4.cs_gpiox, &gpio_initstructure);

		
		gpio_initstructure.gpio_pins =spi4.reset_gpio_pin ;
		gpio_init(spi4.reset_gpiox, &gpio_initstructure);

		gpio_initstructure.gpio_pins = spi4.wp_gpio_pin;
		gpio_init(spi4.wp_gpiox, &gpio_initstructure);

		//设置默认值 CS =1 RST = 1 WP = 1
		gpio_bits_write(spi4.cs_gpiox, spi4.cs_gpio_pin, TRUE);
		gpio_bits_write(spi4.reset_gpiox, spi4.reset_gpio_pin, TRUE);
		gpio_bits_write(spi4.wp_gpiox, spi4.wp_gpio_pin, TRUE);

		/* sck,miso,mosi */
		gpio_initstructure.gpio_pull  = GPIO_PULL_UP;
		gpio_initstructure.gpio_mode  = GPIO_MODE_MUX;
		gpio_initstructure.gpio_pins  = FLASH_CLK_PIN;
		gpio_init(FLASH_CLK_PORT, &gpio_initstructure);
		gpio_pin_mux_config(FLASH_CLK_PORT, FLASH_CLK_SOURCE, FLASH_CLK_MUX);

		gpio_initstructure.gpio_pull  = GPIO_PULL_UP;
		gpio_initstructure.gpio_mode  = GPIO_MODE_MUX;
		gpio_init(FLASH_CLK_PORT, &gpio_initstructure);
		gpio_initstructure.gpio_pins  = FLASH_MISO_PIN;
		gpio_init(FLASH_MISO_PORT, &gpio_initstructure);
		gpio_pin_mux_config(FLASH_MISO_PORT, FLASH_MISO_SOURCE, FLASH_MISO_MUX);

		gpio_initstructure.gpio_pull  = GPIO_PULL_UP;
		gpio_initstructure.gpio_mode  = GPIO_MODE_MUX;
		gpio_init(FLASH_CLK_PORT, &gpio_initstructure);
		gpio_initstructure.gpio_pins  = FLASH_MOSI_PIN;
		gpio_init(FLASH_MOSI_PORT, &gpio_initstructure);
		gpio_pin_mux_config(FLASH_MOSI_PORT,FLASH_MOSI_SOURCE, FLASH_MOSI_MUX);
		
		spi_configuration(SPI4);
		flash->spi.wr = spi_write_read;
		flash->spi.lock = spi_lock;
		flash->spi.unlock = spi_unlock;
		flash->spi.user_data = &spi4;
		/* about 100 microsecond delay */
		flash->retry.delay = retry_delay_100us;
		/* adout 60 seconds timeout */
		flash->retry.times = 60 * 10000;
    }

    return result;
}

/**
 * This function is print debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 */
void sfud_log_debug(const char *file, const long line, const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD](%s:%ld) ", file, line);
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\r\n", log_buf);
    va_end(args);
}

/**
 * This function is print routine info.
 *
 * @param format output format
 * @param ... args
 */
void sfud_log_info(const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD]");
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\r\n", log_buf);
    va_end(args);
}
