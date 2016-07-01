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


#include <network.h>
#include <libc.h>

static message_buffer_t message_list;
static mutex_t wait;

void init_network(){
        spinlock(&wait);
}

int ReceiveMessage(int *source, char *message){
        unsigned int size, out;
        
        if(message_list.num_messages == 0){
                spinlock(&wait);
        }
        
        out = message_list.out;
        size = message_list.messages[out].size;

        memcpy(message, message_list.messages[out].message, size);
        *source = message_list.messages[out].source_id;
        
        message_list.out = (out + 1) % MESSAGELIST_SZ;
        message_list.num_messages--;
        
        return size;
}

int SendMessage(unsigned target_id, void* message, unsigned size){
        return hyp_ipc_send_message(target_id, message, size);
}

void irq_network(){
        int ret = 1, in;
        
       // printf("\nirq_network");
        
        if(message_list.num_messages == MESSAGELIST_SZ){
                unlock(&wait);
                return;
        }
        
        in = message_list.in;
        while(ret && message_list.num_messages < MESSAGELIST_SZ){
                ret = message_list.messages[in].size = hyp_ipc_receive_message(&message_list.messages[in].source_id, message_list.messages[in].message);
                if(!ret){
                        unlock(&wait);
                        return;
                }
                in = message_list.in = (in + 1) % MESSAGELIST_SZ;
                message_list.num_messages++;
        }
}
