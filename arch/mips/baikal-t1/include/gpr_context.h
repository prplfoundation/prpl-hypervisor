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

#define GPR_SIZE (34*4) /* 32 GPR + HI and LO  */

/** 
  * These macros can read/write the saved registers from the stack.  
  * On P5600, the guests share the same GPR set (GPR Shadows are not implemented). Thus,
  * the hypercall parameters are read/write from the stack.
  */

/* Write to a register saved on the stack */
#define MoveToPreviousGuestGPR(reg, value) asm volatile (	\
	"la  $t1, _stack \n					\
	 addiu $t1, $t1, -136 \n				\
	 sw  %1,  %0($t1)"					\
        : : "I" (reg<<2), "r" ((uint32_t) (value)) : "t1")


/* Read from a register from the stack */
#define MoveFromPreviousGuestGPR(reg) ({ int32_t __value;	\
        asm volatile (                                       	\
        "la   $t1, _stack \n					\
         addiu $t1, $t1, -136 \n				\
         lw   %0, %1($t1)" 					\
        : "=r" (__value) : "I" (reg<<2) : "t1");			\
        __value; })


#endif /* _GRP_CONTEXT_H */
