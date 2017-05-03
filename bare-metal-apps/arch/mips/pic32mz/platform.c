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

#include <arch.h>
#include <types.h>
#include <guest_interrupts.h>
#include <malloc.h>
#include <libc.h>

#define NUM_GUEST_INTERRUPTS 10

typedef void interrupt_handler_t();

static interrupt_handler_t * interrupt_handlers[NUM_GUEST_INTERRUPTS] = {[0 ... NUM_GUEST_INTERRUPTS-1] = NULL};

/**
 * @brief Interrupt registration. 
 *   	Register interruts rotines. The valid interrupt numbers are
 *   	in the guest_interrupts.h file. 
 */
uint32_t interrupt_register(interrupt_handler_t *handler, uint32_t interrupt){
	uint32_t i;
	
	for(i=0; i<NUM_GUEST_INTERRUPTS; i++){
		if(interrupt & (1<<i)){
			if (interrupt_handlers[i] == NULL){
				interrupt_handlers[i] = handler;
				return (uint32_t)handler;
			}
		}
	}
		
	return 0;
}

/**
 * @brief General exception routine.
 *   All interrupts or exceptions invoke this routine. Call the 
 *   function handler corresponding to the RIPL field.
 */
void _irq_handler(uint32_t status, uint32_t cause){
	uint32_t i = 0;

	/* extract RIPL field */
	uint32_t ripl = (cause & 0x3FC00) >> 10;
	
	do{
		if(ripl & 1){
			if (interrupt_handlers[i]){
				((interrupt_handler_t*)interrupt_handlers[i])();
			}
		}
		i++;
		ripl >>= 1;
	}while(ripl);
	
}


/**
 * @brief Processor configuration. Called early during the initilization. 
 * 
 */
void init_proc(){
	unsigned int temp_CP0;

	/* configure the interrupt controller to compatibility mode */
	asm volatile("di");         		/* Disable all interrupts */
	mtc0 (CP0_EBASE, 1, 0x9d000000);    	/* Set an EBase value of 0x9D000000 */
	temp_CP0 = mfc0(CP0_CAUSE, 0);      	/* Get Cause */
	temp_CP0 |= CAUSE_IV;           	/* Set Cause IV */
	mtc0(CP0_CAUSE, 0, temp_CP0);       	/* Update Cause */
 
	temp_CP0 = mfc0(CP0_STATUS, 0);     	/* Get Status */
	temp_CP0 &= ~STATUS_BEV;        	/* Clear Status BEV */
	mtc0(CP0_STATUS, 0, temp_CP0);      	/* Update Status */
    
	temp_CP0 = mfc0(CP0_INTCTL,1); 			/* configure intCTL IV */
	temp_CP0 |= 8<<INTCTL_VS_SHIFT;
	mtc0(CP0_INTCTL, 1, temp_CP0);
    
	memset(interrupt_handlers, 0, sizeof(interrupt_handlers));

	asm volatile ("ei");
    
}


/**
 * @brief Determines if a time period was consumed.
 * @param old_time Initial time.
 * @param ms_delay Time period.
 * @return 0 if the period was not consumed, 1 otherwise.
 */
uint32_t wait_time(uint32_t old_time, uint32_t ms_delay){
	uint32_t diff_time;
	uint32_t now = mfc0(CP0_COUNT, 0);
    
	if (now >= old_time)
		diff_time = now - old_time;
	else
		diff_time = 0xffffffff - (old_time - now);

	if(diff_time > (ms_delay * MILISECOND)){
		return 1;
	}
	
	return 0;
}



