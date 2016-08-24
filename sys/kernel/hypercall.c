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
			
			if (vcpu->init){
                MoveToPreviousGuestGPR(REG_V0, MESSAGE_VCPU_NOT_INIT);
                break;
            }
			
			/* message queue full */
			if(vcpu->messages.num_messages == MESSAGELIST_SZ){
                vcpu->guestclt2 |= (5<<GUESTCLT2_GRIPL_SHIFT);
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
			vcpu->guestclt2 |= (5<<GUESTCLT2_GRIPL_SHIFT);
				
			/* Return success to sender */
			MoveToPreviousGuestGPR(REG_V0, message_size);
			
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
			char* message_ptr_mapped = (char*)tlbCreateEntry((uint32_t)message_ptr, curr_vm->base_addr, messagesz, 0xf);
			memcpy(message_ptr_mapped, vcpu->messages.message_list[vcpu->messages.out].message, messagesz);
                        
			/* Return the message size to the receiver */
			MoveToPreviousGuestGPR(REG_V0, messagesz);
			MoveToPreviousGuestGPR(REG_V1, vcpu->messages.message_list[vcpu->messages.out].source_id); 
			
			/* free the message allocation in the message list */
			vcpu->messages.num_messages--;
			vcpu->messages.out = (vcpu->messages.out + 1) % MESSAGELIST_SZ;
			
			break;
		}

                case HCALL_PUF_SHARED_MEMORY:{
                        
                        vcpu_t* vcpu = curr_vcpu;

                        /* Getting parameters from guest*/
                        uint32_t source_id  = MoveFromPreviousGuestGPR(REG_A0); /* Maybe you want to keep track of the guest ID for control purposes. */
                        char* buffer_ptr = (char*)MoveFromPreviousGuestGPR(REG_A1); /* pointer to the guest buffer */
                        
                        /* Copy the message the receiver */
                        char* buffer_ptr_mapped = (char*)tlbCreateEntry((uint32_t)buffer_ptr, curr_vm->base_addr, 32, 0xf); /* map the guest memory */
                        
                        printf("\nGuest buffer as seen by the hypervisor : %s", buffer_ptr_mapped);
                        
                        /* Write on guest's buffer */
                        strcpy(buffer_ptr_mapped, "Hello! You are my guest!");
                        
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
                case HCALL_START_TIMER_GUEST:{
                    uint32_t interval  = MoveFromPreviousGuestGPR(REG_A0);
                    /*register_timer(interval);*/
                    break;
                }
                /* Check if the guest is UP */
                case HCALL_GUEST_UP:{
                    vcpu_t* vcpu;
                    uint32_t target_id  = MoveFromPreviousGuestGPR(REG_A0);
                    vcpu = (vcpu_t*)get_vcpu_from_id(target_id,&be_vcpu_list);
                    if(!vcpu){
                        MoveToPreviousGuestGPR(REG_V0, MESSAGE_VCPU_NOT_FOUND);
                    }else{
                        MoveToPreviousGuestGPR(REG_V0, vcpu->init? MESSAGE_VCPU_NOT_INIT : 1);
                    }
                    break;
                }
                    
		case HCALL_FLASH_READ:{
			vcpu_t* vcpu = curr_vcpu;

			uint8_t * dest = (uint8_t *) tlbCreateEntry((uint32_t) MoveFromPreviousGuestGPR(REG_A0), curr_vm->base_addr, sizeof(uint8_t) * 1024, 0xf);

			flash_read1Kbuffer(dest);

			break;
		}

		case HCALL_FLASH_WRITE:{
			vcpu_t* vcpu = curr_vcpu;

			uint8_t * source = (uint8_t *) tlbCreateEntry((uint32_t) MoveFromPreviousGuestGPR(REG_A0), curr_vm->base_addr, sizeof(uint8_t) * 1024, 0xf);

			flash_write1Kbuffer(source);

		break;
		}

        /* This hypercall is used by PUF API. */
		case ETH_GET_MAC:{

			uint8_t * mac = (uint8_t *) tlbCreateEntry((uint32_t) MoveFromPreviousGuestGPR(REG_A0), curr_vm->base_addr, sizeof(uint8_t) * 6, 0xf);
#ifdef ETHERNET_SUPPORT             
            memcpy(mac, eth_port.macaddr, sizeof(uint8_t) * 6);
#else            
            mac[0] = *((uint8_t *) 0xBF882320);
            mac[1] = *((uint8_t *) 0xBF882321);
            mac[2] = *((uint8_t *) 0xBF882310);
            mac[3] = *((uint8_t *) 0xBF882311);
            mac[4] = *((uint8_t *) 0xBF882300);
            mac[5] = *((uint8_t *) 0xBF882301);
#endif            

            break;
		}
		
#ifdef ETHERNET_SUPPORT         	
        case ETH_RECV_FRAME:{
            
            int32_t framesz;

            char* frame_ptr = (char*)MoveFromPreviousGuestGPR(REG_A0);

            /* Copy the message the receiver */
            char* frame_ptr_mapped = (char*)tlbCreateEntry((uint32_t)frame_ptr, curr_vm->base_addr, MTU, 0xf);
            
            uint8_t* buf = MACH_PHYS_TO_VIRT(MACH_VIRT_TO_PHYS(rx_buf));
            
            framesz = en_ll_input(buf);
            
            memcpy(frame_ptr_mapped, buf, framesz);
            
            /* Return the message size to the receiver */
            MoveToPreviousGuestGPR(REG_V0, framesz);
            
            break;
        }
        case ETH_SEND_FRAME:{
            /* Getting parameters from guest*/
            uint8_t *frame  = (uint8_t *)MoveFromPreviousGuestGPR(REG_A0);
            uint32_t size = MoveFromPreviousGuestGPR(REG_A1);
            
            char* frame_ptr_mapped = (char*)tlbCreateEntry((uint32_t)frame, curr_vm->base_addr, size, 0xf);
            uint8_t* buf = MACH_PHYS_TO_VIRT(MACH_VIRT_TO_PHYS(tx_buf));
            
            memcpy(buf,frame_ptr_mapped,size);
            
            MoveToPreviousGuestGPR(REG_V0, size);
            
            en_ll_output(buf, size);
            
            break;
        }
        
        case ETH_LINK_STATE:{
            MoveToPreviousGuestGPR(REG_V0, eth_port.is_up);
            break;
        }
#endif 	
#ifdef USB_SUPPORT
        case USB_VM_REGISTER:{
            uint32_t guest_id  = (uint32_t)MoveFromPreviousGuestGPR(REG_A0);
            usb_vm_register(guest_id);
            break;
        }
        case USB_VM_GET_DESCRIPTOR:{
            uint8_t *buf  = (uint8_t *)MoveFromPreviousGuestGPR(REG_A0);
            uint32_t size = MoveFromPreviousGuestGPR(REG_A1);
            
            char* frame_ptr_mapped = (char*)tlbCreateEntry((uint32_t)buf, curr_vm->base_addr, size, 0xf);
                        
            size = usb_get_descriptor(frame_ptr_mapped, size);
            
            MoveToPreviousGuestGPR(REG_V0, size);
            
            break;
        }
#endif            
        default:
            break;
    }
	
	return ret;
}
