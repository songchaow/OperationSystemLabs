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
	/* 我把signature部分的位置声明放到了start.s里，故此处不用再写signature的位置 */

}
