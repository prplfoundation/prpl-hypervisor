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

#ifdef PICOTCP

#include <eth.h>
#include <libc.h>

static struct eth_message_list_t eth_message_list;

int eth_send(struct pico_device *dev, void *buf, int len){
    return hyper_eth_send(buf, len);
}

int eth_poll(struct pico_device *dev, int loop_score){
    unsigned int size, out;
        
    asm volatile("di");
    if(eth_message_list.num_packets == 0){
        asm volatile("ei");
        return loop_score;
    }
        
    asm volatile("di");
    while(loop_score > 0){
       
        out = eth_message_list.out;
        size = eth_message_list.ringbuf[out].size;
        
        pico_stack_recv(dev, eth_message_list.ringbuf[out].packet, size);
        
        eth_message_list.out = (out + 1) % ETH_MESSAGELIST_SZ;
        eth_message_list.num_packets--;
        
        loop_score--;
    }
        
    asm volatile("ei");
        
    return loop_score;
}


void eth_irq_network(){
        int ret = 1, in, flag=0;
        
        if(eth_message_list.num_packets == ETH_MESSAGELIST_SZ){
                return;
        }
        in = eth_message_list.in;

        while(ret && eth_message_list.num_packets < ETH_MESSAGELIST_SZ){
                ret = eth_message_list.ringbuf[in].size = hyper_eth_recv(eth_message_list.ringbuf[in].packet);
                if(ret==MESSAGE_EMPTY){
                    return;
                }
                in = eth_message_list.in = (in + 1) % ETH_MESSAGELIST_SZ;
                eth_message_list.num_packets++;
        }
}

#endif




