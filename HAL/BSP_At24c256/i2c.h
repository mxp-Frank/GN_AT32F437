#ifndef __IIC_H
#define	__IIC_H

#include "at32f435_437.h"

#define I2C_TIMEOUT                      0xFFFFFFF

//#define I2Cx_CLKCTRL                   0xB170FFFF   //10K
//#define I2Cx_CLKCTRL                   0xC0E06969   //50K
//#define I2Cx_CLKCTRL                   0x80504C4E   //100K
#define I2Cx_CLKCTRL                   0x30F03C6B   //200K

#define I2Cx_ADDRESS                     0xA0

#define I2Cx_PORT                        I2C2
#define I2Cx_CLK                         CRM_I2C2_PERIPH_CLOCK


#define I2Cx_SCL_GPIO_CLK                CRM_GPIOF_PERIPH_CLOCK
#define I2Cx_SCL_GPIO_PIN                GPIO_PINS_1
#define I2Cx_SCL_GPIO_PinsSource         GPIO_PINS_SOURCE1
#define I2Cx_SCL_GPIO_PORT               GPIOF
#define I2Cx_SCL_GPIO_MUX                GPIO_MUX_4

#define I2Cx_SDA_GPIO_CLK                CRM_GPIOF_PERIPH_CLOCK
#define I2Cx_SDA_GPIO_PIN                GPIO_PINS_0
#define I2Cx_SDA_GPIO_PinsSource         GPIO_PINS_SOURCE0
#define I2Cx_SDA_GPIO_PORT               GPIOF
#define I2Cx_SDA_GPIO_MUX                GPIO_MUX_4

#define I2Cx_EVT_IRQn                    I2C2_EVT_IRQn
#define I2Cx_ERR_IRQn                    I2C2_ERR_IRQn



	
#endif


