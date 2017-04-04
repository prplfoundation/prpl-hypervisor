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

#ifndef _TLB_H_
#define _TLB_H_

#include "types.h"

#define STATICTLB

/* EntryLo bit flags */
#define ENTRYLO_RI		(1<<31)
#define ENTRYLO_XI		(1<<30)
#define ENTRYLO_D		(1<<2)
#define	ENTRYLO_V		(1<<1)
#define ENTRYLO_G		1

#define ENTRYLO_C_SHIFT		3
#define ENTRYLO_PFN_SHIFT	6
#define PAGEMASK_MASK_SHIFT	13
#define ENTRYHI_VPN2_SHIFT	12 /* Field position on reg is 13, but, shift 11 to div by 2.*/
#define ENTRYHI_MASK		0xFFFFE000

/* Page Mask*/
#define PAGEMASK_4KB		0
#define PAGEMASK_16KB		0x3
#define PAGEMASK_64KB		0xF
#define PAGEMASK_256KB		0x3F
#define PAGEMASK_1MB		0xFF
#define PAGEMASK_4MB		0x3FF
#define PAGEMASK_16MB		0xFFF
#define PAGEMASK_64MB		0x3FFF
#define PAGEMASK_256MB		0xFFFF

#define CACHEABLE       1
#define NONCACHEABLE    0

/* cache coherency attributes */
#define WRITE_BACK 4
#define CWB		5
#define CWBE		4
#define WB		3
#define UNCACHED	2


typedef struct tlbentry{
	uint32_t guestid;
	uint32_t index;
	uint32_t entrylo0;
	uint32_t lo0flags;
	uint32_t entrylo1;
	uint32_t lo1flags;
	uint32_t pagemask;
	uint32_t entryhi;
	uint32_t coherency;
}tlbentry_t;

void tlbEntryWrite(struct tlbentry *entry);
uint32_t tlbCreateEntry(uint32_t address, uint32_t baseaddr, uint32_t size, uint32_t tlbindex, uint32_t use_cache);

#endif
