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

/* Boot loader for prplHypervisor on qemu-system-mipsel 
 *
 * Based on the work of Dmitry Dunaev to Baikal Electronics.
 *
 */

#include "regdef.h"
#include "../include/launch.h"
#include "../include/p5600.h"
#include "../include/segcfg.h"
#include "../include/addr.h"
#include "asm.h"
#include "../../common/include/mips_cp0.h"

#define DW_UART_THR		0x00
#define DW_UART_DLL		0x00
#define DW_UART_FCR		0x08
#define DW_UART_LCR		0x0C
#define DW_UART_LSR		0x14

	.set	noreorder
	.set	mips0

	.text
LEAF(_start1)
	/* Init state of this core */
	bal      _cpu_init
	nop

	/* Check CPU Number */
	mfc0     a0, $CP0_PRID, 1
	andi     a0, a0, 0x1F
	bgtz     a0, _secondary_cpu
	nop

	/* Init GCR */
	bal      _gcr_init
	nop
	/* Init UART */
	bal       _uart0_init
	nop
	/* Dispaly loader string */
	la       a0, _boot_str
	bal      _uart0_puts
	nop
	/* Power on other cores */
#ifdef POWER_ALL_CORES
	bal _cpu_pwr_on
	nop
#endif
	/* Boot Kernel */
	li	a0, 0
	li	a1, 0
	li	a2, 0
	li	a3, CPHYSADDR(DTB_OFFSET)
	li	t0, KSEG0ADDR(KERNEL_ENTRY)
	jr	t0
	nop
END(_start1)

LEAF(_gcr_init)
#ifdef GCR_BASE
	/* Set base addresses */
	li       t0, KSEG1ADDR(GCR_BASE)
#ifdef GIC_BASE
	li       t1, CPHYSADDR(GIC_BASE) | MIPS_GCR_BLOCK_ENABLE
	sw       t1, MIPS_GCR_GIC_BASE(t0)
#endif /* GIC_BASE */
#ifdef CPC_BASE
	li       t1, CPHYSADDR(CPC_BASE) | MIPS_GCR_BLOCK_ENABLE
	sw       t1, MIPS_GCR_CPC_BASE(t0)
#endif /* CPC_BASE */
#ifdef CUSTOM_BASE
	li       t1, CPHYSADDR(CUSTOM_BASE) | MIPS_GCR_BLOCK_ENABLE
	sw       t1, MIPS_GCR_CUSTOM_BASE(t0)
#endif /* CUSTOM_BASE */
#endif /* GCR_BASE */
	j  ra
	nop
END(_gcr_init)

	.align  2
LEAF(_cpu_init)
	/* Disable counter interrupts */
	mtc0    zero, $CP0_COMPARE
	mtc0    zero, $CP0_COUNT

	li      t0, 0x10400000
	mtc0    t0, $CP0_STATUS

	li      v0, MIPS_LEGACY_SEGCFG0
	mtc0    v0, $C0_SEGCTL0

	li      v0, MIPS_LEGACY_SEGCFG1
	mtc0    v0, $C0_SEGCTL1

	li      v0, MIPS_LEGACY_SEGCFG2
	mtc0    v0, $C0_SEGCTL2

	mfc0    v0, $C0_CONFIG5
	li      v1, 1
	ins     v0, v1, 0, 30
	mtc0    v0, $C0_CONFIG5

	/* Return */
	j  ra
	nop
END(_cpu_init)

	.align 2
LEAF(_cpu_pwr_on)
	mfc0     t0, $CP0_CONFIG, 3
	li       t1, (1 << 29)
	and      t1, t1, t0
	beq      zero, t1, 1f
	nop
#ifdef GCR_BASE
	li       t0, KSEG1ADDR(GCR_BASE)
	/* Load GCR Config */
	lw       t2, MIPS_GCR_CONFIG(t0)
	/* Number of CPU Cores */
	andi     t2, t2, 0xff
	/* Cluster Power Controller base */
	li       t0, KSEG1ADDR(CPC_BASE)
1:
	/* Set Core Other */
	sll      t1, t2, MIPS_CPC_ADDR_SHFT
	sw       t1, (MIPS_CPC_CLCB_BASE + MIPS_CPC_CL_ADDR_OFF)(t0)
	/* Send Power On Command */
	li       t1, MIPS_CPC_CMD_PWRON
	sw       t1, (MIPS_CPC_COCB_BASE + MIPS_CPC_CO_CMD_OFF)(t0)
	/* Next Core (if core is not 0) */
	addi     t2, t2, -1
	/* Init state of this core */
	bgtz     t2, 1b
	nop
#endif /* GCR_BASE */
1:
	/* Return */
	j  ra
	nop

END(_cpu_pwr_on)

	.align  2
LEAF(_uart0_init)

	li t0, KSEG1ADDR(UART_BASE)
	
	li t1, 0x80  				// DLAB -> 1
	sw t1, DW_UART_LCR(t0)


	li t1, 0x0D					// set DLL = 13 -> 115200 for 24 MHz clock
	sw t1, DW_UART_DLL(t0)

	li t1, 0x03  				// DLAB -> 0, 8 data bits
	sw t1, DW_UART_LCR(t0)


	li t1, 0x71  				// enable FIFOs
	sw t1, DW_UART_FCR(t0)
	
	j  ra
	nop

END(_uart0_init)

	.align  2
LEAF(_uart0_puts)

	li   t0, KSEG1ADDR(UART_BASE)
	move t1, a0
	and  t1, CPHYSMASK
	or  t1, KSEG1

1:	lb   t2, (t1)
	beqz t2, 3f
	nop

2:	lw   t3, DW_UART_LSR(t0)
	andi t3, t3, 0x40
	beqz t3, 2b
	nop

	sw   t2, DW_UART_THR(t0)
	add  t1, 1
	b 1b
	nop

3:	j  ra
	nop

END(_uart0_puts)

	.align  2
LEAF(_uart0_dig)

	li   t0, KSEG1ADDR(UART_BASE)
	addi t1, a0, 0x30

	sw   t1, DW_UART_THR(t0)
	li   t1, '\n'
	sw   t1, DW_UART_THR(t0)
	li   t1, '\r'
	sw   t1, DW_UART_THR(t0)

	j  ra
	nop

END(_uart0_dig)

	.align 2
LEAF(_secondary_cpu)

#ifdef DEBUG
	bal     _uart0_dig
	nop
#endif
	/* Disable watch exception */
	mtc0    zero, $CP0_WATCHLO
	mtc0    zero, $CP0_WATCHHI

	/* a0 contains CPU Num */
	move	t0, a0
	sll     t0, t0, LOG2CPULAUNCH
	li      t1, KSEG1ADDR(CPULAUNCH)
	addu    t0, t1

	/* Set CPU online flag */
	lw      t1, LAUNCH_FLAGS(t0)
	or      t1, LAUNCH_FREADY
	sw      t1, LAUNCH_FLAGS(t0)
1:
	/* Get current cycles */
	mfc0    t1,$CP0_COUNT
	addu    t1,LAUNCHPERIOD
2:
	/* Wait some cycles */
	mfc0    t2, $CP0_COUNT
	subu    t2, t1
	bltz    t2, 2b
	nop

	/* Check start enable flag */
	.set mips3
	sync
	.set mips0
	lw      t1, LAUNCH_FLAGS(t0)
	andi    t1, t1, LAUNCH_FGO
	beqz    t1, 1b

	/* Start booting */
	lw      t2, LAUNCH_PC(t0)
	lw      gp, LAUNCH_GP(t0)
	lw      sp, LAUNCH_SP(t0)
	lw      a0, LAUNCH_A0(t0)

	move    a1, zero
	move    a2, zero
	move    a3, zero

	ori     t1, LAUNCH_FGONE
	jr      t2
	sw      t1, LAUNCH_FLAGS(t0)
END(_secondary_cpu)

	.align 2
_boot_str:
	.asciiz  "Baikal boot loader v.1.3 (multi)\n\r"
