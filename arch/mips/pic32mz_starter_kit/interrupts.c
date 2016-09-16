/*
 * Copyright (c) 2016, prpl Foundation
 * 
 * Permission to use, copy, modify, and/or distribute this software for any purpose with or without 
 * fee is hereby granted, provided that the above copyright notice and this permission notice appear 
 * in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE 
 * FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * This code was written by Carlos Moratelli at Embedded System Group (GSE) at PUCRS/Brazil.
 * 
 */


#include<pic32mz.h>
#include<vcpu.h>
#include<hal.h>
#include<globals.h>
#include <config.h>

static uint32_t tick_count = 0;

uint32_t timer_int_handler(){
    
    uint32_t ret = SUCEEDED;
    
    if (IFS(0) & 0x1000000){
        //curr_vcpu->guestclt2 = (((hal_lr_rcause() & 0x1fc00)>>10) << GUESTCLT2_GRIPL_SHIFT);
        IFSCLR(0) = 0x1000000;
        //putchar('*');
    }
    if (IFS(0) & 0x00004000){
        IFSCLR(0) = 0x00004000;
        /* insert timer interrupt on guest*/
        curr_vcpu->guestclt2 = curr_vcpu->guestclt2 | (3 << GUESTCLT2_GRIPL_SHIFT);
        if ((tick_count++)%QUANTUM_SCHEDULER==0){
            ret = RESCHEDULE;
        }
    }
    
    return ret;
}


/** Hardware interrupt handle */
uint32_t InterruptHandler(){
    uint32_t ret;
    
    /*TODO: Only timer interrupt supported. This must be rewrite due to EIC support. */
    
    ret = timer_int_handler();
    
    /* Check for sw1 button pressed*/
    if (!(PORTB & (1 << 12))) {
        SoftReset();
    }
    
#ifdef ETHERNET_SUPPORT        
        if(tick_count%500==0){
            en_watchdog();
        }
#endif    
        
#ifdef USB_SUPPORT        
        if(tick_count%100==0){
            usb_device_attach();
        }
        
        if(IFS(4) & (1<<4)){
            /* clear global USB interrupt bit */
            IFSCLR(4) = (1<<4);
            usb_int_handler();
        }
#endif
    
    
    return ret;
}
