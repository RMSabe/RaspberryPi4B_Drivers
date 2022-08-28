#ifndef GPCLK_CTRL_H
#define GPCLK_CTRL_H

#define GPCLK0 0
#define GPCLK1 1
#define GPCLK2 2

#define GPCLK_ENDPOINT0 0
#define GPCLK_ENDPOINT1 1
#define GPCLK_ENDPOINT2 2
#define GPCLK_ENDPOINT3 3

#define GPCLK_CLKSRC_NULL 0
#define GPCLK_CLKSRC_OSC 1
#define GPCLK_CLKSRC_TESTDEBUG0 2
#define GPCLK_CLKSRC_TESTDEBUG1 3
#define GPCLK_CLKSRC_PLLA 4
#define GPCLK_CLKSRC_PLLC 5
#define GPCLK_CLKSRC_PLLD 6
#define GPCLK_CLKSRC_HDMIAUX 7

#include <stdbool.h>
#include <stdint.h>

bool gpclk_is_active(void);
bool gpclk_init(void);
void gpclk_map_clk_endpoint_gpio_pinmode(uint8_t gpclk, uint8_t endpoint, void *p_gpio, void *p_pinmode);
void gpclk_init_gpio(uint8_t gpclk, uint8_t endpoint);
void gpclk_deinit_gpio(uint8_t gpclk, uint8_t endpoint);
void gpclk_reset(uint8_t gpclk);
void gpclk_enable(uint8_t gpclk, bool enable);
bool gpclk_is_enabled(uint8_t gpclk);
void gpclk_set_clk_src(uint8_t gpclk, uint8_t clk_src);
uint8_t gpclk_get_clk_src(uint8_t gpclk);
void gpclk_invert_output(uint8_t gpclk, bool invert);
bool gpclk_output_is_inverted(uint8_t gpclk);
void gpclk_set_mash_level(uint8_t gpclk, uint8_t mash_level);
uint8_t gpclk_get_mash_level(uint8_t gpclk);
void gpclk_set_integer_divider(uint8_t gpclk, uint16_t divider);
uint16_t gpclk_get_integer_divider(uint8_t gpclk);
void gpclk_set_fractional_divider(uint8_t gpclk, uint16_t divider);
uint16_t gpclk_get_fractional_divider(uint8_t gpclk);
bool gpclk_is_busy(uint8_t gpclk);
void gpclk_set_kill_bit(uint8_t gpclk, bool bit_value);
bool gpclk_get_kill_bit(uint8_t gpclk);

#endif
