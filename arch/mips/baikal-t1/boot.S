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

This code was written by Carlos Moratelli at Embedded System Group (GSE) at PUCRS/Brazil.

*/
    
    .section .exception
    .org    0xf8
_ebase:
    .word   0x9d000000
_imgptr:
    .word   -1

    .section .vector0
_isr:
    .set noreorder
    .set noat
    .set  nomicromips
        la $k1, general_exception_handler
        jalr $k1
        nop
        eret
        nop
    .set  micromips 

.section .vector_1
_vector_1:
    .set noat
    .set  nomicromips
    la $k1, interrupt_handlers
    lw $k1, 0($k1)
    jalr $k1
    nop
    eret
    nop
    .set  micromips 

.section .vector_2
_vector_2:
    .set noat
    .set  nomicromips
    la $k1, interrupt_handlers
    lw $k1, 4($k1)
    jalr $k1
    nop
    eret
    nop
    .set  micromips 

.section .vector_3
_vector_3:
    .set noat
    .set  nomicromips
    la $k1, interrupt_handlers
    lw $k1, 8($k1)
    jalr $k1
    nop
    eret
    nop
    .set  micromips 

.section .vector_4
_vector_4:
    .set noat
    .set  nomicromips
    la $k1, interrupt_handlers
    lw $k1, 12($k1)
    jalr $k1
    nop
    eret
    nop
    .set  micromips 

.section .vector_5
_vector_5:
    .set noat
    .set  nomicromips
    la $k1, interrupt_handlers
    lw $k1, 16($k1)
    jalr $k1
    nop
    eret
    nop
    .set  micromips 

.section .vector_6
_vector_6:
    .set noat
    .set  nomicromips
    la $k1, interrupt_handlers
    lw $k1, 20($k1)
    jalr $k1
    nop
    eret
    nop
    .set  micromips 

    .set at
    .align 2
    .section .entry
_entry:
    .set noreorder
    .set noat
    .set  nomicromips 
    
    la  $gp, _gp
    la  $sp, _stack
    
    # clear the .bss
    la  $a0, _bss_end
    la  $a1, _bss_start
$BSS_CLEAR:
    sw  $zero, 0($a1)
    slt $v1, $a1, $a0
    addiu   $a1, $a1, 4
    bnez    $v1, $BSS_CLEAR
    nop

    jal hyper_init
    nop

$L1:
    beq $zero, $zero, $L1
    nop
    .set micromips
