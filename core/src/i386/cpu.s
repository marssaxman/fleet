# Enable or disable interrupts.

.section .text
.global _cli
.type _cli, @function
_cli:
	cli
	ret

.global _sti
.type _sti, @function
_sti:
	sti
	ret

