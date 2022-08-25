#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "GPIO_Ctrl.h"
#include "I2C_Ctrl.h"

#define PCF8574_ADDR 0x27
#define DELAY_TIME_US 1000000

#define I2C_PORT I2C_CTRL1
#define I2C_TEST_ENDPOINT I2C_ENDPOINT0

void delay_us(uint32_t time_us)
{
	clock_t start_time = clock();
	while(clock() < (start_time + time_us));
	return;
}

void send_single_byte(uint8_t byte)
{
	i2c_set_rw_bit(I2C_PORT, I2C_WRITE_BIT);
	i2c_set_transfer_length_reg(I2C_PORT, 1);
	i2c_set_fifo_data(I2C_PORT, byte);
	i2c_start_transfer(I2C_PORT);

	while(!i2c_transfer_done(I2C_PORT)) delay_us(1000);
	i2c_clear_fifo(I2C_PORT);

	return;
}

uint8_t read_single_byte(void)
{
	i2c_set_rw_bit(I2C_PORT, I2C_READ_BIT);
	i2c_set_transfer_length_reg(I2C_PORT, 1);
	i2c_start_transfer(I2C_PORT);

	while(!i2c_transfer_done(I2C_PORT)) delay_us(1000);
	return i2c_get_fifo_data(I2C_PORT);
}

uint8_t n_byte = 0;

void loop(void)
{
	send_single_byte(n_byte);
	delay_us(DELAY_TIME_US);
	n_byte++;
	return;
}

int main(int argc, char **argv)
{
	printf("Started\n");
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
	printf("Running...\n");

	i2c_init_default(I2C_PORT, I2C_TEST_ENDPOINT, false);
	i2c_set_slave_addr(I2C_PORT, PCF8574_ADDR);

	while(true) loop();

	printf("Terminated\n");
	return 0;
}
