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


#define TIMER 1 
#define NETWORK 2
#define USB  4

/* interrupts inserted by the hypervisor */
void _irq_handler(uint32_t status, uint32_t cause)
{
    /* extract ripl field */
    uint32_t ripl = (cause & 0x3FC00) >> 10 >> 1;
    
    if (ripl & TIMER){
        irq_timer();
    }
    
    if (ripl & NETWORK){
        irq_network();
    }
    
    if (ripl & USB){
        irq_usb();
    }
   
}


void init_proc(){
    unsigned int temp_CP0;

   /* configure the interrupt controller to compatibility mode */
    asm volatile("di");         /* Disable all interrupts */
    mtc0 (CP0_EBASE, 1, 0x9d000000);    /* Set an EBase value of 0x9D000000 */
    temp_CP0 = mfc0(CP0_CAUSE, 0);      /* Get Cause */
    temp_CP0 |= CAUSE_IV;           /* Set Cause IV */
    mtc0(CP0_CAUSE, 0, temp_CP0);       /* Update Cause */
 
    temp_CP0 = mfc0(CP0_STATUS, 0);     /* Get Status */
    temp_CP0 &= ~STATUS_BEV;        /* Clear Status BEV */
    mtc0(CP0_STATUS, 0, temp_CP0);      /* Update Status */
    
    temp_CP0 = mfc0(12,1); /* intCTL IV must be different of 0 to allow EIC mode. */
    temp_CP0 |= 8<<5;
    mtc0(12, 1, temp_CP0);

    asm volatile ("ei");
    
}


