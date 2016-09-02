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

#ifndef _HAL_H_
#define _HAL_H_

#include <mips_cp0.h>

#ifdef ETHERNET_SUPPORT
#include <ethernet.h>
#endif 

/* COP0 register definition */
#define CPO_CONFIG		0
#define CPO_CONFIG1		0x01
#define CPO_CONFIG2		0x02
#define CPO_CONFIG3		0x03
#define CPO_CONFIG4		0x04
#define CPO_CONFIG5		0x05
#define CPO_CONFIG7		0x07
#define CP0_STATUS	12



/* CP0 specific fields */
#define CONFIG3_VZ 	(1<<23)
#define CONFIG3_VINT    (1<<5)
#define CONFIG3_VEIC    (1<<6)
#define CONFIG3_SP      (1<<4)

#define GUESTCTL0_GM    (1<<31)
#define GUESTCTL0_CP0   (1<<28)
#define GUESTCTL0_GT    (1<<25)
#define GUESTCTL0_CG    (1<<24)
#define GUESTCTL0_CF    (1<<23)
#define GUESTCTL0_G1    (1<<22)
#define GUESTCTL0_DRG   (1<<8)
#define GUESTCTL0_RAD   (1<<9)
#define GUESTCTL0_PIP14 (1<<14)
#define GUESTCTL0_PIP 	(0xff<<10)


#define GUESTCTL0EXT_MG  (1<<0)
#define GUESTCTL0EXT_BG  (1<<1)
#define GUESTCTL0EXT_OG  (1<<2)
#define GUESTCTL0EXT_FCD (1<<3)
#define GUESTCTL0EXT_CGI (1<<4)

#define DEBUG_DM        (1<<30)
/*#define STATUS_EXL      (1<<1)
#define STATUS_ERL      (1<<2)
#define STATUS_BEV      (1<<22)
#define STATUS_IE        1*/
#define STATUS_IM7      (1<<15)
#define CAUSE_WP        (1<<22)
/*#define CAUSE_IP0       (1<<8)
#define CAUSE_IP1       (1<<9)*/
#define CONFIG_K0        0x7
#define	CAUSE_EXECCODE  (0x1F << 2)
/*#define CAUSE_IV        (1<<23)*/
#define CAUSE_IP         0x3FF00
/*#define CAUSE_BD        (1<<31)
#define CAUSE_TI	(1<<30)*/
#define INTCTL_VS        0x2
#define PAGEGRAIN_ESP   (1<<28)
#define SRSCLT_PSS      (0xF<<6)
#define SRSCLT_HSS	(0xF<<26)




#define CONFIG_K0_UNCACHED	0x2
#define SRSCTL_HSS		(0xF << 26)
#define SRSCTL_PSS		(0xF << 6)
#define GUESTCTL1_RID		(0xFF << 16)
#define GUESTCTL1_ID		0xFF
#define SRSCTL_HSS_SHIFT	26
#define SRSCTL_ESS_SHIFT	12
#define SRSCLT_PSS_SHIFT	6
#define SRSMAP_SSV0_SHIFT	0
#define SRSMAP_SSV1_SHIFT	4
#define SRSMAP_SSV2_SHIFT	8
#define SRSMAP_SSV3_SHIFT	12
#define SRSMAP_SSV4_SHIFT	16
#define SRSMAP_SSV5_SHIFT	20
#define SRSMAP_SSV6_SHIFT	24
#define SRSMAP_SSV7_SHIFT	28
#define SRSMAP2_SSV8_SHIFT	0
#define SRSMAP2_SSV9_SHIFT	4
#define CAUSE_EXECCODE_SHIFT	2
#define CAUSE_IP_SHIFT		8
#define CAUSE_PCI_SHIFT		26
#define CAUSE_IPL_SHIFT         10
#define INTCTL_VS_SHIFT		5
#define STATUS_IM_SHIFT		8
#define GUESTCTL1_RID_SHIFT	16
#define GUESTCLT2_GRIPL_SHIFT   24
#define VIEWIPL_IPL_SHIFT       2

//Hypercall fields
#define HYPCODE  (0x3FF<<11)

#define HYPCODE_SHIFT  11

int32_t main(char * _edata, char* _erodata, char* _data);

/** This rotines should be called just from HAL */
int32_t ConfigureGPRShadow();

/** High level routines. This routines are called from kernel.c */
void setGTOffset(int32_t gtoffset);
int32_t isRootMode(); 
int32_t hasVZ();
int32_t LowLevelProcInit();
uint32_t getCauseCode();
uint32_t getInterruptPending();
void setInterruptMask(uint32_t im);
void clearInterruptMask(uint32_t im);
void enableIE();
uint32_t hasGuestID();
uint32_t isRootASID();
void setStatusReg(uint32_t bits);
void setEPC(uint32_t epc);
void setGuestCTL2(uint32_t guestclt2);
uint32_t has1KPageSupport();
void Disable1KPageSupport();

/** Read/Write on guest gpr shadow */
void MoveToPreviousGuestGPR(uint32_t reg, uint32_t value);
uint32_t MoveFromPreviousGuestGPR(uint32_t reg);
void MoveToGuestGPR(uint32_t gpr_id, uint32_t reg, uint32_t value);
uint32_t MoveFromGuestGPR(uint32_t gpr_id, uint32_t reg);

/** Read/Write on guest guest cp0 */
uint32_t MoveFromGuestCP0(uint32_t reg, uint32_t sel);
void MoveToGuestCP0(uint32_t reg, uint32_t sel, uint32_t value);

/** Set the Guest Lowest GPR Shadow */
void setGLowestGShadow(uint32_t lowestshadow);
uint32_t getGLowestGShadow(void);
uint32_t getGuestID(void);

/** Get root.count */
uint32_t getCounter(void); 
int32_t exceptionHandler();

extern void exception_handler_p(void *data);
extern void save_sgpr_ctx(void* registers_p);
extern void restore_sgpr_ctx(void* registers_p);
extern void AsmWriteByte(uint32_t i, uint8_t v);
extern uint8_t AsmReadByte(uint32_t i);

void idlevcpu();


#endif /* _HAL_H_ */
