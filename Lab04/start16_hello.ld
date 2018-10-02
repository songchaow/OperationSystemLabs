/*
 * start16.ld
 *
 * Linker script for the i386 bootsect code
 */
OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(bootsect_start)

SECTIONS
{
	. = 0x7C00;
	.bstext		: { *(.text) }
	.bsdata		: { *(.data) }

	. = 0x7C00 + 510;
	.signature  : { *(.signature)}

}
