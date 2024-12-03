#include "./i2c/bsp_i2c.h"
#include "./i2c/i2c_application.h"

i2c_handle_type hi2cx;

void i2c_lowlevel_init(i2c_handle_type* hi2c)
{
  gpio_init_type gpio_init_structure;

  if(hi2c->i2cx == I2Cx_PORT)
  {
		/* ʹ�ܶ�Ӧ�˿ڵ�ʱ�� */
    crm_periph_clock_enable(I2Cx_CLK, TRUE);
    crm_periph_clock_enable(I2Cx_SCL_GPIO_CLK, TRUE);
    crm_periph_clock_enable(I2Cx_SDA_GPIO_CLK, TRUE);

		/* ���ù������� */
    gpio_pin_mux_config(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_GPIO_PinsSource, I2Cx_SCL_GPIO_MUX);
    gpio_pin_mux_config(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_GPIO_PinsSource, I2Cx_SDA_GPIO_MUX);

		/* ���� GPIO �����������е����ƶ�/�������� */
    gpio_init_structure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
		/* ���� GPIO ģʽΪ����ģʽ */
    gpio_init_structure.gpio_mode           = GPIO_MODE_MUX;
		/* ���� GPIO �������Ϊ��©���ģʽ */
    gpio_init_structure.gpio_out_type       = GPIO_OUTPUT_OPEN_DRAIN;
    gpio_init_structure.gpio_pull           = GPIO_PULL_UP;

    gpio_init_structure.gpio_pins           = I2Cx_SCL_GPIO_PIN;
    gpio_init(I2Cx_SCL_GPIO_PORT, &gpio_init_structure);

    gpio_init_structure.gpio_pins           = I2Cx_SDA_GPIO_PIN;
    gpio_init(I2Cx_SDA_GPIO_PORT, &gpio_init_structure);

    /* I2C ��ʼ�� ���������ٶ� �����˲��� */
    i2c_init(hi2c->i2cx, 0x0F, I2Cx_CLKCTRL);
    /* ���ñ�����ַ */
    i2c_own_address1_set(hi2c->i2cx, I2C_ADDRESS_MODE_7BIT, I2Cx_ADDRESS);
  }
}
