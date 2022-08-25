#ifndef I2C_CTRL_H
#define I2C_CTRL_H

//I2C_CTRL2 and I2C_CTRL7 are used by HDMI controls. Should not be used on peripherals.

#define I2C_CTRL0 0
#define I2C_CTRL1 1
#define I2C_CTRL3 3
#define I2C_CTRL4 4
#define I2C_CTRL5 5
#define I2C_CTRL6 6

//Each I2C Controller can be routed to different GPIO Pins, giving 2 or 3 endpoint possibilities. Read BCM2711 Datasheet for further reference.
#define I2C_ENDPOINT0 0
#define I2C_ENDPOINT1 1
#define I2C_ENDPOINT2 2

#define I2C_WRITE_BIT 0
#define I2C_READ_BIT 1

#include <stdbool.h>
#include <stdint.h>

//This function can be used to retrive GPIO Pin numbers and respective modes from I2C Controller number and its endpoint number.
//By setting GPIO Pin number and modes and I2C manually, it is possible to map an I2C Controller to multiple simultaneous endpoints.
void i2c_ctrl_endpoint_map_to_gpio_pinmode(uint8_t i2c_ctrl, uint8_t endpoint, void *p_sda_pin, void *p_scl_pin, void *p_pinmode);

//This function is used to tell if i2c_init() has already been called
bool i2c_is_active(void);

//This function initializes the header and driver proc file. It must be called before calling any other function in this header. Returns init successful.
bool i2c_init(void);

//Init I2C controller and GPIOs using default parameters.
//If use_400kbps, I2C SCL frequency equals 400kHz.
//Else, I2C SCL frequency equals 100kHz.
void i2c_init_default(uint8_t i2c_ctrl, uint8_t i2c_endpoint, bool use_400kbps);
//Deinit I2C controller and GPIOs using default parameters.
void i2c_deinit_default(uint8_t i2c_ctrl, uint8_t i2c_endpoint);

//Manually enable/disable I2C controller
void i2c_enable(uint8_t i2c_ctrl, bool enable);
bool i2c_is_enabled(uint8_t i2c_ctrl);

//Enable/disable interrupts
void i2c_enable_intr_on_rx(uint8_t i2c_ctrl, bool enable);
bool i2c_intr_on_rx_is_enabled(uint8_t i2c_ctrl);
void i2c_enable_intr_on_tx(uint8_t i2c_ctrl, bool enable);
bool i2c_intr_on_tx_is_enabled(uint8_t i2c_ctrl);
void i2c_enable_intr_on_done(uint8_t i2c_ctrl, bool enable);
bool i2c_intr_on_done_is_enabled(uint8_t i2c_ctrl);

void i2c_start_transfer(uint8_t i2c_ctrl);
void i2c_clear_fifo(uint8_t i2c_ctrl);

//Set read/write bit for next transfer.
void i2c_set_rw_bit(uint8_t i2c_ctrl, bool rw_bit);
bool i2c_get_rw_bit(uint8_t i2c_ctrl);

bool i2c_timeout_occurred(uint8_t i2c_ctrl); //Error timeout
bool i2c_ack_err_occurred(uint8_t i2c_ctrl); //Error NACK (Non acknowledge received)
bool i2c_fifo_is_full(uint8_t i2c_ctrl); //FIFO is full
bool i2c_fifo_is_empty(uint8_t i2c_ctrl); //FIFO is empty
bool i2c_fifo_has_data(uint8_t i2c_ctrl); //FIFO has at least 1 byte
bool i2c_fifo_fits_data(uint8_t i2c_ctrl); //FIFO has space to accept at least 1 byte
bool i2c_fifo_is_almost_full(uint8_t i2c_ctrl); //FIFO is 3/4 or more full
bool i2c_fifo_is_almost_empty(uint8_t i2c_ctrl); //FIFO is 1/4 or less full
bool i2c_transfer_done(uint8_t i2c_ctrl); //Transfer completed
bool i2c_transfer_is_active(uint8_t i2c_ctrl); //Ongoing transfer

//Transfer Length Register: 
//writing on it set next transfer length (in bytes)
//reading from it get remaining bytes to be transferred (if transfer active) or total bytes transferred (if transfer not active)
void i2c_set_transfer_length_reg(uint8_t i2c_ctrl, uint16_t length);
uint16_t i2c_get_transfer_length_reg(uint8_t i2c_ctrl);

//Slave addr: (7BIT FORMAT 0b0xxxxxxx)
void i2c_set_slave_addr(uint8_t i2c_ctrl, uint16_t slave_addr);
uint16_t i2c_get_slave_addr(uint8_t i2c_ctrl);

//Write/Read single byte to/from FIFO.
void i2c_set_fifo_data(uint8_t i2c_ctrl, uint16_t fifo_data);
uint16_t i2c_get_fifo_data(uint8_t i2c_ctrl);

//Manually set Core Clock Divider. I2C SCL Frequency equals Core Clock divided by clkdiv
void i2c_set_clkdiv(uint8_t i2c_ctrl, uint16_t clkdiv);
uint16_t i2c_get_clkdiv(uint8_t i2c_ctrl);

//Manually set rising edge/falling edge hold time, where "delay" is hold time in core clock cycles.
void i2c_set_risingedge_delay(uint8_t i2c_ctrl, uint16_t delay);
uint16_t i2c_get_risingedge_delay(uint8_t i2c_ctrl);
void i2c_set_fallingedge_delay(uint8_t i2c_ctrl, uint16_t delay);
uint16_t i2c_get_fallingedge_delay(uint8_t i2c_ctrl);

//Manually set timeout (in number of I2C SCL clock pulses)
void i2c_set_timeout(uint8_t i2c_ctrl, uint16_t timeout);
uint16_t i2c_get_timeout(uint8_t i2c_ctrl);

//Manually set core clock divider to standard values.
//if use_400kbps, I2C SCL frequency equals 400kHz
//else I2C SCL frequency equals 100kHz
void i2c_set_clkdiv_std(uint8_t i2c_ctrl, bool use_400kbps);

//Manually set rising edge/falling edge hold time based on standard values.
//If use_400kbps, I2C SCL stay 3 quarter cycle up, 1 quarter cycle down
//Else, I2C SCL stay half cycle up, half cycle down
void i2c_set_full_period(uint8_t i2c_ctrl, bool use_400kbps);

#endif
