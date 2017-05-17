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
    

######################## EXCEPTION #############################################
.section .exception
exception:
	.set noreorder
	.set noat
    
	mtc0 $sp, $31, 2		#kscratch1
	mtc0 $gp, $31, 3		#kscratch2
    
	la  $sp, _stack          # set hypervisor stack
    addi $sp, $sp, -136      # reserved space for gpr st stack
    
    la  $gp, _gp
    # $zero ignored.
    sw    $at, 4($sp)           #save all GPR
    sw    $v0, 8($sp)
    sw    $v1, 12($sp)
    sw    $a0, 16($sp)
    sw    $a1, 20($sp)
    sw    $a2, 24($sp)
    sw    $a3, 28($sp)
    sw    $t0, 32($sp)
    sw    $t1, 36($sp)
    sw    $t2, 40($sp)
    sw    $t3, 44($sp)
    sw    $t4, 48($sp)
    sw    $t5, 52($sp)
    sw    $t6, 56($sp)
    sw    $t7, 60($sp)
    sw    $s0, 64($sp)
    sw    $s1, 68($sp)
    sw    $s2, 72($sp)
    sw    $s3, 76($sp)
    sw    $s4, 80($sp)
    sw    $s5, 84($sp)
    sw    $s6, 88($sp)
    sw    $s7, 92($sp)
    sw    $t8, 96($sp)
    sw    $t9, 100($sp)
    sw    $k0, 104($sp)
    sw    $k1, 108($sp)
    #save gp to position 112.
    #save sp to position 116.
    sw    $fp, 120($sp)
    sw    $ra, 124($sp)
    mfc0  $k1, $31, 2	    # saved $sp
    sw	  $k1, 116($sp)	
    mfc0  $k1, $31, 3	    # saved $gp
    sw	  $k1, 112($sp)		
    mfhi  $k1
    sw    $k1, 128($sp)
    mflo  $k1
    sw    $k1, 132($sp)

    la    $k0, general_exception_handler
    jalr  $k0
    nop
	
    lw    $at, 4($sp)           #restore all GPR
    lw    $v0, 8($sp)
    lw    $v1, 12($sp)
    lw    $a0, 16($sp)
    lw    $a1, 20($sp)
    lw    $a2, 24($sp)
    lw    $a3, 28($sp)
    lw    $t0, 32($sp)
    lw    $t1, 36($sp)
    lw    $t2, 40($sp)
    lw    $t3, 44($sp)
    lw    $t4, 48($sp)
    lw    $t5, 52($sp)
    lw    $t6, 56($sp)
    lw    $t7, 60($sp)
    lw    $s0, 64($sp)
    lw    $s1, 68($sp)
    lw    $s2, 72($sp)
    lw    $s3, 76($sp)
    lw    $s4, 80($sp)
    lw    $s5, 84($sp)
    lw    $s6, 88($sp)
    lw    $s7, 92($sp)
    lw    $t8, 96($sp)
    lw    $t9, 100($sp)
    # $k0 restored at the end
    lw    $k1, 108($sp)
    lw    $gp, 112($sp)
    # $sp restored at the end
    lw    $fp, 120($sp)
    lw    $ra, 124($sp)
    lw    $k0, 128($sp)
    mthi  $k0
    lw    $k0, 132($sp)
    mtlo  $k0
    lw    $k0, 104($sp)
    lw	  $sp, 116($sp)	
    
    ehb
    eret
    nop

    
######################## HARDWARE INT 4 ###############################################
.section .vector_hwint_4
vector_hwint_4:
    .set noat

    mtc0 $sp, $31, 2		#kscratch1
    mtc0 $gp, $31, 3		#kscratch2
    
    la  $sp, _stack          # set hypervisor stack
    addi $sp, $sp, -136      # reserved space for gpr st stack
    
    la  $gp, _gp

    sw    $at, 4($sp)		#save all GPR
    sw    $v0, 8($sp)
    sw    $v1, 12($sp)
    sw    $a0, 16($sp)
    sw    $a1, 20($sp)
    sw    $a2, 24($sp)
    sw    $a3, 28($sp)
    sw    $t0, 32($sp)
    sw    $t1, 36($sp)
    sw    $t2, 40($sp)
    sw    $t3, 44($sp)
    sw    $t4, 48($sp)
    sw    $t5, 52($sp)
    sw    $t6, 56($sp)
    sw    $t7, 60($sp)
    sw    $s0, 64($sp)
    sw    $s1, 68($sp)
    sw    $s2, 72($sp)
    sw    $s3, 76($sp)
    sw    $s4, 80($sp)
    sw    $s5, 84($sp)
    sw    $s6, 88($sp)
    sw    $s7, 92($sp)
    sw    $t8, 96($sp)
    sw    $t9, 100($sp)
    sw    $k0, 104($sp)
    sw    $k1, 108($sp)
    #save gp to position 112.
    #save sp to position 116.
    sw    $fp, 120($sp)
    sw    $ra, 124($sp)
    mfc0  $k0, $31, 2	    	# saved $sp
    sw	  $k0, 116($sp)	
    mfc0  $k0, $31, 3	    	# saved $gp
    sw	  $k0, 112($sp)		
    mfhi  $k0
    sw    $k0, 128($sp)
    mflo  $k0
    sw    $k0, 132($sp)

    la $k0, interrupt_handlers
    lw $k0, 8($k0)
    jalr $k0
    nop
	
    lw    $at, 4($sp)          	#restore all GPR
    lw    $v0, 8($sp)
    lw    $v1, 12($sp)
    lw    $a0, 16($sp)
    lw    $a1, 20($sp)
    lw    $a2, 24($sp)
    lw    $a3, 28($sp)
    lw    $t0, 32($sp)
    lw    $t1, 36($sp)
    lw    $t2, 40($sp)
    lw    $t3, 44($sp)
    lw    $t4, 48($sp)
    lw    $t5, 52($sp)
    lw    $t6, 56($sp)
    lw    $t7, 60($sp)
    lw    $s0, 64($sp)
    lw    $s1, 68($sp)
    lw    $s2, 72($sp)
    lw    $s3, 76($sp)
    lw    $s4, 80($sp)
    lw    $s5, 84($sp)
    lw    $s6, 88($sp)
    lw    $s7, 92($sp)
    lw    $t8, 96($sp)
    lw    $t9, 100($sp)
    # $k0 restored at the end
    lw    $k1, 108($sp)
    lw    $gp, 112($sp)
    # $sp restored at the end
    lw    $fp, 120($sp)
    lw    $ra, 124($sp)
    lw    $k0, 128($sp)
    mthi  $k0
    lw    $k0, 132($sp)
    mtlo  $k0
    
    lw    $k0, 104($sp)
    lw	  $sp, 116($sp)	
    
    ehb
    eret
    nop

    
############################## Hypervisor entry point #############################
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

    
    