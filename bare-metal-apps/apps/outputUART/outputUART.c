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

#include <arch.h>
#include <libc.h>


volatile int32_t t2 = 0;

void irq_timer(){
 t2++;     
}


uint8_t buffer[32];

void printf_buffer(uint8_t *buffer, uint32_t size){
    uint32_t i;
    for(i=0;i<size;i++){
        putchar(buffer[i]);
    }
}

int main() {
    uint32_t ret, source;
    uint32_t a;
    
    uint32_t guestid = hyp_get_guest_id();
    
    /* Select output serial 2 = UART2, 6 = UART6 */
    serial_select(UART6);
    printf("\n\routputUART Guest ID %d. Waiting message.");
    while (1){
        ret = ReceiveMessage(&source, buffer, sizeof(buffer), 0);
        if (ret>0){
            printf("\n\routputUART Guest ID %d - size %d ", guestid, ret);
            printf_buffer(buffer, ret);
        }
   }
    
    return 0;
}

