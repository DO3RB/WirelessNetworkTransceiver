/* vim: set tabstop=4 shiftwidth=4 noexpandtab: */

ENTRY(handler_reset)
OUTPUT_ARCH(arm)
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
SEARCH_DIR(.)

PHDRS {
	TEXT PT_LOAD FLAGS(5);
	STCK PT_LOAD FLAGS(6);
	DATA PT_LOAD FLAGS(6);
	EXEC PT_LOAD FLAGS(5);
}

SIZE_FLASH = 0x40000; /* 256kB */
SIZE_BOOT  = 0x02000; /*   8kB */
SIZE_NVM   = 0x01000; /*   4kB */
SIZE_SRAM  = 0x08000; /*  32kB */
SIZE_STACK = DEFINED(SIZE_STACK) ? SIZE_STACK : 0x2400;

MEMORY
{
	LDR (r x) : ORIGIN = 0x00000000, LENGTH = SIZE_BOOT
	ROM (r x) : ORIGIN = SIZE_BOOT, LENGTH = SIZE_FLASH-SIZE_BOOT-SIZE_NVM
	NVM (r  ) : ORIGIN = SIZE_FLASH-SIZE_NVM, LENGTH = SIZE_NVM
	RAM (rwx) : ORIGIN = 0x20000000, LENGTH = SIZE_SRAM
}

SECTIONS
{
	.boot ORIGIN(LDR) (READONLY) :
	{
		. += LENGTH(LDR);
	} > ROM : TEXT

	.text ORIGIN(ROM) (READONLY) :
	{
		KEEP(*(.vectors .vectors.*))
		*(SORT_BY_ALIGNMENT(.rodata));
		*(SORT_BY_ALIGNMENT(.rodata.*));
		*(SORT_BY_ALIGNMENT(.gnu.linkonce.r.*));
		*(.gnu.linkonce.t.* .text .text.*)
		*(.gnu.linkonce.armextab.* .ARM.extab*)

		. = ALIGN(4);
		__console_start = .;
		KEEP(*(.console))
		__console_end = .;

		/* enable-initfini-array default since GCC 4.8 */
		. = ALIGN(4);
		__init_array = .;
		KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*))) /* [[gnu::constructor(101)]] */
		KEEP(*(.init_array)) /* ordered as encountered   [[gnu::constructor]] .ctors */
		__init_count = ABSOLUTE(. - __init_array)/4;
		. = ALIGN(4);
		__fini_array = .;
		KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array.*))) /* [[gnu::destructor(101)]] */
		KEEP(*(.fini_array)) /* ordered as encountered   [[gnu::destructor]] .dtors */
		__fini_count = ABSOLUTE(. - __fini_array)/4;

	} > ROM : TEXT

	/* .ARM.exidx is sorted, so has to go in its own output section. */
	PROVIDE_HIDDEN (__exidx_start = .);
	.ARM.exidx NEXT(4) (READONLY) :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > ROM : TEXT
	PROVIDE_HIDDEN (__exidx_end = .);

	.store ORIGIN(NVM) (NOLOAD) : ALIGN(4)
	{
		*(.store .store.*);
	} > NVM : TEXT

	.bss ORIGIN(RAM) (NOLOAD) : ALIGN(4)
	{
		__bss_start = .;
		*(SORT_BY_ALIGNMENT(.bss));
		*(SORT_BY_ALIGNMENT(.bss.*));
		*(SORT_BY_ALIGNMENT(.gnu.linkonce.b.*));
		__bss_end = ALIGN(4);
	} > RAM : STCK

	.noinit NEXT(4) (NOLOAD) : ALIGN(4)
	{
		*(SORT_BY_ALIGNMENT(.noinit));
		*(SORT_BY_ALIGNMENT(.noinit.*));
		*(SORT_BY_ALIGNMENT(.gnu.linkonce.n.*));
	} > RAM : STCK

	.data NEXT(4) : ALIGN(4)
	{
		*(SORT_BY_ALIGNMENT(.data));
		*(SORT_BY_ALIGNMENT(.data.*));
		*(SORT_BY_ALIGNMENT(.gnu.linkonce.d.*));
	} > RAM AT > ROM : DATA

	.ramfunc NEXT(4) (READONLY) : ALIGN(4)
	{
		*(SORT_BY_NAME(.ramfunc));
		*(SORT_BY_NAME(.ramfunc.*));
	} > RAM AT > ROM : EXEC

	__data_start = ADDR(.data);
	__data_end = ADDR(.data)+ALIGN(SIZEOF(.data),4)+ALIGN(SIZEOF(.ramfunc),4);
	__data_text = LOADADDR(.data);

	.heap NEXT(4) (NOLOAD) : ALIGN(4)
	{
		__heap_base = .;
		. = ORIGIN(RAM)+LENGTH(RAM)-SIZE_STACK;
		__heap_limit = ALIGN(4);
		__heap_size = ABSOLUTE(SIZEOF(.heap));
	} > RAM : DATA

	.stack NEXT(8) (NOLOAD) : ALIGN(8)
	{
		. = ORIGIN(RAM)+LENGTH(RAM);
		__stack = .;
	} > RAM : STCK
}
