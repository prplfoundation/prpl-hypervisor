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
 * This code was written by Sergio Johann at Embedded System Group (GSE) at PUCRS/Brazil.
 * 
 */


#include <os.h>

static funcptr isr[32] = {[0 ... 31] = NULL};

/*
interrupt management routines
*/
void _irq_register(uint32_t mask, funcptr ptr)
{
	int32_t i;

	for (i = 0; i < 32; ++i)
		if (mask & (1 << i))
			isr[i] = ptr;
}

void _irq_handler(uint32_t status, uint32_t cause)
{
	//int32_t i = 0;
	//uint32_t irq;
	/*
	irq = IFS(0);
	
	do {
		if (irq & 0x1){
			if(isr[i]){
				isr[i]();
			}
		}
		irq >>= 1;
		++i;
	} while(irq);*/
    dispatch_isr(0);
}

void _irq_mask_set(uint32_t mask)
{
	/*IECSET(0) = mask;*/
}

void _irq_mask_clr(uint32_t mask)
{
	/*IECCLR(0) = mask;*/
}

void _exception_handler(uint32_t epc, uint32_t opcode)
{
}
