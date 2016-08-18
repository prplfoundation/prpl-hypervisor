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

/* Simple malloc tester application sample */

#include <pic32mz.h>
#include <libc.h>
#include <malloc.h>

extern _heap_size;
extern _heap_start;

volatile int32_t t2 = 0;

void irq_timer(){
    t2++;
}

int main() {
    
    uint8_t* buffer;
    uint8_t* buffer2;
    uint32_t  i;
    int32_t  ret;
    printf("Heap size %d bytes at 0x%x.\n", &_heap_size, &_heap_start);

    while (1){
        buffer = (uint8_t*)malloc(2048);
        buffer2 = (uint8_t*)malloc(2048);
        for(i=0;i<2048;i++)
            buffer[i]=i;
        memcpy(buffer2, buffer, 2048);
        ret = memcmp(buffer, buffer2, 2048);
        if(!ret)
            printf("\nWorking!");
        else
            printf("\nError!");
        free(buffer);
        free(buffer2);
   }
    
    return 0;
}

