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
_stack_size = 2048;

/* now define the output sections  */

SECTIONS 
{
	.kernel 0x80000000:
	{
		. = 0x200;
		*(.vector_hwint_0)
	        . = 0x220;
		*(.vector_hwint_1)
		. = 0x240;
		*(.vector_hwint_2)
		. = 0x260;
		*(.vector_hwint_3)
		. = 0x280;
		*(.vector_hwint_4)
		. = 0x2A0;
		*(.vector_hwint_5)
		. = 0x2C0;
		*(.vector_hwint_6)
		. = 0x2E0;
		*(.vector_hwint_7)
		. = 0x300;
		*(.vector_hwint_8)
		. = 0x320;
		*(.vector_hwint_9)
		. = 0x340;
		*(.vector_hwint_10)
		. = 0x360;
		*(.vector_hwint_11)
		. = 0x380;
		*(.vector_hwint_12)
		. = 0x3A0;
		*(.vector_hwint_13)
		. = 0x3C0;
		*(.vector_hwint_14)
		. = 0x3E0;
		*(.vector_hwint_15)
		. = 0x400;
		*(.vector_hwint_16)
		. = 0x420;
		*(.vector_hwint_17)
		. = 0x440;
		*(.vector_hwint_18)
		. = 0x460;
		*(.vector_hwint_19)
		. = 0x480;
		*(.vector_hwint_20)
		. = 0x4A0;
		*(.vector_hwint_21)
		. = 0x4C0;
		*(.vector_hwint_22)		
		. = 0x4E0;
		*(.vector_hwint_23)
		. = 0x500;
		*(.vector_hwint_24)
		. = 0x520;
		*(.vector_hwint_25)
		. = 0x540;
		*(.vector_hwint_26)
		. = 0x560;
		*(.vector_hwint_27)
		. = 0x580;
		*(.vector_hwint_28)
		. = 0x5A0;
		*(.vector_hwint_29)
		. = 0x5C0;
		*(.vector_hwint_30)
		. = 0x5E0;
		*(.vector_hwint_31)
		. = 0x600;
		*(.vector_hwint_32)
		. = 0x620;
		*(.vector_hwint_33)
		. = 0x640;
		*(.vector_hwint_34)
		. = 0x660;
		*(.vector_hwint_35)
		. = 0x680;
		*(.vector_hwint_36)
		. = 0x6A0;
		*(.vector_hwint_37)
		. = 0x6C0;
		*(.vector_hwint_38)
		. = 0x6E0;
		*(.vector_hwint_39)
		. = 0x700;
		*(.vector_hwint_40)
		. = 0x720;
		*(.vector_hwint_41)
		. = 0x740;
		*(.vector_hwint_42)
		. = 0x760;
		*(.vector_hwint_43)
		. = 0x780;
		*(.vector_hwint_44)
		. = 0x7A0;
		*(.vector_hwint_45)
		. = 0x7C0;
		*(.vector_hwint_46)
		. = 0x7E0;
		*(.vector_hwint_47)
		. = 0x800;
		*(.vector_hwint_48)
		. = 0x820;
		*(.vector_hwint_49)
		. = 0x840;
		*(.vector_hwint_50)
		. = 0x860;
		*(.vector_hwint_51)
		. = 0x880;
		*(.vector_hwint_52)
		. = 0x8A0;
		*(.vector_hwint_53)
		. = 0x8C0;
		*(.vector_hwint_54)
		. = 0x8E0;
		*(.vector_hwint_55)
		. = 0x900;
		*(.vector_hwint_56)
		. = 0x920;
		*(.vector_hwint_57)
		. = 0x940;
		*(.vector_hwint_58)
		. = 0x960;
		*(.vector_hwint_59)
		. = 0x980;
		*(.vector_hwint_60)
		. = 0x9A0;
		*(.vector_hwint_61)
		. = 0x9C0;
		*(.vector_hwint_62)
		. = 0x9E0;
		*(.vector_hwint_63)
		. = 0x2000;
		*(.entry)
		_text = .;
		*(.text)
		*(.text.*)
		_etext = .;
	} > ram

	.__drivers_table_init :
	{
		. = ALIGN(4);
		__drivers_table_init_start = .;
		*(.__drivers_table_init)
		__drivers_table_init_end = .;
	} > ram
    
	.MIPS.abiflags : {
		. = ALIGN(4);
		__MIPS_abiflags_start = .;
		*(.MIPS.abiflags)
		__MIPS_abiflags_end = .;
	} > ram
    
	.reginfo : {
		. = ALIGN(4);
		*(.reginfo)
	} > ram

	.rodata :
	{
		. = ALIGN(4);
		_rodata = .;
		*(.rodata)
		*(.rodata.*)
		_erodata = .;
	} > ram

	.data : 
	{
		. = ALIGN(4); 
		_data = .;
		*(.data)
		*(.data.*)
		_gp = .;
		*(.sdata)
		*(.sdata.*)
		_edata = .;
	} > ram

	.bss :
	{
		. = ALIGN(4);
		_bss_start = .; 
		*(.bss) 
		*(.bss.*)
		*(.sbss) 
		*(.sbss.*)
		*(.scommon)
		_bss_end = . ;
	} > ram
	

}
_end = .;
_heap_start = . ;
_heap_size = LENGTH(ram) - (_heap_start - ORIGIN(ram)) - 4 - _stack_size;
    

