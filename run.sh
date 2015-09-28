#!/bin/sh
NAME=$1
qemu-system-i386 -kernel $NAME/$NAME.bin

