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

static uint32_t serial_port = UART2;

int32_t serial_select(uint32_t serial_number){
	/* select serial 2 or 4 as output */
	switch (serial_number){
		case UART2: serial_port = UART2;
		return UART2;
		case UART6: serial_port = UART6;
		return UART6;
		default:
			return -1;
	}	
}


void putchar(int32_t value){
	if (serial_port == UART2){
		while(U2STA & USTA_UTXBF);
		U2TXREG = value;    
	}else if (serial_port == UART6){
		while(U6STA & USTA_UTXBF);
		U6TXREG = value;    
	}
}

int32_t kbhit(void){
	if (serial_port == UART2){
		return (U2STA & USTA_URXDA);
	}else if (serial_port == UART6){
		return (U6STA & USTA_URXDA);
	}
	return 0;
}

uint32_t getchar(void){
	while(!kbhit());
	if (serial_port == UART2){
		return (uint32_t)U2RXREG;
	}else if (serial_port == UART6){
		return (uint32_t)U6RXREG;
	}
	return 0;
}
