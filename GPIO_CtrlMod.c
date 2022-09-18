#define GPIO_ADDR_LPMODE
#include "BCM2711_GPIO_RegisterMapping.h"
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <asm/io.h>

#define GPIO_PINMODE_INPUT 0
#define GPIO_PINMODE_OUTPUT 1
#define GPIO_PINMODE_ALTFUNC0 4
#define GPIO_PINMODE_ALTFUNC1 5
#define GPIO_PINMODE_ALTFUNC2 6
#define GPIO_PINMODE_ALTFUNC3 7
#define GPIO_PINMODE_ALTFUNC4 3
#define GPIO_PINMODE_ALTFUNC5 2

#define GPIO_PUDCTRL_NOPULL 0
#define GPIO_PUDCTRL_PULLUP 1
#define GPIO_PUDCTRL_PULLDOWN 2

/*
 * GPIO Command Structure (3 BYTES):
 *
 * BYTE0: CMD
 * BYTE1: PIN
 * BYTE2: ARG
 */

#define GPIO_DATAIO_SIZE_BYTES 3

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

static struct proc_dir_entry *gpio_proc = NULL;
static unsigned int *gpio_mapping = NULL;
static void *gpio_data_io = NULL;

unsigned int gpio_is_reg_bit_active(unsigned int register_value, unsigned int reference_bit)
{
	unsigned int bit_value = (register_value & reference_bit);

	if(bit_value == reference_bit) return 1;
	return 0;
}

void gpio_set_pinmode(unsigned int pin_number, unsigned int pinmode)
{
	pinmode &= 0x00000007;
	unsigned int mapping_pos = 0;
	unsigned int bit_offset = (pin_number%10)*3;
	switch((pin_number/10))
	{
		case 0:
			mapping_pos = GPIO_FSEL0_UINTP_POS;
			break;

		case 1:
			mapping_pos = GPIO_FSEL1_UINTP_POS;
			break;

		case 2:
			mapping_pos = GPIO_FSEL2_UINTP_POS;
			break;

		case 3:
			mapping_pos = GPIO_FSEL3_UINTP_POS;
			break;

		case 4:
			mapping_pos = GPIO_FSEL4_UINTP_POS;
			break;

		case 5:
			mapping_pos = GPIO_FSEL5_UINTP_POS;
			break;

		default:
			return;
	}

	gpio_mapping[mapping_pos] &= ~(0x7 << bit_offset);
	gpio_mapping[mapping_pos] |= (pinmode << bit_offset);
	return;
}

unsigned int gpio_get_pinmode(unsigned int pin_number)
{
	unsigned int pinmode = 0;
	unsigned int mapping_pos = 0;
	unsigned int bit_offset = (pin_number%10)*3;

	switch((pin_number/10))
	{
		case 0:
			mapping_pos = GPIO_FSEL0_UINTP_POS;
			break;

		case 1:
			mapping_pos = GPIO_FSEL1_UINTP_POS;
			break;

		case 2:
			mapping_pos = GPIO_FSEL2_UINTP_POS;
			break;

		case 3:
			mapping_pos = GPIO_FSEL3_UINTP_POS;
			break;

		case 4:
			mapping_pos = GPIO_FSEL4_UINTP_POS;
			break;

		case 5:
			mapping_pos = GPIO_FSEL5_UINTP_POS;
			break;

		default:
			return 0;
	}

	pinmode = gpio_mapping[mapping_pos];
	pinmode &= (0x7 << bit_offset);
	return (pinmode >> bit_offset);
}

unsigned int gpio_get_level(unsigned int pin_number)
{
	unsigned int mapping_pos = 0;
	unsigned int bit_offset = pin_number%32;

	if(pin_number < 32) mapping_pos = GPIO_INPUT0_UINTP_POS;
	else mapping_pos = GPIO_INPUT1_UINTP_POS;
	
	return gpio_is_reg_bit_active(gpio_mapping[mapping_pos], (1 << bit_offset));
}

void gpio_set_level(unsigned int pin_number, unsigned int value)
{
	value &= 0x00000001;
	unsigned int mapping_pos = 0;
	unsigned int bit_offset = pin_number%32;

	if(pin_number < 32)
	{
		if(value) mapping_pos = GPIO_OUTPUT0_SET_UINTP_POS;
		else mapping_pos = GPIO_OUTPUT0_CLR_UINTP_POS;
	}
	else
	{
		if(value) mapping_pos = GPIO_OUTPUT1_SET_UINTP_POS;
		else mapping_pos = GPIO_OUTPUT1_CLR_UINTP_POS;
	}

	gpio_mapping[mapping_pos] = (1 << bit_offset);
	return;
}

void gpio_set_pudctrl(unsigned int pin_number, unsigned int pudctrl)
{
	pudctrl &= 0x00000003;
	unsigned int mapping_pos = 0;
	unsigned int bit_offset = (pin_number%16)*2;

	switch((pin_number/16))
	{
		case 0:
			mapping_pos = GPIO_PUDCTRL0_UINTP_POS;
			break;

		case 1:
			mapping_pos = GPIO_PUDCTRL1_UINTP_POS;
			break;

		case 2:
			mapping_pos = GPIO_PUDCTRL2_UINTP_POS;
			break;

		case 3:
			mapping_pos = GPIO_PUDCTRL3_UINTP_POS;
			break;

		default:
			return;
	}

	gpio_mapping[mapping_pos] &= ~(0x3 << bit_offset);
	gpio_mapping[mapping_pos] |= (pudctrl << bit_offset);
	return;
}

unsigned int gpio_get_pudctrl(unsigned int pin_number)
{
	unsigned int pudctrl = 0;
	unsigned int mapping_pos = 0;
	unsigned int bit_offset = (pin_number%16)*2;

	switch((pin_number/16))
	{
		case 0:
			mapping_pos = GPIO_PUDCTRL0_UINTP_POS;
			break;

		case 1:
			mapping_pos = GPIO_PUDCTRL1_UINTP_POS;
			break;

		case 2:
			mapping_pos = GPIO_PUDCTRL2_UINTP_POS;
			break;

		case 3:
			mapping_pos = GPIO_PUDCTRL3_UINTP_POS;
			break;

		default:
			return 0;
	}

	pudctrl = gpio_mapping[mapping_pos];
	pudctrl &= (0x3 << bit_offset);
	return (pudctrl >> bit_offset);
}

unsigned int gpio_event_detected(unsigned int pin_number)
{
	unsigned int mapping_pos = 0;
	unsigned int bit_offset = pin_number%32;

	if(pin_number < 32) mapping_pos = GPIO_EVENTDETECT0_STATUS_UINTP_POS;
	else mapping_pos = GPIO_EVENTDETECT1_STATUS_UINTP_POS;

	return gpio_is_reg_bit_active(gpio_mapping[mapping_pos], (1 << bit_offset));
}

void gpio_enable_risingedge_detect(unsigned int pin_number, unsigned int enable)
{
	enable &= 0x00000001;
	unsigned int mapping_pos = 0;
	unsigned int bit_offset = pin_number%32;

	if(pin_number < 32) mapping_pos = GPIO_REDGEDETECT0_ENABLE_UINTP_POS;
	else mapping_pos = GPIO_REDGEDETECT1_ENABLE_UINTP_POS;

	gpio_mapping[mapping_pos] &= ~(1 << bit_offset);
	gpio_mapping[mapping_pos] |= (enable << bit_offset);
	return;
}

unsigned int gpio_risingedge_detect_is_enabled(unsigned int pin_number)
{
	unsigned int mapping_pos = 0;
	unsigned int bit_offset = pin_number%32;

	if(pin_number < 32) mapping_pos = GPIO_REDGEDETECT0_ENABLE_UINTP_POS;
	else mapping_pos = GPIO_REDGEDETECT1_ENABLE_UINTP_POS;

	return gpio_is_reg_bit_active(gpio_mapping[mapping_pos], (1 << bit_offset));
}

void gpio_enable_fallingedge_detect(unsigned int pin_number, unsigned int enable)
{
	enable &= 0x00000001;
	unsigned int mapping_pos = 0;
	unsigned int bit_offset = pin_number%32;

	if(pin_number < 32) mapping_pos = GPIO_FEDGEDETECT0_ENABLE_UINTP_POS;
	else mapping_pos = GPIO_FEDGEDETECT1_ENABLE_UINTP_POS;

	gpio_mapping[mapping_pos] &= ~(1 << bit_offset);
	gpio_mapping[mapping_pos] |= (enable << bit_offset);
	return;
}

unsigned int gpio_fallingedge_detect_is_enabled(unsigned int pin_number)
{
	unsigned int mapping_pos = 0;
	unsigned int bit_offset = pin_number%32;

	if(pin_number < 32) mapping_pos = GPIO_FEDGEDETECT0_ENABLE_UINTP_POS;
	else mapping_pos = GPIO_FEDGEDETECT1_ENABLE_UINTP_POS;

	return gpio_is_reg_bit_active(gpio_mapping[mapping_pos], (1 << bit_offset));
}

void gpio_enable_high_detect(unsigned int pin_number, unsigned int enable)
{
	enable &= 0x00000001;
	unsigned int mapping_pos = 0;
	unsigned int bit_offset = pin_number%32;

	if(pin_number < 32) mapping_pos = GPIO_HIGHDETECT0_ENABLE_UINTP_POS;
	else mapping_pos = GPIO_HIGHDETECT1_ENABLE_UINTP_POS;

	gpio_mapping[mapping_pos] &= ~(1 << bit_offset);
	gpio_mapping[mapping_pos] |= (enable << bit_offset);
	return;
}

unsigned int gpio_high_detect_is_enabled(unsigned int pin_number)
{
	unsigned int mapping_pos = 0;
	unsigned int bit_offset = pin_number%32;

	if(pin_number < 32) mapping_pos = GPIO_HIGHDETECT0_ENABLE_UINTP_POS;
	else mapping_pos = GPIO_HIGHDETECT1_ENABLE_UINTP_POS;

	return gpio_is_reg_bit_active(gpio_mapping[mapping_pos], (1 << bit_offset));
}

void gpio_enable_low_detect(unsigned int pin_number, unsigned int enable)
{
	enable &= 0x00000001;
	unsigned int mapping_pos = 0;
	unsigned int bit_offset = pin_number%32;

	if(pin_number < 32) mapping_pos = GPIO_LOWDETECT0_ENABLE_UINTP_POS;
	else mapping_pos = GPIO_LOWDETECT1_ENABLE_UINTP_POS;

	gpio_mapping[mapping_pos] &= ~(1 << bit_offset);
	gpio_mapping[mapping_pos] |= (enable << bit_offset);
	return;
}

unsigned int gpio_low_detect_is_enabled(unsigned int pin_number)
{
	unsigned int mapping_pos = 0;
	unsigned int bit_offset = pin_number%32;

	if(pin_number < 32) mapping_pos = GPIO_LOWDETECT0_ENABLE_UINTP_POS;
	else mapping_pos = GPIO_LOWDETECT1_ENABLE_UINTP_POS;

	return gpio_is_reg_bit_active(gpio_mapping[mapping_pos], (1 << bit_offset));
}

void gpio_enable_async_risingedge_detect(unsigned int pin_number, unsigned int enable)
{
	enable &= 0x00000001;
	unsigned int mapping_pos = 0;
	unsigned int bit_offset = pin_number%32;

	if(pin_number < 32) mapping_pos = GPIO_ASYNC_REDGEDETECT0_ENABLE_UINTP_POS;
	else mapping_pos = GPIO_ASYNC_REDGEDETECT1_ENABLE_UINTP_POS;

	gpio_mapping[mapping_pos] &= ~(1 << bit_offset);
	gpio_mapping[mapping_pos] |= (enable << bit_offset);
	return;
}

unsigned int gpio_async_risingedge_detect_is_enabled(unsigned int pin_number)
{
	unsigned int mapping_pos = 0;
	unsigned int bit_offset = pin_number%32;

	if(pin_number < 32) mapping_pos = GPIO_ASYNC_REDGEDETECT0_ENABLE_UINTP_POS;
	else mapping_pos = GPIO_ASYNC_REDGEDETECT1_ENABLE_UINTP_POS;

	return gpio_is_reg_bit_active(gpio_mapping[mapping_pos], (1 << bit_offset));
}

void gpio_enable_async_fallingedge_detect(unsigned int pin_number, unsigned int enable)
{
	enable &= 0x00000001;
	unsigned int mapping_pos = 0;
	unsigned int bit_offset = pin_number%32;

	if(pin_number < 32) mapping_pos = GPIO_ASYNC_FEDGEDETECT0_ENABLE_UINTP_POS;
	else mapping_pos = GPIO_ASYNC_FEDGEDETECT1_ENABLE_UINTP_POS;

	gpio_mapping[mapping_pos] &= ~(1 << bit_offset);
	gpio_mapping[mapping_pos] |= (enable << bit_offset);
	return;
}

unsigned int gpio_async_fallingedge_detect_is_enabled(unsigned int pin_number)
{
	unsigned int mapping_pos = 0;
	unsigned int bit_offset = pin_number%32;

	if(pin_number < 32) mapping_pos = GPIO_ASYNC_FEDGEDETECT0_ENABLE_UINTP_POS;
	else mapping_pos = GPIO_ASYNC_FEDGEDETECT1_ENABLE_UINTP_POS;

	return gpio_is_reg_bit_active(gpio_mapping[mapping_pos], (1 << bit_offset));
}

void gpio_reset_pin(unsigned int pin_number)
{
	gpio_set_pinmode(pin_number, GPIO_PINMODE_INPUT);
	gpio_set_pudctrl(pin_number, GPIO_PUDCTRL_NOPULL);
	gpio_enable_async_risingedge_detect(pin_number, 0);
	gpio_enable_async_fallingedge_detect(pin_number, 0);
	gpio_enable_risingedge_detect(pin_number, 0);
	gpio_enable_fallingedge_detect(pin_number, 0);
	gpio_enable_high_detect(pin_number, 0);
	gpio_enable_low_detect(pin_number, 0);
	gpio_event_detected(pin_number);
	return;
}

ssize_t gpio_mod_usrread(struct file *file, char __user *user, size_t size, loff_t *offset)
{
	copy_to_user(user, gpio_data_io, GPIO_DATAIO_SIZE_BYTES);
	return size;
}

ssize_t gpio_mod_usrwrite(struct file *file, const char __user *user, size_t size, loff_t *offset)
{
	copy_from_user(gpio_data_io, user, GPIO_DATAIO_SIZE_BYTES);
	
	unsigned char *pbyte = (unsigned char*) gpio_data_io;

	switch(pbyte[0])
	{
		case GPIO_CMD_RESET_PIN:
			gpio_reset_pin(pbyte[1]);
			break;

		case GPIO_CMD_SET_LEVEL:
			gpio_set_level(pbyte[1], pbyte[2]);
			break;

		case GPIO_CMD_GET_LEVEL:
			pbyte[2] = gpio_get_level(pbyte[1]);
			break;

		case GPIO_CMD_SET_PINMODE:
			gpio_set_pinmode(pbyte[1], pbyte[2]);
			break;

		case GPIO_CMD_GET_PINMODE:
			pbyte[2] = gpio_get_pinmode(pbyte[1]);
			break;

		case GPIO_CMD_SET_PUDCTRL:
			gpio_set_pudctrl(pbyte[1], pbyte[2]);
			break;

		case GPIO_CMD_GET_PUDCTRL:
			pbyte[2] = gpio_get_pudctrl(pbyte[1]);
			break;

		case GPIO_CMD_GET_EVENT_DETECTED:
			pbyte[2] = gpio_event_detected(pbyte[1]);
			break;

		case GPIO_CMD_SET_ENABLE_REDGEDETECT:
			gpio_enable_risingedge_detect(pbyte[1], pbyte[2]);
			break;

		case GPIO_CMD_GET_ENABLE_REDGEDETECT:
			pbyte[2] = gpio_risingedge_detect_is_enabled(pbyte[1]);
			break;

		case GPIO_CMD_SET_ENABLE_FEDGEDETECT:
			gpio_enable_fallingedge_detect(pbyte[1], pbyte[2]);
			break;

		case GPIO_CMD_GET_ENABLE_FEDGEDETECT:
			pbyte[2] = gpio_fallingedge_detect_is_enabled(pbyte[1]);
			break;

		case GPIO_CMD_SET_ENABLE_ASYNC_REDGEDETECT:
			gpio_enable_async_risingedge_detect(pbyte[1], pbyte[2]);
			break;

		case GPIO_CMD_GET_ENABLE_ASYNC_REDGEDETECT:
			pbyte[2] = gpio_async_risingedge_detect_is_enabled(pbyte[1]);
			break;

		case GPIO_CMD_SET_ENABLE_ASYNC_FEDGEDETECT:
			gpio_enable_async_fallingedge_detect(pbyte[1], pbyte[2]);
			break;

		case GPIO_CMD_GET_ENABLE_ASYNC_FEDGEDETECT:
			pbyte[2] = gpio_async_fallingedge_detect_is_enabled(pbyte[1]);
			break;

		case GPIO_CMD_SET_ENABLE_HIGHDETECT:
			gpio_enable_high_detect(pbyte[1], pbyte[2]);
			break;

		case GPIO_CMD_GET_ENABLE_HIGHDETECT:
			pbyte[2] = gpio_high_detect_is_enabled(pbyte[1]);
			break;

		case GPIO_CMD_SET_ENABLE_LOWDETECT:
			gpio_enable_low_detect(pbyte[1], pbyte[2]);
			break;

		case GPIO_CMD_GET_ENABLE_LOWDETECT:
			pbyte[2] = gpio_low_detect_is_enabled(pbyte[1]);
			break;
	}

	return size;
}

static const struct proc_ops gpio_proc_ops = {
	.proc_read = gpio_mod_usrread,
	.proc_write = gpio_mod_usrwrite
};

static int __init driver_enable(void)
{
	gpio_mapping = (unsigned int*) ioremap(GPIO_BASE_ADDR, GPIO_MAPPING_SIZE_BYTES);
	if(gpio_mapping == NULL)
	{
		printk("GPIO: Error mapping GPIO addr\n");
		return -1;
	}

	gpio_proc = proc_create("GPIO_Ctrl", 0x1B6, NULL, &gpio_proc_ops);
	if(gpio_proc == NULL)
	{
		printk("GPIO: Error creating proc file\n");
		return -1;
	}

	gpio_data_io = vmalloc(GPIO_DATAIO_SIZE_BYTES);
	printk("GPIO Control Driver Enabled\n");
	return 0;
}

static void __exit driver_disable(void)
{
	iounmap(gpio_mapping);
	proc_remove(gpio_proc);
	vfree(gpio_data_io);
	printk("GPIO Control Driver Disabled\n");
	return;
}

module_init(driver_enable);
module_exit(driver_disable);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rafael Sabe");
MODULE_DESCRIPTION("Driver for BCM2711 GPIO Control");
