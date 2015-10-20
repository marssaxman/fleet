# fleet
An exokernel style operating system based on lightweight virtual machines.
We'll use a virtual implementation of the traditional PC architecture instead
of abstracting it under a system call API. We'll put each process in its own
virtual machine, with no shared mutable resources; as in the principle embraced
by Go, processes will share data by communicating instead of communicating by
sharing data. We'll make this efficient by providing zero-copy data transfer
interfaces for our virtual devices. We'll make it secure in "capability" style
by granting each VM only those resources it should be able to manipulate.
Finally, we'll make this system self-regulating by exposing the hypervisor as
a virtual device, thereby allowing our VMs to create new VMs and subdelegate
their resources.

You will need gcc and build tools, configured for i386 or x86-64.
> apt-get build-essential

If you are using an x86-64 machine, you will need i386 libc.
> apt-get g++-multilib libc6-dev-i386

You will need qemu, since that's what makes the whole thing run.
> apt-get qemu

