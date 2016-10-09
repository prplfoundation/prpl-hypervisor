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

/* Simple USB Bare-metal application that detects and read device's descriptors. */

#include <arch.h>
#include <libc.h>
#include <usb_lib.h>
#include <guest_interrupts.h>
#include <hypercalls.h>

#define IDPRODUCT 0
#define IDVENDOR 0x1267

#define MILISECOND (100000000/ 1000)

struct descriptor_decoded descriptor;

uint32_t calc_wait_time(uint32_t time, uint32_t ms_delay){
    uint32_t now = mfc0(CP0_COUNT, 0);
    if ( (now - time) > (ms_delay * MILISECOND)){
        return 1;
    }
    return 0;
}

int main() {
    int32_t ret = 0, old = 0;
    uint32_t tm_poll = 0;
    
    printf("\nPlease any USB device.");
    
    while (1){
        
        if(calc_wait_time(tm_poll, 100)){
            ret = usb_polling();
            if (ret != old){
                if(ret){
                    usb_device_descriptor((char*)&descriptor, sizeof(struct descriptor_decoded));
                    printf("\nDevice connected: idVendor 0x%x idProduct 0x%x", descriptor.idVendor, descriptor.idProduct);
                }else{
                    printf("\nDevice Disconnected");
                }
                old = ret;
            }
            tm_poll = mfc0(CP0_COUNT, 0);
        }
    }

    return 0;
}

