ifneq ($(KERNELRELEASE),)
	obj-m := smd.o
else
	KDIR ?= /home/navy/build
	PWD=$(shell pwd)
	KBUILD_EXTRA_SYMBOLS=/home/navy/mymd/fmd/Module.symvers
default:
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
endif


