CFILES = sample_driver.c

obj-m := foobar.o
foobar-objs := $(CFILES:.c=.o)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
	gcc user_proc_A.c -o user_proc_A
	gcc user_proc_B.c -o user_proc_B

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
	rm user_proc_A
	rm user_proc_B
