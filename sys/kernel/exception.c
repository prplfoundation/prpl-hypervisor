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
 * @file exception.c
 * 
 * @section DESCRIPTION
 * 
 * General exception handler implementation.  
 * 
 */


#include <types.h>
#include <hal.h>
#include <config.h>
#include <exception.h>
#include <libc.h>
#include <globals.h>
#include <mips_cp0.h>
#include <hypercall.h>
#include <board.h>
#include <scheduler.h>

/**
 * @brief Guest exit exception handler. 
 * 
 */
static uint32_t guest_exit_exception(){
	uint32_t guestcause = getGCauseCode();
	uint32_t epc = getEPC();
	
	switch (guestcause) {
		case GUEST_INSTRUCTION_EMULATION:	
			instruction_emulation(epc);
			break;
		case GUEST_HYPERCALL:
			hypercall_execution();			
			break;
		default:
			WARNING("Guest exit cause code %d not supported.", guestcause);
			break;
	}
	
	vcpu_in_execution->pc = epc+4;

	setEPC(vcpu_in_execution->pc);	

	if(vcpu_in_execution->state == VCPU_BLOCKED){
		run_scheduler();
	}
	
	return 0;
}

/**
 * @brief General exception handler. 
 * 
 */
void general_exception_handler(){
	uint32_t CauseCode = getCauseCode();

	switch (CauseCode){
		case    GUEST_EXIT_EXCEPTION:   
				guest_exit_exception();
				break;
		/* TLB load, store or fetch exception */
		case    TLB_LOAD_FETCH_EXCEPTION:                                            
		case    TLB_STORE_EXCEPTION:
		default:
			CRITICAL("VM will be stopped due to error Cause Code 0x%x, EPC 0x%x, VCPU ID 0x%x", CauseCode, getEPC(), vcpu_in_execution->id);
			break;
	}
}
