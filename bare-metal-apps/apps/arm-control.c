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

/* Simple UART Bare-metal application sample */

#include <pic32mz.h>
#include <libc.h>


volatile int32_t t2 = 0;

/* interrupts inserted by the hypervisor */
void _irq_handler(uint32_t status, uint32_t cause)
{
    uint32_t temp_CP0;
    temp_CP0 = mfc0(CP0_CAUSE, 0);
    temp_CP0 &= ~0x3FC00;
    mtc0(CP0_CAUSE, 0, temp_CP0);
    t2++;
}

void udelay(uint32_t usec){
    uint32_t now = mfc0 (CP0_COUNT, 0);
    uint32_t final = now + usec * (CPU_SPEED / 1000000) / 2;

    for (;;) {
        now = mfc0 (CP0_COUNT, 0);
        if ((int32_t) (now - final) >= 0) break;
    }
}

void putchar(int32_t value){
    while(U2STA & USTA_UTXBF);
    U2TXREG = value;    
}

int32_t kbhit(void){
        return (U2STA & USTA_URXDA);
}

int32_t getchar(void){
    while(!kbhit());
    return (uint8_t)U2RXREG;
}


int main() {
    unsigned int temp_CP0;
    int i, j;


   /* configure the interrupt controller to compatibility mode */
    asm volatile("di");         /* Disable all interrupts */
    mtc0 (CP0_EBASE, 1, 0x9d000000);    /* Set an EBase value of 0x9D000000 */
    temp_CP0 = mfc0(CP0_CAUSE, 0);      /* Get Cause */
    temp_CP0 |= CAUSE_IV;           /* Set Cause IV */
    mtc0(CP0_CAUSE, 0, temp_CP0);       /* Update Cause */
 
    temp_CP0 = mfc0(CP0_STATUS, 0);     /* Get Status */
    temp_CP0 &= ~STATUS_BEV;        /* Clear Status IV */
    mtc0(CP0_STATUS, 0, temp_CP0);      /* Update Status */
    
    temp_CP0 = mfc0(12,1); /* intCTL IV must be different of 0 to allow EIC mode. */
    temp_CP0 |= 8<<5;
    mtc0(12, 1, temp_CP0);
    
    
    asm volatile ("ei");
    
    while(1){
        printf("\nMove robot: %d", t2);
        udelay(1000000);
    }
    
    return 0;
}

