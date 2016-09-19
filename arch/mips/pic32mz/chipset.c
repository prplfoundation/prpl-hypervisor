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

/* Interval of interrupt injection on guests */
#define QUANTUM (1 * MILISECOND)


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
    
    asm volatile ("ei");
    /* Wait for the first timer interrupt */ 
    while(1){
        putchar('!');
    };
}



/* Critical error ocurred. Waiting for reset */
void WaitforReset(){
    while(1){};
}
