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


uint32_t vmconf[][VMCONF_NUMCOLUNS] = VMCONF;
uint32_t vmconf_rt[][VMCONF_NUMCOLUNS] = VMCONF_RT;

uint32_t idleconf[][VMCONF_NUMCOLUNS] = {0, 0, 0, IDLEVCPU, 0, (uint32_t)idlevcpu}; 

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

  //free((void*)dom->base_addr);
  nd = (ll_node_t*)((unsigned int)dom - sizeof(ll_node_t));
  ll_remove(nd);
  free(nd);
 
 // flush_tlb();
  
}

/* Cria mapeapmento das VM com base em dados carregados na memoria pelo loader. */
void inMemoryLoad(){
	static uint32_t *confaddr = (uint32_t*)0x80300000;
	
	/* Carrega configura a partir do endereço 0x80300000 */
	while(*confaddr){
		HF_Template[0][0] = *confaddr;
		HF_Template[1][0] = TLBIndex;
		HF_Template[1][1] = *confaddr/0x1000;
		HF_Template[1][2] = *confaddr/0x1000+0x10;
		HF_Template[2][0] = TLBIndex;

		switch ( *(confaddr+1)){
			case HELLFIRE:
				HF_Template[0][3] = HELLFIRE;
				break;
			case BAREOS:
				HF_Template[0][3] = BAREOS;
				break;
			case BAREOS_RT:
				HF_Template[0][3] = BAREOS_RT;
				break;
			default:
				Warning("Undentified VM 0x%x.", *(confaddr+1));
		}
		create_vm(HF_Template);
		confaddr += 2;
		TLBIndex++;
	}
}

/** Initialize the RT VMs */
initializeRTMachines(void){
	unsigned int i;

	Info("Initializing RT Virtual Machines");
	if(vmconf_rt[0][0] != 0){
		/* Create initialize the VM structure */
		for(i=0; vmconf_rt[i][0]; i+=vmconf_rt[i][2]+1){
			create_vm(&vmconf_rt[i]);
		}	
	}else{
		//inMemoryLoad();
	}
}

/** Initialize the VMs */
void initializeMachines(void) {
	unsigned int i;
	

	Info("Initializing Virtual Machines");
	
	/* Pode carregar a tabela de VMs a partir do arquivo config.h ou de configuração carregada na memória a partir do endereço 0x80000500 */
	if(vmconf[0][0] != 0){
		/* Create initialize the VM structure */
		for(i=0; vmconf[i][0]; i+=vmconf[i][2]+1){
			create_vm(&vmconf[i]);
		}
	}else{
	//	inMemoryLoad();
	}
    
	create_vm(idleconf);
}


/** Create and initialize the structure of the VM abstraction */ 
vm_t *create_vm(uint32_t vm[][VMCONF_NUMCOLUNS]) {
	static uint32_t vm_id = 1;  /* vm_id is the guestid */	
	static uint32_t tlbindex = 0; 	/* unique tlb entry */

	vm_t *ret;
	ll_node_t* nd;
	uint32_t i;
	vcpu_t *vcpu;
	
	/* number of fix tlb entries */
	uint32_t ntlbent = vm[0][2];

	if(!(nd = (ll_node_t*) calloc(1, sizeof(ll_node_t) + sizeof(vm_t))))
		return NULL;
    
	ret = (vm_t*)((unsigned int)nd + sizeof(ll_node_t));
    
	//Memory map
	ret->base_addr = vm[0][0];
	ret->size = vm[0][1];
	
	ret->id = vm_id++;  
	ret->os_type = vm[0][3];
	ret->ntlbent = ntlbent;
	ret->init = 1;

	ret->tlbentries = NULL;
        
#ifdef STATICTLB	
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
		ret->tlbentries[i].guestid = vm[i+1][0];
		ret->tlbentries[i].index = tlbindex;
		ret->tlbentries[i].entrylo0 = vm[i+1][1];
		if(ret->tlbentries[i].entrylo0) 
			ret->tlbentries[i].lo0flags = ENTRYLO_V | ENTRYLO_D;
		ret->tlbentries[i].entrylo1 = vm[i+1][2];
		if(ret->tlbentries[i].entrylo1)
			ret->tlbentries[i].lo1flags = ENTRYLO_V | ENTRYLO_D;
		ret->tlbentries[i].pagemask = vm[i+1][3];
		ret->tlbentries[i].entryhi = vm[i+1][4];
		ret->tlbentries[i].coherency = vm[i+1][5];
		ret->tlbentries[i].onhardware = 0;
	}
#else	
	if(ret->os_type != IDLEVCPU){
		ret->vmmap = (memVMMap_t *)calloc(1, sizeof(memVMMap_t)*(ntlbent)); 
		memset(ret->vmmap, 0, sizeof(memVMMap_t)*ntlbent);
	}else{
		ret->vmmap = NULL;
		ret->id = 0;
	}

	/* fill the tlb entries to the VM */
	for(i=0; i<ntlbent; i++, tlbindex++){
		ret->vmmap[i].phyGuestBase = vm[i+1][1];
		ret->vmmap[i].vGuestBase = vm[i+1][4];
		ret->vmmap[i].size = vm[i+1][2];
		ret->vmmap[i].coherency = vm[i+1][5];
	}
#endif	
	/* Set the VM entry Point and scheduler*/
	switch(ret->os_type){
		case BAREOS:
			vcpu = create_vcpu(ret, 0x80000200, 0 ,0, vm[0][4], BAREOS);	
			addVcpu_bestEffortList(vcpu);			
			ll_append(&virtualmachines, nd);
			break;
		case GENERIC: 	
			vcpu = create_vcpu(ret, 0x80000000, 0, 0, vm[0][4], GENERIC);
			addVcpu_bestEffortList(vcpu);
			ll_append(&virtualmachines, nd);
			break;
		case HELLFIRE:
			vcpu = create_vcpu(ret, vm[0][5], 0, 0, vm[0][4], HELLFIRE);
			addVcpu_bestEffortList(vcpu);
			ll_append(&virtualmachines, nd);
			break;
		case BAREOS_RT:
			vcpu = create_vcpu(ret, 0x801000f4, 0, 0, vm[0][4], BAREOS_RT);
			addVcpu_servicesInitList(vcpu);		
			ll_append(&virtualmachines_rt, nd);
			break;
		case LINUX:
			vcpu = create_vcpu(ret, vm[0][5], 0, 0, vm[0][4], LINUX);
			addVcpu_bestEffortList(vcpu);		
			ll_append(&virtualmachines, nd);
			break;
		case IDLEVCPU:
			vcpu = create_vcpu(ret, vm[0][5], 0, 0, vm[0][4], IDLEVCPU);
			idle_vcpu = vcpu;		
			ll_append(&virtualmachines, nd);
			break;
			
		default:
			Warning("OS type 0x%x supported!\n", ret->os_type);
			break;
	}
	
	nd->ptr = ret;
		
	return ret;
}


void machine_init_vm(vm_t *d) {
  
}

vcpu_t *create_vcpu(vm_t *vm, unsigned int entry_point, unsigned int arg, char* stack_pointer, uint32_t pip, uint32_t ostype){	
	static uint32_t vcpu_id=0;
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
	ret->id = vcpu_id;	
	vcpu_id++;
	
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
