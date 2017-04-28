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
 * @file uart_driver.c
 * 
 * @section DESCRIPTION
 * 
 * Interrupt-driven virtualized console driver that queues the guest's UART read/write calls.
 * 
 * The guest's call the HCALL_UART_SEND/HCALL_UART_RECV 
 * hypercalls being blocked to wait by UART tranfers to complete
 * when necessary. 
 * 
 */

#include <globals.h>
#include <hal.h>
#include <pic32mz.h>
#include <mips_cp0.h>
#include <libc.h>
#include <hypercall_defines.h>
#include <hypercall.h>
#include <driver.h>
#include <interrupts.h>
#include <pic32mz.h>
#include <platform.h>


/** 
 * @brief UART TX hypercall.
 * Write characters to the hardware queue and block the VCPU when
 * the queue is full and still there are characteres to tranfer. 
 */
static void send(){
	/*TODO: Implement interrupt-driven support to avoid to block the hypervisor for long periods. */
	
	uint32_t i = 0;
	char* str  = (char*)MoveFromPreviousGuestGPR(REG_A0);
	uint32_t size = MoveFromPreviousGuestGPR(REG_A1); 

	/* Map the guest's buffer. */
	char* str_mapped = (char*)tlbCreateEntry((uint32_t)str, vm_in_execution->base_addr, size, 0xf, NONCACHEABLE);
	
	for(i=0; i<size; i++){
		while(!(UART_LSR & 0x40));
		UART_THR = str_mapped[i];   
	}
	
	MoveToPreviousGuestGPR(REG_V0, size);
}

/**
 * @brief UART Driver init call. 
 */
static void uart_driver_init(){
	
	if (register_hypercall(send, HCALL_UART_SEND) < 0){
		ERROR("Error registering the HCALL_GET_VM_ID hypercall");
		return;
	}
	
	printf("\nUART driver enabled.");
}

driver_init(uart_driver_init);

