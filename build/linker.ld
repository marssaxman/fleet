ENTRY(_start)
SECTIONS {
	/* Load kernel image at the base of high memory. */
	. = 1M;
	__image_base = .;

	.text BLOCK(4K) : ALIGN(4K) {
		/* Multiboot header comes first since it must be within first 8K. */
		*(.multiboot)
		*(.text)
	}

	.note.gnu.build-id BLOCK(4K) : ALIGN(4K) {
		*(.note.gnu.build-id)
	}

	.rodata BLOCK(4K) : ALIGN(4K) {
		*(.rodata)
	}

	.data BLOCK(4K) : ALIGN(4K) {
		*(.data)
	}

	.bss BLOCK(4K) : ALIGN(4K) {
		*(COMMON)
		*(.bss)
	}
	__image_end = .;
}
