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

/* Simple switch button usage on Bare-metal application 
 *
 * Reads SW2 button on Microchip Connectivity Starter Kit.
 * 
 * Enable device access on the cfg file. 
 * 	device_mapping = [ "PORTH", "UART2", "PORTB"];
 */

#include <arch.h>
#include <libc.h>
#include <hypercalls.h>


int main() {
	
	/* Configure pin RB13 for input (SW2) */
	write(TRISBSET, 1 << 13);     	/* RB13 input */
	write(CNPUBSET, 1 << 13);   	/* enable pull-up */
    
	/* Configure pin RH1 for output (LED 2) */
	write(TRISHCLR, 2);
    
	while (1){
        
		/* Read SW2 status */
		if (!(read(PORTB)  & (1 << 13))) {
			/* on/off led */
			write(LATHINV, 2);
			
		}
	}
    
	return 0;
}

