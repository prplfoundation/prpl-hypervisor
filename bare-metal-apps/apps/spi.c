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

void irq_timer(){
 t2++;     
}


/* SP1 configuration */
void setupSPI1(){
    uint32_t rData;
    
    SPI1CON = 0;
    rData=SPI1BUF;
    SPI1BRG=0x1;
    SPI1STATCLR=0x40;
    SPI1CON=0x8220; // use F PB /4 clock frequency
}

int main() {
    /* Select output serial 2 = UART2, 6 = UART6 */
    serial_select(UART2);
    
    printf("\nConfiguring SPI.");
    
    setupSPI1();
    
    while (1){
        printf("\nInt count: %d", t2);
        udelay(1000000);
        SPI1BUF='A';  
   }
    
    return 0;
}

