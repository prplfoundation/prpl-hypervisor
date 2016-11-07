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

/**
 * @file eth.c
 * 
 * @section DESCRIPTION
 * 
 * Ethernet virtualized driver for picoTCP stack implementing the polling method.
 * 
 */

#ifdef PICOTCP

#include <eth.h>
#include <libc.h>
#include <hypercalls.h>
#include <platform.h>

#define ETH_FRAME_SZ 1518

/** 
 * @brief Temporary buffer.
 */
uint8_t frame_buf[ETH_FRAME_SZ];

/** 
 * @brief Ethernet link state.
 */
static uint32_t link_state = 0;

/** 
 * @brief PicoTCP link state checker function.
 * @param dev picoTCP device.
 * @return 0 for down or 1 for link up.
 */
int eth_link_state(struct pico_device *dev){
    return link_state;
}

/** 
 * @brief Ethernet watchdog. Must be pulled around 500ms to check the link state. 
 *  If the link is up, it enables the packet reception. 
 * @param time Old time.
 * @param ms_delay Time interval. 
 */
void eth_watchdog(uint32_t *time, uint32_t ms_delay){
    if (wait_time(*time, ms_delay)){
	    link_state = eth_watch();
            *time = mfc0(CP0_COUNT, 0);
    }
}

/** 
 * @brief PicoTCP send packet function. 
 * @param dev picoTCP device. 
 * @param buf Frame buffer.
 * @param len frame size.
 * @return number of bytes sent. 
 */
int eth_send(struct pico_device *dev, void *buf, int len){
	return eth_send_frame(buf, len);
}

/** 
 * @brief PicoTCP poll function. Perform polling for packet reception.
 * @param dev picoTCP device. 
 * @param loop_score Polling control. 
 * @return Updated value of loop_score.
 */
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




