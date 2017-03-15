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
    
    la  $t8, _stack          # $t8 is reserved for hypervisor usage
    addi $t8, $t8, -128      # initialize stack 

    sw    $at, 0($t8)           #save all GPR
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
    sw    $s0, 60($t8)
    sw    $s1, 64($t8)
    sw    $s2, 68($t8)
    sw    $s3, 72($t8)
    sw    $s4, 76($t8)
    sw    $s5, 80($t8)
    sw    $s6, 84($t8)
    sw    $s7, 88($t8)
    sw    $k0, 92($t8)
    sw    $k1, 96($t8)
    sw    $gp, 100($t8)
    sw    $sp, 104($t8)
    sw    $fp, 108($t8)
    sw    $ra, 112($t8)
    mfc0  $k0, $14          #C0_EPC=14 (Exception PC)
    sw    $k0, 116($t8)
    mfhi  $k1
    sw    $k1, 120($t8)
    mflo  $k1
    sw    $k1, 124($t8)

    move  $sp, $t8          #
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
    lw    $s0, 60($sp)
    lw    $s1, 64($sp)
    lw    $s2, 68($sp)
    lw    $s3, 72($sp)
    lw    $s4, 76($sp)
    lw    $s5, 80($sp)
    lw    $s6, 84($sp)
    lw    $s7, 88($sp)
    lw    $k0, 92($sp)
    lw    $k1, 96($sp)
    lw    $gp, 100($sp)
    #sp is restored at the end
    lw    $fp, 108($sp)
    lw    $ra, 112($sp)
    lw    $t8, 116($sp)
    mtc0  $t8, $14	
    lw    $t8, 120($sp)
    mthi  $t8
    lw    $t8, 124($sp)
    mtlo  $t8
    eret
    nop
    

.section .vector_hwint_4
vector_hwint_2:
    .set noat

    la  $t8, _stack          # $t8 is reserved for hypervisor usage
    addi $t8, $t8, -128      # initialize stack 

    sw    $at, 0($t8)           #save all GPR
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
    sw    $s0, 60($t8)
    sw    $s1, 64($t8)
    sw    $s2, 68($t8)
    sw    $s3, 72($t8)
    sw    $s4, 76($t8)
    sw    $s5, 80($t8)
    sw    $s6, 84($t8)
    sw    $s7, 88($t8)
    sw    $k0, 92($t8)
    sw    $k1, 96($t8)
    sw    $gp, 100($t8)
    sw    $sp, 104($t8)
    sw    $fp, 108($t8)
    sw    $ra, 112($t8)
    mfc0  $k0, $14          #C0_EPC=14 (Exception PC)
    sw    $k0, 116($t8)
    mfhi  $k1
    sw    $k1, 120($t8)
    mflo  $k1
    sw    $k1, 124($t8)

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
    lw    $s0, 60($sp)
    lw    $s1, 64($sp)
    lw    $s2, 68($sp)
    lw    $s3, 72($sp)
    lw    $s4, 76($sp)
    lw    $s5, 80($sp)
    lw    $s6, 84($sp)
    lw    $s7, 88($sp)
    lw    $k0, 92($sp)
    lw    $k1, 96($sp)
    lw    $gp, 100($sp)
    #sp is restored at the end
    lw    $fp, 108($sp)
    lw    $ra, 112($sp)
    lw    $t8, 116($sp)
    mtc0  $t8, $14	
    lw    $t8, 120($sp)
    mthi  $t8
    lw    $t8, 124($sp)
    mtlo  $t8
    lw    $sp, 104($sp)
    eret
    nop

    #Hypervisor entry point. 
    .set at
    .align 2
    .section .entry
_entry:
    .set noreorder
    .set noat

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

    
    .set at
    .align 2
    .section .text
    .global gpr_context_restore
gpr_context_restore:
    la  $a1, _stack
    addi $a1, $a1, -128
    
    lw    $v0, 0($a0)
    sw    $v0, 0($a1)  
    lw    $v0, 4($a0)
    sw    $v0, 4($a1)
    lw    $v0, 8($a0)
    sw    $v0, 8($a1)
    lw    $v0, 12($a0)
    sw    $v0, 12($a1)
    lw    $v0, 16($a0)
    sw    $v0, 16($a1)
    lw    $v0, 20($a0)
    sw    $v0, 20($a1)
    lw    $v0, 24($a0)
    sw    $v0, 24($a1)
    lw    $v0, 28($a0)
    sw    $v0, 28($a1)
    lw    $v0, 32($a0)
    sw    $v0, 32($a1)
    lw    $v0, 36($a0)
    sw    $v0, 36($a1)
    lw    $v0, 40($a0)
    sw    $v0, 40($a1)
    lw    $v0, 44($a0)
    sw    $v0, 44($a1)
    lw    $v0, 48($a0)
    sw    $v0, 48($a1)
    lw    $v0, 52($a0)
    sw    $v0, 52($a1)
    lw    $v0, 56($a0)
    sw    $v0, 56($a1)
    lw    $v0, 60($a0)
    sw    $v0, 60($a1)
    lw    $v0, 64($a0)
    sw    $v0, 64($a1)
    lw    $v0, 68($a0)
    sw    $v0, 68($a1)
    lw    $v0, 72($a0)
    sw    $v0, 72($a1)
    lw    $v0, 76($a0)
    sw    $v0, 76($a1)
    lw    $v0, 80($a0)
    sw    $v0, 80($a1)
    lw    $v0, 84($a0)
    sw    $v0, 84($a1)
    lw    $v0, 88($a0)
    sw    $v0, 88($a1)
    lw    $v0, 92($a0)
    sw    $v0, 92($a1)
    lw    $v0, 96($a0)
    sw    $v0, 96($a1)
    lw    $v0, 100($a0)
    sw    $v0, 100($a1)
    lw    $sp, 104($a0)
    sw    $sp, 104($a1)
    lw    $v0, 108($a0)
    sw    $v0, 108($a1)
    lw    $v0, 112($a0)
    sw    $v0, 112($a1)
    lw    $v0, 116($a0)
    sw    $v0, 116($a1)
    lw    $v0, 120($a0)
    sw    $v0, 120($a1)
    lw    $v0, 124($a0)
    sw    $v0, 124($a1)
    jr    $ra
    nop
    
    .set at
    .align 2
    .section .text
    .global gpr_context_save
gpr_context_save:
    la  $a1, _stack
    addi $a1, $a1, -128
    
    lw    $v0, 0($a1)
    sw    $v0, 0($a0)  
    lw    $v0, 4($a1)
    sw    $v0, 4($a0)
    lw    $v0, 8($a1)
    sw    $v0, 8($a0)
    lw    $v0, 12($a1)
    sw    $v0, 12($a0)
    lw    $v0, 16($a1)
    sw    $v0, 16($a0)
    lw    $v0, 20($a1)
    sw    $v0, 20($a0)
    lw    $v0, 24($a1)
    sw    $v0, 24($a0)
    lw    $v0, 28($a1)
    sw    $v0, 28($a0)
    lw    $v0, 32($a1)
    sw    $v0, 32($a0)
    lw    $v0, 36($a1)
    sw    $v0, 36($a0)
    lw    $v0, 40($a1)
    sw    $v0, 40($a0)
    lw    $v0, 44($a1)
    sw    $v0, 44($a0)
    lw    $v0, 48($a1)
    sw    $v0, 48($a0)
    lw    $v0, 52($a1)
    sw    $v0, 52($a0)
    lw    $v0, 56($a1)
    sw    $v0, 56($a0)
    lw    $v0, 60($a1)
    sw    $v0, 60($a0)
    lw    $v0, 64($a1)
    sw    $v0, 64($a0)
    lw    $v0, 68($a1)
    sw    $v0, 68($a0)
    lw    $v0, 72($a1)
    sw    $v0, 72($a0)
    lw    $v0, 76($a1)
    sw    $v0, 76($a0)
    lw    $v0, 80($a1)
    sw    $v0, 80($a0)
    lw    $v0, 84($a1)
    sw    $v0, 84($a0)
    lw    $v0, 88($a1)
    sw    $v0, 88($a0)
    lw    $v0, 92($a1)
    sw    $v0, 92($a0)
    lw    $v0, 96($a1)
    sw    $v0, 96($a0)
    lw    $v0, 100($a1)
    sw    $v0, 100($a0)
    lw    $sp, 104($a1)
    sw    $sp, 104($a0)
    lw    $v0, 108($a1)
    sw    $v0, 108($a0)
    lw    $v0, 112($a1)
    sw    $v0, 112($a0)
    lw    $v0, 116($a1)
    sw    $v0, 116($a0)
    lw    $v0, 120($a1)
    sw    $v0, 120($a0)
    lw    $v0, 124($a1)
    sw    $v0, 124($a0)
    jr    $ra
    nop
    