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
 * This code was written by Carlos Moratelli at Embedded System Group (GSE) at PUCRS/Brazil.
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

/* Read from Guest CP0 */
#define mfgc0(reg, sel) ({ int32_t __value;                      \
        asm volatile (                                          \
        "mfgc0   %0, $%1, %2"                                    \
        : "=r" (__value) : "K" (reg), "K" (sel));               \
        __value; })

/* Write to Guest CP0 */
#define mtgc0(reg, sel, value) asm volatile (                    \
        "mtgc0   %z0, $%1, %2"                                   \
        : : "r" ((uint32_t) (value)), "K" (reg), "K" (sel))

/* write to previous gpr shadow */
#define MoveToPreviousGuestGPR(reg, value) asm volatile (                    \
        "wrpgpr   $%0, %1"                                   \
        : : "K" (reg), "r" ((uint32_t) (value)))

/* read from previous gpr shadow */        
#define MoveFromPreviousGuestGPR(reg) ({ int32_t __value;                      \
        asm volatile (                                          \
        "rdpgpr   %0, $%1"                                    \
        : "=r" (__value) : "K" (reg));               \
        __value; })

/* TBL write */        
#define tlb_commit() asm volatile ("tlbwi" : : )

        
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
#define CP0_KSCRATCH1		30
#define CP0_KSCRATCH2		30
#define CP0_KSCRATCH3		30
#define CP0_DESAVE              31

#define C0_SEGCTL0 		5,	2
#define C0_SEGCTL1 		5,	3
#define C0_SEGCTL2 		5,	4
#define C0_CONFIG5 		16,	5


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
#define CAUSE_RIPL_MASK         0xFC00
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

/* COP0 register definition */
#define CPO_CONFIG              0
#define CPO_CONFIG1             0x01
#define CPO_CONFIG2             0x02
#define CPO_CONFIG3             0x03
#define CPO_CONFIG4             0x04
#define CPO_CONFIG5             0x05
#define CPO_CONFIG7             0x07
#define CP0_STATUS              12


#define CONFIG3_VZ      (1<<23)
#define CONFIG3_VINT    (1<<5)
#define CONFIG3_VEIC    (1<<6)
#define CONFIG3_SP      (1<<4)

#define CONFIG5_K       (1<<30)

#define GUESTCTL0_GM    (1<<31)
#define GUESTCTL0_CP0   (1<<28)
#define GUESTCTL0_GT    (1<<25)
#define GUESTCTL0_CG    (1<<24)
#define GUESTCTL0_CF    (1<<23)
#define GUESTCTL0_G1    (1<<22)
#define GUESTCTL0_DRG   (1<<8)
#define GUESTCTL0_RAD   (1<<9)
#define GUESTCTL0_PIP14 (1<<14)
#define GUESTCTL0_PIP   (0xff<<10)


#define GUESTCTL0EXT_MG  (1<<0)
#define GUESTCTL0EXT_BG  (1<<1)
#define GUESTCTL0EXT_OG  (1<<2)
#define GUESTCTL0EXT_FCD (1<<3)
#define GUESTCTL0EXT_CGI (1<<4)

#define DEBUG_DM        (1<<30)
#define STATUS_IM7      (1<<15)
#define CAUSE_WP        (1<<22)
#define CONFIG_K0        0x7
#define CAUSE_EXECCODE  (0x1F << 2)
#define CAUSE_IP         0x3FF00
#define INTCTL_VS        0x2
#define PAGEGRAIN_ESP   (1<<28)
#define SRSCLT_PSS      (0xF<<6)
#define SRSCLT_HSS      (0xF<<26)

#define CONFIG_K0_UNCACHED      0x2
#define SRSCTL_HSS              (0xF << 26)
#define SRSCTL_PSS              (0xF << 6)
#define GUESTCTL1_RID           (0xFF << 16)
#define GUESTCTL1_ID            0xFF
#define SRSCTL_HSS_SHIFT        26
#define SRSCTL_ESS_SHIFT        12
#define SRSCLT_PSS_SHIFT        6
#define SRSMAP_SSV0_SHIFT       0
#define SRSMAP_SSV1_SHIFT       4
#define SRSMAP_SSV2_SHIFT       8
#define SRSMAP_SSV3_SHIFT       12
#define SRSMAP_SSV4_SHIFT       16
#define SRSMAP_SSV5_SHIFT       20
#define SRSMAP_SSV6_SHIFT       24
#define SRSMAP_SSV7_SHIFT       28
#define SRSMAP2_SSV8_SHIFT      0
#define SRSMAP2_SSV9_SHIFT      4
#define CAUSE_EXECCODE_SHIFT    2
#define CAUSE_IP_SHIFT          8
#define CAUSE_PCI_SHIFT         26
#define CAUSE_IPL_SHIFT         10
#define INTCTL_VS_SHIFT         5
#define STATUS_IM_SHIFT         8
#define GUESTCTL1_RID_SHIFT     16
#define GUESTCLT2_GRIPL_SHIFT   24
#define VIEWIPL_IPL_SHIFT       2

//Hypercall fields
#define HYPCODE  (0x3FF<<11)

#define HYPCODE_SHIFT  11

/* MIPS REGISTERS */
#define REG_ZERO 0
#define REG_AT   1
#define REG_V0   2
#define REG_V1   3
#define REG_A0   4
#define REG_A1   5
#define REG_A2   6
#define REG_A3   7
#define REG_T0   8
#define REG_T1   9
#define REG_T2   10
#define REG_T3   11
#define REG_T4   12
#define REG_T5   13
#define REG_T6   14
#define REG_T7   15
#define REG_S0   16
#define REG_S1   17
#define REG_S2   18
#define REG_S3   19
#define REG_S4   20
#define REG_S5   21
#define REG_S6   22
#define REG_S7   23
#define REG_T8   24
#define REG_T9   25
#define REG_K0   26 
#define REG_K1   27
#define REG_GP   28 
#define REG_SP   29
#define REG_FP   30
#define REG_RA   31

#define HARDWARE_INT_7 		(1<<17)
#define HARDWARE_INT_6 		(1<<16)
#define HARDWARE_INT_5 		(1<<15)
#define HARDWARE_INT_4 		(1<<14)
#define HARDWARE_INT_3 		(1<<13)
#define HARDWARE_INT_2 		(1<<12)
#define HARDWARE_INT_1 		(1<<11)
#define HARDWARE_INT_0 		(1<<10)
#define REQUESTSOFT_INT_1 	(1<<9)
#define REQUESTSOFT_INT_0 	(1<<8)

#define STATUS_HARDWARE_INT_9 		(1<<18)
#define STATUS_HARDWARE_INT_8 		(1<<16)
#define STATUS_HARDWARE_INT_7 		(1<<15)
#define STATUS_HARDWARE_INT_6 		(1<<14)
#define STATUS_HARDWARE_INT_5 		(1<<13)
#define STATUS_HARDWARE_INT_4 		(1<<12)
#define STATUS_HARDWARE_INT_3 		(1<<11)
#define STATUS_HARDWARE_INT_2 		(1<<12)
#define STATUS_HARDWARE_INT_1 		(1<<8)
#define STATUS_HARDWARE_INT_0 		(1<<9)


#define PERFORMANCE_COUNTER_INT (1<<31)

#define TLB_LOAD_FETCH_EXCEPTION    	0x2
#define TLB_STORE_EXCEPTION    		0x3
#define GUEST_EXIT_EXCEPTION   		0x1b
#define GUEST_INSTRUCTION_EMULATION 0
#define GUEST_HYPERCALL 2



#endif 
