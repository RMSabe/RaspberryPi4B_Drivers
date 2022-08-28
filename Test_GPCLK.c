//A Test executable to test GPCLK output.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "GPIO_Ctrl.h"
#include "GPCLK_Ctrl.h"

#define CLK GPCLK0
#define CLK_ENDPOINT GPCLK_ENDPOINT0 //GPIO 4

int main(int argc, char **argv)
{
	if(!gpio_init())
	{
		printf("GPIO Init Error\nTerminated\n");
		return 0;
	}
	if(!gpclk_init())
	{
		printf("GPCLK Init Error\nTerminated\n");
		return 0;
	}
	printf("Drivers initialized\n");

	gpclk_init_gpio(CLK, CLK_ENDPOINT);
	gpclk_reset(CLK);
	gpclk_set_clk_src(CLK, GPCLK_CLKSRC_OSC);
	gpclk_set_mash_level(CLK, 0);
	gpclk_set_integer_divider(CLK, 4095);
	gpclk_enable(CLK, true);

	printf("Done\n");
	return 0;
}
