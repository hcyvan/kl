ifneq ($(KERNELRELEASE),)
	hostprogs-y := prog
	always := $(hostprogs-y)
else
	KDIR ?= /home/navy/build
	PWD=$(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
endif


