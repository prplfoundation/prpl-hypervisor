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
 * @file gpr_context.c
 * 
 * @section DESCRIPTION
 * 
 *
 */

#include <gpr_context.h>
#include <libc.h>

/** 
 * @brief Stack pointer defined at baikal.ld linker script. 
 */
extern uint32_t _stack;


/** 
 * @brief Copy the GPR from vcpu to the stack. 
 * @param grp_p Pointer to the address where the gpr is saved. 
 */
void gpr_context_restore(uint32_t* gpr_p){
	uint32_t i, j;
	for(i=((uint32_t)(&_stack))-GPR_SIZE, j=0; i<((int)(&_stack)); i+=sizeof(uint32_t*), j++){
		*(uint32_t*)i = gpr_p[j]; 
	}
}


/** 
 * @brief Copy the GPR to from stack to the vcpu. 
 * @param grp_p Pointer to the address where gpr will be saved. 
 */
void gpr_context_save(uint32_t* gpr_p){
	uint32_t i, j;
	for(i=((uint32_t)(&_stack))-GPR_SIZE, j=0; i<((int)(&_stack)); i+=sizeof(uint32_t*), j++){
		gpr_p[j] = ((uint32_t)*(uint32_t*)i); 
	}
}

