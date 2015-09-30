# Stubs for CPU control activities inaccessible from C.

.section .text

# Disable interrupts.
.global _cli
.type _cli, @function
_cli:
	cli
	ret

# Enable interrupts.
.global _sti
.type _sti, @function
_sti:
	sti
	ret

# Fire the usual interrupt.
.global _int3
.type _int3, @function
_int3:
	int $0x3
	ret

.global _hlt
.type _hlt, @function
_hlt:
	hlt
	ret

