# Create a multiboot header so grub knows it can load this executable.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
# Request the sections we care about in our multiboot machine info structure.
.set MEMINFO,  1<<0             # sizes of lower and upper memory regions
.set CMDLINE,  1<<2				# parameter settings
.set SECTIONS, 1<<5				# location of section table
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
# stack, initialize globals, then jump into the C world via _crt().
.section .text
.global _start
.global _gdt_init
.type _start, @function
_start:
	movl $stack_top, %esp
	call _gdt_init
	call _crt
	# The program ends when main returns; halt the machine.
	cli
	hlt
.Lhang:
	jmp .Lhang
.size _start, . - _start

