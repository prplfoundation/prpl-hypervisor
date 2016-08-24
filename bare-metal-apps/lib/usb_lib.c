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
#include <usb_lib.h>

volatile uint32_t device_connected = 0;
uint8_t bufrx[64];

uint32_t wait_device(struct descriptor_decoded *descriptor, uint32_t size){
    while (!device_connected);
    memcpy(descriptor, bufrx, size);
}

void usb_send_data(uint8_t* buf, uint32_t size){
    hyper_usb_send_data(buf, size);
}

void irq_usb(){
    if (!device_connected){
        hyper_usb_get_descr(bufrx, sizeof(bufrx));
        device_connected = 1;
    }else{
        memset(bufrx, 0, sizeof(bufrx));
        device_connected = 0;
    }
}
