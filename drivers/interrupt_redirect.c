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
 * This is a hypercall driver implementation to allow for virtualized I/O.
 *
 * A guest must be allowed to access memory address space. Thus, devices can be mapped 
 * to a VMs using the device_mapping proprierty list on the hypervisor's configuration
 * file. Example:
 * 
 * device_mapping = ["PORTH", "UART1"];
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

/**
 * @brief 
 */
//static const struct vmconf_t *vmconf_p[NVMACHINES];


void interrupt_redirect(){
	uint8_t a;
	IFSCLR(146>>5) = 1 << (146 & 31);
	a = U2RXREG;
	putchar('!');
}


/**
 * @brief Driver init call. 
 *  Configure required interrupts. 
 */
void interrupt_redirect_init(){
	uint32_t i, j, sz, *int_redirect, offset;
	vcpu_t *vcpu;
	

	offset = register_interrupt(interrupt_redirect);
    
	/* Create a local list of pointers to the vmconf data structure to avoid
	   excessive number of pointer redirections during the hypercall execution. */
	for(i=0;i<NVMACHINES;i++){
		vcpu = get_vcpu_from_id(i+1);
		//vmconf_p[i] = vcpu->vm->vmconf;
		sz = vcpu->vm->vmconf->interrupt_redirect_sz;
		if(sz > 0){
			int_redirect = vcpu->vm->vmconf->interrupt_redirect;
			for(j=0; j<sz; j++){
				/* Clear the priority and sub-priority */
				IPCCLR(int_redirect[j] >> 2) = 0x1f << (8 * (int_redirect[j] & 0x03));
				
				/* Set the priority and sub-priority */
				IPCSET(int_redirect[j] >> 2) = 0x1f << (8 * (int_redirect[j] & 0x03));
				
				/* Clear the requested interrupt bit */
				IFSCLR(int_redirect[j] >> 5) = 1 << (int_redirect[j] & 31);
				
				/* Set interrupt handler */
				OFF(int_redirect[j]) = offset;
				
				/* Enable the requested interrupt */
				IECSET(int_redirect[j] >> 5) = 1 << (int_redirect[0] & 31);
				
				INFO("Interrupt %d redirected to VM %s", int_redirect[j], vcpu->vm->vm_name);
			}
		}
	}
    
	INFO("Interrupt redirect handler registered at 0x%x", offset);
}

driver_init(interrupt_redirect_init);

