rmmod proc_relate.ko
make
insmod proc_relate.ko
gcc test.c -std=c99 -o test.o -lm -lpthread
./test.o