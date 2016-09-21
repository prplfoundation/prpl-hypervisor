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
//#include <irq.h>
#include <globals.h>
#include <config.h>


//Best effort vcpu list
linkedlist_t be_vcpu_list = {0};
linkedlist_t virtualmachines = {0};


int initializeShedulers(){
	 ll_init(&be_vcpu_list);
	 ll_init(&virtualmachines);
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


void runScheduler(){
	
	runBestEffortScheduler();
	
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

