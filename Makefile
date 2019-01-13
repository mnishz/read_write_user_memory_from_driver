CFILES = sample_driver.c

obj-m := foobar.o
foobar-objs := $(CFILES:.c=.o)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
	gcc user_app_A.c -o a.out
	gcc user_app_B.c -o b.out

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
	rm *.out
