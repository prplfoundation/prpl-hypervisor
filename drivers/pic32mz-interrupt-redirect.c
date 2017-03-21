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
 * @file interrupt_redirect.c
 * 
 * @section DESCRIPTION
 * 
 * Allows for the interrupt redirection for guests. Use the interrupt_redirect property on 
 * the VM's configuration. For exemple:
 * 
 * interrupt_redirect = ["IRQ_PORTB"];
 * 
 * The interrupt IRQ_PORTB will be redirected to the target guest. 
 * 
 * IMPORTANT: Once an interrupt is handled, this driver disables it (cleaning the IEC bit) and 
 * inject a virtual interrupt to the guest. In order to receive further interrupts, on its
 * interrupt handler, the guest must do the hypercall reenable_interrupt(). 
 * 
 * Some hardware controllers require some action other them only clean its IFS bit. 
 * For exemple, IRQ_UxRX (UARTx receive IRQ) will keep generating interrupts 
 * even after cleaning the IFS bit. The UART controller requires sucessive reads 
 * until the input FIFO to be empty. Disabling the controller interrupts momentaneally allows 
 * the guest manage the cotroller and re-enable the interrutps. 
 *
 */

#include <hypercall.h>
#include <globals.h>
#include <driver.h>
#include <hypercall_defines.h>
#include <mips_cp0.h>
#include <hal.h>
#include <scheduler.h>
#include <libc.h>
#include <vm.h>
#include <guest_interrupts.h>
#include <malloc.h>
#include <interrupts.h>

struct list_t* vcpu_node = NULL;

/**
 * @struct
 * @brief Makes the association between interrupts and guests. 
 */
struct interrupt_mapping{
	uint32_t irq_number; /**< IRQ number on the system. */
	uint32_t irq_guest; /**< IRQ number on guest. */
	vcpu_t *vcpu;	/**< Target VCPU. */
};

static uint32_t interrupt_mapping_sz = 0;
static struct interrupt_mapping * interrupt_mapping_list = NULL;

/**
 * @brief Performs virtual interrupt injection on guests.
*/
void interrupt_injection(){
	uint32_t i;
	uint32_t guestctl;

	/* Find which interrupt is active. */
	for(i=0;i<interrupt_mapping_sz;i++){
		if( IFS(interrupt_mapping_list[i].irq_number >> 5) & (1 << (interrupt_mapping_list[i].irq_number & 31)) ){
			
			/* Clear the hardware interrupt */
			IFSCLR(interrupt_mapping_list[i].irq_number  >> 5) = 1 << (interrupt_mapping_list[i].irq_number & 31);
			
			guestctl = getGuestCTL2();
			
			/* Avoid inject the same interrupt twoice. */
			if (!(guestctl & (interrupt_mapping_list[i].irq_guest << GUESTCLT2_GRIPL_SHIFT))){
				/* Disable interrupt - The guest must reenable the interrupt on its handler.  */
				IECCLR(interrupt_mapping_list[i].irq_number >> 5) = 1 << (interrupt_mapping_list[i].irq_number & 31);
			
				/* If the target VCPU is in execution, inject the virtual interrupt immediately. Otherwise,
				 * the virtual interrupt will be injected on next execution. */
				if(interrupt_mapping_list[i].vcpu == vcpu_in_execution){
					setGuestCTL2(guestctl | (interrupt_mapping_list[i].irq_guest << GUESTCLT2_GRIPL_SHIFT));
				}else{
					interrupt_mapping_list[i].vcpu->guestclt2 |= interrupt_mapping_list[i].irq_guest << GUESTCLT2_GRIPL_SHIFT;
					fast_interrupt_delivery(interrupt_mapping_list[i].vcpu);
				}
				
			}
		}
		
	}

}

/**
 * @brief Hypercall to re-enable interrupts. This hypercall must be called 
 * by the guests to re-enable a certain interrupt if desired. 
 */
void reenable_interrupt(){
	uint32_t i;
	uint32_t interrupt = (uint32_t)MoveFromPreviousGuestGPR(REG_A0);
	
	for(i=0; i<interrupt_mapping_sz; i++){
		if(vcpu_in_execution == interrupt_mapping_list[i].vcpu){
			if (interrupt == interrupt_mapping_list[i].irq_guest){
				IECSET(interrupt_mapping_list[i].irq_number >> 5) = 1 << (interrupt_mapping_list[i].irq_number & 31);
				interrupt_mapping_list[i].vcpu->guestclt2 &= ~(interrupt_mapping_list[i].irq_guest << GUESTCLT2_GRIPL_SHIFT);
				MoveToPreviousGuestGPR(REG_V0, 0);
				return;
			}
		}
	}
	
	MoveToPreviousGuestGPR(REG_V0, 1);
	WARNING("VM %s trying to reenable an non-configured IRQ.", vcpu_in_execution->vm->vm_name);
}


/**
 * @brief Driver init call. 
 *  Configure required interrupts. 
 */
void interrupt_redirect_init(){
	uint32_t i, j, sz;
	uint32_t *int_redirect, offset;
	uint32_t irq_count = 0;
	vcpu_t *vcpu;
		
	/* Determines the total number of interrupt redirections configured in all guests.*/
	for(i=0;i<NVMACHINES;i++){
		vcpu = get_vcpu_from_id(i+1);
		interrupt_mapping_sz += vcpu->vm->vmconf->interrupt_redirect_sz;
	}
	
	if (interrupt_mapping_sz > 0){
		offset = register_interrupt(interrupt_injection);
			
		if (register_hypercall(reenable_interrupt, HCALL_REENABLE_INTERRUPT) < 0){
			ERROR("Error registering the HCALL_IPC_SEND_MSG hypercall");
			return;
		}
		
		
		interrupt_mapping_list = (struct interrupt_mapping*)malloc(interrupt_mapping_sz*sizeof(struct interrupt_mapping));
		if (interrupt_mapping_list == NULL){
			WARNING("Interrupt redirect driver fails on allocate data.");
		}
    
		for(i=0;i<NVMACHINES;i++){
			vcpu = get_vcpu_from_id(i+1);
			sz = vcpu->vm->vmconf->interrupt_redirect_sz;
			if(sz > 0){
				int_redirect = vcpu->vm->vmconf->interrupt_redirect;
				for(j=0; j<sz; j++){
					/* configure the interrupt redirection data.*/
					interrupt_mapping_list[irq_count].irq_number = int_redirect[j];
					interrupt_mapping_list[irq_count].irq_guest = GUEST_USER_DEFINED_INT_1 << j;
					interrupt_mapping_list[irq_count].vcpu = vcpu;

					irq_count++;
				
					/* Clear the priority and sub-priority */
					IPCCLR(int_redirect[j] >> 2) = 0x1f << (8 * (int_redirect[j] & 0x03));
					
					/* Set the priority and sub-priority */
					IPCSET(int_redirect[j] >> 2) = 0x1f << (8 * (int_redirect[j] & 0x03));
				
					/* Clear the requested interrupt bit */
					IFSCLR(int_redirect[j] >> 5) = 1 << (int_redirect[j] & 31);
				
					/* Set interrupt handler */
					OFF(int_redirect[j]) = offset;
				
					/* Enable the requested interrupt */
					IECSET(int_redirect[j] >> 5) = 1 << (int_redirect[j] & 31);
				
					INFO("Interrupt %d redirected to VM %s.", int_redirect[j], vcpu->vm->vm_name);
				}
			}
		}
		INFO("Interrupt redirect handler at 0x%x.", offset);
	}
    
	INFO("Interrupt redirect driver registered.");
}

driver_init(interrupt_redirect_init);

