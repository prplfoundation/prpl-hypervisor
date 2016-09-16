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


static const struct vmconf_t const *vmconf = VMCONF;


void delete_vcpu(vcpu_t *vcpu) {
  ll_node_t *a;

  a = (ll_node_t*)((unsigned int)vcpu - sizeof(ll_node_t));

  ll_remove(a);  
  free(a);
}

vm_t *get_vm(unsigned int id) {
  ll_node_t *a;
  
  for(a = virtualmachines.head ; a ; a = a->next)
    if(((vm_t*)a->ptr)->id == id)
      break;
 
  return a ? a->ptr : NULL;
}

void delete_vm(vm_t *dom) {
  ll_node_t *nd;

  nd = (ll_node_t*)((unsigned int)dom - sizeof(ll_node_t));
  ll_remove(nd);
  free(nd);
 
}


/** Initialize the VMs */
void initializeMachines(void) {
	unsigned int i;
	

	Info("Initializing Virtual Machines");

    if(NVMACHINES > 0){
		/* Create initialize the VM structure */
        for(i=0; i<NVMACHINES; i++){
			create_vm(&vmconf[i]);
		}
	}
}


/** Create and initialize the structure of the VM abstraction */ 
vm_t *create_vm(const struct vmconf_t const *vm) {
	static uint32_t vm_id = 1;  /* vm_id is the guestid */	
	static uint32_t tlbindex = 0; 	/* unique tlb entry */

	vm_t *ret;
	ll_node_t* nd;
	uint32_t i;
	vcpu_t *vcpu;
	
	/* number of fix tlb entries */
    uint32_t ntlbent = vm->num_tlb_entries;

	if(!(nd = (ll_node_t*) calloc(1, sizeof(ll_node_t) + sizeof(vm_t))))
		return NULL;
    
	ret = (vm_t*)((unsigned int)nd + sizeof(ll_node_t));
    
	//Memory map
    ret->base_addr = vm->ram_base;
	
	ret->id = vm_id++;  
	ret->os_type = vm->os_type;
	ret->ntlbent = ntlbent;
	ret->init = 1;

	ret->tlbentries = NULL;
        
	/* allocate a tlb entry to the VM */
	if(ret->os_type != IDLEVCPU){
		ret->tlbentries = (struct tlbentry *)calloc(1, sizeof(struct tlbentry)*(ntlbent)); 
		memset(ret->tlbentries, 0, sizeof(struct tlbentry)*ntlbent);
	}else{
		ret->tlbentries = NULL;
		ret->id = 0;
	}
	/* fill the tlb entries to the VM */
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
		ret->tlbentries[i].onhardware = 0;
	}

	/* Set the VM entry Point and scheduler*/
    vcpu = create_vcpu(ret, vm->vm_entry, 0 ,0, 0, vm->os_type);	
	addVcpu_bestEffortList(vcpu);			
	ll_append(&virtualmachines, nd);
	
	nd->ptr = ret;
		
	return ret;
}


void machine_init_vm(vm_t *d) {
  
}

vcpu_t *create_vcpu(vm_t *vm, unsigned int entry_point, unsigned int arg, char* stack_pointer, uint32_t pip, uint32_t ostype){	
	static uint32_t vcpu_id=1;
	static uint32_t shadow_gpr_to_assign = 0;
	uint32_t num_shadow_gprs;
	
	vcpu_t *ret;
	ll_node_t *nd;

	Info("Creating VCPUs");
        
	if(!(nd = (ll_node_t *) calloc(1, sizeof(vcpu_t)+sizeof(ll_node_t))))
		return NULL;

	ret = (vcpu_t*)((unsigned int)nd + sizeof(ll_node_t));
	
	memset(ret, 0, sizeof(vcpu_t));

	//Set vcpu id and gprshadowset
	//ret->gprshadowset = vcpu_id;
	
	num_shadow_gprs = hal_lr_srsclt();
	num_shadow_gprs = (num_shadow_gprs & SRSCTL_HSS) >> SRSCTL_HSS_SHIFT;
	
    if (ostype == IDLEVCPU){
        ret->gprshadowset = num_shadow_gprs;
    }else
        //Highest shadown gpr is used to 
        if(shadow_gpr_to_assign==num_shadow_gprs){
            ret->gprshadowset=shadow_gpr_to_assign-1;
        }else{
            ret->gprshadowset = shadow_gpr_to_assign;
            shadow_gpr_to_assign++;
        }
	
	ret->pip = pip;
    if (ostype == IDLEVCPU){
        ret->id = 0;  
    }else{
        ret->id = vcpu_id;	
        vcpu_id++;
    }
	
	//Not initialized
	ret->init=1;

	/* initilize the VCPU cp0 registers with the guest cp0 status */
	contextSave(ret);
	
	/* Initialize compare and count registers. */
	ret->cp0_registers[9][0] = 0;
	ret->cp0_registers[11][0] = 0;
		
	ret->pc  = entry_point;
	ret->sp  = (uint32_t)stack_pointer;
		
	ret->arg = arg;
		
	//Vm pointer
	ret->vm = vm;
		
	//Adding to local list of vm's vcpus
	nd->ptr = ret;
	ll_append(&(vm->vcpus), nd);

        return ret;
}
