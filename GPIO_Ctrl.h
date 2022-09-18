#ifndef GPIO_CTRL_H
#define GPIO_CTRL_H

//GPIO Pin Function
#define GPIO_PINMODE_INPUT 0
#define GPIO_PINMODE_OUTPUT 1
#define GPIO_PINMODE_ALTFUNC0 4
#define GPIO_PINMODE_ALTFUNC1 5
#define GPIO_PINMODE_ALTFUNC2 6
#define GPIO_PINMODE_ALTFUNC3 7
#define GPIO_PINMODE_ALTFUNC4 3
#define GPIO_PINMODE_ALTFUNC5 2

//GPIO Pull up/down
#define GPIO_PUDCTRL_NOPULL 0
#define GPIO_PUDCTRL_PULLUP 1
#define GPIO_PUDCTRL_PULLDOWN 2

#include <stdbool.h>
#include <stdint.h>

//Returns true if gpio_init() have already been called.
bool gpio_is_active(void);
//Initializes the GPIO Driver and header. This function must be called before calling any other functions in this header. Returns true if driver initialized successfully.
bool gpio_init(void);

void gpio_reset_pin(uint8_t pin_number);
void gpio_set_pinmode(uint8_t pin_number, uint8_t pinmode);
uint8_t gpio_get_pinmode(uint8_t pin_number);
void gpio_set_pudctrl(uint8_t pin_number, uint8_t pudctrl);
uint8_t gpio_get_pudctrl(uint8_t pin_number);
void gpio_set_level(uint8_t pin_number, bool level);
bool gpio_get_level(uint8_t pin_number);
bool gpio_event_detected(uint8_t pin_number);
void gpio_enable_risingedge_detect(uint8_t pin_number, bool enable);
bool gpio_risingedge_detect_is_enabled(uint8_t pin_number);
void gpio_enable_fallingedge_detect(uint8_t pin_number, bool enable);
bool gpio_fallingedge_detect_is_enabled(uint8_t pin_number);
void gpio_enable_async_risingedge_detect(uint8_t pin_number, bool enable);
bool gpio_async_risingedge_detect_is_enabled(uint8_t pin_number);
void gpio_enable_async_fallingedge_detect(uint8_t pin_number, bool enable);
bool gpio_async_fallingedge_detect_is_enabled(uint8_t pin_number);
void gpio_enable_high_detect(uint8_t pin_number, bool enable);
bool gpio_high_detect_is_enabled(uint8_t pin_number);
void gpio_enable_low_detect(uint8_t pin_number, bool enable);
bool gpio_low_detect_is_enabled(uint8_t pin_number);

#endif
