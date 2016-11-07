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

uint32_t getbuffer(uint8_t *buffer, uint32_t size){
    uint8_t a;
    uint32_t i;
    for(i=0;i<size;i++){
        buffer[i]=getchar();
        putchar(buffer[i]);
        if(buffer[i]=='\r'){
            break;
        }
    }
    return i;
}

int main() {
    uint32_t a, ret, count;
    uint32_t guestid = hyp_get_guest_id();
    
    /* Select output serial 2 = UART2, 6 = UART6 */
    serial_select(UART2);
    
    while (1){
        printf("\n\rInputUART Guest ID %d - press any key plus ENTER ...", guestid);
        ret = getbuffer(buffer, sizeof(buffer));
        if (ret)
            SendMessage(2, buffer, ret);
   }
    
    return 0;
}

