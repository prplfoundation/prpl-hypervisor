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

#include "tlb.h"
#include "types.h"
#include "hypercall_defines.h"
#include <vcpu.h>
#include <globals.h>


/** Get a random tlb index  */
uint32_t tblGetRandomIndex(){
	return hal_lr_random();
}

/** Write an entry to the tbl.
	@param index tlb index. The current entry on index will be lost. 
	@param entry the entry to be written.
 */
void tlbEntryWrite(struct tlbentry *entry){

	/* Guest ID to be write by the root */
	setGuestRID(entry->guestid);
	
	/* write index */
	hal_sr_index(entry->index);
	
	/* write entrylo0 */
	hal_sr_entrylo0((entry->entrylo0 << ENTRYLO_PFN_SHIFT) | entry->lo0flags | (entry->coherency << ENTRYLO_C_SHIFT));

	/* write entrylo1 */
	hal_sr_entrylo1((entry->entrylo1 << ENTRYLO_PFN_SHIFT) | entry->lo1flags | (entry->coherency << ENTRYLO_C_SHIFT));
	
	/* write pagemask */
	hal_sr_pagemask(entry->pagemask << PAGEMASK_MASK_SHIFT);
	
	/* write entryhi. */
	/* FIXME: Ignoring ASID for now. Is it correct? */
	hal_sr_entryhi((entry->entryhi << ENTRYHI_VPN2_SHIFT) & ENTRYHI_MASK);
	
	hal_tlb_commit();
	
}

/** Create a temporary tlb entry for hypercall
 *  Uses the VM base address as virtual address to map the page.
	@param address hypercall address event
	@param baseaddr virtual machine base address
	@param size guest data size
*/
uint32_t tlbCreateEntry(uint32_t address, uint32_t baseaddr, uint32_t size, uint32_t tlbindex){	
	struct tlbentry entry;
	
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
	
	entry.entryhi = (baseaddr - 0x80000000) >> 12;

	/*FIXME: using no cached memory to direct access to guest data */
	entry.coherency = 2;
	
	tlbEntryWrite(&entry);
	
	return (address & 0xFFF) + baseaddr - 0x80000000;
}

uint32_t TLBDynamicMap(vcpu_t* vcpu){
	uint32_t VMba = 0, VMsize;
	struct tlbentry entry;
	uint32_t badVAddr = getBadVAddress();
	uint32_t page, i;
	memVMMap_t *vmmap;
	
	/* TLB map for idle vcpu. The processor will execute in root-user mode. */
	if(vcpu == idle_vcpu){
		entry.guestid = 0;

		entry.index = getRandom();
	
		page = (badVAddr) >> 12;
		if(page & 0x1){
			entry.entrylo1 = page;
			entry.lo1flags = ENTRYLO_V | ENTRYLO_D;
			entry.entrylo0 = 0;
			entry.lo0flags = 0;
		}else{
			entry.entrylo1 = 0;
			entry.lo1flags = 0;
			entry.entrylo0 = page;
			entry.lo0flags = ENTRYLO_V | ENTRYLO_D;
		}
	
		entry.pagemask = PAGEMASK_4KB;
	
		entry.entryhi = (badVAddr >> 12);
		
		entry.coherency = 0;

	}else{
	
		if(!vcpu || !vcpu->vm || !vcpu->vm->vmmap){
			Warning("TLB Dynamic Mapping failed for VCPU %d", vcpu->id);
			return ERROR;
		}
	
		VMsize = vcpu->vm->size;
		vmmap = vcpu->vm->vmmap;
	
		for(i=0; i<vcpu->vm->ntlbent; i++){
			if(badVAddr > vmmap[i].vGuestBase && badVAddr < vmmap[i].vGuestBase + vmmap[i].size)
				break;
		}
	
		if(i == vcpu->vm->ntlbent){
			Warning("TLB Dynamic Mapping failed for VCPU %d due to out of bound address.", vcpu->id);
			return ERROR;
		}
	
		entry.guestid = vcpu->vm->id;

		entry.index = getRandom();
	
		page = ((vmmap[i].phyGuestBase - vmmap[i].vGuestBase)+ badVAddr) >> 12;
	
		entry.entrylo0 = page & ~0x1;
		entry.lo0flags = ENTRYLO_V | ENTRYLO_D;
	
		/* check if the address + size exceeds the page limits, maps the next page */
		if((vmmap[i].phyGuestBase + vmmap[i].size < vmmap[i].phyGuestBase + vmmap[i].size + 0x1000)){
			entry.entrylo1 = page | 1;
			entry.lo1flags = ENTRYLO_V | ENTRYLO_D;	
		}else {
			entry.entrylo1 = 0;
			entry.lo1flags = 0;
		}
	
		entry.pagemask = PAGEMASK_4KB;
	
		entry.entryhi = (badVAddr >> 12) & ~0x1;

		/*FIXME: using no cached memory to direct access to guest data */
		entry.coherency = vmmap[i].coherency;
	}
	
		tlbEntryWrite(&entry);
	
	
}

#if 0
uint32_t TLBDynamicMap(vcpu_t* vcpu){
	uint32_t VMba, VMsize;
	struct tlbentry entry;
	uint32_t badVAddr = getBadVAddress();
	uint32_t page;
	
	if(!vcpu || !vcpu->vm){
		Warning("TLB Dynamic Mapping failed for VCPU %d", vcpu->id);
		return ERROR;
	}
	
	VMba = vcpu->vm->base_addr;
	VMsize = vcpu->vm->size;
	
	if( (badVAddr < VMba - 0x10000000) || (badVAddr > (VMba - 0x10000000) + VMsize)){
		Warning("TLB Dynamic Mapping failed for VCPU %d due to out of baunds address.", vcpu->id);
		return ERROR;
	}
	
	entry.guestid = vcpu->vm->id;

	entry.index = getRandom();
	
	page = ((vcpu->vm->base_addr - 0x80000000) + (badVAddr - 0x80000000)) >> 12;
	//page = (epc - 0x80000000) >> 12;
	
	entry.entrylo0 = page & ~0x1;
	entry.lo0flags = ENTRYLO_V | ENTRYLO_D;
	
	/* check if the address + size exceeds the page limits, maps the next page */
	if((VMba - 0x10000000 + VMsize < VMba - 0x10000000 + VMsize + 0x1000)){
		entry.entrylo1 = page | 1;
		entry.lo1flags = ENTRYLO_V | ENTRYLO_D;
	}else {
		entry.entrylo1 = 0;
		entry.lo1flags = 0;
	}
	
	entry.pagemask = PAGEMASK_4KB;
	
	entry.entryhi = ((badVAddr - 0x80000000) & ~0xFFF) >> 12;

	/*FIXME: using no cached memory to direct access to guest data */
	entry.coherency = 0;
	
	tlbEntryWrite(&entry);
	
	
}
#endif