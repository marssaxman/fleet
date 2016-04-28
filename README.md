# fleet

Fleet is a C99 library OS for unikernel applications, targeting a virtualized
32-bit i386 PC. It is a single-threaded architecture, based on asynchronous,
non-blocking IO.

# Building the library

You will need gcc and build tools, configured for i386 or x86-64.
> apt-get build-essential

If you are using an x86-64 machine, you will need i386 libc.
> apt-get g++-multilib libc6-dev-i386

You will need qemu, since that provides the execution environment.
> apt-get qemu


# Using the library

Setting up a GNU-style cross-compiler toolchain is a hassle, so I didn't.
Instead, there's a Make recipe which configures CFLAGS, LDFLAGS, and LDLIBS to
build a freestanding binary which uses libfleet as its system environment.
There's an example in demo/Makefile, but essentially you just need to add
something like this to your Makefile:


include ~/fleet/build/link.mk


