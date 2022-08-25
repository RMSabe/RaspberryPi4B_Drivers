#include "I2C_Ctrl.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "GPIO_Ctrl.h"

#define I2C_CTRL_PROC_FILE_DIR "/proc/I2C_Ctrl"
#define I2C_CTRL_WAIT_TIME_US 1

#define I2C_CMD_INIT_DEFAULT 0
#define I2C_CMD_DEINIT_DEFAULT 1
#define I2C_CMD_SET_ENABLE 2
#define I2C_CMD_GET_ENABLE 3
#define I2C_CMD_SET_ENABLE_INTR_ON_RX 4
#define I2C_CMD_GET_ENABLE_INTR_ON_RX 5
#define I2C_CMD_SET_ENABLE_INTR_ON_TX 6
#define I2C_CMD_GET_ENABLE_INTR_ON_TX 7
#define I2C_CMD_SET_ENABLE_INTR_ON_DONE 8
#define I2C_CMD_GET_ENABLE_INTR_ON_DONE 9
#define I2C_CMD_START_TRANSFER 10
#define I2C_CMD_CLEAR_FIFO 11
#define I2C_CMD_SET_RW_BIT 12
#define I2C_CMD_GET_RW_BIT 13
#define I2C_CMD_GET_TIMEOUT_OCCURRED 14
#define I2C_CMD_GET_ACK_ERR_OCCURRED 15
#define I2C_CMD_GET_FIFO_IS_FULL 16
#define I2C_CMD_GET_FIFO_IS_EMPTY 17
#define I2C_CMD_GET_FIFO_HAS_DATA 18
#define I2C_CMD_GET_FIFO_FITS_DATA 19
#define I2C_CMD_GET_FIFO_ALMOST_FULL 20
#define I2C_CMD_GET_FIFO_ALMOST_EMPTY 21
#define I2C_CMD_GET_TRANSFER_DONE 22
#define I2C_CMD_GET_TRANSFER_IS_ACTIVE 23
#define I2C_CMD_SET_TRANSFER_LENGTH_REG 24
#define I2C_CMD_GET_TRANSFER_LENGTH_REG 25
#define I2C_CMD_SET_SLAVE_ADDR 26
#define I2C_CMD_GET_SLAVE_ADDR 27
#define I2C_CMD_SET_FIFO_DATA 28
#define I2C_CMD_GET_FIFO_DATA 29
#define I2C_CMD_SET_CLKDIV 30
#define I2C_CMD_GET_CLKDIV 31
#define I2C_CMD_SET_REDGE_DELAY 32
#define I2C_CMD_GET_REDGE_DELAY 33
#define I2C_CMD_SET_FEDGE_DELAY 34
#define I2C_CMD_GET_FEDGE_DELAY 35
#define I2C_CMD_SET_TIMEOUT 36
#define I2C_CMD_GET_TIMEOUT 37
#define I2C_CMD_SET_CLKDIV_STANDARD 38
#define I2C_CMD_SET_FULL_PERIOD 39

#define I2C_DATAIO_SIZE_BYTES 4
#define I2C_DATAIO_SIZE_UINT 1

int i2c_proc_fd = -1;
uint32_t i2c_data_io = 0;

void i2c_ctrl_wait(void)
{
	clock_t start_time = clock();
	while(clock() < (start_time + I2C_CTRL_WAIT_TIME_US));
	return;
}

void i2c_ctrl_endpoint_map_to_gpio_pinmode(uint8_t i2c_ctrl, uint8_t endpoint, void *p_sda_pin, void *p_scl_pin, void *p_pinmode)
{
	switch(i2c_ctrl)
	{
		case I2C_CTRL0:
			switch(endpoint)
			{
				case I2C_ENDPOINT0:
					if(p_pinmode != NULL) *((uint8_t*) p_pinmode) = GPIO_PINMODE_ALTFUNC0;
					if(p_sda_pin != NULL) *((uint8_t*) p_sda_pin) = 0;
					if(p_scl_pin != NULL) *((uint8_t*) p_scl_pin) = 1;
					break;

				case I2C_ENDPOINT1:
					if(p_pinmode != NULL) *((uint8_t*) p_pinmode) = GPIO_PINMODE_ALTFUNC0;
					if(p_sda_pin != NULL) *((uint8_t*) p_sda_pin) = 28;
					if(p_scl_pin != NULL) *((uint8_t*) p_scl_pin) = 29;
					break;

				case I2C_ENDPOINT2:
					if(p_pinmode != NULL) *((uint8_t*) p_pinmode) = GPIO_PINMODE_ALTFUNC1;
					if(p_sda_pin != NULL) *((uint8_t*) p_sda_pin) = 44;
					if(p_scl_pin != NULL) *((uint8_t*) p_scl_pin) = 45;
					break;
			}
			break;

		case I2C_CTRL1:
			switch(endpoint)
			{
				case I2C_ENDPOINT0:
					if(p_pinmode != NULL) *((uint8_t*) p_pinmode) = GPIO_PINMODE_ALTFUNC0;
					if(p_sda_pin != NULL) *((uint8_t*) p_sda_pin) = 2;
					if(p_scl_pin != NULL) *((uint8_t*) p_scl_pin) = 3;
					break;

				case I2C_ENDPOINT1:
					if(p_pinmode != NULL) *((uint8_t*) p_pinmode) = GPIO_PINMODE_ALTFUNC2;
					if(p_sda_pin != NULL) *((uint8_t*) p_sda_pin) = 44;
					if(p_scl_pin != NULL) *((uint8_t*) p_scl_pin) = 45;
					break;
			}
			break;

		case I2C_CTRL3:
			if(p_pinmode != NULL) *((uint8_t*) p_pinmode) = GPIO_PINMODE_ALTFUNC5;
			switch(endpoint)
			{
				case I2C_ENDPOINT0:
					if(p_sda_pin != NULL) *((uint8_t*) p_sda_pin) = 2;
					if(p_scl_pin != NULL) *((uint8_t*) p_scl_pin) = 3;
					break;

				case I2C_ENDPOINT1:
					if(p_sda_pin != NULL) *((uint8_t*) p_sda_pin) = 4;
					if(p_scl_pin != NULL) *((uint8_t*) p_scl_pin) = 5;
					break;
			}
			break;

		case I2C_CTRL4:
			if(p_pinmode != NULL) *((uint8_t*) p_pinmode) = GPIO_PINMODE_ALTFUNC5;
			switch(endpoint)
			{
				case I2C_ENDPOINT0:
					if(p_sda_pin != NULL) *((uint8_t*) p_sda_pin) = 6;
					if(p_scl_pin != NULL) *((uint8_t*) p_scl_pin) = 7;
					break;

				case I2C_ENDPOINT1:
					if(p_sda_pin != NULL) *((uint8_t*) p_sda_pin) = 8;
					if(p_scl_pin != NULL) *((uint8_t*) p_scl_pin) = 9;
					break;
			}
			break;

		case I2C_CTRL5:
			if(p_pinmode != NULL) *((uint8_t*) p_pinmode) = GPIO_PINMODE_ALTFUNC5;
			switch(endpoint)
			{
				case I2C_ENDPOINT0:
					if(p_sda_pin != NULL) *((uint8_t*) p_sda_pin) = 10;
					if(p_scl_pin != NULL) *((uint8_t*) p_scl_pin) = 11;
					break;

				case I2C_ENDPOINT1:
					if(p_sda_pin != NULL) *((uint8_t*) p_sda_pin) = 12;
					if(p_scl_pin != NULL) *((uint8_t*) p_scl_pin) = 13;
					break;
			}
			break;

		case I2C_CTRL6:
			if(p_pinmode != NULL) *((uint8_t*) p_pinmode) = GPIO_PINMODE_ALTFUNC5;
			switch(endpoint)
			{
				case I2C_ENDPOINT0:
					if(p_sda_pin != NULL) *((uint8_t*) p_sda_pin) = 0;
					if(p_scl_pin != NULL) *((uint8_t*) p_scl_pin) = 1;
					break;

				case I2C_ENDPOINT1:
					if(p_sda_pin != NULL) *((uint8_t*) p_sda_pin) = 22;
					if(p_scl_pin != NULL) *((uint8_t*) p_scl_pin) = 23;
					break;
			}
			break;
	}

	return;
}

bool i2c_is_active(void)
{
	return (i2c_proc_fd >= 0);
}

bool i2c_init(void)
{
	if(i2c_is_active()) return true;

	if(!gpio_is_active()) if(!gpio_init()) return false;

	i2c_proc_fd = open(I2C_CTRL_PROC_FILE_DIR, O_RDWR);
	if(i2c_proc_fd < 0) return false;

	return true;
}

void i2c_init_default(uint8_t i2c_ctrl, uint8_t i2c_endpoint, bool use_400kbps)
{
	uint8_t pinmode = 0;
	uint8_t sda_pin = 0;
	uint8_t scl_pin = 0;
	i2c_ctrl_endpoint_map_to_gpio_pinmode(i2c_ctrl, i2c_endpoint, &sda_pin, &scl_pin, &pinmode);

	gpio_reset_pin(sda_pin);
	gpio_reset_pin(scl_pin);
	gpio_set_pinmode(sda_pin, pinmode);
	gpio_set_pinmode(scl_pin, pinmode);
	gpio_set_pudctrl(sda_pin, GPIO_PUDCTRL_PULLUP);
	gpio_set_pudctrl(scl_pin, GPIO_PUDCTRL_PULLUP);

	i2c_data_io = ((I2C_CMD_INIT_DEFAULT << 24) | (i2c_ctrl << 16) | (use_400kbps));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	return;
}

void i2c_deinit_default(uint8_t i2c_ctrl, uint8_t i2c_endpoint)
{
	uint8_t sda_pin = 0;
	uint8_t scl_pin = 0;
	i2c_ctrl_endpoint_map_to_gpio_pinmode(i2c_ctrl, i2c_endpoint, &sda_pin, &scl_pin, NULL);

	gpio_reset_pin(sda_pin);
	gpio_reset_pin(scl_pin);

	i2c_data_io = ((I2C_CMD_DEINIT_DEFAULT << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	return;
}

void i2c_enable(uint8_t i2c_ctrl, bool enable)
{
	i2c_data_io = ((I2C_CMD_SET_ENABLE << 24) | (i2c_ctrl << 16) | (enable));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	return;
}

bool i2c_is_enabled(uint8_t i2c_ctrl)
{
	i2c_data_io = ((I2C_CMD_GET_ENABLE << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	read(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	return (i2c_data_io & 0x00000001);
}

void i2c_enable_intr_on_rx(uint8_t i2c_ctrl, bool enable)
{
	i2c_data_io = ((I2C_CMD_SET_ENABLE_INTR_ON_RX << 24) | (i2c_ctrl << 16) | (enable));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	return;
}

bool i2c_intr_on_rx_is_enabled(uint8_t i2c_ctrl)
{
	i2c_data_io = ((I2C_CMD_GET_ENABLE_INTR_ON_RX << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	read(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	return (i2c_data_io & 0x00000001);
}

void i2c_enable_intr_on_tx(uint8_t i2c_ctrl, bool enable)
{
	i2c_data_io = ((I2C_CMD_SET_ENABLE_INTR_ON_TX << 24) | (i2c_ctrl << 16) | (enable));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	return;
}

bool i2c_intr_on_tx_is_enabled(uint8_t i2c_ctrl)
{
	i2c_data_io = ((I2C_CMD_GET_ENABLE_INTR_ON_TX << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	read(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	return (i2c_data_io & 0x00000001);
}

void i2c_enable_intr_on_done(uint8_t i2c_ctrl, bool enable)
{
	i2c_data_io = ((I2C_CMD_SET_ENABLE_INTR_ON_DONE << 24) | (i2c_ctrl << 16) | (enable));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	return;
}

bool i2c_intr_on_done_is_enabled(uint8_t i2c_ctrl)
{
	i2c_data_io = ((I2C_CMD_GET_ENABLE_INTR_ON_DONE << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	read(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	return (i2c_data_io & 0x00000001);
}

void i2c_start_transfer(uint8_t i2c_ctrl)
{
	i2c_data_io = ((I2C_CMD_START_TRANSFER << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	return;
}

void i2c_clear_fifo(uint8_t i2c_ctrl)
{
	i2c_data_io = ((I2C_CMD_CLEAR_FIFO << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	return;
}

void i2c_set_rw_bit(uint8_t i2c_ctrl, bool rw_bit)
{
	i2c_data_io = ((I2C_CMD_SET_RW_BIT << 24) | (i2c_ctrl << 16) | (rw_bit));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	return;
}

bool i2c_get_rw_bit(uint8_t i2c_ctrl)
{
	i2c_data_io = ((I2C_CMD_GET_RW_BIT << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	read(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	return (i2c_data_io & 0x00000001);
}

bool i2c_timeout_occurred(uint8_t i2c_ctrl)
{
	i2c_data_io = ((I2C_CMD_GET_TIMEOUT_OCCURRED << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	read(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	return (i2c_data_io & 0x00000001);
}

bool i2c_ack_err_occurred(uint8_t i2c_ctrl)
{
	i2c_data_io = ((I2C_CMD_GET_ACK_ERR_OCCURRED << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	read(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	return (i2c_data_io & 0x00000001);
}

bool i2c_fifo_is_full(uint8_t i2c_ctrl)
{
	i2c_data_io = ((I2C_CMD_GET_FIFO_IS_FULL << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	read(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	return (i2c_data_io & 0x00000001);
}

bool i2c_fifo_is_empty(uint8_t i2c_ctrl)
{
	i2c_data_io = ((I2C_CMD_GET_FIFO_IS_EMPTY << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	read(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	return (i2c_data_io & 0x00000001);
}

bool i2c_fifo_has_data(uint8_t i2c_ctrl)
{
	i2c_data_io = ((I2C_CMD_GET_FIFO_HAS_DATA << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	read(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	return (i2c_data_io & 0x00000001);
}

bool i2c_fifo_fits_data(uint8_t i2c_ctrl)
{
	i2c_data_io = ((I2C_CMD_GET_FIFO_FITS_DATA << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	read(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	return (i2c_data_io & 0x00000001);
}

bool i2c_fifo_is_almost_full(uint8_t i2c_ctrl)
{
	i2c_data_io = ((I2C_CMD_GET_FIFO_ALMOST_FULL << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	read(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	return (i2c_data_io & 0x00000001);
}

bool i2c_fifo_is_almost_empty(uint8_t i2c_ctrl)
{
	i2c_data_io = ((I2C_CMD_GET_FIFO_ALMOST_EMPTY << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	read(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	return (i2c_data_io & 0x00000001);
}

bool i2c_transfer_done(uint8_t i2c_ctrl)
{
	i2c_data_io = ((I2C_CMD_GET_TRANSFER_DONE << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	read(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	return (i2c_data_io & 0x00000001);
}

bool i2c_transfer_is_active(uint8_t i2c_ctrl)
{
	i2c_data_io = ((I2C_CMD_GET_TRANSFER_IS_ACTIVE << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	read(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	return (i2c_data_io & 0x00000001);
}

void i2c_set_transfer_length_reg(uint8_t i2c_ctrl, uint16_t length)
{
	i2c_data_io = ((I2C_CMD_SET_TRANSFER_LENGTH_REG << 24) | (i2c_ctrl << 16) | (length));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	return;
}

uint16_t i2c_get_transfer_length_reg(uint8_t i2c_ctrl)
{
	i2c_data_io = ((I2C_CMD_GET_TRANSFER_LENGTH_REG << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	read(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	return (i2c_data_io & 0x0000FFFF);
}

void i2c_set_slave_addr(uint8_t i2c_ctrl, uint16_t slave_addr)
{
	i2c_data_io = ((I2C_CMD_SET_SLAVE_ADDR << 24) | (i2c_ctrl << 16) | (slave_addr));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	return;
}

uint16_t i2c_get_slave_addr(uint8_t i2c_ctrl)
{
	i2c_data_io = ((I2C_CMD_GET_SLAVE_ADDR << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	read(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	return (i2c_data_io & 0x0000FFFF);
}

void i2c_set_fifo_data(uint8_t i2c_ctrl, uint16_t fifo_data)
{
	i2c_data_io = ((I2C_CMD_SET_FIFO_DATA << 24) | (i2c_ctrl << 16) | (fifo_data));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	return;
}

uint16_t i2c_get_fifo_data(uint8_t i2c_ctrl)
{
	i2c_data_io = ((I2C_CMD_GET_FIFO_DATA << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	read(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	return (i2c_data_io & 0x0000FFFF);
}

void i2c_set_clkdiv(uint8_t i2c_ctrl, uint16_t clkdiv)
{
	i2c_data_io = ((I2C_CMD_SET_CLKDIV << 24) | (i2c_ctrl << 16) | (clkdiv));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	return;
}

uint16_t i2c_get_clkdiv(uint8_t i2c_ctrl)
{
	i2c_data_io = ((I2C_CMD_GET_CLKDIV << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	read(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	return (i2c_data_io & 0x0000FFFF);
}

void i2c_set_risingedge_delay(uint8_t i2c_ctrl, uint16_t delay)
{
	i2c_data_io = ((I2C_CMD_SET_REDGE_DELAY << 24) | (i2c_ctrl << 16) | (delay));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	return;
}

uint16_t i2c_get_risingedge_delay(uint8_t i2c_ctrl)
{
	i2c_data_io = ((I2C_CMD_GET_REDGE_DELAY << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	read(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	return (i2c_data_io & 0x0000FFFF);
}

void i2c_set_fallingedge_delay(uint8_t i2c_ctrl, uint16_t delay)
{
	i2c_data_io = ((I2C_CMD_SET_FEDGE_DELAY << 24) | (i2c_ctrl << 16) | (delay));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	return;
}

uint16_t i2c_get_fallingedge_delay(uint8_t i2c_ctrl)
{
	i2c_data_io = ((I2C_CMD_GET_FEDGE_DELAY << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	read(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	return (i2c_data_io & 0x0000FFFF);
}

void i2c_set_timeout(uint8_t i2c_ctrl, uint16_t timeout)
{
	i2c_data_io = ((I2C_CMD_SET_TIMEOUT << 24) | (i2c_ctrl << 16) | (timeout));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	return;
}

uint16_t i2c_get_timeout(uint8_t i2c_ctrl)
{
	i2c_data_io = ((I2C_CMD_GET_TIMEOUT << 24) | (i2c_ctrl << 16));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	read(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	return (i2c_data_io & 0x0000FFFF);
}

void i2c_set_clkdiv_std(uint8_t i2c_ctrl, bool use_400kbps)
{
	i2c_data_io = ((I2C_CMD_SET_CLKDIV_STANDARD << 24) | (i2c_ctrl << 16) | (use_400kbps));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	return;
}

void i2c_set_full_period(uint8_t i2c_ctrl, bool use_400kbps)
{
	i2c_data_io = ((I2C_CMD_SET_FULL_PERIOD << 24) | (i2c_ctrl << 16) | (use_400kbps));
	write(i2c_proc_fd, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	i2c_ctrl_wait();
	return;
}
