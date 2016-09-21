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
#include <driver.h>
#include <mips_cp0.h>
#include <vcpu.h>
#include <config.h>
#include <globals.h>
#include <driver.h>

#define MAX_NUM_INTERRUPTS 10
#define VECTOR_1_OFFSET 0x220

typedef void  handler_vector_t();

handler_vector_t * interrupt_handlers[MAX_NUM_INTERRUPTS] = {NULL};

uint32_t register_interrupt(handler_vector_t * handler){
    uint32_t i;
    for(i=0; i<MAX_NUM_INTERRUPTS; i++){
        if(interrupt_handlers[i] == NULL){
            interrupt_handlers[i] = handler;
            return VECTOR_1_OFFSET + i*32;
        }
    }
    return 0;
}

void interrupt_init(){
    uint32_t temp_CP0, offset;
    
    /* All interrupt levels are handled at GPR shadow 7 
    Other GPR shadows are used for VM execution. */
    PRISS = 7<<28 | 7<<24 | 7<<20 | 7<<16 | 7<<12 | 7<<8 | 7<<4;

    /* Configure the processor to vectored interrupt mode. */
    mtc0 (CP0_EBASE, 1, 0x9d000000);    /* Set an EBase value of 0x9D000000 */
    temp_CP0 = mfc0(CP0_CAUSE, 0);      /* Get Cause */
    temp_CP0 |= CAUSE_IV;               /* Set Cause IV */
    mtc0(CP0_CAUSE, 0, temp_CP0);       /* Update Cause */
    INTCONSET = INTCON_MVEC;            /* Set the MVEC bit - Vetored interrupt mode. */
    temp_CP0 = mfc0(CP0_STATUS, 0);     /* Get Status */
    temp_CP0 &= ~STATUS_BEV;            /* Clear Status IV */
    temp_CP0 &= ~STATUS_EXL; 
    mtc0(CP0_STATUS, 0, temp_CP0);      /* Update Status */
    
    printf("\nPIC32mz in Vectored Interrupt Mode.");
}

driver_init(interrupt_init);

