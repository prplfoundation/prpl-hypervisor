#include <globals.h>
#include "pic32mz.h"

void freq_config(){
    /* configured in the bootloader */    
    
}


void configure_timer(){
    unsigned int temp_CP0;

    /* configure the interrupt controller to compatibility mode */
    PRISS = 7<<28 | 7<<24 | 7<<20 | 7<<16 | 7<<12 | 7<<8 | 7<<4;
    
    Info("Configuring Timer");

    mtc0 (CP0_EBASE, 1, 0x9d000000);    /* Set an EBase value of 0x9D000000 */
    temp_CP0 = mfc0(CP0_CAUSE, 0);      /* Get Cause */
    temp_CP0 |= CAUSE_IV;           /* Set Cause IV */
    mtc0(CP0_CAUSE, 0, temp_CP0);       /* Update Cause */
    INTCONCLR = INTCON_MVEC;        /* Clear the MVEC bit */
    temp_CP0 = mfc0(CP0_STATUS, 0);     /* Get Status */
    temp_CP0 &= ~STATUS_BEV;        /* Clear Status IV */
    mtc0(CP0_STATUS, 0, temp_CP0);      /* Update Status */

    /* Using PIC32's timers 2/3 for 32bit counter*/
    T2CON = 0;
    TMR2 = 0x0;
    TMR3 = 0;
    PR2 = 0x86A0;
    PR3 = 0x1;
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
    while(1){};
}


void timer_int_handler(){
    
    if (IFS(0) & 0x00000200){
        IFSCLR(0) = 0x00000200;
        putchar('1');
    }
    if (IFS(0) & 0x00004000){
        IFSCLR(0) = 0x00004000;
    }
}

