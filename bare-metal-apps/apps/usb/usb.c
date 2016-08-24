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

/* Simple UART and Blink Bare-metal application sample */

#include <pic32mz.h>
#include <libc.h>
#include <usb_lib.h>


volatile int32_t t2 = 0;
uint8_t tx[3] = {0, 0, 0};

void irq_timer(){
    t2++;
}

struct descriptor_decoded descriptor;

int main() {
    
    /* Pin RH0 as ouput (LED 1)*/
    TRISHCLR = 1;
    
    uint32_t guest_id = hyp_get_guest_id();
    
    /* register this VM for USB interrupts */
    hyper_usb_vm_register(guest_id);
    
    printf("\nVM#%d: Please connect the OWI Robotic Arm to the USB.", guest_id);
    
    wait_device(&descriptor, sizeof(descriptor));
    
    printf("\nUSB Device connected: idVendor 0x%04x idProduct 0x%04x ", descriptor.idVendor, descriptor.idProduct);
        
    while (1){
        /* Blink Led */
        LATHINV = 1;

        /* Robotic Arm Blink Led */
        tx[2] = ~tx[2];
        usb_send_data(tx, 3);

        /* 1 second delay */
        udelay(1000000);
        
   }
    
    return 0;
}

