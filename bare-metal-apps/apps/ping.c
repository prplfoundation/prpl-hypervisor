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


/*************************************************************
 * Ping-Pong application - Inter-VM communication.
 * To execute the Ping-Pong modify the APP_LIST variable in 
 * the main Makefile to compile the ping.c and pong.c files.
 * Example:
 *      APP_LIST=  ping pong
 * */


#include <pic32mz.h>
#include <libc.h>
#include <network.h>


volatile int32_t t2 = 0;

void irq_timer(){
 t2++;     
}


char message_buffer[128];


int main() {
    int32_t ret, source;
    printf("\nping VM ID %d", hyp_get_guest_id());
    while (1){
        udelay(1000000); /* send a message by second. */        
        sprintf(message_buffer, "%s %d", "ping?", t2);
        ret = SendMessage(2, message_buffer, strlen(message_buffer)+1);
        if (ret<=0){
            print_net_error(ret);
        }else{
            ret = ReceiveMessage(&source, message_buffer, 1);
            if(ret)
                printf("\nping VM: message from VM ID %d: \"%s\" (%d bytes)", source, message_buffer, ret);
        }
    }
    
    return 0;
}

