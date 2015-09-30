#!/bin/sh
qemu-system-i386 \
	-serial stdio \
	-monitor stdio \
	-kernel $1

