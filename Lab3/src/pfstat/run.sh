rmmod pfstat.ko
make
insmod pfstat.ko
gcc test.c -std=c99 -o test.o -lm -lpthread
./test.o