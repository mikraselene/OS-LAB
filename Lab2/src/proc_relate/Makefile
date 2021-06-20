obj-m += proc_relate.o
ccflags-y := -std=gnu11 -Wdeclaration-after-statement

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
