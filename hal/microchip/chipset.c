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

This code was written by Sergio Johann at Embedded System Group (GSE) at PUCRS/Brazil.

*/

#include <globals.h>
#include <config.h>
#include <hal.h>
#include "pic32mz.h"
#include "ethernet.h"

PIC32MZ_DEVCFG (
    _DEVCFG0_JTAG_DISABLE |      /* Disable JTAG port */
    _DEVCFG0_TRC_DISABLE,        /* Disable trace port */
    _DEVCFG1_FNOSC_SPLL |        /* System clock supplied by SPLL */
    _DEVCFG1_POSCMOD_DISABLE |   /* Primary oscillator disabled */
    _DEVCFG1_CLKO_DISABLE,       /* CLKO output disable */

    _DEVCFG2_FPLLIDIV_1 |        /* PLL input divider = 1 */
    _DEVCFG2_FPLLRNG_5_10 |      /* PLL input range is 5-10 MHz */
    _DEVCFG2_FPLLICLK_FRC |      /* Select FRC as input to PLL */
    _DEVCFG2_FPLLMULT(50) |      /* PLL multiplier = 50x */
    _DEVCFG2_FPLLODIV_2,         /* PLL postscaler = 1/2 */
    _DEVCFG3_FETHIO |            /* Default Ethernet pins */
    _DEVCFG3_USERID(0xffff));    /* User-defined ID */


void hardware_config(){
    TRISBSET =  (1 << 12);     /* SW1 - RB12 (active low) */
    CNPUB =     (1 << 12);     /* enable pull-up */
    
    /* SPI1 pin map */
    ANSELBCLR = 0x0008; /* pin B3 used as output for CS */
    TRISBCLR = 0x0008;
    TRISDCLR = 0x0002;  /* pin D1 used as output for SCLK */
    TRISFCLR = 0x0020;  /* pin F5 used as output for MOSI */
    TRISFSET = 0x0010;  /* pin F4 used as input for MISO */
    LATFSET = 0x0010;

    SDI1R = 2;  /* pin F4 as SPI1 data input */
    RPF5R = 5;  /* pin F5 as SPI1 data output */

    /* SPI config settings */

    SPI1BRG = 4; /* Set clock divider to selected_clock/10: selected_clk/(2*(4+1)) */
    SPI1CON = 0x8120; /* enable SPI / master mode / data transition from high to low clk */
}



uint32_t tick_count = 0;

void configure_timer(){
    unsigned int temp_CP0;

    /* All interrupt levels are handled at GPR shadow 7 */
    /* Other GPR shadows are used for VM execution. */
    PRISS = 7<<28 | 7<<24 | 7<<20 | 7<<16 | 7<<12 | 7<<8 | 7<<4;
    
    Info("Configuring Timer");

    mtc0 (CP0_EBASE, 1, 0x9d000000);    /* Set an EBase value of 0x9D000000 */
    temp_CP0 = mfc0(CP0_CAUSE, 0);      /* Get Cause */
    temp_CP0 |= CAUSE_IV;           /* Set Cause IV */
    mtc0(CP0_CAUSE, 0, temp_CP0);       /* Update Cause */
    INTCONSET = INTCON_MVEC;        /* Set the MVEC bit - Vetored interrupt mode. */
    temp_CP0 = mfc0(CP0_STATUS, 0);     /* Get Status */
    temp_CP0 &= ~STATUS_BEV;        /* Clear Status IV */
    temp_CP0 &= ~STATUS_EXL; 
    mtc0(CP0_STATUS, 0, temp_CP0);      /* Update Status */
    
    /* Interrupt 14 (timer 3) handled at 0x9d000200 */
    OFF(14) = 0x200;

    /* Using PIC32's timers 2/3 for 32bit counter*/
    T2CON = 0;
    TMR2 = 0x0;
    TMR3 = 0;

    /* Scheduler quantum */    
    PR2 = (QUANTUM & 0xFFFF);
    PR3 = (QUANTUM >> 16);
        
    IPCSET(2) = 0x00001f00;
    IFSCLR(0) = 0x00000200;
    IECSET(0) = 0x00000200;
    IPCSET(3) = 0x001f0000;
    IFSCLR(0) = 0x00004000;
    IECSET(0) = 0x00004000;
    T2CON |= 0x8008;
    
    Info("Starting Hypervisor Execution");
    
  //  start_vm_timer(1500000);
    
    asm volatile ("ei");
    /* Wait for the first timer interrupt */ 
    while(1){
        putchar('!');
    };
}


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
        if ((tick_count++)%5==0){
            ret = RESCHEDULE;
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
        usb_device_attach();
    }
#endif

    }
    return ret;
}


void register_timer(uint32_t interval){
    curr_vcpu->timer_interval = interval;
    start_vm_timer(interval);
    
}


void start_vm_timer(uint32_t interval){
    T4CON = 0;
    TMR4 = 0x0;
    TMR5 = 0;

    PR4 = interval & 0xffff;
    PR5 = interval >> 16;
                   
    OFF(24) = 0x200;
        
    IPCSET(4) = 0x1C000000;
    IFSCLR(0) = 0x1000000;
    IECSET(0) = 0x80000;
    IPCSET(6) = 0xe;
    IFSCLR(0) = 0x80000;
    IECSET(0) = 0x1000000;      
    T4CON |= 0x8008;
}

void stop_vm_timer(){
    T4CON = 0;
    IFSCLR(0) = 0x1000000;
}

/* Performs software reset to the board. */
void SoftReset(){
    
    printf("Reset button (sw1) pressed. Performing software reset.");
    udelay(1000000);
    
    NVMKEY = 0x0;
    NVMKEY = 0xAA996655;
    NVMKEY = 0x556699AA;

    RSWRST |= 1;
    
    /* read RSWRST register to trigger reset */
    volatile int* p = &RSWRST;
    *p;
    
    /* prevent any unwanted code execution until reset occurs*/
    while(1) ;  
}

