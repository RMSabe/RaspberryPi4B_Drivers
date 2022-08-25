obj-m += GPIO_CtrlMod.o I2C_CtrlMod.o

KDIR = /lib/modules/$(shell uname -r)/build/

all:
  make -C $(KDIR) M=$(shell pwd) modules
  
clean:
  make -C $(KDIR) M=$(shell pwd) clean
