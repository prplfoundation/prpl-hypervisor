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

/**
 * @file timer.c
 * 
 * @section DESCRIPTION
 * 
 * Timer interrupt subsystem. This timer is used for VCPU scheduling and and 
 * virtual timer interrupt injection on Guests. 
 * 
 * Every guest receive timer interrupt each 1ms. This will be replaced soon with 
 * a configurable guest timer. 
 */

#include <globals.h>
#include <config.h>
#include <hal.h>
#include <pic32mz.h>
#include <mips_cp0.h>

/* Interval of interrupt injection on guests */
#define QUANTUM (1 * MILISECOND)

static uint32_t tick_count = 0;

/**
 * @brief Time interrupt handler.
 * 
 * Perfoms VCPUs scheduling and virtual timer interrupt injection on guests. 
 */
static void timer_interrupt_handler(){
    uint32_t temp_CP0;
    uint32_t ret;
    
    IFSCLR(0) = 1;
    
    if (tick_count%QUANTUM_SCHEDULER==0){
        contextSave(NULL, temp_CP0, ret);           
        run_scheduler();
        configureGuestExecution(RESCHEDULE);    
    }else{
        setGuestCTL2(3 << GUESTCLT2_GRIPL_SHIFT);
    }
    
    tick_count++;
    
    temp_CP0 = mfc0(CP0_COUNT, 0);
    temp_CP0 += QUANTUM;
    mtc0(CP0_COMPARE, 0, temp_CP0);
}

/**
 * @brief Configures the CP0 timer.
 * 
 * This function will never return to the calling code. It waits for the
 * first timer interrupt.
 */
void start_timer(){
    uint32_t temp_CP0, offset;
    
    offset = register_interrupt(timer_interrupt_handler);
    OFF(0) = offset;
    printf ("\nCP0 Timer interrupt registered at 0x%x.", offset);
    
    IPC(0) = 0x1f;
    IFSCLR(0) = 1;
    IECSET(0) = 1;
    
    temp_CP0 = mfc0(CP0_COUNT, 0);
    temp_CP0 += 10000;
    mtc0(CP0_COMPARE, 0, temp_CP0);

    asm volatile ("ei");    
    
    /* Wait for a timer interrupt */
    while(1){};
}


/* Critical error ocurred. Waiting for reset */
void WaitforReset(){
    while(1){};
}

