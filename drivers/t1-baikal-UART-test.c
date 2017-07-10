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
 * @file t1-baikal-UART-test.c
 * 
 * @section DESCRIPTION
 * 
 * Simple test to the interrupt subsystem using UART interrupts. 
 *
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
	char a;
	
	while( (UART0_LSR & 0x1) ){
		a = UART0_RBR;
		UART0_RBR = a;
	}
}

/**
 * @brief Driver init call.
 * Register the interrupt handler and enable UART interrupts.  
 */
void uart_init(){
	uint32_t temp;
	
	if( register_interrupt(interrupt_handler, 3) == 0){
		INFO("UART Baikal-T1 driver registration ERROR.");
		return 0;
	}

	/*Clean input FIFO */ 
	while( (UART0_LSR & 0x1) ){
		UART0_RBR;
	}
	
	UART0_IER = 0x1;

	temp = mfc0(CP0_STATUS, 0);   
	temp |= (STATUS_IM3 << STATUS_IM_SHIFT);
	mtc0(CP0_STATUS, 0, temp);   

	GIC_SH_MAP48_CORE = 1;
	GIC_SH_MAP48_PIN = 0x80000001;
	GIC_SH_POL63_32 = 1<<16;
	
	GIC_SH_SMASK63_32 = 1<<16;
	
	INFO("UART Baikal-T1 driver test.");
}

driver_init(uart_init);

