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

#include <boot.h>
#include <mips/regdef.h>


/* Patch the interrupt vector at 0x80000180, 0x80000000 and 0x80000200,for now. 
   We are using IV mode. For now, the int_vector routine handle all interrupt or exeception.
   The propose of this is to make the execution jumps to the int_vector routine when in exception or interrupt context.
*/
	.set noreorder
	
	
/**************************************************************************************
   V I  E X C E P T I O N   H A N D L E R S
**************************************************************************************/
LEAF(exception_handler_p)
exception_handler_p:			
	mfc0 a1, $9, 0          /*Get counter as early as possible to keep precision*/
	mfgc0 a2, $9,0 		/*Get guest counter as early as possible to keep precision*/
	la k0, exception_vector	/* Load address of the interrupt handler */
	jr k0			/* Jump into the interrupt handler */
	nop
END(exception_handler_p)	

LEAF(hal_patch_exception_vector)
hal_patch_exception_vector:
	#Patch interrupt vector
	
	la    t1, exception_handler_p
	lw    t2, 0(t1)
	sw    t2, 0(a0)
	lw    t2, 4(t1)
	sw    t2, 4(a0)
	lw    t2, 8(t1)
	sw    t2, 8(a0)
	lw    t2, 12(t1)
	sw    t2, 12(a0)
	lw    t2, 16(t1)
	sw    t2, 16(a0)
	lw    t2, 20(t1)
	sw    t2, 20(a0)
	jr    ra
	nop

END(hal_patch_exception_vector)


LEAF(exception_vector)
exception_vector:   
    add a1,a0,0
    li a0, 0        /* init parameter is 0. Its not the first execution of exceptionHandler */
    lw k1, exceptionHandler
    nop
    jalr k1
    nop

    .global vmmexit
vmmexit:
    ehb
    eret
    nop
END(exception_vector)


L2:
	beq	zero, zero, L2
	nop
	
/** load COP0 root.config reg */
LEAF(hal_lr_rconfig)
hal_lr_rconfig:
	jr ra
	mfc0 v0, $16, 0
END(hal_lr_rconfig)

/** load COP0 root.config1 reg */
LEAF(hal_lr_rconfig1)
hal_lr_rconfig1:
	jr ra
	mfc0 v0, $16, 0x1
END(hal_lr_rconfig1)

/** load COP0 root.config2 reg */
LEAF(hal_lr_rconfig2)
hal_lr_rconfig2:
	jr ra
	mfc0 v0, $16, 0x2
END(hal_lr_rconfig2)

/** load COP0 root.config3 reg */
LEAF(hal_lr_rconfig3)
hal_lr_rconfig3:
	jr ra
	mfc0 v0, $16, 0x3
END(hal_lr_rconfig3)

/** load COP0 root.config4 reg */
LEAF(hal_lr_rconfig4)
hal_lr_rconfig4:
	jr ra
	mfc0 v0, $16, 0x4
END(hal_lr_rconfig4)

/** load COP0 root.config5 reg */
LEAF(hal_lr_rconfig5)
hal_lr_rconfig5:
	jr ra
	mfc0 v0, $16, 0x5
END(hal_lr_rconfig5)

/** load COP0 root.config7 reg */
LEAF(hal_lr_rconfig7)
hal_lr_rconfig7:
	jr ra
	mfc0 v0, $16, 0x7
END(hal_lr_rconfig7)


/* save COP0 root.config reg */
LEAF(hal_sr_rconfig)
hal_sr_rconfig:
	jr ra
	mtc0 a0, $16, 0
END(hal_sr_rconfig)

/* load CP0 GuestCtl0 reg */
LEAF(hal_lr_guestctl0)
hal_lr_guestctl0:
	jr ra
	mfc0 v0, $12, 0x6
END(hal_lr_guestctl0)

/* save CP0 GuestCtl0 reg */
LEAF(hal_sr_guestctl0)
hal_sr_guestctl0:
	jr ra
	mtc0 a0, $12, 0x6
END(hal_sr_guestctl0)

/* load CP0 GuestCtl0Ext reg */
LEAF(hal_lr_guestctl0Ext)
hal_lr_guestctl0Ext:
	jr ra
	mfc0 v0, $11, 0x4
END(hal_lr_guestctl0Ext)

/* save CP0 GuestCtl0Ext reg */
LEAF(hal_sr_guestctl0Ext)
hal_sr_guestctl0Ext:
	jr ra
	mtc0 a0, $11, 0x4
END(hal_sr_guestctl0Ext)


/* load COP0 root.debug reg */
LEAF(hal_lr_rdebug)
hal_lr_rdebug:
	jr ra
	mfc0 v0, $23, 0
END(hal_lr_rdebug)

/* load COP0 root.status reg */
LEAF(hal_lr_rstatus)
hal_lr_rstatus:
	jr ra
	mfc0 v0, $12, 0
END(hal_lr_rstatus)

/* save COP0 root.status reg */
LEAF(hal_sr_rstatus)
hal_sr_rstatus:	
	jr ra
	mtc0 a0, $12, 0
END(hal_sr_rstatus)

/* load COP0 root.cause reg */
LEAF(hal_lr_rcause)
hal_lr_rcause:
	jr ra
	mfc0 v0, $13, 0
END(hal_lr_rcause)

/* save COP0 root.cause reg */
LEAF(hal_sr_rcause)
hal_sr_rcause:
	jr ra
	mtc0 a0, $13, 0
END(hal_sr_rcause)

/* save COP0 root.count reg */
LEAF(hal_sr_rcount)
hal_sr_rcount:
	jr ra
	mtc0 a0, $9, 0
END(hal_sr_rcount)

/* load COP0 root.count reg */
LEAF(hal_lr_rcount)
hal_lr_rcount:
	jr ra
	mfc0 v0, $9, 0
END(hal_lr_rcount)


/* save COP0 root.compare reg */
LEAF(hal_sr_rcompare)
hal_sr_rcompare:
	jr ra
	mtc0 a0, $11, 0
END(hal_sr_rcompare)

/* load COP0 root.compare reg */
LEAF(hal_lr_rcompare)
hal_lr_rcompare:
	jr ra
	mfc0 v0, $9, 0
END(hal_lr_rcompare)

/* load COP0 root.SRSCLT reg */
LEAF(hal_lr_srsclt)
hal_lr_srsclt:
	jr ra
	mfc0 v0, $12, 2
END(hal_lr_srsclt)

/* load COP0 root.SRSCLT reg */
LEAF(hal_lr_pcr)
hal_lr_pcr:
	jr ra
	mfc0 v0, $25, 0
END(hal_lr_pcr)

/* save COP0 root.SRSCLT reg */
LEAF(hal_sr_srsclt)
hal_sr_srsclt:
	jr ra
	mtc0 a0, $12, 2
END(hal_sr_srsclt)

/* load COP0 guest.SRSCLT reg */
LEAF(hal_lr_G_srsclt)
hal_lr_G_srsclt:
	jr ra
	mfgc0 v0, $12, 2
END(hal_lr_G_srsclt)

/* load from CP0 SRSMap reg */
LEAF(hal_lr_srsmap)
hal_lr_srsmap:
	jr ra
	mfc0 v0, $12, 0x3
END(hal_lr_srsmap)

/* load from CP0 SRSMap2 reg */
LEAF(hal_lr_srsmap2)
hal_lr_srsmap2:
	jr ra
	mfc0 v0, $12, 0x5
END(hal_lr_srsmap2)

/* load COP0 guest.Status reg */
LEAF(hal_lr_G_status)
hal_lr_G_status:
	jr ra
	mfgc0 v0, $12, 0
END(hal_lr_G_status)

/* save COP0 guest.SRSCLT reg */
LEAF(hal_sr_G_srsclt)
hal_sr_G_srsclt:
	jr ra
	mtgc0 a0, $12, 2
END(hal_sr_G_srsclt)

/* save COP0 root.SRSMap reg */
LEAF(hal_sr_srsmap)
hal_sr_srsmap:
	jr ra
	mtc0 a0, $12, 3
END(hal_sr_srsmap)

/* save COP0 root.SRSMap2 reg */
LEAF(hal_sr_srsmap2)
hal_sr_srsmap2:
	jr ra
	mtc0 a0, $12, 5
END(hal_sr_srsmap2)

/* save COP0 root.intctl reg */
LEAF(hal_sr_intctl)
hal_sr_intctl:
	jr ra
	mtc0 a0, $12, 1
END(hal_sr_intctl)

/* load COP0 root.intctl reg */
LEAF(hal_lr_intctl)
hal_lr_intctl:
	jr ra
	mfc0 v0, $12, 1
END(hal_lr_intctl)

/* save COP0 root.index reg */
LEAF(hal_sr_index)
hal_sr_index:
	jr ra
	mtc0 a0, $0, 0
END(hal_sr_index)

/* load COP0 root.index reg */
LEAF(hal_lr_index)
hal_lr_index:
	jr ra
	mfc0 v0, $0, 0
END(hal_lr_index)

/* load COP0 root.random reg */
LEAF(hal_lr_random)
hal_lr_random:
	jr ra
	mfc0 v0, $1, 0
END(hal_lr_random)

/* save COP0 root.entrylo0 reg */
LEAF(hal_sr_entrylo0)
hal_sr_entrylo0:
	jr ra
	mtc0 a0, $2, 0
END(hal_sr_entrylo0)

/* load COP0 root.entrylo0 reg */
LEAF(hal_lr_entrylo0)
hal_lr_entrylo0:
	jr ra
	mfc0 v0, $2, 0
END(hal_lr_entrylo0)

/* save COP0 root.entrylo1 reg */
LEAF(hal_sr_entrylo1)
hal_sr_entrylo1:
	jr ra
	mtc0 a0, $3, 0
END(hal_sr_entrylo1)

/* load COP0 root.entrylo1 reg */
LEAF(hal_lr_entrylo1)
hal_lr_entrylo1:
	jr ra
	mfc0 v0, $3, 0
END(hal_lr_entrylo1)

/* save COP0 root.pagemask reg */
LEAF(hal_sr_pagemask)
hal_sr_pagemask:
	jr ra
	mtc0 a0, $5, 0
END(hal_sr_pagemask)

/* load COP0 root.pagemask reg */
LEAF(hal_lr_pagemask)
hal_lr_pagemask:
	jr ra
	mfc0 v0, $5, 0
END(hal_lr_pagemask)

/* save COP0 root.pagegrain reg */
LEAF(hal_sr_pagegrain)
hal_sr_pagegrain:
	jr ra
	mtc0 a0, $5, 1
END(hal_sr_pagegrain)

/* load COP0 root.pagegrain reg */
LEAF(hal_lr_pagegrain)
hal_lr_pagegrain:
	jr ra
	mfc0 v0, $5, 1
END(hal_lr_pagegrain)

/* save COP0 root.entryhi reg */
LEAF(hal_sr_entryhi)
hal_sr_entryhi:
	jr ra
	mtc0 a0, $10, 0
END(hal_sr_entryhi)

/* load COP0 root.entryhi reg */
LEAF(hal_lr_entryhi)
hal_lr_entryhi:
	jr ra
	mfc0 v0, $10, 0
END(hal_lr_entryhi)

/* tlb indexed write */
LEAF(hal_tlb_commit)
hal_tlb_commit:
	ehb
	tlbwi
	jr ra
	nop
END(hal_tlb_commit)

/* load CPO root.epc */
LEAF(hal_sr_epc)
hal_sr_epc:
	jr ra
	mtc0 a0, $14, 0
END(hal_sr_epc)

/* save CP0 epc */
LEAF(hal_lr_epc)
hal_lr_epc:
	jr ra
	mfc0 v0, $14, 0
END(hal_lr_epc)

/* read CP0 badVAaddr */
LEAF(hal_lr_badVAaddr)
hal_lr_badVAaddr:
	jr ra
	mfc0 v0, $8, 0
END(hal_lr_badVAaddr)

/* read CP0 badinstr */
LEAF(hal_lr_badinstr)
hal_lr_badinstr:
	jr ra
	mfc0 v0, $8, 1
END(hal_lr_badinstr)


/* read CP0 perfCounter0 */
LEAF(hal_lr_perfcounter0)
hal_lr_perfcounter0:
	jr ra
	mfc0 v0, $25, 1
END(hal_lr_perfcounter0)

/* read CP0 perfCounter0ctl */
LEAF(hal_lr_perfcounter0ctl)
hal_lr_perfcounter0ctl:
	jr ra
	mfc0 v0, $25, 0
END(hal_lr_perfcounter0ctl)

/* read CP0 perfCounter1 */
LEAF(hal_lr_perfcounter1)
hal_lr_perfcounter1:
	jr ra
	mfc0 v0, $25, 3
END(hal_lr_perfcounter1)

/* read CP0 perfCounter1ctl */
LEAF(hal_lr_perfcounter1ctl)
hal_lr_perfcounter1ctl:
	jr ra
	mfc0 v0, $25, 2
END(hal_lr_perfcounter1ctl)

/* read CP0 perfCounter0 */
LEAF(hal_sr_perfcounter0)
hal_sr_perfcounter0:
	jr ra
	mtc0 a0, $25, 1
END(hal_sr_perfcounter0)

/* read CP0 perfCounter0ctl */
LEAF(hal_sr_perfcounter0ctl)
hal_sr_perfcounter0ctl:
	jr ra
	mtc0 a0, $25, 0
END(hal_sr_perfcounter0ctl)

/* read CP0 perfCounter1 */
LEAF(hal_sr_perfcounter1)
hal_sr_perfcounter1:
	jr ra
	mtc0 a0, $25, 3
END(hal_sr_perfcounter1)

/* read CP0 perfCounter1ctl */
LEAF(hal_sr_perfcounter1ctl)
hal_sr_perfcounter1ctl:
	jr ra
	mtc0 a0, $25, 2
END(hal_sr_perfcounter1ctl)

/* read CP0 badinstr */
LEAF(hal_lr_badinstrP)
hal_lr_badinstrP:
	jr ra
	mfc0 v0, $8, 2
END(hal_lr_badinstrP)

/* save CPO root.guestctl1 */
LEAF(hal_sr_guestclt1)
hal_sr_guestclt1:
	jr ra
	mtc0 a0, $10, 4
END(hal_sr_guestclt1)

/* load CPO root.guestctl1 */
LEAF(hal_lr_guestclt1)
hal_lr_guestclt1:
	jr ra
	mfc0 v0, $10, 4
END(hal_lr_guestclt1)

/* load CPO root.guestctl2 */
LEAF(hal_lr_guestclt2)
hal_lr_guestclt2:
	jr ra
	mfc0 v0, $10, 5
END(hal_lr_guestclt2)

/* load from CP0 GuestCtl3 reg */
LEAF(hal_lr_guestctl3)
hal_lr_guestctl3:
	jr ra
	mfc0 v0, $10, 0x6
END(hal_lr_guestctl3)

/* save to CP0 GuestCtl3 reg */
LEAF(hal_sr_guestctl3)
hal_sr_guestctl3:
	jr ra
	mtc0 a0, $10, 0x6
END(hal_sr_guestctl3)

/* save to CP0 GuestCtl2 reg */
LEAF(hal_sr_guestctl2)
hal_sr_guestctl2:
	jr ra
	mtc0 a0, $10, 0x5
END(hal_sr_guestctl2)

/* save to CP0 GTOffset reg */
LEAF(hal_sr_gtoffset)
hal_sr_gtoffset:
	jr ra
	mtc0 a0, $12, 0x7
END(hal_sr_gtoffset)

/* load from CP0 EBase reg */
LEAF(hal_lr_ebase)
hal_lr_ebase:
	jr ra
	mfc0 v0, $15, 0x1
END(hal_lr_ebase)

LEAF(hal_lr_pid)
hal_lr_pid:
	jr ra
	mfc0 v0, $15, 0x0
END(hal_lr_pid)	

LEAF(save_sgpr_ctx)
save_sgpr_ctx:
	.set noat
	rdpgpr	k0, $1
	sw	k0, 0x0(a0)
	rdpgpr	k0, $2
	sw	k0, 0x4(a0)
	rdpgpr	k0, $3
	sw	k0, 0x8(a0)
	rdpgpr	k0, $4
	sw	k0, 0xC(a0)
	rdpgpr	k0, $5
	sw	k0, 0x10(a0)
	rdpgpr	k0, $6
	sw	k0, 0x14(a0)
	rdpgpr	k0, $7
	sw	k0, 0x18(a0)
	rdpgpr	k0, $8
	sw	k0, 0x1C(a0)
	rdpgpr	k0, $9
	sw	k0, 0x20(a0)
	rdpgpr	k0, $10
	sw	k0, 0x24(a0)
	rdpgpr	k0, $11
	sw	k0, 0x28(a0)
	rdpgpr	k0, $12
	sw	k0, 0x2C(a0)
	rdpgpr	k0, $13
	sw	k0, 0x30(a0)
	rdpgpr	k0, $14
	sw	k0, 0x34(a0)
	rdpgpr	k0, $15
	sw	k0, 0x38(a0)
	rdpgpr	k0, $16
	sw	k0, 0x3C(a0)
	rdpgpr	k0, $17
	sw	k0, 0x40(a0)
	rdpgpr	k0, $18
	sw	k0, 0x44(a0)
	rdpgpr	k0, $19
	sw	k0, 0x48(a0)
	rdpgpr	k0, $20
	sw	k0, 0x4C(a0)
	rdpgpr	k0, $21
	sw	k0, 0x50(a0)
	rdpgpr	k0, $22
	sw	k0, 0x54(a0)
	rdpgpr	k0, $23
	sw	k0, 0x58(a0)
	rdpgpr	k0, $24
	sw	k0, 0x5C(a0)
	rdpgpr	k0, $25
	sw	k0, 0x60(a0)
	rdpgpr	k0, $26
	sw	k0, 0x64(a0)
	rdpgpr	k0, $27
	sw	k0, 0x68(a0)
	rdpgpr	k0, $28
	sw	k0, 0x6C(a0)
	rdpgpr	k0, $29
	sw	k0, 0x70(a0)
	rdpgpr	k0, $30
	sw	k0, 0x74(a0)
	rdpgpr	k0, $31
	sw	k0, 0x78(a0)
	mfhi	k0
	sw	k0, 0x7C(a0)
	mflo	k0
	sw	k0, 0x80(a0)
	jr	ra
	nop
	.set at
END(save_sgpr_ctx)


LEAF(restore_sgpr_ctx)
restore_sgpr_ctx:
	.set noat
	lw	k0, 0x0(a0)
	wrpgpr	$1, k0	
	lw	k0, 0x4(a0)
	wrpgpr	$2, k0
	lw	k0, 0x8(a0)
	wrpgpr	$3, k0
	lw	k0, 0xC(a0)
	wrpgpr	$4, k0
	lw	k0, 0x10(a0)
	wrpgpr	$5, k0
	lw	k0, 0x14(a0)
	wrpgpr	$6, k0
	lw	k0, 0x18(a0)
	wrpgpr	$7, k0
	lw	k0, 0x1C(a0)
	wrpgpr	$8, k0
	lw	k0, 0x20(a0)
	wrpgpr	$9, k0
	lw	k0, 0x24(a0)
	wrpgpr	$10, k0
	lw	k0, 0x28(a0)
	wrpgpr	$11, k0
	lw	k0, 0x2C(a0)
	wrpgpr	$12, k0
	lw	k0, 0x30(a0)
	wrpgpr	$13, k0
	lw	k0, 0x34(a0)
	wrpgpr	$14, k0
	lw	k0, 0x38(a0)
	wrpgpr	$15, k0
	lw	k0, 0x3C(a0)
	wrpgpr	$16, k0
	lw	k0, 0x40(a0)
	wrpgpr	$17, k0
	lw	k0, 0x44(a0)
	wrpgpr	$18, k0
	lw	k0, 0x48(a0)
	wrpgpr	$19, k0
	lw	k0, 0x4C(a0)
	wrpgpr	$20, k0
	lw	k0, 0x50(a0)
	wrpgpr	$21, k0
	lw	k0, 0x54(a0)
	wrpgpr	$22, k0
	lw	k0, 0x58(a0)
	wrpgpr	$23, k0
	lw	k0, 0x5C(a0)
	wrpgpr	$24, k0
	lw	k0, 0x60(a0)
	wrpgpr	$25, k0
	lw	k0, 0x64(a0)
	wrpgpr	$26, k0
	lw	k0, 0x68(a0)
	wrpgpr	$27, k0
	lw	k0, 0x6C(a0)
	wrpgpr	$28, k0
	lw	k0, 0x70(a0)
	wrpgpr	$29, k0
	lw	k0, 0x74(a0)
	wrpgpr	$30, k0
	lw	k0, 0x78(a0)
	wrpgpr	$31, k0
	lw	k0, 0x7C(a0)
	mthi	k0
	lw	k0, 0x80(a0)
	mtlo	k0
	jr	ra
	nop
	.set at
END(restore_sgpr_ctx)

LEAF(AsmResetPerformanceCounters)
AsmResetPerformanceCounters:
	.set noreorder
	
	add 		k0,ra,0

	add		a0,zero,zero	
	la		a1,hal_sr_perfcounter0		
	jalr		a1
	nop
	
	li		a0,0x0B	
	la		a1,hal_sr_perfcounter0ctl		
	jalr		a1
	nop
	
	add		a0,zero,zero	
	la		a1,hal_sr_perfcounter1		
	jalr		a1
	nop
	
	li		a0,0x180000B	
	la		a1,hal_sr_perfcounter1ctl		
	jalr		a1
	nop
	
	add		ra,k0,0
	
	jr		ra
	nop
	
	.set reorder
END(AsmResetPerformanceCounters)

LEAF(AsmReadByte)
AsmReadByte:
	.set noreorder
	
	jr    ra
	lb    v0, 0(a0)
		
	.set reorder
END(AsmReadByte)

LEAF(AsmWriteByte)
AsmWriteByte:
	.set noreorder

	jr    ra
	sb    a1, 0(a0)
	
	.set reorder
END(AsmWriteByte)

LEAF(idlevcpu)
idlevcpu:
	.set noreorder
	beq	zero, zero, idlevcpu
	nop
	.set reorder
END(idlevcpu)

LEAF(hal_start_hyper)
hal_start_hyper:
        la k0, exceptionHandler
        mtc0 k0, $14, 0
        la ra, vmmexit
        wrpgpr ra, ra
        wrpgpr sp, sp
        ehb
        eret
        nop
END(hal_start_hyper)


