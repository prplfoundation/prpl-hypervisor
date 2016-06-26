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
    .global _isr
    .ent _isr
_isr:
    .set noreorder
    .set noat
        /*la   $gp, _gp
        la  $sp, _stack    */

        add $a1,$a0,0
        li $a0, 0        /* init parameter is 0. Its not the first execution of exceptionHandler */
        mfc0 $a1, $9    
        mfgc0 $a2, $9
        la $k1, exceptionHandler
        jalr $k1
        nop
    
    
isr_return:
    eret
    nop
.end _isr


    .set at
    .align 2
    .text
    .global _entry
    .ent    _entry
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

    # jump to main
   /* mfc0 $a0, $13*/
    jal main
    nop

$L1:
    beq $zero, $zero, $L1
    nop
.end _entry

	


/** load COP0 root.config reg */
.text
.global hal_lr_rconfig
.ent hal_lr_rconfig
hal_lr_rconfig:
	jr $ra
	mfc0 $v0, $16, 0
.end hal_lr_rconfig


/** load COP0 root.config1 reg */
.global hal_lr_rconfig1
.ent hal_lr_rconfig1
hal_lr_rconfig1:
	jr $ra
	mfc0 $v0, $16, 0x1
.end hal_lr_rconfig1

/** load COP0 root.config2 reg */
.global hal_lr_rconfig2
.ent hal_lr_rconfig2
hal_lr_rconfig2:
	jr $ra
	mfc0 $v0, $16, 0x2
.end hal_lr_rconfig2

/** load COP0 root.config3 reg */
.global hal_lr_rconfig3
.ent hal_lr_rconfig3
hal_lr_rconfig3:
	jr $ra
	mfc0 $v0, $16, 0x3
.end hal_lr_rconfig3

/** load COP0 root.config4 reg */
.global hal_lr_rconfig4
.ent hal_lr_rconfig4
hal_lr_rconfig4:
	jr $ra
	mfc0 $v0, $16, 0x4
.end hal_lr_rconfig4

/** load COP0 root.config5 reg */
.global hal_lr_rconfig5
.ent hal_lr_rconfig5
hal_lr_rconfig5:
	jr $ra
	mfc0 $v0, $16, 0x5
.end hal_lr_rconfig5

/** load COP0 root.config7 reg */
.global hal_lr_rconfig7
.ent hal_lr_rconfig7
hal_lr_rconfig7:
	jr $ra
	mfc0 $v0, $16, 0x7
.end hal_lr_rconfig7


/* save COP0 root.config reg */
.global hal_sr_rconfig
.ent hal_sr_rconfig
hal_sr_rconfig:
	jr $ra
	mtc0 $a0, $16, 0
.end hal_sr_rconfig

/* load CP0 GuestCtl0 reg */
.global hal_lr_guestctl0
.ent hal_lr_guestctl0
hal_lr_guestctl0:
	jr $ra
	mfc0 $v0, $12, 0x6
.end hal_lr_guestctl0

/* save CP0 GuestCtl0 reg */
.global hal_sr_guestctl0
.ent hal_sr_guestctl0
hal_sr_guestctl0:
	jr $ra
	mtc0 $a0, $12, 0x6
.end hal_sr_guestctl0

/* load CP0 GuestCtl0Ext reg */
.global hal_lr_guestctl0Ext
.ent hal_lr_guestctl0Ext
hal_lr_guestctl0Ext:
	jr $ra
	mfc0 $v0, $11, 0x4
.end hal_lr_guestctl0Ext

/* save CP0 GuestCtl0Ext reg */
.global hal_sr_guestctl0Ext
.ent hal_sr_guestctl0Ext
hal_sr_guestctl0Ext:
	jr $ra
	mtc0 $a0, $11, 0x4
.end hal_sr_guestctl0Ext


/* load COP0 root.debug reg */
.global hal_lr_rdebug
.ent hal_lr_rdebug
hal_lr_rdebug:
	jr $ra
	mfc0 $v0, $23, 0
.end hal_lr_rdebug

/* load COP0 root.status reg */
.text
.global hal_lr_rstatus
.ent hal_lr_rstatus
hal_lr_rstatus:
	jr $ra
	mfc0 $v0, $12, 0
.end hal_lr_rstatus

/* save COP0 root.status reg */
.text
.global hal_sr_rstatus
.ent hal_sr_rstatus
hal_sr_rstatus:	
	jr $ra
	mtc0 $a0, $12, 0
.end hal_sr_rstatus

/* load COP0 root.cause reg */
.global hal_lr_rcause
.ent hal_lr_rcause
hal_lr_rcause:
	jr $ra
	mfc0 $v0, $13, 0
.end hal_lr_rcause

/* save COP0 root.cause reg */
.global hal_sr_rcause
.ent hal_sr_rcause
hal_sr_rcause:
	jr $ra
	mtc0 $a0, $13, 0
.end hal_sr_rcause

/* save COP0 root.count reg */
.global hal_sr_rcount
.ent hal_sr_rcount
hal_sr_rcount:
	jr $ra
	mtc0 $a0, $9, 0
.end hal_sr_rcount

/* load COP0 root.count reg */
.global hal_lr_rcount
.ent hal_lr_rcount
hal_lr_rcount:
	jr $ra
	mfc0 $v0, $9, 0
.end hal_lr_rcount


/* save COP0 root.compare reg */
.global hal_sr_rcompare
.ent hal_sr_rcompare
hal_sr_rcompare:
	jr $ra
	mtc0 $a0, $11, 0
.end hal_sr_rcompare

/* load COP0 root.compare reg */
.global hal_lr_rcompare
.ent hal_lr_rcompare
hal_lr_rcompare:
	jr $ra
	mfc0 $v0, $9, 0
.end hal_lr_rcompare

/* load COP0 root.SRSCLT reg */
.global hal_lr_srsclt
.ent hal_lr_srsclt
hal_lr_srsclt:
	jr $ra
	mfc0 $v0, $12, 2
.end hal_lr_srsclt

/* load COP0 root.SRSCLT reg */
.global hal_lr_pcr
.ent hal_lr_pcr
hal_lr_pcr:
	jr $ra
	mfc0 $v0, $25, 0
.end hal_lr_pcr

/* save COP0 root.SRSCLT reg */
.global hal_sr_srsclt
.ent hal_sr_srsclt
hal_sr_srsclt:
	jr $ra
	mtc0 $a0, $12, 2
.end hal_sr_srsclt

/* load COP0 guest.SRSCLT reg */
.global hal_lr_G_srsclt
.ent hal_lr_G_srsclt
hal_lr_G_srsclt:
	jr $ra
	mfgc0 $v0, $12, 2
.end hal_lr_G_srsclt

/* load from CP0 SRSMap reg */
.global hal_lr_srsmap
.ent hal_lr_srsmap
hal_lr_srsmap:
	jr $ra
	mfc0 $v0, $12, 0x3
.end hal_lr_srsmap

/* load from CP0 SRSMap2 reg */
.global hal_lr_srsmap2
.ent hal_lr_srsmap2
hal_lr_srsmap2:
	jr $ra
	mfc0 $v0, $12, 0x5
.end hal_lr_srsmap2

/* load COP0 guest.Status reg */
.global hal_lr_G_status
.ent hal_lr_G_status
hal_lr_G_status:
	jr $ra
	mfgc0 $v0, $12, 0
.end hal_lr_G_status

/* save COP0 guest.SRSCLT reg */
.global hal_sr_G_srsclt
.ent hal_sr_G_srsclt
hal_sr_G_srsclt:
	jr $ra
	mtgc0 $a0, $12, 2
.end hal_sr_G_srsclt

/* save COP0 root.SRSMap reg */
.global hal_sr_srsmap
.ent hal_sr_srsmap
hal_sr_srsmap:
	jr $ra
	mtc0 $a0, $12, 3
.end hal_sr_srsmap

/* save COP0 root.SRSMap2 reg */
.global hal_sr_srsmap2
.ent hal_sr_srsmap2
hal_sr_srsmap2:
	jr $ra
	mtc0 $a0, $12, 5
.end hal_sr_srsmap2

/* save COP0 root.intctl reg */
.global hal_sr_intctl
.ent hal_sr_intctl
hal_sr_intctl:
	jr $ra
	mtc0 $a0, $12, 1
.end hal_sr_intctl

/* load COP0 root.intctl reg */
.global hal_lr_intctl
.ent hal_lr_intctl
hal_lr_intctl:
	jr $ra
	mfc0 $v0, $12, 1
.end hal_lr_intctl

/* save COP0 root.index reg */
.global hal_sr_index
.ent hal_sr_index
hal_sr_index:
	jr $ra
	mtc0 $a0, $0, 0
.end hal_sr_index

/* load COP0 root.index reg */
.global hal_lr_index
.ent hal_lr_index
hal_lr_index:
	jr $ra
	mfc0 $v0, $0, 0
.end hal_lr_index

/* load COP0 root.random reg */
.global hal_lr_random
.ent hal_lr_random
hal_lr_random:
	jr $ra
	mfc0 $v0, $1, 0
.end hal_lr_random

/* save COP0 root.entrylo0 reg */
.global hal_sr_entrylo0
.ent hal_sr_entrylo0
hal_sr_entrylo0:
	jr $ra
	mtc0 $a0, $2, 0
.end hal_sr_entrylo0

/* load COP0 root.entrylo0 reg */
.global hal_lr_entrylo0
.ent hal_lr_entrylo0
hal_lr_entrylo0:
	jr $ra
	mfc0 $v0, $2, 0
.end hal_lr_entrylo0

/* save COP0 root.entrylo1 reg */
.global hal_sr_entrylo1
.ent hal_sr_entrylo1
hal_sr_entrylo1:
	jr $ra
	mtc0 $a0, $3, 0
.end hal_sr_entrylo1

/* load COP0 root.entrylo1 reg */
.global hal_lr_entrylo1
.ent hal_lr_entrylo1
hal_lr_entrylo1:
	jr $ra
	mfc0 $v0, $3, 0
.end hal_lr_entrylo1

/* save COP0 root.pagemask reg */
.global hal_sr_pagemask
.ent hal_sr_pagemask
hal_sr_pagemask:
	jr $ra
	mtc0 $a0, $5, 0
.end hal_sr_pagemask

/* load COP0 root.pagemask reg */
.global hal_lr_pagemask
.ent hal_lr_pagemask
hal_lr_pagemask:
	jr $ra
	mfc0 $v0, $5, 0
.end hal_lr_pagemask

/* save COP0 root.pagegrain reg */
.global hal_sr_pagegrain
.ent hal_sr_pagegrain
hal_sr_pagegrain:
	jr $ra
	mtc0 $a0, $5, 1
.end hal_sr_pagegrain

/* load COP0 root.pagegrain reg */
.global hal_lr_pagegrain
.ent hal_lr_pagegrain
hal_lr_pagegrain:
	jr $ra
	mfc0 $v0, $5, 1
.end hal_lr_pagegrain

/* save COP0 root.entryhi reg */
.global hal_sr_entryhi
.ent hal_sr_entryhi
hal_sr_entryhi:
	jr $ra
	mtc0 $a0, $10, 0
.end hal_sr_entryhi

/* load COP0 root.entryhi reg */
.global hal_lr_entryhi
.ent hal_lr_entryhi
hal_lr_entryhi:
	jr $ra
	mfc0 $v0, $10, 0
.end hal_lr_entryhi

/* tlb indexed write */
.global hal_tlb_commit
.ent hal_tlb_commit
hal_tlb_commit:
	ehb
	tlbwi
	jr $ra
	nop
.end hal_tlb_commit

/* load CPO root.epc */
.global hal_sr_epc
.ent hal_sr_epc
hal_sr_epc:
	jr $ra
	mtc0 $a0, $14, 0
.end hal_sr_epc

/* save CP0 epc */
.global hal_lr_epc
.ent hal_lr_epc
hal_lr_epc:
	jr $ra
	mfc0 $v0, $14, 0
.end hal_lr_epc

/* read CP0 badVAaddr */
.global hal_lr_badVAaddr
.ent  hal_lr_badVAaddr
hal_lr_badVAaddr:
	jr $ra
	mfc0 $v0, $8, 0
.end hal_lr_badVAaddr

/* read CP0 badinstr */
.global hal_lr_badinstr
.ent hal_lr_badinstr
hal_lr_badinstr:
	jr $ra
	mfc0 $v0, $8, 1
.end hal_lr_badinstr


/* read CP0 perfCounter0 */
.global hal_lr_perfcounter0
.ent hal_lr_perfcounter0
hal_lr_perfcounter0:
	jr $ra
	mfc0 $v0, $25, 1
.end hal_lr_perfcounter0

/* read CP0 perfCounter0ctl */
.global hal_lr_perfcounter0ctl
.ent hal_lr_perfcounter0ctl
hal_lr_perfcounter0ctl:
	jr $ra
	mfc0 $v0, $25, 0
.end hal_lr_perfcounter0ctl

/* read CP0 perfCounter1 */
.global hal_lr_perfcounter1
.ent hal_lr_perfcounter1
hal_lr_perfcounter1:
	jr $ra
	mfc0 $v0, $25, 3
.end hal_lr_perfcounter1

/* read CP0 perfCounter1ctl */
.global hal_lr_perfcounter1ctl
.ent hal_lr_perfcounter1ctl
hal_lr_perfcounter1ctl:
	jr $ra
	mfc0 $v0, $25, 2
.end hal_lr_perfcounter1ctl

/* read CP0 perfCounter0 */
.global hal_sr_perfcounter0
.ent hal_sr_perfcounter0
hal_sr_perfcounter0:
	jr $ra
	mtc0 $a0, $25, 1
.end hal_sr_perfcounter0

/* read CP0 perfCounter0ctl */
.global hal_sr_perfcounter0ctl
.ent hal_sr_perfcounter0ctl
hal_sr_perfcounter0ctl:
	jr $ra
	mtc0 $a0, $25, 0
.end hal_sr_perfcounter0ctl

/* read CP0 perfCounter1 */
.global hal_sr_perfcounter1
.ent hal_sr_perfcounter1
hal_sr_perfcounter1:
	jr $ra
	mtc0 $a0, $25, 3
.end hal_sr_perfcounter1

/* read CP0 perfCounter1ctl */
.global hal_sr_perfcounter1ctl
.ent hal_sr_perfcounter1ctl
hal_sr_perfcounter1ctl:
	jr $ra
	mtc0 $a0, $25, 2
.end hal_sr_perfcounter1ctl

/* read CP0 badinstr */
.global hal_lr_badinstrP
.ent hal_lr_badinstrP
hal_lr_badinstrP:
	jr $ra
	mfc0 $v0, $8, 2
.end hal_lr_badinstrP

/* save CPO root.guestctl1 */
.global hal_sr_guestclt1
.ent hal_sr_guestclt1
hal_sr_guestclt1:
	jr $ra
	mtc0 $a0, $10, 4
.end hal_sr_guestclt1

/* load CPO root.guestctl1 */
.global hal_lr_guestclt1
.ent hal_lr_guestclt1
hal_lr_guestclt1:
	jr $ra
	mfc0 $v0, $10, 4
.end hal_lr_guestclt1

/* load CPO root.guestctl2 */
.global hal_lr_guestclt2
.ent hal_lr_guestclt2
hal_lr_guestclt2:
	jr $ra
	mfc0 $v0, $10, 5
.end hal_lr_guestclt2

/* load from CP0 GuestCtl3 reg */
.global hal_lr_guestctl3
.ent hal_lr_guestctl3
hal_lr_guestctl3:
	jr $ra
	mfc0 $v0, $10, 0x6
.end hal_lr_guestctl3

/* save to CP0 GuestCtl3 reg */
.global hal_sr_guestctl3
.ent hal_sr_guestctl3
hal_sr_guestctl3:
	jr $ra
	mtc0 $a0, $10, 0x6
.end hal_sr_guestctl3

/* save to CP0 GuestCtl2 reg */
.global hal_sr_guestctl2
.ent hal_sr_guestctl2
hal_sr_guestctl2:
	jr $ra
	mtc0 $a0, $10, 0x5
.end hal_sr_guestctl2

/* save to CP0 GTOffset reg */
.global hal_sr_gtoffset
.ent hal_sr_gtoffset
hal_sr_gtoffset:
	jr $ra
	mtc0 $a0, $12, 0x7
.end hal_sr_gtoffset

/* load from CP0 EBase reg */
.global hal_lr_ebase
.ent hal_lr_ebase
hal_lr_ebase:
	jr $ra
	mfc0 $v0, $15, 0x1
.end hal_lr_ebase

.global hal_lr_pid
.ent hal_lr_pid
hal_lr_pid:
	jr $ra
	mfc0 $v0, $15, 0x0
.end hal_lr_pid	


.global hal_lr_viewipl
.ent hal_lr_viewipl
hal_lr_viewipl:
        jr $ra
        mfc0 $v0, $12, 0x4
.end hal_lr_viewipl 



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

.global AsmResetPerformanceCounters
.ent AsmResetPerformanceCounters
AsmResetPerformanceCounters:
	.set noreorder
	
	add 		$k0,$ra,0

	add		$a0,$zero,$zero	
	la		$a1,hal_sr_perfcounter0		
	jalr		$a1
	nop
	
	li		$a0,0x0B	
	la		$a1,hal_sr_perfcounter0ctl		
	jalr		$a1
	nop
	
	add		$a0,$zero,$zero	
	la		$a1,hal_sr_perfcounter1		
	jalr		$a1
	nop
	
	li		$a0,0x180000B	
	la		$a1,hal_sr_perfcounter1ctl		
	jalr		$a1
	nop
	
	add		$ra,$k0,0
	
	jr		$ra
	nop
	
	.set reorder
.end AsmResetPerformanceCounters

.global AsmReadByte
.ent AsmReadByte
AsmReadByte:
	.set noreorder
	
	jr    $ra
	lb    $v0, 0($a0)
		
	.set reorder
.end AsmReadByte

.global AsmWriteByte
.ent AsmWriteByte
AsmWriteByte:
	.set noreorder

	jr    $ra
	sb    $a1, 0($a0)
	
	.set reorder
.end AsmWriteByte

/*.global idlevcpu
.ent idlevcpu
idlevcpu:
	.set noreorder
	beq	$zero, $zero, idlevcpu
	nop
	.set reorder
.end idlevcpu*/

.global hal_start_hyper
.ent hal_start_hyper
hal_start_hyper:
        .set noreorder
        la $k1, _gp
        wrpgpr $gp, $k1
        la $k1, _stack
        wrpgpr $sp, $k1
        jr $ra
        /*mfc0 $v0, $12, 2*/
        nop
        .set reorder
.end hal_start_hyper


