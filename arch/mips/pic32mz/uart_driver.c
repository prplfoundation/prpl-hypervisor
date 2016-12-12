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
 * @struct 
 * @brief Keep the status of queued UART transfers.
*/
struct transfer_info{
	uint32_t buf_addr; /**< Guest's buffer address */
	uint32_t next_character; /**< Next character to transmit  */
	uint32_t size; /**< buffer size */
	vcpu_t* vcpu; /**< VCPU owner */ 
};

/** Tranfer status for each VCPU. */
static struct transfer_info transfers[NVMACHINES];

/** Active tranfer */
static volatile int32_t active_transfer = -1;

/**
 * @brief Schedule a UART TX transfer. Block the VCPU and enable UART TX interrupt. 
 * @param addr Transmit buffer on guest. 
 * @param next_character Next character to transmit. 
 * @param size Buffer size. 
 */
static void scheduler_transfer_interrupt(uint32_t addr, uint32_t next_character, uint32_t size){
	uint32_t j = vcpu_in_execution->id - 1;
	
	if(active_transfer < 0){
		/* Enable the requested interrupt */
		IECSET(UART_IRQ_TX >> 5) = 1 << (UART_IRQ_TX & 31);
		
		active_transfer = j;
	}
	
	transfers[j].vcpu = vcpu_in_execution;
	transfers[j].next_character = next_character;
	transfers[j].size = size;
	transfers[j].vcpu->state = VCPU_BLOCKED;
	transfers[j].buf_addr = addr;
}

/**
 * @brief UART TX Interrupt handler routine. 
 */
static void transfer_tx_done(){
	uint32_t j = active_transfer;
	
	/* Clear TX interrupt flag */
	IFSCLR(UART_IRQ_TX >> 5) = 1 << (UART_IRQ_TX & 31);
	
	/* Map the guest's buffer. */
	char* str_mapped = (char*)tlbCreateEntry(transfers[j].buf_addr, transfers[j].vcpu->vm->base_addr, transfers[j].size, 0xf, CACHEABLE);
	
	/* Write characters to the hardware queue. */
	for (; transfers[j].next_character < transfers[j].size && !(UARTSTAT&USTA_UTXBF); transfers[j].next_character++){
		UARTTXREG = str_mapped[transfers[j].next_character];
	}

	/* If the transfer is finished unblock the VCPU and schedule the 
	 next tranfer if necessary. */
	if(transfers[j].next_character >= transfers[j].size){
		transfers[j].vcpu->state = VCPU_RUNNING;
		transfers[j].vcpu = NULL;

		for(j=0; j<NVMACHINES; j++){
			if(transfers[j].vcpu != NULL){
				active_transfer = j;
				return;
			}
		}
		
		IECCLR(UART_IRQ_TX >> 5) = 1 << (UART_IRQ_TX & 31);
		active_transfer = -1;
	}
}

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
	char* str_mapped = (char*)tlbCreateEntry((uint32_t)str, vm_in_execution->base_addr, size, 0xf, CACHEABLE);
	
	/* Write to the hardware queue. */
	if(active_transfer<0){
		for(i=0; i<size && !(UARTSTAT&USTA_UTXBF); i++){
			UARTTXREG = str_mapped[i];
		}
	}
	
	/* UART TX queue is full. Enable interrupt tranfer mode.  */
	if(i<size){
		scheduler_transfer_interrupt((uint32_t)str, i, size);
	}

	MoveToPreviousGuestGPR(REG_V0, size);
}

/**
 * UART receive hypercall.
 * 
 */
/*static void recv(){
	TODO: Implement interrupt-driven receiver driver 
}*/


/**
 * @brief UART Driver init call. 
 */
static void uart_driver_init(){
	uint32_t offset;
	
	if (register_hypercall(send, HCALL_UART_SEND) < 0){
		ERROR("Error registering the HCALL_GET_VM_ID hypercall");
		return;
	}
	
	if ((offset = register_interrupt(transfer_tx_done)) == 0){
		WARNING("Error on UART interrupt registration.");
		return;
	}
	
	OFF(UART_IRQ_TX) = offset;
	
	/* Interrupt is generated and asserted when all characters have been transmitted */
	UARTSTAT = (UARTSTAT & ~(3 << 14)) | 1 << 14;
	
	/* Clear the priority and sub-priority */
	IPCCLR(UART_IRQ_TX >> 2) = 0x1f << (8 * (UART_IRQ_TX & 0x03));
		
	/* Set the priority and sub-priority */
	IPCSET(UART_IRQ_TX >> 2) = 0x1f << (8 * (UART_IRQ_TX & 0x03));
	
	/* Clear the requested interrupt bit */
	IFSCLR(UART_IRQ_TX >> 5) = 1 << (UART_IRQ_TX & 31);

	memset(transfers, 0, sizeof(transfers));
	
	printf("\nUART driver enabled. Interrupt vector 0x%x", offset);
}

driver_init(uart_driver_init);

