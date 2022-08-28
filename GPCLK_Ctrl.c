#include "GPCLK_Ctrl.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#include "GPIO_Ctrl.h"

#define GPCLK_CTRL_PROC_FILE_DIR "/proc/GPCLK_Ctrl"
#define GPCLK_CTRL_WAIT_TIME_US 1

#define GPCLK_DATAIO_SIZE_BYTES 4
#define GPCLK_DATAIO_SIZE_UINT 1

#define GPCLK_CMD_RESET 0
#define GPCLK_CMD_SET_ENABLE 1
#define GPCLK_CMD_GET_ENABLE 2
#define GPCLK_CMD_SET_CLKSRC 3
#define GPCLK_CMD_GET_CLKSRC 4
#define GPCLK_CMD_SET_INVERT_OUTPUT 5
#define GPCLK_CMD_GET_INVERT_OUTPUT 6
#define GPCLK_CMD_SET_MASH_LEVEL 7
#define GPCLK_CMD_GET_MASH_LEVEL 8
#define GPCLK_CMD_SET_IDIVIDER 9
#define GPCLK_CMD_GET_IDIVIDER 10
#define GPCLK_CMD_SET_FDIVIDER 11
#define GPCLK_CMD_GET_FDIVIDER 12
#define GPCLK_CMD_GET_IS_BUSY 13
#define GPCLK_CMD_SET_KILL_BIT 14
#define GPCLK_CMD_GET_KILL_BIT 15

int gpclk_proc_fd = -1;
uint32_t gpclk_data_io = 0;

void gpclk_ctrl_wait(void)
{
	clock_t start_time = clock();
	while(clock() < (start_time + GPCLK_CTRL_WAIT_TIME_US));
	return;
}

bool gpclk_is_active(void)
{
	return (gpclk_proc_fd >= 0);
}

bool gpclk_init(void)
{
	if(gpclk_is_active()) return true;

	if(!gpio_is_active()) if(!gpio_init()) return false;

	gpclk_proc_fd = open(GPCLK_CTRL_PROC_FILE_DIR, O_RDWR);
	return gpclk_is_active();
}

void gpclk_map_clk_endpoint_gpio_pinmode(uint8_t gpclk, uint8_t endpoint, void *p_gpio, void *p_pinmode)
{
	switch(gpclk)
	{
		case GPCLK0:
			switch(endpoint)
			{
				case GPCLK_ENDPOINT0:
					if(p_gpio != NULL) *((uint8_t*) p_gpio) = 4;
					if(p_pinmode != NULL) *((uint8_t*) p_pinmode) = GPIO_PINMODE_ALTFUNC0;
					break;

				case GPCLK_ENDPOINT1:
					if(p_gpio != NULL) *((uint8_t*) p_gpio) = 20;
					if(p_pinmode != NULL) *((uint8_t*) p_pinmode) = GPIO_PINMODE_ALTFUNC5;
					break;

				case GPCLK_ENDPOINT2:
					if(p_gpio != NULL) *((uint8_t*) p_gpio) = 32;
					if(p_pinmode != NULL) *((uint8_t*) p_pinmode) = GPIO_PINMODE_ALTFUNC0;
					break;

				case GPCLK_ENDPOINT3:
					if(p_gpio != NULL) *((uint8_t*) p_gpio) = 34;
					if(p_pinmode != NULL) *((uint8_t*) p_pinmode) = GPIO_PINMODE_ALTFUNC0;
					break;
			}
			break;

		case GPCLK1:
			switch(endpoint)
			{
				case GPCLK_ENDPOINT0:
					if(p_gpio != NULL) *((uint8_t*) p_gpio) = 5;
					if(p_pinmode != NULL) *((uint8_t*) p_pinmode) = GPIO_PINMODE_ALTFUNC0;
					break;

				case GPCLK_ENDPOINT1:
					if(p_gpio != NULL) *((uint8_t*) p_gpio) = 21;
					if(p_pinmode != NULL) *((uint8_t*) p_pinmode) = GPIO_PINMODE_ALTFUNC5;
					break;

				case GPCLK_ENDPOINT2:
					if(p_gpio != NULL) *((uint8_t*) p_gpio) = 42;
					if(p_pinmode != NULL) *((uint8_t*) p_pinmode) = GPIO_PINMODE_ALTFUNC0;
					break;

				case GPCLK_ENDPOINT3:
					if(p_gpio != NULL) *((uint8_t*) p_gpio) = 44;
					if(p_pinmode != NULL) *((uint8_t*) p_pinmode) = GPIO_PINMODE_ALTFUNC0;
					break;
			}
			break;

		case GPCLK2:
			switch(endpoint)
			{
				case GPCLK_ENDPOINT0:
					if(p_gpio != NULL) *((uint8_t*) p_gpio) = 6;
					if(p_pinmode != NULL) *((uint8_t*) p_pinmode) = GPIO_PINMODE_ALTFUNC0;
					break;

				case GPCLK_ENDPOINT1:
					if(p_gpio != NULL) *((uint8_t*) p_gpio) = 43;
					if(p_pinmode != NULL) *((uint8_t*) p_pinmode) = GPIO_PINMODE_ALTFUNC0;
					break;
			}
			break;
	}

	return;
}

void gpclk_init_gpio(uint8_t gpclk, uint8_t endpoint)
{
	uint8_t pin = 0;
	uint8_t pinmode = 0;
	gpclk_map_clk_endpoint_gpio_pinmode(gpclk, endpoint, &pin, &pinmode);
	gpio_reset_pin(pin);
	gpio_set_pinmode(pin, pinmode);
	return;
}

void gpclk_deinit_gpio(uint8_t gpclk, uint8_t endpoint)
{
	uint8_t pin = 0;
	gpclk_map_clk_endpoint_gpio_pinmode(gpclk, endpoint, &pin, NULL);
	gpio_reset_pin(pin);
	return;
}

void gpclk_reset(uint8_t gpclk)
{
	gpclk_data_io = ((GPCLK_CMD_RESET << 24) | (gpclk << 16));
	write(gpclk_proc_fd, &gpclk_data_io, GPCLK_DATAIO_SIZE_BYTES);
	gpclk_ctrl_wait();
	return;
}

void gpclk_enable(uint8_t gpclk, bool enable)
{
	gpclk_data_io = ((GPCLK_CMD_SET_ENABLE << 24) | (gpclk << 16) | enable);
	write(gpclk_proc_fd, &gpclk_data_io, GPCLK_DATAIO_SIZE_BYTES);
	gpclk_ctrl_wait();
	return;
}

bool gpclk_is_enabled(uint8_t gpclk)
{
	gpclk_data_io = ((GPCLK_CMD_GET_ENABLE << 24) | (gpclk << 16));
	write(gpclk_proc_fd, &gpclk_data_io, GPCLK_DATAIO_SIZE_BYTES);
	gpclk_ctrl_wait();
	read(gpclk_proc_fd, &gpclk_data_io, GPCLK_DATAIO_SIZE_BYTES);
	return (gpclk_data_io & 0x00000001);
}

void gpclk_set_clk_src(uint8_t gpclk, uint8_t clk_src)
{
	gpclk_data_io = ((GPCLK_CMD_SET_CLKSRC << 24) | (gpclk << 16) | clk_src);
	write(gpclk_proc_fd, &gpclk_data_io, GPCLK_DATAIO_SIZE_BYTES);
	gpclk_ctrl_wait();
	return;
}

uint8_t gpclk_get_clk_src(uint8_t gpclk)
{
	gpclk_data_io = ((GPCLK_CMD_GET_CLKSRC << 24) | (gpclk << 16));
	write(gpclk_proc_fd, &gpclk_data_io, GPCLK_DATAIO_SIZE_BYTES);
	gpclk_ctrl_wait();
	read(gpclk_proc_fd, &gpclk_data_io, GPCLK_DATAIO_SIZE_BYTES);
	return (gpclk_data_io & 0x000000FF);
}

void gpclk_invert_output(uint8_t gpclk, bool invert)
{
	gpclk_data_io = ((GPCLK_CMD_SET_INVERT_OUTPUT << 24) | (gpclk << 16) | invert);
	write(gpclk_proc_fd, &gpclk_data_io, GPCLK_DATAIO_SIZE_BYTES);
	gpclk_ctrl_wait();
	return;
}

bool gpclk_output_is_inverted(uint8_t gpclk)
{
	gpclk_data_io = ((GPCLK_CMD_GET_INVERT_OUTPUT << 24) | (gpclk << 16));
	write(gpclk_proc_fd, &gpclk_data_io, GPCLK_DATAIO_SIZE_BYTES);
	gpclk_ctrl_wait();
	read(gpclk_proc_fd, &gpclk_data_io, GPCLK_DATAIO_SIZE_BYTES);
	return (gpclk_data_io & 0x00000001);
}

void gpclk_set_mash_level(uint8_t gpclk, uint8_t mash_level)
{
	gpclk_data_io = ((GPCLK_CMD_SET_MASH_LEVEL << 24) | (gpclk << 16) | mash_level);
	write(gpclk_proc_fd, &gpclk_data_io, GPCLK_DATAIO_SIZE_BYTES);
	gpclk_ctrl_wait();
	return;
}

uint8_t gpclk_get_mash_level(uint8_t gpclk)
{
	gpclk_data_io = ((GPCLK_CMD_GET_MASH_LEVEL << 24) | (gpclk << 16));
	write(gpclk_proc_fd, &gpclk_data_io, GPCLK_DATAIO_SIZE_BYTES);
	gpclk_ctrl_wait();
	read(gpclk_proc_fd, &gpclk_data_io, GPCLK_DATAIO_SIZE_BYTES);
	return (gpclk_data_io & 0x000000FF);
}

void gpclk_set_integer_divider(uint8_t gpclk, uint16_t divider)
{
	gpclk_data_io = ((GPCLK_CMD_SET_IDIVIDER << 24) | (gpclk << 16) | divider);
	write(gpclk_proc_fd, &gpclk_data_io, GPCLK_DATAIO_SIZE_BYTES);
	gpclk_ctrl_wait();
	return;
}

uint16_t gpclk_get_integer_divider(uint8_t gpclk)
{
	gpclk_data_io = ((GPCLK_CMD_GET_IDIVIDER << 24) | (gpclk << 16));
	write(gpclk_proc_fd, &gpclk_data_io, GPCLK_DATAIO_SIZE_BYTES);
	gpclk_ctrl_wait();
	read(gpclk_proc_fd, &gpclk_data_io, GPCLK_DATAIO_SIZE_BYTES);
	return (gpclk_data_io & 0x0000FFFF);
}

void gpclk_set_fractional_divider(uint8_t gpclk, uint16_t divider)
{
	gpclk_data_io = ((GPCLK_CMD_SET_FDIVIDER << 24) | (gpclk << 16) | divider);
	write(gpclk_proc_fd, &gpclk_data_io, GPCLK_DATAIO_SIZE_BYTES);
	gpclk_ctrl_wait();
	return;
}

uint16_t gpclk_get_fractional_divider(uint8_t gpclk)
{
	gpclk_data_io = ((GPCLK_CMD_GET_FDIVIDER << 24) | (gpclk << 16));
	write(gpclk_proc_fd, &gpclk_data_io, GPCLK_DATAIO_SIZE_BYTES);
	gpclk_ctrl_wait();
	read(gpclk_proc_fd, &gpclk_data_io, GPCLK_DATAIO_SIZE_BYTES);
	return (gpclk_data_io & 0x0000FFFF);
}

bool gpclk_is_busy(uint8_t gpclk)
{
	gpclk_data_io = ((GPCLK_CMD_GET_IS_BUSY << 24) | (gpclk << 16));
	write(gpclk_proc_fd, &gpclk_data_io, GPCLK_DATAIO_SIZE_BYTES);
	gpclk_ctrl_wait();
	read(gpclk_proc_fd, &gpclk_data_io, GPCLK_DATAIO_SIZE_BYTES);
	return (gpclk_data_io & 0x00000001);
}

void gpclk_set_kill_bit(uint8_t gpclk, bool bit_value)
{
	gpclk_data_io = ((GPCLK_CMD_SET_KILL_BIT << 24) | (gpclk << 16) | bit_value);
	write(gpclk_proc_fd, &gpclk_data_io, GPCLK_DATAIO_SIZE_BYTES);
	gpclk_ctrl_wait();
	return;
}

bool gpclk_get_kill_bit(uint8_t gpclk)
{
	gpclk_data_io = ((GPCLK_CMD_GET_KILL_BIT << 24) | (gpclk << 16));
	write(gpclk_proc_fd, &gpclk_data_io, GPCLK_DATAIO_SIZE_BYTES);
	gpclk_ctrl_wait();
	read(gpclk_proc_fd, &gpclk_data_io, GPCLK_DATAIO_SIZE_BYTES);
	return (gpclk_data_io & 0x00000001);
}
