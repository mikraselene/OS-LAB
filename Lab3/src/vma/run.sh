rmmod vma.ko
make
insmod vma.ko
gcc test.c -std=c99 -o test.o -lm -lpthread
./test.o