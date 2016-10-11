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
ENTRY(_entry)

/* specify HF-RISC memory areas */

MEMORY 
{
flash       (rx)  : ORIGIN = 0x9d000000, LENGTH = 32k    /* Flash area*/
ram     (rw!x) : ORIGIN = 0x80000000, LENGTH = 32K /* RAM area. */
}

/* define a global symbol _stack  */

_stack = ORIGIN(ram) + LENGTH(ram);
_stack_size = 1024;

/* now define the output sections  */

SECTIONS 
{
    .text :
    {
        *(.exception)
        . = 0x200;
        *(.vector0)
        . = 0x220;
        *(.vector_1)
        . = 0x240;
        *(.vector_2)
        . = 0x260;
        *(.vector_3)
        . = 0x280;
        *(.vector_4)
        . = 0x300;
        *(.vector_5)
        . = 0x320;
        *(.vector_6)
        . = 0x1000;
        *(.entry)
        _text = .;
        *(.text)
        *(.text.*)
        _etext = .;
    } > flash

    .__drivers_table_init :
    {
        __drivers_table_init_start = .;
        *(.__drivers_table_init)
        __drivers_table_init_end = .;
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
        . = ALIGN(4);
        _bss_end = . ;
    } > ram

}
    _end = .;
    _heap_start = . ;
    _heap_size = LENGTH(ram) - (_heap_start - ORIGIN(ram)) - _stack_size;
    

