/*
Copyright (c) 2016, prpl Foundation

Permission to use, copy, modify, and/or distribute this software for any purpose with or without 
fee is hereby granted, provided that the above copyright notice and this permission notice appear 
in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE 
FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

This code was written by Sergio Johann at Embedded System Group (GSE) at PUCRS/Brazil.

*/

OUTPUT_FORMAT("elf32-tradlittlemips")

MEMORY 
{
ram     (rw!x) : ORIGIN = 0x80000000, LENGTH = 64K /* RAM area. */
}

/* define a global symbol _stack  */

_stack = ORIGIN(ram) + LENGTH(ram);
_stack_size = 1024;

/* now define the output sections  */

SECTIONS 
{
	.kernel 0x80000000:
	{
		. = 0x200;
		*(.exception)
	/*        . = 0x600;
		*(.vector_hwint_0)
		. = 0x800;
		*(.vector_hwint_1)*/
		. = 0xA00;
		*(.vector_hwint_4)
	/*        . = 0x8'00;
		*(.vector_hwint_3)*/
		. = 0x2000;
		*(.entry)
		_text = .;
		*(.text)
		*(.text.*)
		_etext = .;
	} > ram

	.__drivers_table_init :
	{
		__drivers_table_init_start = .;
		*(.__drivers_table_init)
		__drivers_table_init_end = .;
	} > ram
    
	.MIPS.abiflags : {
		__MIPS_abiflags_start = .;
		*(.MIPS.abiflags)
		__MIPS_abiflags_end = .;
	} > ram
    
	.reginfo : {
		*(.reginfo)
	} > ram

	.rodata :
	{
		_rodata = .;
		*(.rodata)
		*(.rodata.*)
		_erodata = .;
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
		_edata = .;
	} > ram

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
_heap_start = . ;
_heap_size = LENGTH(ram) - (_heap_start - ORIGIN(ram)) - _stack_size;
    

