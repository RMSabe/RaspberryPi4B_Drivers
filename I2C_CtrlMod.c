#define I2C_ADDR_LPMODE
#include "BCM2711_I2C_RegisterMapping.h"
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <asm/io.h>

#define I2C_CTRL0 0
#define I2C_CTRL1 1
#define I2C_CTRL3 3
#define I2C_CTRL4 4
#define I2C_CTRL5 5
#define I2C_CTRL6 6

#define I2C_WRITE_BIT 0
#define I2C_READ_BIT 1

#define I2C_100KBPS_CLKDIV_VALUE 1500
#define I2C_400KBPS_CLKDIV_VALUE 375

#define I2C_DATAIO_SIZE_BYTES 4
#define I2C_DATAIO_SIZE_UINT 1

/*
 * I2C CMD Structure: (1 UINT)
 *
 * UINT BITS 31-24: CMD
 * UINT BITS 23-16: I2C_CTRL
 * UINT BITS 15-0: ARGS
 */

#define I2C_CMD_INIT_DEFAULT 0
#define I2C_CMD_DEINIT_DEFAULT 1
#define I2C_CMD_SET_ENABLE 2
#define I2C_CMD_GET_ENABLE 3
#define I2C_CMD_SET_ENABLE_INTR_ON_RX 4
#define I2C_CMD_GET_ENABLE_INTR_ON_RX 5
#define I2C_CMD_SET_ENABLE_INTR_ON_TX 6
#define I2C_CMD_GET_ENABLE_INTR_ON_TX 7
#define I2C_CMD_SET_ENABLE_INTR_ON_DONE 8
#define I2C_CMD_GET_ENABLE_INTR_ON_DONE 9
#define I2C_CMD_START_TRANSFER 10
#define I2C_CMD_CLEAR_FIFO 11
#define I2C_CMD_SET_RW_BIT 12
#define I2C_CMD_GET_RW_BIT 13
#define I2C_CMD_GET_TIMEOUT_OCCURRED 14
#define I2C_CMD_GET_ACK_ERR_OCCURRED 15
#define I2C_CMD_GET_FIFO_IS_FULL 16
#define I2C_CMD_GET_FIFO_IS_EMPTY 17
#define I2C_CMD_GET_FIFO_HAS_DATA 18
#define I2C_CMD_GET_FIFO_FITS_DATA 19
#define I2C_CMD_GET_FIFO_ALMOST_FULL 20
#define I2C_CMD_GET_FIFO_ALMOST_EMPTY 21
#define I2C_CMD_GET_TRANSFER_DONE 22
#define I2C_CMD_GET_TRANSFER_IS_ACTIVE 23
#define I2C_CMD_SET_TRANSFER_LENGTH_REG 24
#define I2C_CMD_GET_TRANSFER_LENGTH_REG 25
#define I2C_CMD_SET_SLAVE_ADDR 26
#define I2C_CMD_GET_SLAVE_ADDR 27
#define I2C_CMD_SET_FIFO_DATA 28
#define I2C_CMD_GET_FIFO_DATA 29
#define I2C_CMD_SET_CLKDIV 30
#define I2C_CMD_GET_CLKDIV 31
#define I2C_CMD_SET_REDGE_DELAY 32
#define I2C_CMD_GET_REDGE_DELAY 33
#define I2C_CMD_SET_FEDGE_DELAY 34
#define I2C_CMD_GET_FEDGE_DELAY 35
#define I2C_CMD_SET_TIMEOUT 36
#define I2C_CMD_GET_TIMEOUT 37
#define I2C_CMD_SET_CLKDIV_STANDARD 38
#define I2C_CMD_SET_FULL_PERIOD 39

static struct proc_dir_entry *i2c_proc = NULL;
static unsigned int *i2c0_mapping = NULL;
static unsigned int *i2c1_mapping = NULL;
static unsigned int *i2c3_mapping = NULL;
static unsigned int *i2c4_mapping = NULL;
static unsigned int *i2c5_mapping = NULL;
static unsigned int *i2c6_mapping = NULL;
static unsigned int i2c_data_io = 0;

static int i2c_is_reg_bit_active(unsigned int register_value, unsigned int reference_bit)
{
	unsigned int bit_value_from_reg = (register_value & reference_bit);
	if(bit_value_from_reg == reference_bit) return 1;
	return 0;
}

static void i2c_ctrl_map_to_pointer(int i2c_ctrl, unsigned int **p_pointer)
{
	switch(i2c_ctrl)
	{
		case I2C_CTRL0:
			if(p_pointer != NULL) *p_pointer = i2c0_mapping;
			break;

		case I2C_CTRL1:
			if(p_pointer != NULL) *p_pointer = i2c1_mapping;
			break;

		case I2C_CTRL3:
			if(p_pointer != NULL) *p_pointer = i2c3_mapping;
			break;

		case I2C_CTRL4:
			if(p_pointer != NULL) *p_pointer = i2c4_mapping;
			break;

		case I2C_CTRL5:
			if(p_pointer != NULL) *p_pointer = i2c5_mapping;
			break;

		case I2C_CTRL6:
			if(p_pointer != NULL) *p_pointer = i2c6_mapping;
			break;
	}

	return;
}

//=====================================================================

static void i2c_enable(int i2c_ctrl, unsigned int enable)
{
	enable &= 0x00000001;
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	i2c_reg[I2C_CTRL_UINTP_POS] &= ~(1 << 15);
	i2c_reg[I2C_CTRL_UINTP_POS] |= (enable << 15);
	return;
}

static int i2c_is_enabled(int i2c_ctrl)
{
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	return i2c_is_reg_bit_active(i2c_reg[I2C_CTRL_UINTP_POS], (1 << 15));
}

static void i2c_enable_intr_on_rx(int i2c_ctrl, unsigned int enable)
{
	enable &= 0x00000001;
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	i2c_reg[I2C_CTRL_UINTP_POS] &= ~(1 << 10);
	i2c_reg[I2C_CTRL_UINTP_POS] |= (enable << 10);
	return;
}

static int i2c_intr_on_rx_is_enabled(int i2c_ctrl)
{
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	return i2c_is_reg_bit_active(i2c_reg[I2C_CTRL_UINTP_POS], (1 << 10));
}

static void i2c_enable_intr_on_tx(int i2c_ctrl, unsigned int enable)
{
	enable &= 0x00000001;
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	i2c_reg[I2C_CTRL_UINTP_POS] &= ~(1 << 9);
	i2c_reg[I2C_CTRL_UINTP_POS] |= (enable << 9);
	return;
}

static int i2c_intr_on_tx_is_enabled(int i2c_ctrl)
{
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	return i2c_is_reg_bit_active(i2c_reg[I2C_CTRL_UINTP_POS], (1 << 9));
}

static void i2c_enable_intr_on_done(int i2c_ctrl, unsigned int enable)
{
	enable &= 0x00000001;
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	i2c_reg[I2C_CTRL_UINTP_POS] &= ~(1 << 8);
	i2c_reg[I2C_CTRL_UINTP_POS] |= (enable << 8);
	return;
}

static int i2c_intr_on_done_is_enabled(int i2c_ctrl)
{
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	return i2c_is_reg_bit_active(i2c_reg[I2C_CTRL_UINTP_POS], (1 << 8));
}

static void i2c_start_transfer(int i2c_ctrl)
{
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	i2c_reg[I2C_CTRL_UINTP_POS] |= (1 << 7);
	return;
}

static void i2c_clear_fifo(int i2c_ctrl)
{
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	i2c_reg[I2C_CTRL_UINTP_POS] |= (1 << 4);
	return;
}

static void i2c_set_rw_bit(int i2c_ctrl, unsigned int rw_bit)
{
	rw_bit &= 0x00000001;
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	i2c_reg[I2C_CTRL_UINTP_POS] &= ~(1);
	i2c_reg[I2C_CTRL_UINTP_POS] |= (rw_bit);
	return;
}

static int i2c_get_rw_bit(int i2c_ctrl)
{
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	return i2c_is_reg_bit_active(i2c_reg[I2C_CTRL_UINTP_POS], (1));
}

//=====================================================================

static int i2c_timeout_occurred(int i2c_ctrl)
{
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	if(i2c_is_reg_bit_active(i2c_reg[I2C_STATUS_UINTP_POS], (1 << 9)))
	{
		i2c_reg[I2C_STATUS_UINTP_POS] |= (1 << 9);
		return 1;
	}

	return 0;
}

static int i2c_ack_err_occurred(int i2c_ctrl)
{
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	if(i2c_is_reg_bit_active(i2c_reg[I2C_STATUS_UINTP_POS], (1 << 8)))
	{
		i2c_reg[I2C_STATUS_UINTP_POS] |= (1 << 8);
		return 1;
	}

	return 0;
}

static int i2c_fifo_is_full(int i2c_ctrl)
{
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	return i2c_is_reg_bit_active(i2c_reg[I2C_STATUS_UINTP_POS], (1 << 7));
}

static int i2c_fifo_is_empty(int i2c_ctrl)
{
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	return i2c_is_reg_bit_active(i2c_reg[I2C_STATUS_UINTP_POS], (1 << 6));
}

static int i2c_fifo_has_data(int i2c_ctrl)
{
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	return i2c_is_reg_bit_active(i2c_reg[I2C_STATUS_UINTP_POS], (1 << 5));
}

static int i2c_fifo_fits_data(int i2c_ctrl)
{
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	return i2c_is_reg_bit_active(i2c_reg[I2C_STATUS_UINTP_POS], (1 << 4));
}

static int i2c_fifo_is_almost_full(int i2c_ctrl)
{
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	return i2c_is_reg_bit_active(i2c_reg[I2C_STATUS_UINTP_POS], (1 << 3));
}

static int i2c_fifo_is_almost_empty(int i2c_ctrl)
{
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	return i2c_is_reg_bit_active(i2c_reg[I2C_STATUS_UINTP_POS], (1 << 2));
}

static int i2c_transfer_done(int i2c_ctrl)
{
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	if(i2c_is_reg_bit_active(i2c_reg[I2C_STATUS_UINTP_POS], (1 << 1)))
	{
		i2c_reg[I2C_STATUS_UINTP_POS] |= (1 << 1);
		return 1;
	}

	return 0;
}

static int i2c_transfer_is_active(int i2c_ctrl)
{
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	return i2c_is_reg_bit_active(i2c_reg[I2C_STATUS_UINTP_POS], (1));
}

//=====================================================================

static void i2c_set_transfer_length_reg(int i2c_ctrl, unsigned int length)
{
	length &= 0x0000FFFF;
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	i2c_reg[I2C_DATALENGTH_UINTP_POS] = length;
	return;
}

static unsigned int i2c_get_transfer_length_reg(int i2c_ctrl)
{
	unsigned int length = 0;
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	length = i2c_reg[I2C_DATALENGTH_UINTP_POS];
	length &= 0x0000FFFF;
	return length;
}

//=====================================================================

static void i2c_set_slave_addr(int i2c_ctrl, unsigned int slave_addr)
{
	slave_addr &= 0x0000007F;
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	i2c_reg[I2C_SLAVEADDR_UINTP_POS] = slave_addr;
	return;
}

static unsigned int i2c_get_slave_addr(int i2c_ctrl)
{
	unsigned int slave_addr = 0;
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	slave_addr = i2c_reg[I2C_SLAVEADDR_UINTP_POS];
	slave_addr &= 0x0000007F;
	return slave_addr;
}

//=====================================================================

static void i2c_set_fifo_data(int i2c_ctrl, unsigned int data)
{
	data &= 0x000000FF;
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	i2c_reg[I2C_DATAFIFO_UINTP_POS] = data;
	return;
}

static unsigned int i2c_get_fifo_data(int i2c_ctrl)
{
	unsigned int data = 0;
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	data = i2c_reg[I2C_DATAFIFO_UINTP_POS];
	data &= 0x000000FF;
	return data;
}

//=====================================================================

static void i2c_set_clkdiv_std(int i2c_ctrl, unsigned int use_400kbps)
{
	use_400kbps &= 0x00000001;
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	if(use_400kbps) i2c_reg[I2C_CLKDIV_UINTP_POS] = I2C_400KBPS_CLKDIV_VALUE;
	else i2c_reg[I2C_CLKDIV_UINTP_POS] = I2C_100KBPS_CLKDIV_VALUE;
	
	return;
}

static void i2c_set_clkdiv(int i2c_ctrl, unsigned int clkdiv)
{
	clkdiv &= 0x0000FFFF;
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	i2c_reg[I2C_CLKDIV_UINTP_POS] = clkdiv;
	return;
}

static unsigned int i2c_get_clkdiv(int i2c_ctrl)
{
	unsigned int clkdiv = 0;
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	clkdiv = i2c_reg[I2C_CLKDIV_UINTP_POS];
	clkdiv &= 0x0000FFFF;
	return clkdiv;
}

//=====================================================================

static void i2c_set_full_period(int i2c_ctrl, unsigned int use_400kbps)
{
	use_400kbps &= 0x00000001;
	unsigned int *i2c_reg = NULL;
	unsigned int delay_0 = 0;
	unsigned int delay_1 = 0;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	if(use_400kbps)
	{
		delay_0 = 3*I2C_400KBPS_CLKDIV_VALUE/4;
		delay_1 = I2C_400KBPS_CLKDIV_VALUE/4;
	}
	else
	{
		delay_0 = I2C_100KBPS_CLKDIV_VALUE/2;
		delay_1 = I2C_100KBPS_CLKDIV_VALUE/2;
	}

	i2c_reg[I2C_DATADELAY_UINTP_POS] = ((delay_0 << 16) | (delay_1));
	return;
}

static void i2c_set_redge_delay(int i2c_ctrl, unsigned int delay)
{
	delay &= 0x0000FFFF;
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	i2c_reg[I2C_DATADELAY_UINTP_POS] &= ~(0xFFFF);
	i2c_reg[I2C_DATADELAY_UINTP_POS] |= (delay);
	return;
}

static unsigned int i2c_get_redge_delay(int i2c_ctrl)
{
	unsigned int delay = 0;
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	delay = i2c_reg[I2C_DATADELAY_UINTP_POS];
	delay &= 0x0000FFFF;
	return delay;
}

static void i2c_set_fedge_delay(int i2c_ctrl, unsigned int delay)
{
	delay &= 0x0000FFFF;
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	i2c_reg[I2C_DATADELAY_UINTP_POS] &= ~(0xFFFF << 16);
	i2c_reg[I2C_DATADELAY_UINTP_POS] |= (delay << 16);
	return;
}

static unsigned int i2c_get_fedge_delay(int i2c_ctrl)
{
	unsigned int delay = 0;
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	delay = i2c_reg[I2C_DATADELAY_UINTP_POS];
	delay &= 0xFFFF0000;
	return (delay >> 16);
}

//=====================================================================

static void i2c_set_timeout(int i2c_ctrl, unsigned int timeout)
{
	timeout &= 0x0000FFFF;
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	i2c_reg[I2C_CLKTIMEOUT_UINTP_POS] = timeout;
	return;
}

static unsigned int i2c_get_timeout(int i2c_ctrl)
{
	unsigned int timeout = 0;
	unsigned int *i2c_reg = NULL;
	i2c_ctrl_map_to_pointer(i2c_ctrl, &i2c_reg);

	timeout = i2c_reg[I2C_CLKTIMEOUT_UINTP_POS];
	timeout &= 0x0000FFFF;
	return timeout;
}

//=====================================================================

static void i2c_init_default(int i2c_ctrl, unsigned int use_400kbps)
{
	use_400kbps &= 0x00000001;

	i2c_enable(i2c_ctrl, 1);
	i2c_enable_intr_on_rx(i2c_ctrl, 0);
	i2c_enable_intr_on_tx(i2c_ctrl, 0);
	i2c_enable_intr_on_done(i2c_ctrl, 0);

	i2c_clear_fifo(i2c_ctrl);

	i2c_set_clkdiv_std(i2c_ctrl, use_400kbps);
	i2c_set_full_period(i2c_ctrl, use_400kbps);

	i2c_set_timeout(i2c_ctrl, 20);
	return;
}

static void i2c_deinit_default(int i2c_ctrl)
{
	i2c_clear_fifo(i2c_ctrl);
	i2c_set_transfer_length_reg(i2c_ctrl, 0);
	i2c_enable(i2c_ctrl, 0);
	return;
}

//=====================================================================

ssize_t i2c_mod_usrread(struct file *file, char __user *user, size_t size, loff_t *offset)
{
	copy_to_user(user, &i2c_data_io, I2C_DATAIO_SIZE_BYTES);
	return size;
}

ssize_t i2c_mod_usrwrite(struct file *file, const char __user *user, size_t size, loff_t *offset)
{
	copy_from_user(&i2c_data_io, user, I2C_DATAIO_SIZE_BYTES);

	unsigned int cmd = (i2c_data_io >> 24);
	unsigned int ctrl = ((i2c_data_io & 0x00FF0000) >> 16);
	unsigned int arg = (i2c_data_io & 0x0000FFFF);

	switch(cmd)
	{
		case I2C_CMD_INIT_DEFAULT:
			i2c_init_default(ctrl, arg);
			break;

		case I2C_CMD_DEINIT_DEFAULT:
			i2c_deinit_default(ctrl);
			break;

		case I2C_CMD_SET_ENABLE:
			i2c_enable(ctrl, arg);
			break;

		case I2C_CMD_GET_ENABLE:
			i2c_data_io = i2c_is_enabled(ctrl);
			break;

		case I2C_CMD_SET_ENABLE_INTR_ON_RX:
			i2c_enable_intr_on_rx(ctrl, arg);
			break;

		case I2C_CMD_GET_ENABLE_INTR_ON_RX:
			i2c_data_io = i2c_intr_on_rx_is_enabled(ctrl);
			break;

		case I2C_CMD_SET_ENABLE_INTR_ON_TX:
			i2c_enable_intr_on_tx(ctrl, arg);
			break;

		case I2C_CMD_GET_ENABLE_INTR_ON_TX:
			i2c_data_io = i2c_intr_on_tx_is_enabled(ctrl);
			break;

		case I2C_CMD_SET_ENABLE_INTR_ON_DONE:
			i2c_enable_intr_on_done(ctrl, arg);
			break;

		case I2C_CMD_GET_ENABLE_INTR_ON_DONE:
			i2c_data_io = i2c_intr_on_done_is_enabled(ctrl);
			break;

		case I2C_CMD_START_TRANSFER:
			i2c_start_transfer(ctrl);
			break;

		case I2C_CMD_CLEAR_FIFO:
			i2c_clear_fifo(ctrl);
			break;

		case I2C_CMD_SET_RW_BIT:
			i2c_set_rw_bit(ctrl, arg);
			break;

		case I2C_CMD_GET_RW_BIT:
			i2c_data_io = i2c_get_rw_bit(ctrl);
			break;

		case I2C_CMD_GET_TIMEOUT_OCCURRED:
			i2c_data_io = i2c_timeout_occurred(ctrl);
			break;

		case I2C_CMD_GET_ACK_ERR_OCCURRED:
			i2c_data_io = i2c_ack_err_occurred(ctrl);
			break;

		case I2C_CMD_GET_FIFO_IS_FULL:
			i2c_data_io = i2c_fifo_is_full(ctrl);
			break;

		case I2C_CMD_GET_FIFO_IS_EMPTY:
			i2c_data_io = i2c_fifo_is_empty(ctrl);
			break;

		case I2C_CMD_GET_FIFO_HAS_DATA:
			i2c_data_io = i2c_fifo_has_data(ctrl);
			break;

		case I2C_CMD_GET_FIFO_FITS_DATA:
			i2c_data_io = i2c_fifo_fits_data(ctrl);
			break;

		case I2C_CMD_GET_FIFO_ALMOST_FULL:
			i2c_data_io = i2c_fifo_is_almost_full(ctrl);
			break;

		case I2C_CMD_GET_FIFO_ALMOST_EMPTY:
			i2c_data_io = i2c_fifo_is_almost_empty(ctrl);
			break;

		case I2C_CMD_GET_TRANSFER_DONE:
			i2c_data_io = i2c_transfer_done(ctrl);
			break;

		case I2C_CMD_GET_TRANSFER_IS_ACTIVE:
			i2c_data_io = i2c_transfer_is_active(ctrl);
			break;

		case I2C_CMD_SET_TRANSFER_LENGTH_REG:
			i2c_set_transfer_length_reg(ctrl, arg);
			break;

		case I2C_CMD_GET_TRANSFER_LENGTH_REG:
			i2c_data_io = i2c_get_transfer_length_reg(ctrl);
			break;

		case I2C_CMD_SET_SLAVE_ADDR:
			i2c_set_slave_addr(ctrl, arg);
			break;

		case I2C_CMD_GET_SLAVE_ADDR:
			i2c_data_io = i2c_get_slave_addr(ctrl);
			break;

		case I2C_CMD_SET_FIFO_DATA:
			i2c_set_fifo_data(ctrl, arg);
			break;

		case I2C_CMD_GET_FIFO_DATA:
			i2c_data_io = i2c_get_fifo_data(ctrl);
			break;

		case I2C_CMD_SET_CLKDIV:
			i2c_set_clkdiv(ctrl, arg);
			break;

		case I2C_CMD_GET_CLKDIV:
			i2c_data_io = i2c_get_clkdiv(ctrl);
			break;

		case I2C_CMD_SET_REDGE_DELAY:
			i2c_set_redge_delay(ctrl, arg);
			break;

		case I2C_CMD_GET_REDGE_DELAY:
			i2c_data_io = i2c_get_redge_delay(ctrl);
			break;

		case I2C_CMD_SET_FEDGE_DELAY:
			i2c_set_fedge_delay(ctrl, arg);
			break;

		case I2C_CMD_GET_FEDGE_DELAY:
			i2c_data_io = i2c_get_fedge_delay(ctrl);
			break;

		case I2C_CMD_SET_TIMEOUT:
			i2c_set_timeout(ctrl, arg);
			break;

		case I2C_CMD_GET_TIMEOUT:
			i2c_data_io = i2c_get_timeout(ctrl);
			break;

		case I2C_CMD_SET_CLKDIV_STANDARD:
			i2c_set_clkdiv_std(ctrl, arg);
			break;

		case I2C_CMD_SET_FULL_PERIOD:
			i2c_set_full_period(ctrl, arg);
			break;
	}

	return size;
}

static const struct proc_ops i2c_proc_ops = {
	.proc_read = i2c_mod_usrread,
	.proc_write = i2c_mod_usrwrite
};

static int __init driver_enable(void)
{
	i2c0_mapping = (unsigned int*) ioremap(I2C0_BASE_ADDR, I2C_MAPPING_SIZE_BYTES);
	i2c1_mapping = (unsigned int*) ioremap(I2C1_BASE_ADDR, I2C_MAPPING_SIZE_BYTES);
	i2c3_mapping = (unsigned int*) ioremap(I2C3_BASE_ADDR, I2C_MAPPING_SIZE_BYTES);
	i2c4_mapping = (unsigned int*) ioremap(I2C4_BASE_ADDR, I2C_MAPPING_SIZE_BYTES);
	i2c5_mapping = (unsigned int*) ioremap(I2C5_BASE_ADDR, I2C_MAPPING_SIZE_BYTES);
	i2c6_mapping = (unsigned int*) ioremap(I2C6_BASE_ADDR, I2C_MAPPING_SIZE_BYTES);
	
	if(i2c0_mapping == NULL)
	{
		printk("Error mapping I2C0\n");
		return -1;
	}
	if(i2c1_mapping == NULL)
	{
		printk("Error mapping I2C1\n");
		return -1;
	}
	if(i2c3_mapping == NULL)
	{
		printk("Error mapping I2C3\n");
		return -1;
	}
	if(i2c4_mapping == NULL)
	{
		printk("Error mapping I2C4\n");
		return -1;
	}
	if(i2c5_mapping == NULL)
	{
		printk("Error mapping I2C5\n");
		return -1;
	}
	if(i2c6_mapping == NULL)
	{
		printk("Error mapping I2C6\n");
		return -1;
	}

	i2c_proc = proc_create("I2C_Ctrl", 0x1B6, NULL, &i2c_proc_ops);
	if(i2c_proc == NULL)
	{
		printk("Error creating proc file\n");
		return -1;
	}

	printk("I2C Control Driver Enabled\n");
	return 0;
}

static void __exit driver_disable(void)
{
	iounmap(i2c0_mapping);
	iounmap(i2c1_mapping);
	iounmap(i2c3_mapping);
	iounmap(i2c4_mapping);
	iounmap(i2c5_mapping);
	iounmap(i2c6_mapping);
	proc_remove(i2c_proc);
	printk("I2C Control Driver Disabled\n");
	return;
}

module_init(driver_enable);
module_exit(driver_disable);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rafael Sabe");
MODULE_DESCRIPTION("Driver for BCM2711 I2C Control");
