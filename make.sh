#!/bin/sh
gcc -o isotp-test-pc src/isotp.c src/timer.c test/test.c -I./src -lpthread
arm-linux-gnueabihf-gcc -o isotp-test-arm src/isotp.c src/timer.c test/test.c -I./src -lpthread
