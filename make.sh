#!/bin/sh
arm-linux-gnueabihf-gcc -o isotp-test src/isotp.c src/timer.c test/test.c -I./src -lpthread
