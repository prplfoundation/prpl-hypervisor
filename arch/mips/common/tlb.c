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

#include <tlb.h>
#include <types.h>
#include <hypercall_defines.h>
#include <vcpu.h>
#include <globals.h>
#include <mips_cp0.h>
#include <hal.h>



/** Get a random tlb index  */
uint32_t tblGetRandomIndex(){
	return mfc0(CP0_RANDOM, 0);
}

/** Write an entry to the tbl.
	@param index tlb index. The current entry on index will be lost. 
	@param entry the entry to be written.
 */
void tlbEntryWrite(struct tlbentry *entry){

	/* Guest ID to be write by the root */
	setGuestRID(entry->guestid);
	
	/* write index */
	mtc0(CP0_INDEX, 0, entry->index);
	
	/* write entrylo0 */
	mtc0(CP0_ENTRYLO0, 0, (entry->entrylo0 << ENTRYLO_PFN_SHIFT) | entry->lo0flags | (entry->coherency << ENTRYLO_C_SHIFT));

	/* write entrylo1 */
	mtc0(CP0_ENTRYLO1, 0, (entry->entrylo1 << ENTRYLO_PFN_SHIFT) | entry->lo1flags | (entry->coherency << ENTRYLO_C_SHIFT));
	
	/* write pagemask */
	mtc0(CP0_PAGEMASK, 0, entry->pagemask << PAGEMASK_MASK_SHIFT);
	
	/* write entryhi. */
	/* FIXME: Ignoring ASID for now. Is it correct? */
	mtc0(CP0_ENTRYHI, 0, (entry->entryhi << ENTRYHI_VPN2_SHIFT) & ENTRYHI_MASK);
	
	tlb_commit();
	
}

/** Create a temporary tlb entry mapped to non cachable area.
 *  Uses the VM base address as virtual address to map the page.
 *    @param address hypercall address event
 *    @param baseaddr virtual machine base address
 *    @param size guest data size
 *    @param use_cache Determines if the page will be mapped to KSEG1 or KSEG2
 *    @return Mapped address. 
 */
uint32_t tlbCreateEntry(uint32_t address, uint32_t baseaddr, uint32_t size, uint32_t tlbindex, uint32_t use_cache){ 
    struct tlbentry entry;
    uint32_t KSEG = use_cache? 0x80000000 : 0xA0000000;
    
    /* this is a root entry */
    entry.guestid = 0;
    
    /* FIXME: highest tlb entry reserved for hypercalls mapping. Depending on the hardware the highest can be 0xf. */
    entry.index = tlbindex;
    
    entry.entrylo0 = ((baseaddr - 0x80000000) + (address - 0x80000000)) >> 12;
    
    entry.lo0flags = ENTRYLO_V | ENTRYLO_D;
    
    /* check if the address + size exceeds the page limits, maps the next page */
    if((address/0x1000) < ((address+size)/0x1000)){
        entry.entrylo1 = ((baseaddr - 0x80000000) + address + size - 0x80000000) >> 12;
        entry.lo1flags = ENTRYLO_V | ENTRYLO_D;
    }else {
        entry.entrylo1 = 0;
        entry.lo1flags = 0;
    }
    
    entry.pagemask = PAGEMASK_4KB;
    
    entry.entryhi = (baseaddr - KSEG) >> 12;
    
    /*FIXME: using no cached memory to direct access to guest data */
    entry.coherency = 2;
    
    tlbEntryWrite(&entry);
    
    return (address & 0xFFF) + baseaddr - KSEG;
}
