# fleet

I want to develop an exokernel style operating system where virtual machines
take the place of processes, shared libraries, and even threads; a system where
isolation is the default, security policy is implemented like a firewall and
concurrency issues are managed through asynchronous communication.

This is a tall order, of course, so the first goal of the fleet project is to
build a simple foundation for the small, tweakable, unfussy experimental apps
I'll need to build while figuring out what I'm really trying to do, how it
might be done, and whether it is actually practical at all.

In concrete terms, libfleet is an implementation of the C99 standard library,
targeting a generic 32-bit i386 PC, which boots on bare metal and sets up a
usable machine environment before invoking main().

While C99 compliance is a goal, support for POSIX definitely is not, and it
remains to be seen how useful C's synchronous IO model can be when the whole
point of the IO architecture here is asynchrony. Nor do I plan to offer
preemptive threading or a 64-bit build option in the near future, though they
may come along after I've had more time to explore minimalism.


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


