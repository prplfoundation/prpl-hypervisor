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

    # copy .data from flash to SRAM
    la  $a0, _edata
    la  $a1, _data
    la  $a2, _erodata
$COPY_DATA:
    lw  $v0, 0($a2)
    sw  $v0, 0($a1)
    slt $v1, $a1, $a0
    addiu   $a1, $a1, 4
    addiu   $a2, $a2, 4
    bnez    $v1, $COPY_DATA
    nop

    jal hyper_init
    nop

$L1:
    beq $zero, $zero, $L1
    nop
    .set micromips


.text
.global save_sgpr_ctx
.ent save_sgpr_ctx
save_sgpr_ctx:
	.set noat
	rdpgpr	$k0, $1
	sw	$k0, 0x0($a0)
	rdpgpr	$k0, $2
	sw	$k0, 0x4($a0)
	rdpgpr	$k0, $3
	sw	$k0, 0x8($a0)
	rdpgpr	$k0, $4
	sw	$k0, 0xC($a0)
	rdpgpr	$k0, $5
	sw	$k0, 0x10($a0)
	rdpgpr	$k0, $6
	sw	$k0, 0x14($a0)
	rdpgpr	$k0, $7
	sw	$k0, 0x18($a0)
	rdpgpr	$k0, $8
	sw	$k0, 0x1C($a0)
	rdpgpr	$k0, $9
	sw	$k0, 0x20($a0)
	rdpgpr	$k0, $10
	sw	$k0, 0x24($a0)
	rdpgpr	$k0, $11
	sw	$k0, 0x28($a0)
	rdpgpr	$k0, $12
	sw	$k0, 0x2C($a0)
	rdpgpr	$k0, $13
	sw	$k0, 0x30($a0)
	rdpgpr	$k0, $14
	sw	$k0, 0x34($a0)
	rdpgpr	$k0, $15
	sw	$k0, 0x38($a0)
	rdpgpr	$k0, $16
	sw	$k0, 0x3C($a0)
	rdpgpr	$k0, $17
	sw	$k0, 0x40($a0)
	rdpgpr	$k0, $18
	sw	$k0, 0x44($a0)
	rdpgpr	$k0, $19
	sw	$k0, 0x48($a0)
	rdpgpr	$k0, $20
	sw	$k0, 0x4C($a0)
	rdpgpr	$k0, $21
	sw	$k0, 0x50($a0)
	rdpgpr	$k0, $22
	sw	$k0, 0x54($a0)
	rdpgpr	$k0, $23
	sw	$k0, 0x58($a0)
	rdpgpr	$k0, $24
	sw	$k0, 0x5C($a0)
	rdpgpr	$k0, $25
	sw	$k0, 0x60($a0)
	rdpgpr	$k0, $26
	sw	$k0, 0x64($a0)
	rdpgpr	$k0, $27
	sw	$k0, 0x68($a0)
	rdpgpr	$k0, $28
	sw	$k0, 0x6C($a0)
	rdpgpr	$k0, $29
	sw	$k0, 0x70($a0)
	rdpgpr	$k0, $30
	sw	$k0, 0x74($a0)
	rdpgpr	$k0, $31
	sw	$k0, 0x78($a0)
	mfhi	$k0
	sw	$k0, 0x7C($a0)
	mflo	$k0
	sw	$k0, 0x80($a0)
	jr	$ra
	nop
	.set at
.end save_sgpr_ctx


.global restore_sgpr_ctx
.ent restore_sgpr_ctx
restore_sgpr_ctx:
	.set noat
	lw	$k0, 0x0($a0)
	wrpgpr	$1, $k0	
	lw	$k0, 0x4($a0)
	wrpgpr	$2, $k0
	lw	$k0, 0x8($a0)
	wrpgpr	$3, $k0
	lw	$k0, 0xC($a0)
	wrpgpr	$4, $k0
	lw	$k0, 0x10($a0)
	wrpgpr	$5, $k0
	lw	$k0, 0x14($a0)
	wrpgpr	$6, $k0
	lw	$k0, 0x18($a0)
	wrpgpr	$7, $k0
	lw	$k0, 0x1C($a0)
	wrpgpr	$8, $k0
	lw	$k0, 0x20($a0)
	wrpgpr	$9, $k0
	lw	$k0, 0x24($a0)
	wrpgpr	$10, $k0
	lw	$k0, 0x28($a0)
	wrpgpr	$11, $k0
	lw	$k0, 0x2C($a0)
	wrpgpr	$12, $k0
	lw	$k0, 0x30($a0)
	wrpgpr	$13, $k0
	lw	$k0, 0x34($a0)
	wrpgpr	$14, $k0
	lw	$k0, 0x38($a0)
	wrpgpr	$15, $k0
	lw	$k0, 0x3C($a0)
	wrpgpr	$16, $k0
	lw	$k0, 0x40($a0)
	wrpgpr	$17, $k0
	lw	$k0, 0x44($a0)
	wrpgpr	$18, $k0
	lw	$k0, 0x48($a0)
	wrpgpr	$19, $k0
	lw	$k0, 0x4C($a0)
	wrpgpr	$20, $k0
	lw	$k0, 0x50($a0)
	wrpgpr	$21, $k0
	lw	$k0, 0x54($a0)
	wrpgpr	$22, $k0
	lw	$k0, 0x58($a0)
	wrpgpr	$23, $k0
	lw	$k0, 0x5C($a0)
	wrpgpr	$24, $k0
	lw	$k0, 0x60($a0)
	wrpgpr	$25, $k0
	lw	$k0, 0x64($a0)
	wrpgpr	$26, $k0
	lw	$k0, 0x68($a0)
	wrpgpr	$27, $k0
	lw	$k0, 0x6C($a0)
	wrpgpr	$28, $k0
	lw	$k0, 0x70($a0)
	wrpgpr	$29, $k0
	lw	$k0, 0x74($a0)
	wrpgpr	$30, $k0
	lw	$k0, 0x78($a0)
	wrpgpr	$31, $k0
	lw	$k0, 0x7C($a0)
	mthi	$k0
	lw	$k0, 0x80($a0)
	mtlo	$k0
	jr	$ra
	nop
	.set at
.end restore_sgpr_ctx


.global hal_config_hyper_gpr_shadow
.ent hal_config_hyper_gpr_shadow
hal_config_hyper_gpr_shadow:
        .set noreorder
        la $k1, _gp
        wrpgpr $gp, $k1
        la $k1, _stack
        wrpgpr $sp, $k1
        jr $ra
        nop
        .set reorder
.end hal_config_hyper_gpr_shadow


