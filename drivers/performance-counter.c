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
 * @file performance-counter.c
 * 
 * @section DESCRIPTION
 * 
 * Allows for the guests to control the performance counters. 
 *
 * Used for performance/benchmarks measurements. 
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
#include <pic32mz.h>


/**
 * @brief Start performance counter.
 * 
 * a0: performance conter control 0 configuration.
 * a1: performance conter control 1 configuration.
 * v0: returns always 0. 
*/
void performance_counter_start(){
	uint32_t control0 = MoveFromPreviousGuestGPR(REG_A0);
	uint32_t control1 = MoveFromPreviousGuestGPR(REG_A1);
	
	mtc0(CP0_PERFCTL0, 1, 0);
	mtc0(CP0_PERFCTL1, 3, 0);
	
	mtc0(CP0_PERFCTL0, 0, control0);
	mtc0(CP0_PERFCTL1, 2, control1);
	
	MoveToPreviousGuestGPR(REG_V0, 0);
}

/**
 * @brief Stop performance counter.
 * a0: destination buffer.
 * v0: returns always 0.
 */
void performance_counter_stop(){
	uint32_t memory_addr = MoveFromPreviousGuestGPR(REG_A0);

	uint32_t* ptr_mapped = (uint32_t*)tlbCreateEntry(memory_addr, vm_in_execution->base_addr, 2*sizeof(uint32_t), 0xf, CACHEABLE);

	ptr_mapped[0] = mfc0(CP0_PERFCTL0, 1);
	ptr_mapped[1] = mfc0(CP0_PERFCTL1, 3);
	
	mtc0(CP0_PERFCTL0, 0, 0);
	mtc0(CP0_PERFCTL1, 2, 0);

	MoveToPreviousGuestGPR(REG_V0, 0);
}


/**
 * @brief Driver init call.  Registers the hypercalls. 
 */
void performance_init(){

	if (register_hypercall(performance_counter_start, HCALL_PERFORMANCE_COUNTER_START) < 0){
		ERROR("Error registering the HCALL_PERFORMANCE_COUNTER_START hypercall");
		return;
	}
    
	if (register_hypercall(performance_counter_stop, HCALL_PERFORMANCE_COUNTER_STOP) < 0){
		ERROR("Error registering the HCALL_PERFORMANCE_COUNTER_STOP hypercall");
		return;
	}
    
	INFO("Performance counter driver registered.");
}

driver_init(performance_init);

