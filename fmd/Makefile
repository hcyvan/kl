ifneq ($(KERNELRELEASE),)
	obj-m := fmd.o
#	fmd-y := fmd.o
else
	KDIR ?= /home/navy/build
	PWD=$(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
endif


