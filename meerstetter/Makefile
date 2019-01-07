CC=clang
CFLAGS=--std=gnu11 -march=native -O3 -Weverything -pedantic

all:
	$(CC) -shared -fPIC -o wrapper.so wrapper.c ComPort/ComPort_Linux.c MeComAPI/MePort_Linux.c MeComAPI/private/*.c  -lm
