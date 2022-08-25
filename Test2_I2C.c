#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#include "GPIO_Ctrl.h"
#include "I2C_Ctrl.h"

#define I2C_PORT I2C_CTRL1
#define I2C_PORT_ENDPOINT I2C_ENDPOINT0

#define MCP4725_ADDR 0x60

void delay_us(uint32_t time_us)
{
	clock_t start_time = clock();
	while(clock() < (start_time + time_us));
	return;
}

void mcp4725_set_output(uint16_t value)
{
	value &= 0x0FFF;
	uint8_t high_byte = (value >> 8);
	uint8_t low_byte = (value & 0x00FF);

	i2c_set_rw_bit(I2C_PORT, I2C_WRITE_BIT);
	i2c_set_transfer_length_reg(I2C_PORT, 2);
	i2c_set_fifo_data(I2C_PORT, high_byte);
	i2c_set_fifo_data(I2C_PORT, low_byte);
	i2c_start_transfer(I2C_PORT);

	while(!i2c_transfer_done(I2C_PORT)) delay_us(100);
	i2c_clear_fifo(I2C_PORT);

	return;
}

float x = 0.0f;
float y = 0.0f;
int16_t val = 0;

void loop(void)
{
	x = 0.0f;
	while(x < 6.28f)
	{
		y = sinf(x);
		val = roundf(2047*(1.0f + y));
		mcp4725_set_output(val);
		delay_us(3200);
		x += 0.01f;
	}

	return;
}

int main(int argc, char **argv)
{
	if(!gpio_init())
	{
		printf("GPIO Init Error\nTerminated\n");
		return 0;
	}

	if(!i2c_init())
	{
		printf("I2C Init Error\nTerminated\n");
		return 0;
	}

	printf("Started\n");

	i2c_init_default(I2C_PORT, I2C_PORT_ENDPOINT, true);
	i2c_set_slave_addr(I2C_PORT, MCP4725_ADDR);

	printf("Running...\n");

	while(true) loop();
	return 0;
}
