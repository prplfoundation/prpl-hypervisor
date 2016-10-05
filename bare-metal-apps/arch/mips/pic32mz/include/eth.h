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

#ifndef ETH_H
#define ETH_H

#ifdef PICOTCP

#include <arch.h>
#include <libc.h>
#include <network.h>

#include "pico_defines.h"
#include "pico_stack.h"



/* Ethernet Send/Receive */

int32_t hyper_eth_send(void *buf, int len);
int32_t hyper_eth_poll(void *buf, int len);
int32_t eth_link_state(struct pico_device *dev);
void eth_get_mac(uint8_t *mac);


#define ETH_MESSAGE_SZ 1536
#define ETH_MESSAGELIST_SZ 5

struct eth_message_t{
        uint32_t size; /* size of each message in message_list */
        uint8_t packet[ETH_MESSAGE_SZ];
};


struct eth_message_list_t{
        uint32_t in;
        uint32_t out;
        volatile uint32_t num_packets;
        struct eth_message_t ringbuf[ETH_MESSAGELIST_SZ];
};


int eth_send(struct pico_device *dev, void *buf, int len);
int eth_poll(struct pico_device *dev, int loop_score);
    
#endif
#endif

