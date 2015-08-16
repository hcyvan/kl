# Comment/uncomment the following line to disable/enable debugging
DEBUG = y

# Add the debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
	DEBFLAGS = -O -g -DFDR_DEBUG
else
	DEBFLAGS = -O2
endif
ccflags-y += $(DEBFLAGS)

ifneq ($(KERNELRELEASE),)
	obj-m := fdr.o
	fdr-y := main.o proc.o
else
	KDIR ?= /home/navy/build
	PWD=$(shell pwd)
default:
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
endif


