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

#define IDPRODUCT 0
#define IDVENDOR 0x1267

#define MILISECOND (100000000/ 1000)

volatile int32_t t2 = 0;
uint8_t tx[3] = {0, 0, 0};

void irq_timer(){
    t2++;
}

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
    uint32_t tm_poll = 0, tm_blink = 0;
    
    /* Pin RH0 as ouput (LED 1)*/
    TRISHCLR = 1;
    
    printf("\nPlease connect the OWI Robotic Arm to the USB.");
    
    while (1){
        
        if(calc_wait_time(tm_poll, 100)){
            ret = hyper_usb_polling();
            if (ret != old){
                if(ret){
                    hyper_usb_get_descr((char*)&descriptor, sizeof(struct descriptor_decoded));
                    printf("\nDevice connected: idVendor 0x%x idProduct 0x%x", descriptor.idVendor, descriptor.idProduct);
                }else{
                    printf("\nDevice Disconnected");
                }
                old = ret;
            }
            tm_poll = mfc0(CP0_COUNT, 0);
        }
            
        if(calc_wait_time(tm_blink, 1000)){            
            /* Blink Led */
            LATHINV = 1;
            tm_blink = mfc0(CP0_COUNT, 0);
        }
    }

            /* Robotic Arm Blink Led */
            /*tx[2] = ~tx[2];
            usb_send_data(tx, 3);*/
    return 0;
}

