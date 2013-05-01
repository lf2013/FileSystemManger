###############################Makefile文件#####################################
obj-m   :=hook.o
EXTRA_CFLAGS := -Dsymname=sys_call_table
KDIR   := /lib/modules/$(shell uname -r)/build
PWD   := $(shell pwd)
default:
	make -C $(KDIR) SUBDIRS=$(PWD) modules
	gcc -o watch file_watch.c
	make clean
clean:
	rm -rf .*.cmd *.mod.c *.o *.order .tmp* *.symvers
