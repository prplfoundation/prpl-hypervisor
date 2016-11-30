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
 * @file pic32mz-interrupt-latency-test.c
 * 
 * @section DESCRIPTION
 * 
 * Test the interrupt latency on internal hypervisor drivers. 
 *
 * Test procedure for PIC32mz Starter Kit:
 * 
 * 	* Connect a signal generator to pin RD10 (pin 37 on J12 connector). 
 *        You can use another board for that. The application output_signal 
 * 	  works with PIC32mz Curiosity boards for this purpose. 
 * 
 *      * Connect a logical analizer to pins RD10 and RF4 (pin 21 on J12 connector).
 * 
 * 	* The test consists of measure the time difference between the interrupt on RD10
 * 	  and the response on pin RF4.
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
#include <uart.h>
#include <interrupts.h>

void interrupt_handler(){
	
	IFSCLR(3) = 1<<25;
	
	if(PORTD & (1<<10)){
		LATFSET = 1 << 4;
	}else{
		LATFCLR = 1 << 4;

	}
}

/**
 * @brief Driver init call.
 * Register the interrupt handler and enable the timer.  
 */
void start_latency_init(){
	uint32_t offset;
	
	TRISDSET =  1 << 10;     	/* RD10 (Input pin) */
	CNPUDSET =  1 << 10;     /* enable pull-up */
	
	TRISFCLR = 1 << 4; /* RD0 as output*/
	LATFCLR = 1 << 4;
	CNPUFCLR =  1 << 4;
	CNPDFCLR =  1 << 4;
	ANSELFCLR = 1 << 4;

	/* Configure interrupt for pin RD10 */
	offset = register_interrupt(interrupt_handler);
	OFF(121) = offset;
	
	CNCONDSET = 0x8000;
	CNENDSET = 1<<10;
	IPC(30) = 0x1f<<8;
	IFSCLR(3) = 1<<25;
	IECSET(3) = 1<<25;
	
	INFO("PIC32mz driver for interrupt latency test.");
}

driver_init(start_latency_init);

