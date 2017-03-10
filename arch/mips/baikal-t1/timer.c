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
 * @file timer.c
 * 
 * @section DESCRIPTION
 * 
 * Timer interrupt subsystem. This timer is used for VCPU scheduling and  
 * virtual timer interrupt injection on Guests. 
 * 
 * Every guest receive timer interrupt each 1ms. This will be replaced soon with 
 * a configurable guest timer. 
 */

#include <globals.h>
#include <hal.h>
#include <pic32mz.h>
#include <mips_cp0.h>
#include <guest_interrupts.h>
#include <scheduler.h>
#include <interrupts.h>
#include <libc.h>

#define SYSTEM_TICK_INTERVAL (SYSTEM_TICK_US * MICROSECOND)
#define QUEST_TICK_INTERVAL (GUEST_QUANTUM_MS * MILISECOND)


/**
 * @brief Configures the COMPARE register to the next interrupt. 
 * 
 * @param interval Time interval to the next interrupt in CPU ticks (CPU_FREQ/2)
 */
inline void calc_next_timer_interrupt(uint32_t interval){
	uint32_t count;
	
	count = mfc0(CP0_COUNT, 0);
	count += interval;
	mtc0(CP0_COMPARE, 0, count);
	
	IFSCLR(0) = 1;  
}

/**
 * @brief Time interrupt handler.
 * 
 * Perfoms VCPUs scheduling and virtual timer interrupt injection on guests. 
 */
static void timer_interrupt_handler(){
	static uint32_t past = 0;
	uint32_t now, diff_time;
	
	calc_next_timer_interrupt(SYSTEM_TICK_INTERVAL);
	
	run_scheduler();
	
	now = mfc0(CP0_COUNT, 0);
	if (now >= past)
		diff_time = now - past;
	else
		diff_time = 0xffffffff - (past - now);

	/* Insert a virtual timer interrupt to the guest each other timer tick. */
	if(diff_time >= QUEST_TICK_INTERVAL){
		setGuestCTL2(getGuestCTL2() | (GUEST_TIMER_INT << GUESTCLT2_GRIPL_SHIFT));
		past = now;
	}
    
}

/**
 * @brief Configures the CP0 timer.
 * 
 * This function will never return to the calling code. It waits for the
 * first timer interrupt.
 */
void start_timer(){
	uint32_t temp_CP0, offset;
    
	offset = register_interrupt(timer_interrupt_handler);
	INFO("CP0 Timer interrupt registered at 0x%x.", offset);
        printf("\n0x%x\n", mfc0(16, 3));
        
        /* Stop counter */
        GIC_SH_CONFIG |= GIC_SH_CONFIG_COUNTSTOP;
        
       // GIC_CL_RMASK = GIC_CL_TIMER_MASK;
         GIC_CL_RMASK = 0xffffffff;
        
        GIC_SH_COUNTERLO = 0;
        GIC_SH_COUNTERHI = 0;
        
        GIC_CL_COMPARELO = 0xffffffff;
        GIC_CL_COMPAREHI = 1;
        
        GIC_SH_SMASK31_0 = 0xffffffff; /* GIC local 64-bit timer interrupt */
  //      GIC_SH_MAP2_PIN = 0x10000002;
        
        INFO("Starting hypervisor execution.\n");

        //GIC_CL_SMASK = GIC_CL_TIMER_MASK;
        GIC_CL_SMASK = 0xffffffff;
        
        GIC_CL_COREi_TIMER_MAP = 0x80000002;
        GIC_CL_COREi_COMPARE_MAP = 0x80000002; 
        GIC_SH_MAP2_PIN = 0x80000002;
        GIC_SH_MAP2_CORE = 2;
        
	asm volatile ("ei");    
        
        /* Start counter */
        GIC_SH_CONFIG &= ~GIC_SH_CONFIG_COUNTSTOP;
    
	/* Wait for a timer interrupt */
        offset = 0;
	while(1){
            offset++;
            if(offset%90000000 == 0){
                printf("0x%x\n", GIC_SH_PEND31_0);
                printf("0x%x\n", mfc0(CP0_STATUS, 0));
                printf("0x%x\n", GIC_SH_COUNTERLO);
                printf("0x%x\n", mfc0(CP0_CAUSE, 0));
            }
        };
}

