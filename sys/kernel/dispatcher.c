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

#include <globals.h>
#include <vm.h>
#include <scheduler.h>

void dispatcher(){	
	int i;
	setPreviousShadowSet(curr_vcpu->gprshadowset);
		
	//Check if needs to save GP registers context
	if(vcpu_sgpr[curr_vcpu->gprshadowset] != NULL && vcpu_sgpr[curr_vcpu->gprshadowset] != curr_vcpu){
		save_sgpr_ctx(vcpu_sgpr[curr_vcpu->gprshadowset]->gp_registers);			   
		restore_sgpr_ctx(curr_vcpu->gp_registers);
		vcpu_sgpr[curr_vcpu->gprshadowset]=curr_vcpu;
	}else{
		if(curr_vcpu->init){
                    //FIXME: This line results in critical fault. Must be debugged. 
                    //restore_sgpr_ctx(curr_vcpu->gp_registers);
		}
		vcpu_sgpr[curr_vcpu->gprshadowset]=curr_vcpu;		
	}

	setLowestGShadow(curr_vcpu->gprshadowset);	
	setGuestID(curr_vm->id);
	
	/* Avoid to enter in guest mode for hypervisor tasks. */
	if(curr_vm->id!=0){
		setGuestMode();
	}
	
	//Initialize vcpu
	if(curr_vcpu->init){
		curr_vcpu->cp0_registers[9][0]=getCounter();
		curr_vcpu->init = 0;
	}
}
