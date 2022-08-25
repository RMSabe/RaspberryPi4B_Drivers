#ifndef I2C_CTRL_H
#define I2C_CTRL_H

#define I2C_CTRL0 0
#define I2C_CTRL1 1
#define I2C_CTRL3 3
#define I2C_CTRL4 4
#define I2C_CTRL5 5
#define I2C_CTRL6 6

#define I2C_ENDPOINT0 0
#define I2C_ENDPOINT1 1
#define I2C_ENDPOINT2 2

#define I2C_WRITE_BIT 0
#define I2C_READ_BIT 1

#include <stdbool.h>
#include <stdint.h>

void i2c_ctrl_endpoint_map_to_gpio_pinmode(uint8_t i2c_ctrl, uint8_t endpoint, void *p_sda_pin, void *p_scl_pin, void *p_pinmode);
bool i2c_is_active(void);
bool i2c_init(void);
void i2c_init_default(uint8_t i2c_ctrl, uint8_t i2c_endpoint, bool use_400kbps);
void i2c_deinit_default(uint8_t i2c_ctrl, uint8_t i2c_endpoint);
void i2c_enable(uint8_t i2c_ctrl, bool enable);
bool i2c_is_enabled(uint8_t i2c_ctrl);
void i2c_enable_intr_on_rx(uint8_t i2c_ctrl, bool enable);
bool i2c_intr_on_rx_is_enabled(uint8_t i2c_ctrl);
void i2c_enable_intr_on_tx(uint8_t i2c_ctrl, bool enable);
bool i2c_intr_on_tx_is_enabled(uint8_t i2c_ctrl);
void i2c_enable_intr_on_done(uint8_t i2c_ctrl, bool enable);
bool i2c_intr_on_done_is_enabled(uint8_t i2c_ctrl);
void i2c_start_transfer(uint8_t i2c_ctrl);
void i2c_clear_fifo(uint8_t i2c_ctrl);
void i2c_set_rw_bit(uint8_t i2c_ctrl, bool rw_bit);
bool i2c_get_rw_bit(uint8_t i2c_ctrl);
bool i2c_timeout_occurred(uint8_t i2c_ctrl);
bool i2c_ack_err_occurred(uint8_t i2c_ctrl);
bool i2c_fifo_is_full(uint8_t i2c_ctrl);
bool i2c_fifo_is_empty(uint8_t i2c_ctrl);
bool i2c_fifo_has_data(uint8_t i2c_ctrl);
bool i2c_fifo_fits_data(uint8_t i2c_ctrl);
bool i2c_fifo_is_almost_full(uint8_t i2c_ctrl);
bool i2c_fifo_is_almost_empty(uint8_t i2c_ctrl);
bool i2c_transfer_done(uint8_t i2c_ctrl);
bool i2c_transfer_is_active(uint8_t i2c_ctrl);
void i2c_set_transfer_length_reg(uint8_t i2c_ctrl, uint16_t length);
uint16_t i2c_get_transfer_length_reg(uint8_t i2c_ctrl);
void i2c_set_slave_addr(uint8_t i2c_ctrl, uint16_t slave_addr);
uint16_t i2c_get_slave_addr(uint8_t i2c_ctrl);
void i2c_set_fifo_data(uint8_t i2c_ctrl, uint16_t fifo_data);
uint16_t i2c_get_fifo_data(uint8_t i2c_ctrl);
void i2c_set_clkdiv(uint8_t i2c_ctrl, uint16_t clkdiv);
uint16_t i2c_get_clkdiv(uint8_t i2c_ctrl);
void i2c_set_risingedge_delay(uint8_t i2c_ctrl, uint16_t delay);
uint16_t i2c_get_risingedge_delay(uint8_t i2c_ctrl);
void i2c_set_fallingedge_delay(uint8_t i2c_ctrl, uint16_t delay);
uint16_t i2c_get_fallingedge_delay(uint8_t i2c_ctrl);
void i2c_set_timeout(uint8_t i2c_ctrl, uint16_t timeout);
uint16_t i2c_get_timeout(uint8_t i2c_ctrl);
void i2c_set_clkdiv_std(uint8_t i2c_ctrl, bool use_400kbps);
void i2c_set_full_period(uint8_t i2c_ctrl, bool use_400kbps);

#endif
