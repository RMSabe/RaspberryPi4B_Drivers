//General Purpose Clock

#ifndef GPCLK_CTRL_H
#define GPCLK_CTRL_H

//GPCLK Controllers.
#define GPCLK0 0
#define GPCLK1 1
#define GPCLK2 2

//GPCLK Pin Endpoints.
#define GPCLK_ENDPOINT0 0
#define GPCLK_ENDPOINT1 1
#define GPCLK_ENDPOINT2 2
#define GPCLK_ENDPOINT3 3

//GPCLK Clock Source.
#define GPCLK_CLKSRC_NULL 0 //GND == Clock output stays on 0
#define GPCLK_CLKSRC_OSC 1
#define GPCLK_CLKSRC_TESTDEBUG0 2
#define GPCLK_CLKSRC_TESTDEBUG1 3
#define GPCLK_CLKSRC_PLLA 4
#define GPCLK_CLKSRC_PLLC 5
#define GPCLK_CLKSRC_PLLD 6
#define GPCLK_CLKSRC_HDMIAUX 7

#include <stdbool.h>
#include <stdint.h>

//Returns true if gpclk_init() has already been called.
bool gpclk_is_active(void);
//Initializes GPCLK Driver and Header. This function must be called before calling any other functions in this header. Returns true if driver initialized successfully.
bool gpclk_init(void);

//Maps a GPCLK controller and endpoint to its respective GPIO Pin and Function. This may be useful if user wants to manually control the GPCLK output GPIO.
void gpclk_map_clk_endpoint_gpio_pinmode(uint8_t gpclk, uint8_t endpoint, uint8_t *p_gpio, uint8_t *p_pinmode);

//Sets the endpoint GPIO pin to output the GPCLK controller signal.
void gpclk_init_gpio(uint8_t gpclk, uint8_t endpoint);
//Resets the endpoint GPIO pin.
void gpclk_deinit_gpio(uint8_t gpclk, uint8_t endpoint);

void gpclk_reset(uint8_t gpclk);
void gpclk_enable(uint8_t gpclk, bool enable);
bool gpclk_is_enabled(uint8_t gpclk);
void gpclk_set_clk_src(uint8_t gpclk, uint16_t clk_src);
uint16_t gpclk_get_clk_src(uint8_t gpclk);
void gpclk_invert_output(uint8_t gpclk, bool invert);
bool gpclk_output_is_inverted(uint8_t gpclk);

//Mash Level: This feature adds a fractional value to the integer clock divider. See BCM2711 Datasheet for further reference.
//0: No MASH (Integer Clock Divider only)
//1-3: MASH Level: Integer and Fractional Clock Divider.
void gpclk_set_mash_level(uint8_t gpclk, uint16_t mash_level);
uint16_t gpclk_get_mash_level(uint8_t gpclk);

void gpclk_set_integer_divider(uint8_t gpclk, uint16_t divider);
uint16_t gpclk_get_integer_divider(uint8_t gpclk);
void gpclk_set_fractional_divider(uint8_t gpclk, uint16_t divider);
uint16_t gpclk_get_fractional_divider(uint8_t gpclk);
bool gpclk_is_busy(uint8_t gpclk);

//When bit is set, it immediatly stops the clock signal. Other settings (including enable) will remain unchanged.
//This bit must be cleared manually to resume clock operation.
void gpclk_set_kill_bit(uint8_t gpclk, bool bit_value);
bool gpclk_get_kill_bit(uint8_t gpclk);

#endif
