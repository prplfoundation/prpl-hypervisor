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

/*
 *  Baikal-T (P5600) core address definitions
  *
 */

#ifndef _GRP_CONTEXT_H
#define _GRP_CONTEXT_H

#include <types.h>

#define GPR_SIZE (34*4) /* 32 GPR + HI and LO  */

void gpr_context_restore(uint32_t* gpr_p);
void gpr_context_save(uint32_t* gpr_p);

/** 
  * These functions can read/write the saved registers from the stack.  
  * On P5600, the guests share the same GPR set (GPR Shadows are not implemented). Thus,
  * the hypercall parameters are read/write from the stack.
  */
void MoveToPreviousGuestGPR(uint32_t reg, uint32_t value);
uint32_t MoveFromPreviousGuestGPR(uint32_t reg);


#endif /* _GRP_CONTEXT_H */
