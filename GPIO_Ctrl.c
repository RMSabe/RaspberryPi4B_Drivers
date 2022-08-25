#include "GPIO_Ctrl.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define GPIO_CTRL_PROC_FILE_DIR "/proc/GPIO_Ctrl"
#define GPIO_CTRL_WAIT_TIME_US 1

#define GPIO_DATAIO_SIZE_BYTES 4
#define GPIO_DATAIO_SIZE_UINT 1

#define GPIO_CMD_RESET_PIN 0
#define GPIO_CMD_SET_LEVEL 1
#define GPIO_CMD_GET_LEVEL 2
#define GPIO_CMD_SET_PINMODE 3
#define GPIO_CMD_GET_PINMODE 4
#define GPIO_CMD_SET_PUDCTRL 5
#define GPIO_CMD_GET_PUDCTRL 6
#define GPIO_CMD_GET_EVENT_DETECTED 7
#define GPIO_CMD_SET_ENABLE_REDGEDETECT 8
#define GPIO_CMD_GET_ENABLE_REDGEDETECT 9
#define GPIO_CMD_SET_ENABLE_FEDGEDETECT 10
#define GPIO_CMD_GET_ENABLE_FEDGEDETECT 11
#define GPIO_CMD_SET_ENABLE_ASYNC_REDGEDETECT 12
#define GPIO_CMD_GET_ENABLE_ASYNC_REDGEDETECT 13
#define GPIO_CMD_SET_ENABLE_ASYNC_FEDGEDETECT 14
#define GPIO_CMD_GET_ENABLE_ASYNC_FEDGEDETECT 15
#define GPIO_CMD_SET_ENABLE_HIGHDETECT 16
#define GPIO_CMD_GET_ENABLE_HIGHDETECT 17
#define GPIO_CMD_SET_ENABLE_LOWDETECT 18
#define GPIO_CMD_GET_ENABLE_LOWDETECT 19

uint32_t gpio_data_io = 0;
int gpio_proc_fd = -1;

bool gpio_is_active(void)
{
	return (gpio_proc_fd >= 0);
}

void gpio_ctrl_wait(void)
{
	clock_t start_time = clock();
	while(clock() < (start_time + GPIO_CTRL_WAIT_TIME_US));
	return;
}

bool gpio_init(void)
{
	if(gpio_is_active()) return true;

	gpio_proc_fd = open(GPIO_CTRL_PROC_FILE_DIR, O_RDWR);
	return gpio_is_active();
}

void gpio_reset_pin(uint8_t pin_number)
{
	gpio_data_io = ((GPIO_CMD_RESET_PIN << 24) | (pin_number << 16));
	write(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	gpio_ctrl_wait();
	return;
}

void gpio_set_pinmode(uint8_t pin_number, uint8_t pinmode)
{
	gpio_data_io = ((GPIO_CMD_SET_PINMODE << 24) | (pin_number << 16) | pinmode);
	write(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	gpio_ctrl_wait();
	return;
}

uint8_t gpio_get_pinmode(uint8_t pin_number)
{
	gpio_data_io = ((GPIO_CMD_GET_PINMODE << 24) | (pin_number << 16));
	write(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	gpio_ctrl_wait();
	read(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	return (gpio_data_io & 0x000000FF);
}

void gpio_set_pudctrl(uint8_t pin_number, uint8_t pudctrl)
{
	gpio_data_io = ((GPIO_CMD_SET_PUDCTRL << 24) | (pin_number << 16) | pudctrl);
	write(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	gpio_ctrl_wait();
	return;
}

uint8_t gpio_get_pudctrl(uint8_t pin_number)
{
	gpio_data_io = ((GPIO_CMD_GET_PUDCTRL << 24) | (pin_number << 16));
	write(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	gpio_ctrl_wait();
	read(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	return (gpio_data_io & 0x000000FF);
}

void gpio_set_level(uint8_t pin_number, bool level)
{
	gpio_data_io = ((GPIO_CMD_SET_LEVEL << 24) | (pin_number << 16) | level);
	write(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	gpio_ctrl_wait();
	return;
}

bool gpio_get_level(uint8_t pin_number)
{
	gpio_data_io = ((GPIO_CMD_GET_LEVEL << 24) | (pin_number << 16));
	write(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	gpio_ctrl_wait();
	read(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	return (gpio_data_io & 0x00000001);
}

bool gpio_event_detected(uint8_t pin_number)
{
	gpio_data_io = ((GPIO_CMD_GET_EVENT_DETECTED << 24) | (pin_number << 16));
	write(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	gpio_ctrl_wait();
	read(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	return (gpio_data_io & 0x00000001);
}

void gpio_enable_risingedge_detect(uint8_t pin_number, bool enable)
{
	gpio_data_io = ((GPIO_CMD_SET_ENABLE_REDGEDETECT << 24) | (pin_number << 16) | enable);
	write(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	gpio_ctrl_wait();
	return;
}

bool gpio_risingedge_detect_is_enabled(uint8_t pin_number)
{
	gpio_data_io = ((GPIO_CMD_GET_ENABLE_REDGEDETECT << 24) | (pin_number << 16));
	write(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	gpio_ctrl_wait();
	read(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	return (gpio_data_io & 0x00000001);
}

void gpio_enable_fallingedge_detect(uint8_t pin_number, bool enable)
{
	gpio_data_io = ((GPIO_CMD_SET_ENABLE_FEDGEDETECT << 24) | (pin_number << 16) | enable);
	write(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	gpio_ctrl_wait();
	return;
}

bool gpio_fallingedge_detect_is_enabled(uint8_t pin_number)
{
	gpio_data_io = ((GPIO_CMD_GET_ENABLE_FEDGEDETECT << 24) | (pin_number << 16));
	write(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	gpio_ctrl_wait();
	read(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	return (gpio_data_io & 0x00000001);
}

void gpio_enable_async_risingedge_detect(uint8_t pin_number, bool enable)
{
	gpio_data_io = ((GPIO_CMD_SET_ENABLE_ASYNC_REDGEDETECT << 24) | (pin_number << 16) | enable);
	write(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	gpio_ctrl_wait();
	return;
}

bool gpio_async_risingedge_detect_is_enabled(uint8_t pin_number)
{
	gpio_data_io = ((GPIO_CMD_GET_ENABLE_ASYNC_REDGEDETECT << 24) | (pin_number << 16));
	write(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	gpio_ctrl_wait();
	read(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	return (gpio_data_io & 0x00000001);
}

void gpio_enable_async_fallingedge_detect(uint8_t pin_number, bool enable)
{
	gpio_data_io = ((GPIO_CMD_SET_ENABLE_ASYNC_FEDGEDETECT << 24) | (pin_number << 16) | enable);
	write(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	gpio_ctrl_wait();
	return;
}

bool gpio_async_fallingedge_detect_is_enabled(uint8_t pin_number)
{
	gpio_data_io = ((GPIO_CMD_GET_ENABLE_ASYNC_FEDGEDETECT << 24) | (pin_number << 16));
	write(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	gpio_ctrl_wait();
	read(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	return (gpio_data_io & 0x00000001);
}

void gpio_enable_high_detect(uint8_t pin_number, bool enable)
{
	gpio_data_io = ((GPIO_CMD_SET_ENABLE_HIGHDETECT << 24) | (pin_number << 16) | enable);
	write(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	gpio_ctrl_wait();
	return;
}

bool gpio_high_detect_is_enabled(uint8_t pin_number)
{
	gpio_data_io = ((GPIO_CMD_GET_ENABLE_HIGHDETECT << 24) | (pin_number << 16));
	write(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	gpio_ctrl_wait();
	read(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	return (gpio_data_io & 0x00000001);
}

void gpio_enable_low_detect(uint8_t pin_number, bool enable)
{
	gpio_data_io = ((GPIO_CMD_SET_ENABLE_LOWDETECT << 24) | (pin_number << 16) | enable);
	write(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	gpio_ctrl_wait();
	return;
}

bool gpio_low_detect_is_enabled(uint8_t pin_number)
{
	gpio_data_io = ((GPIO_CMD_GET_ENABLE_LOWDETECT << 24) | (pin_number << 16));
	write(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	gpio_ctrl_wait();
	read(gpio_proc_fd, &gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	return (gpio_data_io & 0x00000001);
}
