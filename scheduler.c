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

#include <scheduler.h>
#include <vcpu.h>
#include <libc.h>
#include <irq.h>
#include <globals.h>
#include <config.h>


//Best effort vcpu list
linkedlist_t be_vcpu_list;
linkedlist_t be_vcpu_list_waiting;

//Real Time vcpu lists
linkedlist_t rt_vcpu_inactive_list;
linkedlist_t rt_vcpu_active_list;
linkedlist_t rt_vcpu_waiting_list;

//Real time services initialization vcpu list
linkedlist_t rt_services_init_vcpu_list;

int initializeShedulers(){
	 ll_init(&be_vcpu_list);
	 ll_init(&be_vcpu_list_waiting);
	 ll_init(&rt_vcpu_inactive_list);
	 ll_init(&rt_vcpu_active_list);
	 ll_init(&rt_vcpu_waiting_list);
	 ll_init(&rt_services_init_vcpu_list);
	 ll_init(&virtualmachines);
         ll_init(&virtualmachines_rt);
	 return 0;
}

ll_node_t* curr_node = NULL;

void runBestEffortScheduler(){ //Simple round robin
	
	if(!curr_node)
		curr_node = be_vcpu_list.head;
	
	if(curr_vcpu){		
		if(curr_node && curr_node->next){
			curr_node = curr_node->next;			
		}else{
			curr_node = be_vcpu_list.head;			
		}		
	}else{
		curr_node = be_vcpu_list.head;
	}
	
	if(curr_node != NULL){
		curr_vcpu = (vcpu_t*) curr_node->ptr;		
	}else{
		curr_vcpu = idle_vcpu;
	}

}

int32_t remove_vm_and_runBestEffortScheduler(){
	
	ll_node_t* node = curr_node->next;	
		
	ll_remove(curr_node);
	
	if(be_vcpu_list.count==0){
		curr_vcpu = idle_vcpu;
	}
		
	if(!node)
		node = be_vcpu_list.head;
		
	curr_node = node;	
	
	if(curr_node)
		curr_vcpu = (vcpu_t*) curr_node->ptr;

	if(curr_vcpu == idle_vcpu)
		return -1;
		
	return 0;	
}

#define ALTERNATIVE_EDF

/* Earliest Deadline First */
vcpu_t* runRTscheduler(){
#ifndef ALTERNATIVE_EDF
	uint16_t deadline_dif = 0xffff;
#endif
	ll_node_t* node_i;
	vcpu_t* next_vcpu = NULL;
	vcpu_t* v;
#ifdef ALTERNATIVE_EDF
	static vcpu_t* current = NULL;
	static int sucess = 0;
	uint32_t earlydeadline;
	static uint32_t turn = 0;
	
	if(current && current->task.wcet_counter == 0 && sucess){
		current->task.relative_deadline = (current->task.relative_deadline - turn) + turn + current->task.deadline;
		current->task.release_time += current->task.period;
	}
		
	sucess = 0;
	earlydeadline = 0xffffffff;

	for(node_i = rt_vcpu_active_list.head; node_i != NULL ; node_i = node_i->next){
		v = (vcpu_t*)node_i->ptr;
		if(earlydeadline > v->task.relative_deadline && v->task.release_time <= turn){
			current = v;
			earlydeadline = v->task.relative_deadline;
			sucess = 1;
		}
	}
		
	if(sucess && current->task.wcet_counter == 0){
		current->task.wcet_counter = current->task.wcet;
	}
		
	if(sucess){
		current->task.wcet_counter--;
		curr_vcpu = current;
	}else{
		curr_vcpu = NULL;
	}
		
	turn++;

#else
	for(node_i = rt_vcpu_active_list.head; node_i != NULL ; node_i = node_i->next){
		v = (vcpu_t*)node_i->ptr;
		
		if(v->task.period > 0){
			v->task.deadline_counter++;
			if((v->task.deadline - v->task.deadline_counter) < deadline_dif){ //Is it the earliest deadline?
				if(v->task.wcet_counter > 0){
					deadline_dif = v->task.deadline - v->task.deadline_counter;
					next_vcpu = v;
				}				
			}
			
			if (v->task.deadline_counter == v->task.deadline){	//If it is on deadline
				if (v->task.wcet_counter > 0)							//did it finish?
					v->task.deadline_misses++;							//deadline miss
				
				v->task.wcet_counter = v->task.wcet;		
				v->task.deadline_counter = 0;							
			}			
		}
	}

	curr_vcpu = v;
#endif	

	return curr_vcpu;
}


void runScheduler(){
	void *sucess = NULL;
	
	//Priority to RT scheduler (always)
	if(rt_vcpu_active_list.count>0){
		sucess = runRTscheduler();
	}
	
	if(!sucess){
		runBestEffortScheduler();
	}	
	
}

ll_node_t* get_vcpu_node_from_task_name(char* unique_name, linkedlist_t* vcpu_list){
	
	uint32_t uhash = hash(unique_name);	
	
	//Search on vcpu list
	ll_node_t* node_i;
	vcpu_t* vcpu=NULL;
	
	for(node_i = vcpu_list->head; node_i != NULL ; node_i = node_i->next){
		vcpu = (vcpu_t*)node_i->ptr;		
		if(vcpu->task.unique_name_hash==uhash)
			return node_i;				
	}
	
	return NULL;	
}

vcpu_t* get_vcpu_from_id(uint32_t id, linkedlist_t* vcpu_list){
	
	//Search on vcpu list
	ll_node_t* node_i;
	vcpu_t* vcpu=NULL;
	
	for(node_i = vcpu_list->head; node_i != NULL ; node_i = node_i->next){
		vcpu = (vcpu_t*)node_i->ptr;		
		if(vcpu->vm->id==id)
			return vcpu;				
	}
	
	return NULL;	
} 

int32_t scheduler_block_vcpu(vcpu_t *vcpu){
	ll_node_t* node_i;
	vcpu_t* vcpu_i=NULL;
	
	for(node_i = be_vcpu_list.head; node_i != NULL ; node_i = node_i->next){
		vcpu_i = (vcpu_t*)node_i->ptr;		
		if(vcpu_i==vcpu){			
			ll_remove(node_i);
			ll_append(&be_vcpu_list_waiting,node_i);
			return 0;
		}	
	}
	
	return -1;	
}

int32_t scheduler_unblock_vcpu(vcpu_t *vcpu){
	ll_node_t* node_i;
	vcpu_t* vcpu_i=NULL;
	
	for(node_i = be_vcpu_list_waiting.head; node_i != NULL ; node_i = node_i->next){
		vcpu_i = (vcpu_t*)node_i->ptr;		
		if(vcpu_i==vcpu){			
			ll_remove(node_i);
			ll_append(&be_vcpu_list,node_i);
			return 0;
		}	
	}
	
	return -1;	
}

int32_t addVcpu_bestEffortList(vcpu_t *vcpu){
	ll_node_t *vnd;
	if(!(vnd = (ll_node_t *) calloc(1,sizeof(ll_node_t))))
		return -1;

	vnd->ptr=vcpu;
	vnd->priority=0; 
	ll_append(&(be_vcpu_list), vnd);
	
	return 0;  		    
}

int addVcpu_toList(vcpu_t *vcpu,linkedlist_t* linkedlist){
	
	ll_node_t *vnd;
	if(!(vnd = (ll_node_t *) calloc(1,sizeof(ll_node_t))))
		return -1;

	vnd->ptr=vcpu;
	vnd->priority=0; 
	ll_append(linkedlist, vnd);
	
	return 0;  		    
}

int addVcpu_servicesInitList(vcpu_t *vcpu){
	
	ll_node_t *vnd;
	if(!(vnd = (ll_node_t *) calloc(1,sizeof(ll_node_t))))
		return -1;

	vnd->ptr=vcpu;
	vnd->priority=0; 
	ll_append(&(rt_services_init_vcpu_list), vnd);
	
	return 0;  		    
}

int addVcpu_realTimeInactiveList(vcpu_t *vcpu){
	ll_node_t *vnd;
	if(!(vnd = (ll_node_t *) calloc(1,sizeof(ll_node_t))))
		return -1;

	vnd->ptr=vcpu;
	vnd->priority=0; 
	ll_append(&(rt_vcpu_inactive_list), vnd);
	
	return 0;  		    
}
