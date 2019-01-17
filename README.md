# read_write_user_memory_from_driver
Sample code to read/write user process's memory on virtual address from Linux device driver at any given time  
Linuxのデバイスドライバから、仮想メモリ上のユーザプロセスメモリを任意のタイミングで読み書きするサンプルコード  

# how to run
$ git clone https://github.com/mnishz/read_write_user_memory_from_driver.git  
$ cd read\_write\_user\_memory\_from\_driver  
$ make  
$ sudo insmod foobar.ko  
$ sudo ./user_proc_A  
$ sudo ./user_proc_B (in another console)  
$ sudo rmmod foobar  
