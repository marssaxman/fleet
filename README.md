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
a virtual device and thereby allowing our VMs to create new VMs and subdelegate
their resources.

