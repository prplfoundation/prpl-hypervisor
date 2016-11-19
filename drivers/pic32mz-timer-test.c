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

/**
 * @file pic32mz-timer-test.c
 * 
 * @section DESCRIPTION
 * 
 * Interrupt subsystem stress test. 
 *
 * Uses the PIC32mz's timers 2 and 3 as a single 32 bits timer count to generate timer interrupts. 
 * 
 * The propose of this driver is generate stress tests of the interrupt subsystem. 
 * 
 */

#include <hypercall.h>
#include <globals.h>
#include <driver.h>
#include <hypercall_defines.h>
#include <mips_cp0.h>
#include <hal.h>
#include <scheduler.h>
#include <libc.h>

/* Time interval in microseconds. */
#define QUANTUM_US 10

/* Timer tick count  */
uint32_t tick_count = 0;

void timer_interrupt_hander(){
	
	if (IFS(0) & 0x00004000){
		IFSCLR(0) = 0x00004000;
	}
    
	tick_count++;
    
	if(tick_count%50000==0){
		putchar('!');
	}
}

/**
 * @brief Driver init call.
 * Register the interrupt handler and enable the timer.  
 */
void timer_test_init(){
	uint32_t offset;
	uint32_t interval = QUANTUM_US * 1000;
	
    	offset = register_interrupt(timer_interrupt_hander);
	OFF(14) = offset;
	
	/* Using PIC32's timers 2/3 for 32bit counter */
	T2CON = 0;
	TMR2 = 0x0;
	TMR3 = 0;

	PR2 = (interval & 0xFFFF);
	PR3 = (interval >> 16);
        
	IPCSET(2) = 0x00001f00;
	IFSCLR(0) = 0x00000200;
	IECSET(0) = 0x00000200;
	IPCSET(3) = 0x001f0000;
	IFSCLR(0) = 0x00004000;
	IECSET(0) = 0x00004000;
	T2CON |= 0x8008;

	INFO("PIC32mz driver for 32 bits timer test enabled with %dus interval .", QUANTUM_US);
}

driver_init(timer_test_init);

