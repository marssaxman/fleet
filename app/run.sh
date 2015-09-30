#!/bin/sh
qemu-system-i386 \
	-serial stdio \
	-kernel $1

