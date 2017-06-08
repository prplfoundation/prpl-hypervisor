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
######################## Interrupt Vector #############################################
.section .vector_hwint_0
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 0
	j 	vector_hwint
	nop
.section .vector_hwint_1
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 1
	j 	vector_hwint
	nop
.section .vector_hwint_2
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 2
	j 	vector_hwint
	nop
.section .vector_hwint_3
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 3
	j 	vector_hwint
	nop
.section .vector_hwint_4
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 4
	j 	vector_hwint
	nop
.section .vector_hwint_5
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 5
	j 	vector_hwint
	nop
.section .vector_hwint_6
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 6
	j 	vector_hwint
	nop
.section .vector_hwint_7
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 7
	j 	vector_hwint
	nop
.section .vector_hwint_8
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 8
	j 	vector_hwint
	nop
.section .vector_hwint_9
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 9
	j 	vector_hwint
	nop
.section .vector_hwint_10
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 10
	j 	vector_hwint
	nop
.section .vector_hwint_11
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 11
	j 	vector_hwint
	nop
.section .vector_hwint_12
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 12
	j 	vector_hwint
	nop
.section .vector_hwint_13
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 13
	j 	vector_hwint
	nop
.section .vector_hwint_14
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 14
	j 	vector_hwint
	nop
.section .vector_hwint_15
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 15
	j 	vector_hwint
	nop
.section .vector_hwint_16
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 16
	j 	vector_hwint
	nop
.section .vector_hwint_17
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 17
	j 	vector_hwint
	nop
.section .vector_hwint_18
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 18
	j 	vector_hwint
	nop
.section .vector_hwint_19
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 19
	j 	vector_hwint
	nop
.section .vector_hwint_20
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 20
	j 	vector_hwint
	nop
.section .vector_hwint_21
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 21
	j 	vector_hwint
	nop
.section .vector_hwint_22
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 22
	j 	vector_hwint
	nop
.section .vector_hwint_23
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 23
	j 	vector_hwint
	nop
.section .vector_hwint_24
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 24
	j 	vector_hwint
	nop
.section .vector_hwint_25
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 25
	j 	vector_hwint
	nop
.section .vector_hwint_26
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 26
	j 	vector_hwint
	nop
.section .vector_hwint_27	
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 27
	j 	vector_hwint
	nop
.section .vector_hwint_28
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 28
	j 	vector_hwint
	nop
.section .vector_hwint_29
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 29
	j 	vector_hwint
	nop
.section .vector_hwint_30
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 30
	j 	vector_hwint
	nop
.section .vector_hwint_31
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 31
	j 	vector_hwint
	nop
.section .vector_hwint_32
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 32
	j 	vector_hwint
	nop
.section .vector_hwint_33
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 33
	j 	vector_hwint
	nop
.section .vector_hwint_34
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 34
	j 	vector_hwint
	nop
.section .vector_hwint_35
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 35
	j 	vector_hwint
	nop
.section .vector_hwint_36
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 36
	j 	vector_hwint
	nop
.section .vector_hwint_37
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 37
	j 	vector_hwint
	nop
.section .vector_hwint_38
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 38
	j 	vector_hwint
	nop
.section .vector_hwint_39
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 37
	j 	vector_hwint
	nop
.section .vector_hwint_40
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 40
	j 	vector_hwint
	nop
.section .vector_hwint_41
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 41
	j 	vector_hwint
	nop
.section .vector_hwint_42
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 42
	j 	vector_hwint
	nop
.section .vector_hwint_43
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 43
	j 	vector_hwint
	nop
.section .vector_hwint_44
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 44
	j 	vector_hwint
	nop
.section .vector_hwint_45
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 45
	j 	vector_hwint
	nop
.section .vector_hwint_46
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 46
	j 	vector_hwint
	nop
.section .vector_hwint_47
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 47
	j 	vector_hwint
	nop
.section .vector_hwint_48
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 48
	j 	vector_hwint
	nop
.section .vector_hwint_49
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 49
	j 	vector_hwint
	nop
.section .vector_hwint_50
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 50
	j 	vector_hwint
	nop
.section .vector_hwint_51
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 51
	j 	vector_hwint
	nop
.section .vector_hwint_52
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 52
	j 	vector_hwint
	nop
.section .vector_hwint_53
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 53
	j 	vector_hwint
	nop
.section .vector_hwint_54
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 54
	j 	vector_hwint
	nop
.section .vector_hwint_55
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 55
	j 	vector_hwint
	nop
.section .vector_hwint_56
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 56
	j 	vector_hwint
	nop
.section .vector_hwint_57
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 57
	j 	vector_hwint
	nop
.section .vector_hwint_58
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 58
	j 	vector_hwint
	nop
.section .vector_hwint_59
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 59
	j 	vector_hwint
	nop
.section .vector_hwint_60
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 60
	j 	vector_hwint
	nop
.section .vector_hwint_61
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 61
	j 	vector_hwint
	nop
.section .vector_hwint_62
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 62
	j 	vector_hwint
	nop
.section .vector_hwint_63
	mtc0 	$a0, $31, 4		#kscratch3
	li	$a0, 63
	j 	vector_hwint
	nop


######################## Context save/restore ###############################################
vector_hwint:
    .set noat

    mtc0 $sp, $31, 2		#kscratch1
    mtc0 $gp, $31, 3		#kscratch2
    
    la  $sp, _stack          # set hypervisor stack
    addi $sp, $sp, -136      # reserved space for gpr st stack
    
    la  $gp, _gp

    sw    $at, 4($sp)		#save all GPR
    sw    $v0, 8($sp)
    sw    $v1, 12($sp)
    #a0 saved at the end
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
    mfc0  $k0, $31, 4	    	# saved $a0
    sw	  $k0, 16($sp)		
    mfhi  $k0
    sw    $k0, 128($sp)
    mflo  $k0
    sw    $k0, 132($sp)

    la $k0, __interrupt_handler
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

    
    