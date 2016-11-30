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
 * Generates output signal on pin RPD3 (J5 MOSI) on Curiosity board. 
 *
 * Used as an interrupt generator for interrupt latency tests. See
 * documentation for more details. 
 * 
 * 
 */

#include <arch.h>
#include <libc.h>
#include <hypercalls.h>
#include <guest_interrupts.h>
#include <platform.h>
#include <io.h>


int main() {
	uint32_t time = mfc0(CP0_COUNT, 0);
	uint32_t interval = 0;
	
	/* Configure PIN RPD3 as output */
	writeio(TRISDCLR, 8);
	
	srand(0xdeadbeef);
   
	while (1){
		if(wait_time(time, interval)){
			time = mfc0(CP0_COUNT, 0);
			writeio(LATDINV, 8);			
			interval = random()%10 + 1;
		}
		
	}    
	return 0;
}

