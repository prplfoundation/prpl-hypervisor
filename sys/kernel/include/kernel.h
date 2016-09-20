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

#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "types.h"

#define HARDWARE_INT_7 		(1<<17)
#define HARDWARE_INT_6 		(1<<16)
#define HARDWARE_INT_5 		(1<<15)
#define HARDWARE_INT_4 		(1<<14)
#define HARDWARE_INT_3 		(1<<13)
#define HARDWARE_INT_2 		(1<<12)
#define HARDWARE_INT_1 		(1<<11)
#define HARDWARE_INT_0 		(1<<10)
#define REQUESTSOFT_INT_1 	(1<<9)
#define REQUESTSOFT_INT_0 	(1<<8)

#define STATUS_HARDWARE_INT_9 		(1<<18)
#define STATUS_HARDWARE_INT_8 		(1<<16)
#define STATUS_HARDWARE_INT_7 		(1<<15)
#define STATUS_HARDWARE_INT_6 		(1<<14)
#define STATUS_HARDWARE_INT_5 		(1<<13)
#define STATUS_HARDWARE_INT_4 		(1<<12)
#define STATUS_HARDWARE_INT_3 		(1<<11)
#define STATUS_HARDWARE_INT_2 		(1<<12)
#define STATUS_HARDWARE_INT_1 		(1<<8)
#define STATUS_HARDWARE_INT_0 		(1<<9)


#define PERFORMANCE_COUNTER_INT (1<<31)

#define GUESTEXITEXCEPTION      0x1b


void general_exception_handler();
void configureGuestExecution(uint32_t exCause);

#endif
