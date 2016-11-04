/*
 C o*pyright (c) 2016, prpl Foundation
 
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

#include <uart.h>
#include <types.h>
#include <arch.h>

static uint32_t serial_port = UART4;

int32_t serial_select(uint32_t serial_number){
	/* Only UART4 suported. */
	switch (serial_number){
		case UART4: serial_port = UART4;
		return UART4;
		default:
			return -1;
	}	
}


void putchar(int32_t value){
#ifdef VIRTUALIZED_IO
	while(readio(U4STA) & USTA_UTXBF);
	writeio(U4TXREG, value);    
#else
	while(U4STA & USTA_UTXBF);
	U4TXREG = value;    
#endif	
}

int32_t kbhit(void){
#ifdef VIRTUALIZED_IO
	return (readio(U4STA) & USTA_URXDA);
#else
	return (U4STA & USTA_URXDA);
#endif		
}

uint32_t getchar(void){
	while(!kbhit());
#ifdef VIRTUALIZED_IO	
	return (uint32_t)readio(U2RXREG);
#else
	return (uint32_t)U2RXREG;
#endif 
}
