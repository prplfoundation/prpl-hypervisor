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
 * @file inter-vm-comm.c
 * 
 * @section DESCRIPTION
 * 
 * Inter-VM communication mechanism. This implementation will be replaced
 * for a more convenient driver soon. 
 */

#include <hypercall.h>
#include <scheduler.h>
#include <hal.h>
#include <globals.h>
#include <tlb.h>
#include <driver.h>
#include <hypercall_defines.h>
#include <mips_cp0.h>


/**
 * @brief Hypercall implementation. Returns the VM identifier number for the calling VM. 
 * V0 guest register will be replaced with the VM id. 
 */
void get_vm_id(){
    MoveToPreviousGuestGPR(REG_V0,curr_vcpu->vm->id);
}

/**
 * @brief Checks if a given VM is running.
 * V0 guest register will be replaced with 1 if the VM is running or MESSAGE_VCPU_NOT_INIT otherwise. MESSAGE_VCPU_NOT_FOUND 
 * can be returned if the target VM is not running. 
 */
void guest_is_up(){
    vcpu_t* vcpu;
    uint32_t target_id  = MoveFromPreviousGuestGPR(REG_A0);
    vcpu = (vcpu_t*)get_vcpu_from_id(target_id,&be_vcpu_list);
    if(!vcpu){
        MoveToPreviousGuestGPR(REG_V0, MESSAGE_VCPU_NOT_FOUND);
    }else{
        MoveToPreviousGuestGPR(REG_V0, vcpu->init? MESSAGE_VCPU_NOT_INIT : 1);
    }
}

/**
 * @brief Send a message for a target VM. 
 * Calling convention (guest registers): 
 *   a0 = Target process ID
 *   a1 = Source message pointer 
 *   a2 = Message size 
 *   v0 = true/false (Succeded/failed)
 */
void intervm_send_msg(){
    vcpu_t* vcpu;
                       
    /* Getting parameters from guest */
    uint32_t target_id  = MoveFromPreviousGuestGPR(REG_A0);
    char* message_ptr = (char*)MoveFromPreviousGuestGPR(REG_A1); 
    uint32_t message_size = MoveFromPreviousGuestGPR(REG_A2);
                        
    /* check if the message has acceptable size */
    if(message_size > MESSAGE_SZ){
        MoveToPreviousGuestGPR(REG_V0, MESSAGE_TOO_BIG);
        return;
    }
                        
    /* Try to locate the destiny VCPU */
    vcpu = (vcpu_t*)get_vcpu_from_id(target_id,&be_vcpu_list);

     /* destination vcpu not found */
     if(vcpu == NULL){
        MoveToPreviousGuestGPR(REG_V0, MESSAGE_VCPU_NOT_FOUND);
        return;
     }
                        
     if (vcpu->init){
        MoveToPreviousGuestGPR(REG_V0, MESSAGE_VCPU_NOT_INIT);
        return;
     }
                        
     /* message queue full */
     if(vcpu->messages.num_messages == MESSAGELIST_SZ){
        vcpu->guestclt2 |= (5<<GUESTCLT2_GRIPL_SHIFT);
        MoveToPreviousGuestGPR(REG_V0, MESSAGE_FULL);
        return;
     }                                
     
     /* copy message to message queue */
     char* message_ptr_mapped = (char*)tlbCreateEntry((uint32_t)message_ptr, curr_vm->base_addr, message_size, 0xf, CACHEABLE);
     memcpy(vcpu->messages.message_list[vcpu->messages.in].message,message_ptr_mapped,message_size);
     vcpu->messages.message_list[vcpu->messages.in].size = message_size;
     vcpu->messages.message_list[vcpu->messages.in].source_id = curr_vcpu->id;
                        
     vcpu->messages.num_messages++;
     vcpu->messages.in = (vcpu->messages.in + 1) % MESSAGELIST_SZ;
                        
     /* generate virtual interrupt to guest */
     vcpu->guestclt2 |= (5<<GUESTCLT2_GRIPL_SHIFT);
                                
     /* Return success to sender */
     MoveToPreviousGuestGPR(REG_V0, message_size);
     
}

/**
 * @brief Receive a message. 
 * Calling convention (guest registers): 
 *   a0 = Process ID
 *   a1 = Target message pointer 
 *   v0 = Message size 
 */
void intervm_recv_msg(){
    vcpu_t* vcpu = curr_vcpu;
    uint32_t messagesz;

    /* No messages in the receiver queue */
    if(vcpu->messages.num_messages == 0){
        MoveToPreviousGuestGPR(REG_V0, MESSAGE_EMPTY); 
        return;
    }

    /* Getting parameters from guest*/
    uint32_t source_id  = MoveFromPreviousGuestGPR(REG_A0);
    char* message_ptr = (char*)MoveFromPreviousGuestGPR(REG_A1);

    /* Copy the message the receiver */
    messagesz = vcpu->messages.message_list[vcpu->messages.out].size;
    char* message_ptr_mapped = (char*)tlbCreateEntry((uint32_t)message_ptr, curr_vm->base_addr, messagesz, 0xf, CACHEABLE);
    memcpy(message_ptr_mapped, vcpu->messages.message_list[vcpu->messages.out].message, messagesz);
    
    /* Return the message size to the receiver */
    MoveToPreviousGuestGPR(REG_V0, messagesz);
    MoveToPreviousGuestGPR(REG_V1, vcpu->messages.message_list[vcpu->messages.out].source_id); 
                        
    /* free the message allocation in the message list */
    vcpu->messages.num_messages--;
    vcpu->messages.out = (vcpu->messages.out + 1) % MESSAGELIST_SZ;
                       
}

/**
 * @brief Driver init call.  Registers the hypercalls. 
 */
void intervm_init(){
    if (register_hypercall(get_vm_id, HCALL_GET_VM_ID) < 0){
        printf("\nError registering the HCALL_GET_VM_ID hypercall");
        return;
    }
    
    if (register_hypercall(intervm_send_msg, HCALL_IPC_SEND_MSG) < 0){
        printf("\nError registering the HCALL_IPC_SEND_MSG hypercall");
        return;
    }

    if (register_hypercall(intervm_recv_msg, HCALL_IPC_RECV_MSG) < 0){
        printf("\nError registering the HCALL_IPC_RECV_MSG hypercall");
        return;
    }
    
    if (register_hypercall(guest_is_up, HCALL_GUEST_UP) < 0){
        printf("\nError registering the HCALL_IPC_RECV_MSG hypercall");
        return;
    }
    
    printf("\nInter-VM comunication hypercalls registered.");
}

driver_init(intervm_init);

