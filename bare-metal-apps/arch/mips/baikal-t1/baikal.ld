OUTPUT_FORMAT("elf32-tradlittlemips")
ENTRY(_entry)

MEMORY 
{
    ram		(rw!x) : ORIGIN = 0x80000000, LENGTH = RAM_SIZE	/* RAM area. */
}

/* define a global symbol _stack  */

_stack = ORIGIN(ram) + LENGTH(ram);
_stack_size = STACK_SIZE;

/* now define the output sections  */

SECTIONS 
{
	.text :
	{
		*(.exception)
		. = 0x200;
		*(.vector0)
		. = 0x1000;
		*(.e_entry)
		_text = .;
		*(.text)
	        *(.text.*)
		_etext = .;
		. = ALIGN(4);
	} > ram

	.MIPS.abiflags : {
		*(.MIPS.abiflags)
		. = ALIGN(4);
	} > ram

	.reginfo : {
		*(.reginfo)
		. = ALIGN(4);
	} > ram
    
	.rodata :
	{
		_rodata = .;
		*(.rodata)
		*(.rodata.*)
		_erodata = .;
		. = ALIGN(4);
	} > ram

	.data : 
	{
		_data = .;
		*(.data)
		*(.data.*)
		_gp = .;
		*(.sdata)
		*(.sdata.*)
		. = ALIGN(16); 
	} > ram

	_edata = .;

	.bss :
	{
		_bss_start = .; 
		*(.bss) 
		*(.bss.*)
		*(.sbss) 
		*(.sbss.*)
		*(.scommon)
		. = ALIGN(4);				
		_bss_end = . ;
		
	} > ram
	

}
_end = .;
. = ALIGN(4); 
_heap_start = . ;
_heap_size = LENGTH(ram) - (_heap_start - ORIGIN(ram)) - 4 - _stack_size;
