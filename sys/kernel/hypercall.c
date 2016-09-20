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
 * @file hypercall.c
 * 
 * @section DESCRIPTION
 * 
 * Hypercall registration interface and execution.
 * 
 * All hypercalls are registered in execution time. Any device driver can register its own hypercalls. 
 * A table is kept with the pointers to the corresponding calls. The position on this table corresponds
 * to the hypercall code. 
 */

#include <hypercall.h>
#include <scheduler.h>
#include <hal.h>
#include <globals.h>
#include <tlb.h>

/* Hypercalls table */
static hypercall_t* hypercall_table[HCALL_TABLE_SIZE];

/**
 * @brief Register a hypercall. 
 * @param hyper Function pointer to the hypercall code. 
 * @param code Hypercall code.
 * @return Greater than zero on success, otherwise error code. 
 */
int32_t register_hypercall(hypercall_t* hyper, uint32_t code){
    /* The hypercall table is small to keep small footprint as well. It can be increased 
     throught HCALL_TABLE_START_SIZE */
    if (code > HCALL_TABLE_SIZE-1 || code < 0){
        return HCALL_CODE_INVALID;
    }
    
    /* Hypercall code already registered. */
    if(hypercall_table[code]){
        return HCALL_CODE_USED;
    }
    
    /* Register the hypercall */
    hypercall_table[code] = hyper;
    
    return code;
}

/**
 * @brief Called during guest exit exception to execute the required hypercall.
 */
void hypercall_execution(){
    /* Get the hypervall code */
    uint32_t code = getHypercallCode(); 
    
    /* Hypercall not implemented */
    if ( (code > HCALL_TABLE_SIZE-1) || (code < 0) || (!hypercall_table[code])){
        MoveToPreviousGuestGPR(REG_V0, HCALL_NOT_IMPLEMENTED);
        return;
    }
    
    /* Execute the hypercall */
    hypercall_table[code]();
    
}

