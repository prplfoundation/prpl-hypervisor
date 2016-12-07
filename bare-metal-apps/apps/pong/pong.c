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

/*************************************************************
 * Ping-Pong application - Inter-VM communication.
 * 
 * To execute the Ping-Pong set the CFG_FILE on the main 
 * Makefile to the sample-ping-pong.cfg configuration file.  
 * 
 * The ping-pong sample measures the inter-VM communication 
 * latency with different message sizes. 
 */

#include <arch.h>
#include <libc.h>
#include <guest_interrupts.h>
#include <hypercalls.h>
#include <platform.h>
#include <network.h>


uint32_t message_buffer[256];

int main() {
	uint32_t source;
	int32_t ret;
	uint32_t timesstart, timenow, diff_time;
	
	serial_select(UART2);
	
	printf("\nWait...");
	
	memset(message_buffer, 0, sizeof(message_buffer));
	
	while (1){
		/* Receive a message */
		ret = ReceiveMessage(&source, message_buffer, sizeof(message_buffer), 1);
		if (ret<0){
			print_net_error(ret);
		}else{
			/* get current time and calculates one way delay. */
			timenow = mfc0(CP0_COUNT, 0);
			
			memcpy(&timesstart, message_buffer, sizeof(uint32_t));
			
			diff_time = calc_diff_time(timenow, timesstart);
			
			/* send the calculated delay back to sender VM. */
			memcpy(message_buffer, &diff_time, sizeof(uint32_t));
			
			SendMessage(source, message_buffer, ret);
		}
	}
    
	return 0;
}

