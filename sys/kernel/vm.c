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
 * @file vcpu.c
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
 * @brief Responsable by the VM's data initialization.
 * Called once during the hypervisor's startup. 
 */
void initializeMachines(void) {
	unsigned int i;

	INFO("Initializing Virtual Machines.");

	scheduler_info.vcpu_ready_list = NULL;
	
	scheduler_info.virtual_machines_list = NULL;
	
	if(NVMACHINES > 0){
		for(i=0; i<NVMACHINES; i++){
			/* VMCONF is automatically generated and it can be found at the config.h file. 
			*   It is read during hypervisor startup for the VM and VCPU's configuration. 
			*/
			create_vm(&VMCONF[i]);
		}
	}else{
		WARNING("There is no VM configuration. ");
	}
}


/**
 * @brief Create and initialize the data structure for a VM. 
 * Use the VM's data information from VMCONF.
 * @param vm Array of initial VM's configuration. See VMCONF at config.h file.
 * @return Pointer to the VM data structure. 
 */
vm_t *create_vm(const struct vmconf_t const *vm_conf) {
	static uint32_t vm_id = 1;  /* vm_id is the used as guestid */	
	static uint32_t tlbindex = 0; 	/* unique tlb entry */
	uint32_t entry_point = 0;

	vm_t *vm;
	uint32_t i;
	vcpu_t *vcpu;
	
	INFO("Configuring %s VM starting at 0x%x RAM address.", vm_conf->vm_name, vm_conf->ram_base);
    
	/* Number of fix TLB entries */
	uint32_t ntlbent = vm_conf->num_tlb_entries;
	
	vm = (vm_t*)malloc(sizeof(vm_t));
	
	vm->vcpus = NULL;
	
	vm->vmconf = vm_conf;
	
	vm->vm_name = vm_conf->vm_name;
	
	vm->base_addr = vm_conf->ram_base;
	
	vm->id = vm_id++;  
	vm->os_type = vm_conf->os_type;
	vm->ntlbent = ntlbent;
	vm->init = 1;

	vm->tlbentries = NULL;
	
	/* Allocate a TLB entry to the VM */
	vm->tlbentries = (struct tlbentry *)malloc(sizeof(struct tlbentry)*(ntlbent)); 
	memset(vm->tlbentries, 0, sizeof(struct tlbentry)*ntlbent);

	/* Fill the TLB entries to the VM */
	for(i=0; i<ntlbent; i++, tlbindex++){
		vm->tlbentries[i].guestid = vm->id;
		vm->tlbentries[i].index = tlbindex;
		vm->tlbentries[i].entrylo0 = vm_conf->tlb[i].entrylo0;
		if(vm->tlbentries[i].entrylo0) 
			vm->tlbentries[i].lo0flags = ENTRYLO_V | ENTRYLO_D;
		vm->tlbentries[i].entrylo1 = vm_conf->tlb[i].entrylo1;
		if(vm->tlbentries[i].entrylo1)
			vm->tlbentries[i].lo1flags = ENTRYLO_V | ENTRYLO_D;
		vm->tlbentries[i].pagemask = vm_conf->tlb[i].pagemask;
		vm->tlbentries[i].entryhi = vm_conf->tlb[i].entryhi;
		vm->tlbentries[i].coherency = vm_conf->tlb[i].coherency;
		/* Write TLB on hardware */
		tlbEntryWrite(&vm->tlbentries[i]);
	}

	if(vm->os_type == BARE_METAL){
		entry_point = BARE_METAL_ENTRY_POINT;
	}else{
		WARNING("OS type not defined.");
	}
	
	/* Set the VM entry Point and scheduler*/
	vcpu = create_vcpu(vm, entry_point);	
	
	list_append(&scheduler_info.vcpu_ready_list, vcpu);
	list_append(&scheduler_info.virtual_machines_list, vm);
	list_append(&vm->vcpus, vcpu);
	
	return vm;
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
	
	vcpu_t *vcpu;
	
	vcpu = (vcpu_t*)malloc(sizeof(vcpu_t));

	memset(vcpu, 0, sizeof(vcpu_t));

	num_shadow_gprs = mfc0(CP0_SRSCTL, 2);
	num_shadow_gprs = (num_shadow_gprs & SRSCTL_HSS) >> SRSCTL_HSS_SHIFT;
	
	/* Highest shadown gpr is used to the hypervisor */
	if(shadow_gpr_to_assign==num_shadow_gprs){
		vcpu->gprshadowset=shadow_gpr_to_assign-1;
	}else{
		vcpu->gprshadowset = shadow_gpr_to_assign;
		shadow_gpr_to_assign++;
	}
	
	vcpu->id = vcpu_id;	
	vcpu_id++;
	
	/* Mark VCPU as not initialized */
	vcpu->init=1;

	/* Initialize compare and count registers. */
	vcpu->cp0_registers[9][0] = 0;
	vcpu->cp0_registers[11][0] = 0;
		
	vcpu->pc  = entry_point;

	/* Point to the VM owner */
	vcpu->vm = vm;
		
	return vcpu;
}
