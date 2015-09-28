#!/bin/sh
NAME=$1
qemu-system-i386 -kernel app/$NAME/$NAME.bin

