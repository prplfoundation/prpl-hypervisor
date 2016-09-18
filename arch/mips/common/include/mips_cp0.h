/*
 * Copyright (c) 2016, prpl Foundation
 * 
 * Permission to use, copy, modify, and/or distribute this software for any purpose with or without 
 * fee is hereby granted, provided that the above copyright notice and this permission notice appear 
 * in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE 
 * FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * This code was written by Sergio Johann at Embedded System Group (GSE) at PUCRS/Brazil.
 */

#ifndef __MIPS_CP0_H__
#define __MIPS_CP0_H__

/* Read from CP0 */
#define mfc0(reg, sel) ({ int32_t __value;                      \
        asm volatile (                                          \
        "mfc0   %0, $%1, %2"                                    \
        : "=r" (__value) : "K" (reg), "K" (sel));               \
        __value; })

/* Write to CP0 */
#define mtc0(reg, sel, value) asm volatile (                    \
        "mtc0   %z0, $%1, %2"                                   \
        : : "r" ((uint32_t) (value)), "K" (reg), "K" (sel))

#define tlb_commit() asm volatile ("tlbwi" : : )                                   \

        
/* CP0 registers */
#define CP0_INDEX               0
#define CP0_RANDOM              1
#define CP0_ENTRYLO0            2
#define CP0_ENTRYLO1            3
#define CP0_CONTEXT             4
#define CP0_USERLOCAL           4
#define CP0_PAGEMASK            5
#define CP0_PAGEGRAIN           5
#define CP0_WIRED               6
#define CP0_HWRENA              7
#define CP0_BADVADDR            8
#define CP0_COUNT               9
#define CP0_ENTRYHI             10
#define CP0_GUESTCTL1           10
#define CP0_GUESTCTL2           10
#define CP0_GUESTCTL3           10
#define CP0_COMPARE             11
#define CP0_GUESTCLT0EXT        11
#define CP0_STATUS              12
#define CP0_INTCTL              12
#define CP0_SRSCTL              12
#define CP0_SRSMAP              12
#define CP0_SRSMAP2             12
#define CP0_VIEW_IPL            12
#define CP0_SRSMAP2             12
#define CP0_GUESTCTL0           12
#define CP0_GTOOFFSET           12
#define CP0_CAUSE               13
#define CP0_NESTEDEXC           13
#define CP0_VIEW_RIPL           13
#define CP0_EPC                 14
#define CP0_NESTEDEPC           14
#define CP0_PRID                15
#define CP0_EBASE               15
#define CP0_CDMMBASE            15
#define CP0_CONFIG              16
#define CP0_CONFIG1             16
#define CP0_CONFIG2             16
#define CP0_CONFIG3             16
#define CP0_CONFIG4             16
#define CP0_CONFIG5             16
#define CP0_CONFIG7             16
#define CP0_LLADDR              17
#define CP0_WATCHLO             18
#define CP0_WATCHHI             19
#define CP0_DEBUG               23
#define CP0_TRACECONTROL        23
#define CP0_TRACECONTROL2       23
#define CP0_USERTRACEDATA1      23
#define CP0_TRACEBPC            23
#define CP0_DEBUG2              23
#define CP0_DEPC                24
#define CP0_USERTRACEDATA2      24
#define CP0_PERFCTL0            25
#define CP0_PERFCNT0            25
#define CP0_PERFCTL1            25
#define CP0_PERFCNT1            25
#define CP0_ERRCTL              26
#define CP0_TAGLO               28
#define CP0_DATALO              28
#define CP0_ERROREPC            30
#define CP0_DESAVE              31

/* STATUS register */
#define STATUS_CU0              0x10000000
#define STATUS_RP               0x08000000
#define STATUS_RE               0x02000000
#define STATUS_BEV              0x00400000
#define STATUS_SR               0x00100000
#define STATUS_NMI              0x00080000
#define STATUS_IPL(x)           ((x) << 10)
#define STATUS_UM               0x00000010
#define STATUS_ERL              0x00000004
#define STATUS_EXL              0x00000002
#define STATUS_IE               0x00000001

/* CAUSE register */
#define CAUSE_BD                0x80000000
#define CAUSE_TI                0x40000000
#define CAUSE_CE                0x30000000
#define CAUSE_DC                0x08000000
#define CAUSE_IV                0x00800000
#define CAUSE_RIPL(r)           ((r)>>10 & 63)
#define CAUSE_IP1               0x00020000
#define CAUSE_IP0               0x00010000
#define CAUSE_EXC_CODE          0x0000007c

#define CAUSE_Int               0
#define CAUSE_AdEL              (4 << 2)
#define CAUSE_AdES              (5 << 2)
#define CAUSE_IBE               (6 << 2)
#define CAUSE_DBE               (7 << 2)
#define CAUSE_Sys               (8 << 2)
#define CAUSE_Bp                (9 << 2)
#define CAUSE_RI                (10 << 2)
#define CAUSE_CPU               (11 << 2)
#define CAUSE_Ov                (12 << 2)
#define CAUSE_Tr                (13 << 2)

#endif 
