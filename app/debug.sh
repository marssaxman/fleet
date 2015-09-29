#!/bin/sh
qemu-system-i386 -s -S -kernel $1 &
gdb $1

