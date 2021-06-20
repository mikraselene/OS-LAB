rmmod sysx.ko
make
insmod sysx.ko
gcc test.c -std=c99 -o test.o -lm -lpthread
./test.o