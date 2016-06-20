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

#include <hypercall.h>
#include <scheduler.h>
#include <hal.h>
#include <globals.h>
#include <tlb.h>

/**Handle hypercalls */
int32_t HypercallHandler(){
		
	int32_t ret = SUCEEDED;
	uint32_t hypercallCode = getHypercallCode(); 
	rtvcpu_par* h;
	int i=0;
					
	switch(hypercallCode){
		case HCALL_INFO_GET_ID:
			MoveToPreviousGuestGPR(REG_V0,curr_vcpu->vm->id);		
			break;
		
		case HCALL_RT_CREATE_SERVICE:{								
			vcpu_t* vcpu;						
			//Get parameters
			h=(rtvcpu_par*)(MoveFromPreviousGuestGPR(REG_A0)+10000000);
			h = (rtvcpu_par*)tlbCreateEntry(MoveFromPreviousGuestGPR(REG_A0), curr_vm->base_addr, sizeof(rtvcpu_par), 0xf);
			//Copy service name from guest vm (Copying pointed contents)
			char* service_name = (char*)malloc(strlen(h->service_name));

			Info("MUSTANG: Created RT-SERVICE: %s",service_name);
												
			//Create vcpu with task_addr as parameter
			/* FIXME: PIP needed during RTvcpu creation */
			vcpu = create_vcpu(curr_vm,h->entry_point,h->task_addr,h->stack_pointer, 0, BAREOS_RT);
			
			//Setting task parameters
			vcpu->task.unique_name = service_name;
			vcpu->task.unique_name_hash = hash(service_name);
			    
			//Scheduling parameters*/
			vcpu->task.deadline = h->deadline ;
			vcpu->task.period   = h->period   ;
			vcpu->task.wcet     = h->wcet     ;
								
			//Add to rt-inactive list			
			addVcpu_realTimeInactiveList(vcpu);
													
			break;
		}
		case HCALL_RT_LAUNCH_SERVICE:{
			
			//Launch service vcpu on name (Check if it's not too much overhead)
			char* service_name = (char*)tlbCreateEntry(MoveFromPreviousGuestGPR(REG_A0), curr_vm->base_addr, SERVICE_NAME_SZ, 0xf);
			
			ll_node_t* node = get_vcpu_node_from_task_name(service_name,&rt_vcpu_inactive_list);
			
			//Put vcpu on active list
			ll_remove(node);
			ll_append(&rt_vcpu_active_list,node);
			
			//contextSave(NULL);
			ret = RESCHEDULE;
		
			break;		
		}
		case HCALL_IPC_SEND_MSG:{
			vcpu_t* vcpu;
			
			/* Getting parameters from guest */
			uint32_t target_id  = MoveFromPreviousGuestGPR(REG_A0);
			char* message_ptr = (char*)MoveFromPreviousGuestGPR(REG_A1); 
			uint32_t message_size = MoveFromPreviousGuestGPR(REG_A2);
			
			/* check if the message has acceptable size */
			if(message_size > MESSAGE_SZ){
				MoveToPreviousGuestGPR(REG_V0, MESSAGE_TOO_BIG);
				break;
			}
			
			/* Try to locate the destiny VCPU */
			vcpu = (vcpu_t*)get_vcpu_from_id(target_id,&be_vcpu_list);

			/* destination vcpu not found */
			if(vcpu == NULL){
				MoveToPreviousGuestGPR(REG_V0, MESSAGE_VCPU_NOT_FOUND);
				break;
			}
			
			/* message queue full */
			if(vcpu->messages.num_messages == MESSAGELIST_SZ){
				MoveToPreviousGuestGPR(REG_V0, MESSAGE_FULL);
				break;
			}
				
			/* copy message to message queue */
			char* message_ptr_mapped = (char*)tlbCreateEntry((uint32_t)message_ptr, curr_vm->base_addr, message_size, 0xf);
			memcpy(vcpu->messages.message_list[vcpu->messages.in].message,message_ptr_mapped,message_size);
			vcpu->messages.message_list[vcpu->messages.in].size = message_size;
			vcpu->messages.message_list[vcpu->messages.in].source_id = curr_vcpu->id;
			
			vcpu->messages.num_messages++;
			vcpu->messages.in = (vcpu->messages.in + 1) % MESSAGELIST_SZ;
			
			/* generate virtual interrupt to guest */
			vcpu->guestclt2 |= (1<<NETWORK_VINTERRUPT);
			vcpu->cp0_registers[12][0] |= (1<<NETWORK_VINTERRUPT);
				
			/* Return success to sender */
			MoveToPreviousGuestGPR(REG_V0, message_size);
			
			/* Set next vcpu to the target vcpu*/
			target_vcpu = vcpu;
			ret = CHANGE_TO_TARGET_VCPU;
												
			break;
		}
		case HCALL_IPC_RECV_MSG:{
			
			vcpu_t* vcpu = curr_vcpu;
			uint32_t messagesz;

			/* No messages in the receiver queue */
			if(vcpu->messages.num_messages == 0){
				MoveToPreviousGuestGPR(REG_V0, MESSAGE_EMPTY); 
				break;
			}

			/* Getting parameters from guest*/
			uint32_t source_id  = MoveFromPreviousGuestGPR(REG_A0);
			char* message_ptr = (char*)MoveFromPreviousGuestGPR(REG_A1);

			/* Copy the message the receiver */
			messagesz = vcpu->messages.message_list[vcpu->messages.out].size;
			char* message_ptr_mapped = (char*)tlbCreateEntry((uint32_t)message_ptr, vcpu->vm->base_addr, messagesz, 0x1f);
			memcpy(message_ptr_mapped, vcpu->messages.message_list[vcpu->messages.out].message, messagesz);
			
			/* Return the message size to the receiver */
			MoveToPreviousGuestGPR(REG_V0, messagesz);
			MoveToPreviousGuestGPR(REG_V1, vcpu->messages.message_list[vcpu->messages.out].source_id); 
			
			/* free the message allocation in the message list */
			vcpu->messages.num_messages--;
			vcpu->messages.out = (vcpu->messages.out + 1) % MESSAGELIST_SZ;
			
			break;
		}
		case HCALL_INTERRUPT_GUEST:{
			vcpu_t* vcpu;
			
			//Getting parameters from guest
			uint32_t target_id  = MoveFromPreviousGuestGPR(REG_A0);
		
			//Check if there's a task waiting for this message
			vcpu = (vcpu_t*)get_vcpu_from_id(target_id, &be_vcpu_list);
			if(vcpu){
				vcpu->guestclt2 |= (1<<14);
				vcpu->cp0_registers[12][0] |= (1<<14);
			}
		
			break;
		}
		case HCALL_HALT_GUEST:{
			ret = PROGRAM_ENDED;
			break;			
		}		
		default:
			break;
		
		break;		
	}	
	
	return ret;
}
