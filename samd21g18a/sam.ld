/* vim: set tabstop=4 shiftwidth=4 noexpandtab: */

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
SEARCH_DIR(.)

PHDRS {
	text PT_LOAD FLAGS(5);
	stck PT_LOAD FLAGS(6);
	data PT_LOAD FLAGS(6);
	exec PT_LOAD FLAGS(5);
}

SIZE_FLASH = 0x40000; /* 256kB */
SIZE_BOOT  = 0x02000; /*   8kB */
SIZE_NVM   = 0x01000; /*   4kB */
SIZE_SRAM  = 0x08000; /*  32kB */

MEMORY
{
	boot  (r x) : ORIGIN = 0x00000000,  LENGTH = SIZE_BOOT
	rom   (r x) : ORIGIN = SIZE_BOOT, LENGTH = SIZE_FLASH-SIZE_BOOT-SIZE_NVM
	nvm   (r  ) : ORIGIN = SIZE_FLASH-SIZE_NVM, LENGTH = SIZE_NVM
	ram   (rwx) : ORIGIN = 0x20000000,  LENGTH = SIZE_SRAM
}

STACK_SIZE = DEFINED(STACK_SIZE) ? STACK_SIZE : DEFINED(__stack_size__) ? __stack_size__ : 0x2400;

ENTRY(handler_reset)
/*ENTRY(_start) crt0 */

SECTIONS
{
	. = ORIGIN(boot);

	.boot ALIGN(4) (READONLY) :
	{
		. += LENGTH(boot);
	} > rom : text

	. = ORIGIN(rom);

	.text ALIGN(4) (READONLY) :
	{
		__text_start = .;
		KEEP(*(.vectors .vectors.*))
		*(.text .text.* .gnu.linkonce.t.*)
		*(.glue_7t) *(.glue_7)
		*(.rodata .rodata* .gnu.linkonce.r.*)
		*(.ARM.extab* .gnu.linkonce.armextab.*)

		/* Support C constructors, and C destructors in both user code
		   and the C library. This also provides support for C++ code. */
		. = ALIGN(4);
		KEEP(*(.init))
		. = ALIGN(4);
		__preinit_array_start = .;
		KEEP (*(.preinit_array))
		__preinit_array_end = .;

		. = ALIGN(4);
		__init_array_start = .;
		KEEP (*(SORT(.init_array.*)))
		KEEP (*(.init_array))
		__init_array_end = .;

		. = ALIGN(4);
		KEEP (*crtbegin.o(.ctors))
		KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
		KEEP (*(SORT(.ctors.*)))
		KEEP (*crtend.o(.ctors))

		. = ALIGN(4);
		KEEP(*(.fini))

		. = ALIGN(4);
		__fini_array_start = .;
		KEEP (*(.fini_array))
		KEEP (*(SORT(.fini_array.*)))
		__fini_array_end = .;

		KEEP (*crtbegin.o(.dtors))
		KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
		KEEP (*(SORT(.dtors.*)))
		KEEP (*crtend.o(.dtors))
	} > rom : text

	/* .ARM.exidx is sorted, so has to go in its own output section.  */
	PROVIDE_HIDDEN (__exidx_start = .);
	.ARM.exidx ALIGN(4) (READONLY) :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > rom
	PROVIDE_HIDDEN (__exidx_end = .);

	__text_end = ALIGN(4);

	. = ORIGIN(nvm);
	
	.store ALIGN(4) (NOLOAD) :
	{
		*(.store .store.*);
	} > nvm : text

	. = ORIGIN(ram);

	.stack ALIGN(8) (NOLOAD) :
	{
		. += STACK_SIZE;
	} > ram : stck
	__stack = .;

	.data ALIGN(4) : AT (__text_end)
	{
		__data_start = .;
		*(SORT_BY_ALIGNMENT(.data));
		*(SORT_BY_ALIGNMENT(.data.*));
	} > ram : data

	.ramfunc ALIGN(4) (READONLY) : AT (ALIGN(__text_end+SIZEOF(.data),4))
	{
		*(SORT_BY_NAME(.ramfunc));
		*(SORT_BY_NAME(.ramfunc.*));
	} > ram : exec

	__data_end = ALIGN(4);

	.bss ALIGN(4) (NOLOAD) :
	{
		__bss_start = .;
		*(SORT_BY_ALIGNMENT(.bss));
		*(SORT_BY_ALIGNMENT(.bss.*));
		*(SORT_BY_ALIGNMENT(COMMON));
	} > ram : data
	__bss_end = ALIGN(4);

	.heap ALIGN(4) (NOLOAD) :
	{
		__heap_base = .;
		. = ORIGIN(ram)+LENGTH(ram);
	} > ram : data
	__heap_limit = .;
	__heap_size = SIZEOF(.heap);
}
