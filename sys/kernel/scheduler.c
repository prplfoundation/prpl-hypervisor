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


struct scheduler_info_t scheduler_info = {NULL, NULL, NULL};

/**
 * @brief round-robin scheduler implementation. 
 * 
 * @return Pointer to the node in the list of VCPUs of the scheduled VCPU. 
 */
struct list_t* round_robin_scheduler(){
	if(!scheduler_info.vcpu_executing_nd || !scheduler_info.vcpu_executing_nd->next){
		return scheduler_info.vcpu_ready_list;
	}else{
		return scheduler_info.vcpu_executing_nd->next;
	}
}


/**
 * @brief Scheduler routine. Must be invoke in the timer interrupt routine. 
 * 
 */
void run_scheduler(){
	
	scheduler_info.vcpu_executing_nd = round_robin_scheduler();
	
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


