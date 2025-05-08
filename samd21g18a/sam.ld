/* vim: set tabstop=4 shiftwidth=4 noexpandtab: */

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
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

MEMORY
{
	LDR (r x) : ORIGIN = 0x00000000, LENGTH = SIZE_BOOT
	ROM (r x) : ORIGIN = SIZE_BOOT, LENGTH = SIZE_FLASH-SIZE_BOOT-SIZE_NVM
	NVM (r  ) : ORIGIN = SIZE_FLASH-SIZE_NVM, LENGTH = SIZE_NVM
	RAM (rwx) : ORIGIN = 0x20000000, LENGTH = SIZE_SRAM
}

STACK_SIZE = DEFINED(STACK_SIZE) ? STACK_SIZE : DEFINED(__stack_size__) ? __stack_size__ : 0x2400;

ENTRY(handler_reset) /* ENTRY(_start) newlib crt0 */

SECTIONS
{
	.boot ORIGIN(LDR) (READONLY) :
	{
		. += LENGTH(LDR);
	} > ROM : TEXT

	.text ORIGIN(ROM) (READONLY) :
	{
		KEEP (*(.vectors .vectors.*))
		*(SORT_BY_ALIGNMENT(.gnu.linkonce.r.*));
		*(SORT_BY_ALIGNMENT(.rodata));
		*(SORT_BY_ALIGNMENT(.rodata.*));
		*(.gnu.linkonce.t.* .text .text.*)
		*(.gnu.linkonce.armextab.* .ARM.extab*)
		*(.glue_7) *(.glue_7t)

		. = ALIGN(4);
		__console_start = .;
		KEEP (*(.console .console.*))
		__console_end = .;

		. = ALIGN(4);
		__preinit_array = .;
		KEEP (*(.preinit_array))
		__preinit_count = ABSOLUTE(. - __preinit_array)/4;

		. = ALIGN(4);
		__init_array = .;
		KEEP (*(SORT(.init_array.*)))
		KEEP (*(.init_array))
		__init_count = ABSOLUTE(. - __init_array)/4;

		. = ALIGN(4);
		__fini_array_start = .;
		KEEP (*(.fini_array))
		KEEP (*(SORT(.fini_array.*)))
		__fini_array_end = .;

	} > ROM : TEXT

	/* .ARM.exidx is sorted, so has to go in its own output section.  */
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

	.data ORIGIN(RAM) : ALIGN(4)
	{
		*(SORT_BY_ALIGNMENT(.data));
		*(SORT_BY_ALIGNMENT(.data.*));
	} > RAM AT > ROM : DATA

	.ramfunc NEXT(4) (READONLY) : ALIGN(4)
	{
		*(SORT_BY_NAME(.ramfunc));
		*(SORT_BY_NAME(.ramfunc.*));
	} > RAM AT > ROM : EXEC

	__data_start = ADDR(.data);
	__data_end = ADDR(.data)+ALIGN(SIZEOF(.data),4)+ALIGN(SIZEOF(.ramfunc),4);
	__data_text = LOADADDR(.data);

	.bss NEXT(4) (NOLOAD) : ALIGN(4)
	{
		__bss_start = .;
		*(SORT_BY_ALIGNMENT(.bss));
		*(SORT_BY_ALIGNMENT(.bss.*));
		*(SORT_BY_ALIGNMENT(COMMON));
		__bss_end = ALIGN(4);
	} > RAM : DATA

	.heap NEXT(4) (NOLOAD) : ALIGN(4)
	{
		__heap_base = .;
		. = ORIGIN(RAM)+LENGTH(RAM)-STACK_SIZE;
		__heap_limit = ALIGN(4);
		__heap_size = SIZEOF(.heap);
	} > RAM : DATA

	.stack NEXT(8) (NOLOAD) : ALIGN(8)
	{
		. = ORIGIN(RAM)+LENGTH(RAM);
		__stack = .;
	} > RAM : STCK
}
