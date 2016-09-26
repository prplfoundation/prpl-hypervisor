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

#define TICKS_BEFORE_SCHEDULING ((QUANTUM_SCHEDULER*1000*MICROSECOND)/QUANTUM)

struct scheduler_info_t scheduler_info = {NULL, NULL, NULL, NULL};

/** 
 * @brief Tick counter. Used for VM's scheduling. 
 */ 
static uint32_t tick_count = 0;

/** 
 * Keeps a pointer to the last scheduled VCPU. 
 */
static struct list_t *last_scheduled = NULL;

/**
 * @brief round-robin scheduler implementation. 
 * 
 * @return Pointer to the node in the list of VCPUs of the scheduled VCPU. 
 */
struct list_t* round_robin_scheduler(){
	if(!last_scheduled || !last_scheduled->next){
		last_scheduled = scheduler_info.vcpu_ready_list;
	}else{
		last_scheduled = last_scheduled->next;
	}
	return last_scheduled;
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
void fast_interrupt_delivery(struct list_t *target){

	/* Do not reschedule if the VCPU is already in execution. */
	if (target == scheduler_info.vcpu_executing_nd){
		return;
	}
	
	scheduler_info.next_vcpu = target;
}


/**
 * @brief Scheduler routine. Must be invoked in the timer interrupt routine. 
 * 
 */
void run_scheduler(){
	if (scheduler_info.next_vcpu){
		contextSave();           	
		scheduler_info.vcpu_executing_nd = scheduler_info.next_vcpu;
		contextRestore();
		scheduler_info.next_vcpu = NULL;
		/* Gives an extra execution time to the target VM if the current period is finishing. */
		tick_count = (tick_count % TICKS_BEFORE_SCHEDULING >= TICKS_BEFORE_SCHEDULING)? (tick_count-2) : tick_count;
	}else{
		if ( tick_count % TICKS_BEFORE_SCHEDULING == 0){
			contextSave();   
			scheduler_info.vcpu_executing_nd = round_robin_scheduler();
			contextRestore();
		}
	}
	tick_count++;	
}


/**
 * @brief Returns a VCPU corresponding to the id.
 * 
 * @return Pointer to the VCPU. 
 */
vcpu_t* get_vcpu_from_id(uint32_t id){
	struct list_t* vm_list;
	vm_list = scheduler_info.virtual_machines_list;
	
	while(vm_list){
		if(id == ((vm_t*)vm_list->elem)->id){
			struct list_t *vcpu_l = ((vm_t*)vm_list->elem)->vcpus;
			return (vcpu_t*)vcpu_l->elem;
		}
		vm_list = vm_list->next;
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
struct list_t* get_fast_int_vcpu_node(uint32_t fast_int){
	uint32_t i, fint;
	vm_t* vm;
	struct list_t* vm_list;
	vm_list = scheduler_info.virtual_machines_list;
	
	
	while(vm_list){
		vm = (vm_t*)vm_list->elem;
		for(i=0;i<vm->fast_int_sz;i++){
			if(fast_int == vm->fast_interrupts[i]){
				return ((vm_t*)vm_list->elem)->vcpus;
				
			}
		}
		vm_list = vm_list->next;
	}
	return NULL;	
} 



