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

#ifndef _GRP_CONTEXT_H
#define _GRP_CONTEXT_H

/** 
  * These macros can read/write registers on the previous GPR Shadow.  
  * On PIC32MZ, the guests are kept on separated GRP shadows. Thus,
  * the hypercall parameters are read from them.
  */

/* Write to previous gpr shadow */
#define MoveToPreviousGuestGPR(reg, value) asm volatile (                    \
        "wrpgpr   $%0, %1"                                   \
        : : "K" (reg), "r" ((uint32_t) (value)))

/* Read from previous gpr shadow */        
#define MoveFromPreviousGuestGPR(reg) ({ int32_t __value;                      \
        asm volatile (                                          \
        "rdpgpr   %0, $%1"                                    \
        : "=r" (__value) : "K" (reg));               \
        __value; })

#endif /* _GRP_CONTEXT_H */
