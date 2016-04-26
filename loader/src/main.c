// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "args.h"

static void usage() {
	fprintf(stderr,
"Usage:\n"
"  fleet <file> [arg...]\n"
	);
}

int main(int argc, char **argv) {
	// Parse fleet options.
	int argi = 1;
	while (argi < argc) {
		const char *args = argv[argi];
		if ('-' != args[0]) break;
		if (!strcmp(args, "-h") || !strcmp(args, "--help")) {
			usage();
			exit(0);
		} else {
			fprintf(stderr, "fleet: unrecognized option '%s'\n", args);
			fprintf(stderr, "Try 'fleet --help' for more information.\n");
			exit(EXIT_FAILURE);
		}
	}

	// Options are parsed, so the next item must be the executable file.
	if (argi >= argc) {
		fprintf(stderr, "fleet: missing executable file argument\n");
		fprintf(stderr, "Try 'fleet --help' for more information.\n");
		exit(EXIT_FAILURE);
	}
	char *execimage = argv[argi++];

	// Any remaining arguments should be passed through to the executable as
	// the "kernel command line" through qemu's '-append' option, so we'll
	// pack them back up into a single, quote-escaped string.
	char *cmdline = 0;
	if (argi < argc) {
		// One space between each pair of args, plus trailing null.
		size_t valsize = argc - argi;
		// Plus the length of each arg value
		for (int j = argi; j < argc; ++j) {
			valsize += strlen(argv[j]);
		}
		cmdline = malloc(valsize);
		size_t offset = 0;
		for (int j = argi; j < argc; ++j) {
			size_t arglen = strlen(argv[j]);
			memcpy(&cmdline[offset], argv[j], arglen);
			offset += arglen;
			cmdline[offset++] = ' ';
		}
		cmdline[valsize-1] = '\0';
	}

	// We'll use qemu to build a virtual machine for this executable.
	struct args qemu;
	args_init(&qemu);
	args_push(&qemu, "qemu-system-i386");
	args_push2(&qemu, "-kernel", execimage);
	if (cmdline) {
		args_push2(&qemu, "-append", cmdline);
	}

	// Don't reboot the machine if its guest process crashes; quit instead.
	args_push(&qemu, "-no-reboot");
	// Skip the default PC devices; we'll configure hardware explicitly.
	args_push(&qemu, "-nodefaults");
	// Don't display the emulator's monitor window.
	args_push(&qemu, "-nographic");
	// Don't use the monitor UI at all, in fact.
	args_push2(&qemu, "-monitor", "none");
	// Direct debug console output through stderr.
	args_push2(&qemu, "-debugcon", "stdio");

	// Route stdio through COM1 for the time being.
	args_push2(&qemu, "-serial", "stdio");

	execvp(*qemu.vec, qemu.vec);
	return EXIT_FAILURE;

	// args_exit(&qemu);
	// free(cmdline);
}

