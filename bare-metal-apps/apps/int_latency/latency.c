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

/* This example is used to measure the hypervisor's interrupt redirection latency.
 * 
 * 
 */

#include <arch.h>
#include <libc.h>
#include <hypercalls.h>
#include <guest_interrupts.h>
#include <platform.h>
#include <io.h>

volatile uint32_t t2 = 0;

void irq_pin(){
	uint32_t d;
	
	d = readio(PORTD);
	if(d & (1<<10)){
		writeio(LATFSET, 1 << 4);
	}else{
		writeio(LATFCLR, 1 << 4);
	}
	
	t2++;
	
	reenable_interrupt(GUEST_USER_DEFINED_INT_1);
}


int main() {
	uint32_t timer = 0;
    
	interrupt_register(irq_pin, GUEST_USER_DEFINED_INT_1);
	
	ENABLE_LED1;
	
	/* RD0 as output*/
	writeio(TRISFCLR, 1 << 4);
	writeio(LATFCLR, 1 << 4);
	writeio(CNPUFCLR, 1 << 4);
	writeio(CNPDFCLR, 1 << 4);
	writeio(ANSELFCLR, 1 << 4);
	
	/* configure interrupt for PORTD pin RD10*/
	writeio(CNCONDSET, 0x8000);
	writeio(CNENDSET, 1 << 10);
   
	while (1){
		if(wait_time(timer, 1000)) {
			/* Blink Led */
			TOGGLE_LED1;
			
			timer = mfc0(CP0_COUNT, 0);
			
		}
	}
    
	return 0;
}

