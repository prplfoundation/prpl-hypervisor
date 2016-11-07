OUTPUT_FORMAT("elf32-tradlittlemips")
ENTRY(_entry)

MEMORY 
{
flash		(rx)   : ORIGIN = 0x9d000000, LENGTH = 32K	/* Flash area*/
ram		(rw!x) : ORIGIN = 0x80000000, LENGTH = 64K	/* RAM area. */
}

/* define a global symbol _stack  */

_stack = ORIGIN(ram) + LENGTH(ram);

/* now define the output sections  */

SECTIONS 
{
	.text :
	{
		KEEP(*(.exception))
		. = 0x200;
		KEEP(*(.vector0))
		. = 0x1000;
		_text = .;
		crt0.o(*.text)
		*(.text)
	        *(.text.*)
		_etext = .;
	} > flash

	.MIPS.abiflags : {
        __MIPS_abiflags_start = .;
        *(.MIPS.abiflags)
        __MIPS_abiflags_end = .;
    } > flash
	

	.rodata :
	{
		_rodata = .;
		*(.rodata)
		*(.rodata.*)
		_erodata = .;
	} > flash

	.data : AT (ADDR (.rodata) + SIZEOF (.rodata))
	{
		_data = .;
		*(.data)
		*(.data.*)
		_gp = . + 0x7ff0;
		*(.sdata)
		*(.sdata.*)
		. = ALIGN(16);
	} > ram

	_edata = .;

	.bss :
	{
		_bss_start = .;
		*(.sbss) 
		*(.sbss.*)
		*(.bss) 
		*(.bss.*)
		. = ALIGN(4);
		_bss_end = . ;
	} > ram

}
	_end = .;

