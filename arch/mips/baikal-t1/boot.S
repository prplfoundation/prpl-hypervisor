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
_isr:
    .set noreorder
    .set noat
    
    la  $t8, _stack

    sw    $at, 0($t8)
    sw    $v0, 4($t8)
    sw    $v1, 8($t8)
    sw    $a0, 12($t8)
    sw    $a1, 16($t8)
    sw    $a2, 20($t8)
    sw    $a3, 24($t8)
    sw    $t0, 28($t8)
    sw    $t1, 32($t8)
    sw    $t2, 36($t8)
    sw    $t3, 40($t8)
    sw    $t4, 44($t8)
    sw    $t5, 48($t8)
    sw    $t6, 52($t8)
    sw    $t7, 56($t8)
    sw    $ra, 60($t8)
    mfc0  $k0, $14        #C0_EPC=14 (Exception PC)
    sw    $k0, 64($t8)
    mfhi  $k1
    sw    $k1, 68($t8)
    mflo  $k1
    sw    $k1, 72($t8)

    la    $t8, general_exception_handler
    jalr  $t8
    nop
	
    #Restore all temporary registers
    lw    $at, 0($sp)
    lw    $v0, 4($sp)
    lw    $v1, 8($sp)
    lw    $a0, 12($sp)
    lw    $a1, 16($sp)
    lw    $a2, 20($sp)
    lw    $a3, 24($sp)
    lw    $t0, 28($sp)
    lw    $t1, 32($sp)
    lw    $t2, 36($sp)
    lw    $t3, 40($sp)
    lw    $t4, 44($sp)
    lw    $t5, 48($sp)
    lw    $t6, 52($sp)
    lw    $t7, 56($sp)
    lw    $t8, 60($sp)
    lw    $t9, 64($sp)
    lw    $ra, 68($sp)
    lw    $k0, 72($sp)
    lw    $k1, 76($sp)
    mthi  $k1
    lw    $k1, 80($sp)
    mtlo  $k1
    mtc0  $k0, $14	
    eret
    nop
    


    
.section .vector_hwint_4
vector_hwint_2:
    .set noat

    la  $sp, _stack

    sw    $at, 0($sp)
    sw    $v0, 4($sp)
    sw    $v1, 8($sp)
    sw    $a0, 12($sp)
    sw    $a1, 16($sp)
    sw    $a2, 20($sp)
    sw    $a3, 24($sp)
    sw    $t0, 28($sp)
    sw    $t1, 32($sp)
    sw    $t2, 36($sp)
    sw    $t3, 40($sp)
    sw    $t4, 44($sp)
    sw    $t5, 48($sp)
    sw    $t6, 52($sp)
    sw    $t7, 56($sp)
    sw    $t8, 60($sp)
    sw    $t9, 64($sp)
    sw    $ra, 68($sp)
    mfc0  $k0, $14        #C0_EPC=14 (Exception PC)
    sw    $k0, 72($sp)
    mfhi  $k1
    sw    $k1, 76($sp)
    mflo  $k1
    sw    $k1, 80($sp)

    la $k1, interrupt_handlers
    lw $k1, 8($k1)
    jalr $k1
    nop
	
    #Restore all temporary registers
    lw    $at, 0($sp)
    lw    $v0, 4($sp)
    lw    $v1, 8($sp)
    lw    $a0, 12($sp)
    lw    $a1, 16($sp)
    lw    $a2, 20($sp)
    lw    $a3, 24($sp)
    lw    $t0, 28($sp)
    lw    $t1, 32($sp)
    lw    $t2, 36($sp)
    lw    $t3, 40($sp)
    lw    $t4, 44($sp)
    lw    $t5, 48($sp)
    lw    $t6, 52($sp)
    lw    $t7, 56($sp)
    lw    $t8, 60($sp)
    lw    $t9, 64($sp)
    lw    $ra, 68($sp)
    lw    $k0, 72($sp)
    lw    $k1, 76($sp)
    mthi  $k1
    lw    $k1, 80($sp)
    mtlo  $k1
    mtc0  $k0, $14	
    eret
    nop

    
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
