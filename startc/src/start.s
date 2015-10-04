# Create a multiboot header so grub knows it can load this executable.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # sizes of lower and upper memory regions
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Allocate a section which will hold a call stack.
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# Implement the entrypoint. Finish configuring the processor, set up the call
# stack, initialize globals, then jump into the C world. We will pass in the
# multiboot signature and information pointer as parameters.
.section .text
.global _startc
.global _start
.type _start, @function
_start:
	movl $stack_top, %esp
	push %ebx
	push %eax
	call _gdt_init
	call _startc
	cli
	hlt
.Lhang:
	jmp .Lhang
.size _start, . - _start

