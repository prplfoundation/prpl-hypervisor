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

/**
 * @file vm.c
 * 
 * @section DESCRIPTION
 * 
 * Create the VM and VCPU's data controle structures used internally by the hypervisor 
 * to manage them. 
 * 
 * At this moment, a VM is associated with only one VCPU. 
 */

#include <vm.h>
#include <hal.h>
#include <scheduler.h>
#include <libc.h>
#include <vcpu.h>
#include <config.h>
#include <malloc.h>
#include <tlb.h>
#include <types.h>
#include <globals.h>
#include <mips_cp0.h>


/**
 * @brief Remove a VCPU from the VCPU's list. 
 * @param vcpu Pointer to the VCPU to be removed. 
 */
void delete_vcpu(vcpu_t *vcpu) {
  ll_node_t *a;

  a = (ll_node_t*)((unsigned int)vcpu - sizeof(ll_node_t));

  ll_remove(a);  
  free(a);
}

/**
 * @brief Get a VM based on its ID number. 
 * @param id Identification number.
 * @return A pointer the the VM data structure or NULL if not found. 
 */
vm_t *get_vm(unsigned int id) {
  ll_node_t *a;
  
  for(a = virtualmachines.head ; a ; a = a->next)
    if(((vm_t*)a->ptr)->id == id)
      break;
 
  return a ? a->ptr : NULL;
}

/**
 * @brief Remove a VM from the VM's list.
 * @param vm Pointer to the VM to be removed. .
 */
void delete_vm(vm_t *vm) {
  ll_node_t *nd;

  nd = (ll_node_t*)((unsigned int)vm - sizeof(ll_node_t));
  ll_remove(nd);
  free(nd);
}


/**
 * @brief Responsable by the VM's data initialization.
 * Called once during the hypervisor's startup. 
 */
void initializeMachines(void) {
	unsigned int i;

	Info("Initializing Virtual Machines");

    if(NVMACHINES > 0){
        for(i=0; i<NVMACHINES; i++){
            /* VMCONF is automatically generated and it can be found at the config.h file. 
             *   It is read during hypervisor startup for the VM and VCPU's configuration. 
             */
            create_vm(&VMCONF[i]);
		}
	}else{
        Warning("There is no VM configuration. ");
    }
}


/**
 * @brief Create and initialize the data structure for a VM. 
 * Use the VM's data information from VMCONF.
 * @param vm Array of initial VM's configuration. See VMCONF at config.h file.
 * @return Pointer to the VM data structure. 
 */
vm_t *create_vm(const struct vmconf_t const *vm) {
	static uint32_t vm_id = 1;  /* vm_id is the used as guestid */	
	static uint32_t tlbindex = 0; 	/* unique tlb entry */

	vm_t *ret;
	ll_node_t* nd;
	uint32_t i;
	vcpu_t *vcpu;
    
	/* Number of fix TLB entries */
    uint32_t ntlbent = vm->num_tlb_entries;

	if(!(nd = (ll_node_t*) calloc(1, sizeof(ll_node_t) + sizeof(vm_t))))
		return NULL;
    
	ret = (vm_t*)((unsigned int)nd + sizeof(ll_node_t));
    ret->base_addr = vm->ram_base;
	
	ret->id = vm_id++;  
	ret->os_type = vm->os_type;
	ret->ntlbent = ntlbent;
	ret->init = 1;

	ret->tlbentries = NULL;
        
	/* Allocate a TLB entry to the VM */
	ret->tlbentries = (struct tlbentry *)calloc(1, sizeof(struct tlbentry)*(ntlbent)); 
	memset(ret->tlbentries, 0, sizeof(struct tlbentry)*ntlbent);

    /* Fill the TLB entries to the VM */
	for(i=0; i<ntlbent; i++, tlbindex++){
        ret->tlbentries[i].guestid = ret->id;
		ret->tlbentries[i].index = tlbindex;
		ret->tlbentries[i].entrylo0 = vm->tlb[i].entrylo0;
		if(ret->tlbentries[i].entrylo0) 
			ret->tlbentries[i].lo0flags = ENTRYLO_V | ENTRYLO_D;
        ret->tlbentries[i].entrylo1 = vm->tlb[i].entrylo1;
		if(ret->tlbentries[i].entrylo1)
			ret->tlbentries[i].lo1flags = ENTRYLO_V | ENTRYLO_D;
        ret->tlbentries[i].pagemask = vm->tlb[i].pagemask;
        ret->tlbentries[i].entryhi = vm->tlb[i].entryhi;
        ret->tlbentries[i].coherency = vm->tlb[i].coherency;
        /* Write TLB on hardware */
        tlbEntryWrite(&ret->tlbentries[i]);
	}

	/* Set the VM entry Point and scheduler*/
    vcpu = create_vcpu(ret, vm->vm_entry);	
	addVcpu_bestEffortList(vcpu);			
	ll_append(&virtualmachines, nd);
	
	nd->ptr = ret;
		
	return ret;
}


/**
 * @brief Create and initialize the data structure for a VCPU. 
 * @param vm VM that is associated to this VCPU. 
 * @param entry_point VCPU initial program counter address.
 * @return Pointer to the VM data structure. 
 */
vcpu_t *create_vcpu(vm_t *vm, unsigned int entry_point){	
	static uint32_t vcpu_id=1;
	static uint32_t shadow_gpr_to_assign = 0;
	uint32_t num_shadow_gprs;
	
	vcpu_t *ret;
	ll_node_t *nd;

	if(!(nd = (ll_node_t *) calloc(1, sizeof(vcpu_t)+sizeof(ll_node_t))))
		return NULL;

	ret = (vcpu_t*)((unsigned int)nd + sizeof(ll_node_t));
	
	memset(ret, 0, sizeof(vcpu_t));

	num_shadow_gprs = mfc0(CP0_SRSCTL, 2);
	num_shadow_gprs = (num_shadow_gprs & SRSCTL_HSS) >> SRSCTL_HSS_SHIFT;
	
    /* Highest shadown gpr is used to the hypervisor */
    if(shadow_gpr_to_assign==num_shadow_gprs){
        ret->gprshadowset=shadow_gpr_to_assign-1;
    }else{
        ret->gprshadowset = shadow_gpr_to_assign;
        shadow_gpr_to_assign++;
    }
	
    ret->id = vcpu_id;	
    vcpu_id++;
	
	/* Mark VCPU as not initialized */
	ret->init=1;

	/* Initialize the VCPU with the default state of the Guest CP0 */
	contextSave(ret);
	
	/* Initialize compare and count registers. */
	ret->cp0_registers[9][0] = 0;
	ret->cp0_registers[11][0] = 0;
		
	ret->pc  = entry_point;

    /* Point to the VM owner */
	ret->vm = vm;
		
	/* Adding to a global list of VCPUs */
	nd->ptr = ret;
	ll_append(&(vm->vcpus), nd);

    return ret;
}
