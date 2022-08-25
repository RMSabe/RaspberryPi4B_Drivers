//A Test executable to test GPIO input/output.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "GPIO_Ctrl.h"

#define BUTTON_0 4
#define BUTTON_1 5

#define LED_0 12
#define LED_1 13
#define LED_2 16
#define LED_3 17

void delay_us(uint32_t time_us)
{
	clock_t start_time = clock();
	while(clock() < (start_time + time_us));
	return;
}

void led_set_output(uint8_t value)
{
	value &= 0x0F;

	gpio_set_level(LED_3, (value & 0x8));
	gpio_set_level(LED_2, (value & 0x4));
	gpio_set_level(LED_1, (value & 0x2));
	gpio_set_level(LED_0, (value & 0x1));

	return;
}

uint8_t led_val = 0;

void loop(void)
{
	if(!gpio_get_level(BUTTON_0))
	{
		led_val--;
		led_set_output(led_val);
		while(!gpio_get_level(BUTTON_0)) delay_us(1000);
	}
	if(!gpio_get_level(BUTTON_1))
	{
		led_val++;
		led_set_output(led_val);
		while(!gpio_get_level(BUTTON_1)) delay_us(1000);
	}

	delay_us(10000);
	return;
}

int main(int argc, char **argv)
{
	if(!gpio_init())
	{
		printf("GPIO Init Error\nTerminated\n");
		return 0;
	}
	printf("Running...\n");
	
	gpio_reset_pin(LED_0);
	gpio_reset_pin(LED_1);
	gpio_reset_pin(LED_2);
	gpio_reset_pin(LED_3);

	gpio_set_pinmode(LED_0, GPIO_PINMODE_OUTPUT);
	gpio_set_pinmode(LED_1, GPIO_PINMODE_OUTPUT);
	gpio_set_pinmode(LED_2, GPIO_PINMODE_OUTPUT);
	gpio_set_pinmode(LED_3, GPIO_PINMODE_OUTPUT);

	gpio_reset_pin(BUTTON_0);
	gpio_reset_pin(BUTTON_1);
	
	gpio_set_pinmode(BUTTON_0, GPIO_PINMODE_INPUT);
	gpio_set_pinmode(BUTTON_1, GPIO_PINMODE_INPUT);
	gpio_set_pudctrl(BUTTON_0, GPIO_PUDCTRL_PULLUP);
	gpio_set_pudctrl(BUTTON_1, GPIO_PUDCTRL_PULLUP);

	while(true) loop();
	return 0;
}
