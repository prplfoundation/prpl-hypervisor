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

/*
 *  Baikal-T (P5600) core system blocks definitions
 *  (c)2014 Baikal Electronics OJSC
 *
 */

#ifndef _ASM_P5600_H
#define _ASM_P5600_H

/* Offsets from the GCR base address to various control blocks */
#define MIPS_GCR_CONFIG		0x00
#define MIPS_GCR_SELF_BASE	0x08
#define MIPS_GCR_CUSTOM_BASE	0x60
#define MIPS_GCR_GIC_BASE	0x80
#define MIPS_GCR_CPC_BASE	0x88
/* Block enable flags */
#define MIPS_GCR_BLOCK_ENABLE	0x01

/* Offsets from the CPC base address to various control blocks */
#define MIPS_CPC_GCB_BASE	0x0000
#define MIPS_CPC_CLCB_BASE	0x2000
#define MIPS_CPC_COCB_BASE	0x4000
/* CPC Core Local Block offsets */
#define MIPS_CPC_CL_CMD_OFF	0x0000
#define MIPS_CPC_CL_STATUS_OFF	0x0008
#define MIPS_CPC_CL_ADDR_OFF	0x0010
/* CPC Core Other Block offsets */
#define MIPS_CPC_CO_CMD_OFF	0x0000
#define MIPS_CPC_CO_STATUS_OFF	0x0008
#define MIPS_CPC_CO_ADDR_OFF	0x0010
/* CPC Blocks Commands */
#define MIPS_CPC_CMD_CLKOFF	1
#define MIPS_CPC_CMD_PWROFF	2
#define MIPS_CPC_CMD_PWRON	3
#define MIPS_CPC_CMD_RESET	4
/* CPC Core Other address shift */
#define MIPS_CPC_ADDR_SHFT	16

/*Global Interrupt Controler Registers */
#define P5600_GIC_BASE(a)        *(volatile unsigned*) (0xBBDC0000 + a)

#define REG_MASK_OFFSET(int) 	((int/32) * 4)
#define INT_BIT(int)		(1<<(int%32))

#define GIC_SH_CONFIG           P5600_GIC_BASE(0)
#define GIC_SH_COUNTERLO        P5600_GIC_BASE(0x10)
#define GIC_SH_COUNTERHI        P5600_GIC_BASE(0x14)
#define GIC_SH_REV              P5600_GIC_BASE(0x20)
#define GIC_SH_GID_CONFIG31_0  	P5600_GIC_BASE(0x080)
#define GIC_SH_GID_CONFIG63_32  P5600_GIC_BASE(0x084)

#define GIC_SH_RMASK(int)	(P5600_GIC_BASE(0x300 + REG_MASK_OFFSET(int)) = INT_BIT(int))
#define GIC_SH_SMASK(int)	(P5600_GIC_BASE(0x380 + REG_MASK_OFFSET(int)) = INT_BIT(int))
#define GIC_SH_MASK(int)	(P5600_GIC_BASE(0x400 + REG_MASK_OFFSET(int)) & INT_BIT(int))
#define GIC_SH_PEND		(P5600_GIC_BASE(0x480 + REG_MASK_OFFSET(int)) & INT_BIT(int))
#define GIC_SH_MAP_PIN(int)	P5600_GIC_BASE(0x500 + (int * 4))
#define GIC_SH_MAP_CORE(int)	P5600_GIC_BASE(0x2000 + (int * 0x20))
#define GIC_SH_POL_HIGH(int)	(P5600_GIC_BASE(0x100 + REG_MASK_OFFSET(int)) |= INT_BIT(int))
#define GIC_SH_POL_LOW(int)	(P5600_GIC_BASE(0x100 + REG_MASK_OFFSET(int)) &= (~INT_BIT(int)))


/*Global Interrupt Controler Registers Bits */
#define GIC_SH_CONFIG_COUNTSTOP (1<<28)
#define GIC_SH_CONFIG_VZE	(1<<30)


/*Global Control Block Registers */
#define P5600_GCB_BASE(a)       *(volatile unsigned*) (0xBFBF8000 + a)
#define GCB_GIC                 P5600_GCB_BASE(0x80)

/* Global Control Block Register Bits */
#define GCR_GIC_EN              1

/* Core-Local Register Map */
#define P5600_GIC_CL_BASE(a)        *(volatile unsigned*) (0xBBDC8000 + a)
#define GIC_CL_COREi_CTL            P5600_GIC_CL_BASE(0x00)
#define GIC_CL_COREi_PEND           P5600_GIC_CL_BASE(0x04)
#define GIC_CL_COREi_MASK           P5600_GIC_CL_BASE(0x08)
#define GIC_CL_COREi_RMASK          P5600_GIC_CL_BASE(0x0C)
#define GIC_CL_COREi_SMASK          P5600_GIC_CL_BASE(0x10)
#define GIC_CL_COREi_COMPARE_MAP    P5600_GIC_CL_BASE(0x44) /*COUNTER/COMPARE*/
#define GIC_CL_COREi_TIMER_MAP      P5600_GIC_CL_BASE(0x48)
#define GIC_CL_COMPARELO            P5600_GIC_CL_BASE(0xA0)
#define GIC_CL_COMPAREHI            P5600_GIC_CL_BASE(0xA4) 
#define GIC_CL_ID                   P5600_GIC_CL_BASE(0x88)

/* Core-Local Registers Bits */
#define GIC_CL_TIMER_MASK       0x4


#endif /* _ASM_P5600_H */
