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
 * @file scheduler.c
 * 
 * @section DESCRIPTION
 * 
 * Round-robin scheduler algorithm. The round robin algorithm may be substituted by any other 
 * scheduler policy. The only requirment is that the scheduler routine must set 
 * scheduler_info.vcpu_executing_nd to point to the next VCPU node to be executed. 
 * 
 */

#include <scheduler.h>
#include <vcpu.h>
#include <globals.h>
#include <linkedlist.h>

#define TICKS_BEFORE_SCHEDULING ( QUANTUM_SCHEDULER_MS / (SYSTEM_TICK_US/1000))

struct scheduler_info_t scheduler_info = {NULL, NULL, NULL};

/** 
 * @brief Tick counter. Used for VM's scheduling. 
 */ 
static uint32_t tick_count = 0;

/** 
 * Keeps a pointer to the last scheduled VCPU. 
 */
static uint32_t next_rr = 0;

/**
 * @brief round-robin scheduler implementation. 
 * 
 * @return Pointer to the node in the list of VCPUs of the scheduled VCPU. 
 */
static vcpu_t* round_robin_scheduler(){
	uint32_t current = next_rr;
	
	next_rr = (next_rr + 1) % queue_count(scheduler_info.vcpu_ready_list);
	
	return (vcpu_t*)queue_get(scheduler_info.vcpu_ready_list, current);
}


/**
 * @brief Performs a context-switching to a target VCPU. This is called 
 * from device drivers interrupt context and it allows for a 
 * VM to attend to a interrupt faster. The device driver should called
 * get_fast_int_vcpu_node() to discovery if a VCPU is associated
 * to its interrupts. 
 * 
 * @param target Target VCPU. 
 * 
 */
void fast_interrupt_delivery(vcpu_t *target){
	/* Needs implementation */
}

/**
 * @brief Scheduler routine. Must be invoked in the timer interrupt routine. 
 * 
 */
void run_scheduler(){
	if ( tick_count % TICKS_BEFORE_SCHEDULING == 0){
		contextSave();   
		scheduler_info.vcpu_executing = round_robin_scheduler();
		contextRestore();
	}
	tick_count++;	
}

/**
 * @brief Returns a VCPU corresponding to the id.
 * @param id VCPU id number.
 * @return Pointer to the VCPU. 
 */
vcpu_t* get_vcpu_from_id(uint32_t id){
	uint32_t i;
	struct vcpu_t* vcpu;
	
	for(i=0; i < queue_count(scheduler_info.vcpu_ready_list); i++){
		vcpu = queue_get(scheduler_info.vcpu_ready_list, i);
		if (vcpu->id == id){
			return vcpu;
		}
	}
	
	return NULL;
} 


/**
 * @brief Returns a VCPU associated with a fast interrupt policy. The
 * device drivers must call this routine to discovery if its interrupts
 * are associated with a VCPU. When an interrupt is associated to a VM, 
 * the device driver should call fast_interrupt_delivery() to perform 
 * a context-switching to the target VM during its interrupt handler. 
 * 
 * @param fast_int Target interrupt. 
 *
 * @return Pointer to the target VCPU. The drivers should keep this pointer 
 * to use with the fast_interrupt_delivery() call. 
 *
 */
vcpu_t* get_fast_int_vcpu_node(uint32_t fast_int){
	uint32_t i, j;
	vm_t* vm;
	vcpu_t* v;
	
	for(j=0; j < queue_count(scheduler_info.vcpu_ready_list); j++){
		
		v = (vcpu_t*)queue_get(scheduler_info.vcpu_ready_list, j);
		
		if (!v){
			continue;
		}
		
		vm = (vm_t*)v->vm;
		
		for(i=0;i<vm->vmconf->fast_int_sz;i++){
			if(fast_int == vm->vmconf->fast_interrupts[i]){
				return v;
				
			}
		}
	}
	return NULL;	
} 



