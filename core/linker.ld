/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* Begin putting sections at 1 MiB, a conventional place for kernels to be
	   loaded at by the bootloader. */
	. = 1M;

	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}

	/* Make sure the GNU notes information is placed after .text. Failure to
	   to do so may push the GRUB multiboot information beyond the first 8k
	   and GRUB will not identify this kernel as multiboot capable.
	   Alternative to this is to compile the final binary with this linker
	   option to exclude this unique header:
	   -Wl,--build-id=none */

	.note.gnu.build-id BLOCK(4K) : ALIGN(4K)
	{
		*(.note.gnu.build-id)
	}

	/* Read-only data. */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		/* Metadata describing the program interface. */
		__manifest = .;
		KEEP(*(.manifest))
		__manifest_end = .;
		/* Constant data defined by the program. */
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
		__data_end = .;
	}

	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
}
