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

/**
 * @file interrupts.c
 * 
 * @section DESCRIPTION
 * 
 * PIC32mz interrupt subsystem. Initializes the pic32mz interrupts
 * and allows for the registration of interrupt handlers. 
 * 
 * The hypervisor configures the pic32mz in vectored mode and uses 
 * the OFF register to configure the vector address of the interrupt. 
 * A device driver, after invoke register_interrupt() must configure
 * the corresponding OFF register with the function return.
 *
 */

#include <driver.h>
#include <mips_cp0.h>
#include <vcpu.h>
#include <config.h>
#include <globals.h>
#include <driver.h>
#include <interrupts.h>
#include <libc.h>
#include <exception.h>

#define MAX_NUM_INTERRUPTS 64

/** 
 * @brief interrupt_handlers is an array of pointers to 
 * functions. Device drivers call register_interrupt() to 
 * register their interrupt handler. 
 */
handler_vector_t * interrupt_handlers[MAX_NUM_INTERRUPTS] = {NULL};

void __interrupt_handler(uint32_t interrupt_number){
	if(interrupt_handlers[interrupt_number]){
		interrupt_handlers[interrupt_number]();
		return;
	}
	WARNING("Interrupt %d not registered.", interrupt_number);
}

/** 
 * @brief Register a new interrupt handler. 
 * @param handler Function pointer. 
 * @param interrupt_number Interrupt vector number.
 * @return Interrupt number or 0 if the handler could not be registered. 
 */
uint32_t register_interrupt(handler_vector_t * handler, uint32_t interrupt_number){
	/*Interrupt 0 is the general exception handler. */
	if (interrupt_number > 0 && interrupt_number < 64){
		if(interrupt_handlers[interrupt_number] == NULL){
			interrupt_handlers[interrupt_number] = handler;
			return interrupt_number;			
		}
	}
	return 0;
}

/** 
 * @brief Configures the P5600 in vectored interrupt mode. 
 */
static void interrupt_init(){
	uint32_t temp;
	
	memset(interrupt_handlers, 0, sizeof(interrupt_handlers));
	
	/* General exception handler at interrupt vector 0 */
	interrupt_handlers[0] = general_exception_handler;
    
	/*Enable the Global Interrupt Controller */
	GCB_GIC |= 1;
	
	mtc0(CP0_EBASE, 0, 0x80000000);
    
	/* Configure the processor to vectored interrupt mode. */
	temp = mfc0(CP0_CAUSE, 0);      /* Get Cause */
	temp |= CAUSE_IV;               /* Set Cause IV */
	mtc0(CP0_CAUSE, 0, temp);       /* Update Cause */

	temp = mfc0(CP0_STATUS, 0);     /* Get Status */
	temp &= ~STATUS_BEV;            /* Clear Status BEV */
	temp &= ~STATUS_EXL; 
	mtc0(CP0_STATUS, 0, temp);      /* Update Status */
    
	temp = mfc0(CP0_INTCTL, 1);
	temp = (temp & ~(0x1f<<INTCTL_VS_SHIFT)) | (0x1 << INTCTL_VS_SHIFT);
	mtc0(CP0_INTCTL, 1, temp);
	
	/* GIC in Virtualization mode */
	GIC_SH_CONFIG |= GIC_SH_CONFIG_VZE;	
	
	GIC_SH_MAP48_PIN = 0x8000010e;
	GIC_SH_MAP55_PIN = 0x8000010e;
	
	asm volatile("ehb");
   
	INFO("P5600 core in Vectored Interrupt Mode.");
}

driver_init(interrupt_init);

