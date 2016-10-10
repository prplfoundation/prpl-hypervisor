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

/* Simple UART and Blink Bare-metal application sample using virtualized IO. */

#include <arch.h>
#include <libc.h>
#include <hypercalls.h>
#include <guest_interrupts.h>
#include <platform.h>


volatile int32_t t2 = 0;


void irq_timer(){
    t2++;
}

int main() {
    
    interrupt_register(irq_timer, GUEST_TIMER_INT);
    
    write(TRISHCLR, 1);
    
    while (1){
        printf("\nBlink red LED! Total of %d timer ticks.", t2);
        
	/* Blink Led */
	write(LATHINV, 1);
	
        /* 1 second delay */
        udelay(1000000);
   }
    
    return 0;
}

