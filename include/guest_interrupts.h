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
 * @file guest_interrupts.c
 * 
 * @section DESCRIPTION
 * 
 * Interrupt values for bare-metal applications.
 */

#ifndef __GUEST_INTERRUPTS_H
#define __GUEST_INTERRUPTS_H

#ifndef BAIKAL_T1

#define GUEST_TIMER_INT			1
#define GUEST_INTERVM_INT		2
#define GUEST_USB_INT			4
#define GUEST_ETH_INT			8
#define GUEST_USER_DEFINED_INT_1	0x10
#define GUEST_USER_DEFINED_INT_2	0x20

#else
#define GUEST_INTERVM_INT		2
#define GUEST_TIMER_INT			0x20
#endif

#endif

