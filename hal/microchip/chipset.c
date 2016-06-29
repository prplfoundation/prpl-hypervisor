#include <globals.h>
#include <config.h>
#include <hal.h>
#include "pic32mz.h"

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


void freq_config(){
    /* configured in the bootloader */    
    
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
        curr_vcpu->guestclt2 = (((hal_lr_rcause() & 0x1fc00)>>10) << GUESTCLT2_GRIPL_SHIFT);
        if ((tick_count++)%10==0){
            ret = RESCHEDULE;
        }
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
