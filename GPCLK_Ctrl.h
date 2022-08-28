#ifndef GPCLK_CTRL_H
#define GPCLK_CTRL_H

#define GPCLK0 0
#define GPCLK1 1
#define GPCLK2 2

#define GPCLK_ENDPOINT0 0
#define GPCLK_ENDPOINT1 1
#define GPCLK_ENDPOINT2 2
#define GPCLK_ENDPOINT3 3

#define GPCLK_CLKSRC_NULL 0 //GND (OUTPUT = 0)
#define GPCLK_CLKSRC_OSC 1
#define GPCLK_CLKSRC_TESTDEBUG0 2
#define GPCLK_CLKSRC_TESTDEBUG1 3
#define GPCLK_CLKSRC_PLLA 4
#define GPCLK_CLKSRC_PLLC 5
#define GPCLK_CLKSRC_PLLD 6
#define GPCLK_CLKSRC_HDMIAUX 7

#include <stdbool.h>
#include <stdint.h>

//returns true if gpclk_init() have already been called.
bool gpclk_is_active(void);
//initializes header and proc file. This function must be called before calling any other functions in this header.
bool gpclk_init(void);

//The GPCLK controllers can be routed to different endpoints.
//This function can be used to map a GPCLK controller and endpoint to its respective GPIO and PINMODE.
void gpclk_map_clk_endpoint_gpio_pinmode(uint8_t gpclk, uint8_t endpoint, void *p_gpio, void *p_pinmode);

//Initializes a GPIO as GPCLK Output.
void gpclk_init_gpio(uint8_t gpclk, uint8_t endpoint);
//Reset the GPIO being used as GPCLK Output.
void gpclk_deinit_gpio(uint8_t gpclk, uint8_t endpoint);

void gpclk_reset(uint8_t gpclk);
void gpclk_enable(uint8_t gpclk, bool enable);
bool gpclk_is_enabled(uint8_t gpclk);
void gpclk_set_clk_src(uint8_t gpclk, uint8_t clk_src);
uint8_t gpclk_get_clk_src(uint8_t gpclk);
void gpclk_invert_output(uint8_t gpclk, bool invert);
bool gpclk_output_is_inverted(uint8_t gpclk);

//Mash level allows to add a fractionary divider to the src clk divider.
//Mash level 0 is integer clk divider.
//Mash level 1 to 3 allows fractionary divider to be used along with the integer divider.
//For further reference, read the GPCLK section of the BCM2711 Datasheet.
void gpclk_set_mash_level(uint8_t gpclk, uint8_t mash_level);
uint8_t gpclk_get_mash_level(uint8_t gpclk);

void gpclk_set_integer_divider(uint8_t gpclk, uint16_t divider);
uint16_t gpclk_get_integer_divider(uint8_t gpclk);
void gpclk_set_fractional_divider(uint8_t gpclk, uint16_t divider);
uint16_t gpclk_get_fractional_divider(uint8_t gpclk);
bool gpclk_is_busy(uint8_t gpclk);

//Kill bit does not self clear.
//It must be cleared to resume GPCLK operation.
//However kill bit won't reset other settings. All other settings (including enable bit) stay at their condition.
void gpclk_set_kill_bit(uint8_t gpclk, bool bit_value);
bool gpclk_get_kill_bit(uint8_t gpclk);

#endif
