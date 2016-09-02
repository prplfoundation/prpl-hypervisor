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

#ifndef _BOOT_H_
#define _BOOT_H_





/* Load gest CP0 registers */
uint32_t hal_lr_guestctl0();

/* Load root CPO registers  */
void hal_sr_guestctl0(uint32_t value);

uint32_t hal_lr_guestctl0Ext();
void hal_sr_guestctl0Ext(uint32_t value);

/* config3 */
uint32_t hal_lr_rconfig3();

/* config  */
uint32_t hal_lr_rconfig();
void hal_sr_rconfig(uint32_t config);

/* debug */
uint32_t hal_lr_rdebug();

/* status */
uint32_t hal_lr_rstatus();
void hal_sr_rstatus(uint32_t status);

/* cause */
int32_t hal_lr_rcause();
void hal_sr_rcause(uint32_t cause);

/* count */
void hal_sr_rcount(uint32_t count);
uint32_t hal_lr_rcount();

/* compare */
void hal_sr_rcompare(uint32_t compare);
uint32_t hal_lr_rcompare();

/* SRSCtl */
uint32_t hal_lr_srsclt();
void hal_sr_srsclt(uint32_t srsclt);

/* guest.SRSCtl */
uint32_t hal_lr_G_srsclt();
void hal_sr_G_srsclt(uint32_t srsclt);

/* SRSMap */
void hal_sr_srsmap(uint32_t srsmap);

/* SRSMap 2 */
void hal_sr_srsmap2(uint32_t srsmpa2);

/* IntCtl */
void hal_sr_intctl(uint32_t intctl);
uint32_t hal_lr_intctl();

/* index */
void hal_sr_index(uint32_t index);
uint32_t hal_lr_index();

/* Random */
uint32_t hal_lr_random();

/* entryLOo */
void hal_sr_entrylo0(uint32_t entrylo0);
uint32_t hal_lr_entrylo0();

/* entryLo1 */
void hal_sr_entrylo1(uint32_t entrylo1);
uint32_t hal_lr_entrylo1();

/* PageMask */
void hal_sr_pagemask(uint32_t pagemask);
uint32_t hal_lr_pagemask();

/* Pagegrain */
void hal_sr_pagegrain(uint32_t pagegrain);
uint32_t hal_lr_pagegrain();

/* entryhi */
void hal_sr_entryhi(uint32_t entryhi);
uint32_t hal_lr_entryhi();

/* tlb commit*/
void hal_tlb_commit();

/* EPC */
void hal_sr_epc(uint32_t epc);
uint32_t hal_lr_epc();

/* hal_sr_guestclt1 */
void hal_sr_guestclt1(uint32_t guestctl1);
uint32_t hal_lr_guestclt1();

/* BadVAddress */
uint32_t hal_lr_badVAaddr();

/* BadInstr */
uint32_t hal_lr_badinstr();

/* BadInstrP */
uint32_t hal_lr_badinstrP();

/* guest guestctl3 */
uint32_t hal_lr_guestctl3();
void hal_sr_guestctl3(uint32_t guestctl3);

/* read from performance counter register'*/
uint32_t hal_lr_pcr();

/* tlb inicialization */
void init_tlb();

/* Patch the addresses 0x80000180 and 0x80000000 aiming to jump 
 * to the hal_int_handler() routine in case of interrupt or exception.  
 */
void hal_intVectorInit();

/* GTOffset */
void hal_sr_gtoffset(int32_t gtoffset);

void hal_sr_guestctl2(uint32_t);

/* read process id COP0 $15*/
uint32_t hal_lr_pid();

/* read CP0 random reg */
uint32_t hal_lr_random();

#endif /* _BOOT_H_ */
