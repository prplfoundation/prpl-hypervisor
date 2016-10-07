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
#include <hypercalls.h>

#define ETH_FRAME_SZ 1518

#define MILISECOND (100000000/ 1000)

uint8_t frame_buf[ETH_FRAME_SZ];

static uint32_t link_state = 0;

uint32_t calc_wait_time(uint32_t time, uint32_t ms_delay){
    uint32_t now = mfc0(CP0_COUNT, 0);
    if ( (now - time) > (ms_delay * MILISECOND)){
        return 1;
    }
    return 0;
}


int eth_link_state(struct pico_device *dev){
    return link_state;
}

void eth_watchdog(uint32_t *time, uint32_t ms_delay){
    if (calc_wait_time(*time, ms_delay)){
	    link_state = eth_watch();
            *time = mfc0(CP0_COUNT, 0);
    }
}

void eth_get_mac(uint8_t *mac){
	eth_mac(mac);
}

int eth_send(struct pico_device *dev, void *buf, int len){
	return eth_send_frame(buf, len);
}

int eth_poll(struct pico_device *dev, int loop_score){
    int32_t size;
    
    while(loop_score > 0){
        
	size = eth_recv_frame(frame_buf);
        if (size<=0){
            break;
        }
        pico_stack_recv(dev, frame_buf, size);
        loop_score--;
    }
    
    return loop_score;
}

#endif




