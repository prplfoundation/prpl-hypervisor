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
#include <network.h>
#include <guest_interrupts.h>
#include <hypercalls.h>
#include <platform.h>

#define NUMBER_OF_ROUNDS 10000

static void irq_timer(){
	return;
}


static uint8_t message_buffer[256];

int main() {
	int32_t ret;
	uint32_t timestart;
	uint32_t timenow;
	uint32_t diff_time;
	uint32_t i;
	uint32_t source;
	uint32_t worst_one_way = 0;
	uint32_t worst_round_trip = 0;
	uint32_t best_one_way = 9999999;
	uint32_t best_round_trip = 9999999;
	uint32_t average_one_way = 0;
	uint32_t average_round_trip = 0;
	uint32_t message_size = 64;
	
	memset(message_buffer, 0, sizeof(message_buffer));
	
	interrupt_register(irq_timer, GUEST_TIMER_INT);
    
	printf("Measuring Inter VM communication latency.");
	
	while (1){
		
		worst_one_way = 0;
		worst_round_trip = 0;
		best_one_way = 9999999;
		best_round_trip = 9999999;
		average_one_way = 0;
		average_round_trip = 0;
		
		for(i = 0; i < NUMBER_OF_ROUNDS; i++){
			/* start time */
			timestart = mfc0(CP0_COUNT, 0);
			
			/* send the initial time to the recipient VM. */
			memcpy(message_buffer, &timestart, sizeof(uint32_t));
		
			/* send message to VM 2. */
			ret = SendMessage(2, message_buffer, message_size);
		
			if (ret<0){
				print_net_error(ret);
			}else{
				/* Receive VM's 2 response */
				ret = ReceiveMessage(&source, message_buffer, sizeof(message_buffer), 1);
			
				if (ret<0){
					print_net_error(ret);
				}else{	
					if(ret){
						/* get current time */
						timenow = mfc0(CP0_COUNT, 0);
						
						/* calc round trip average, worst and best cases. */
						diff_time = calc_diff_time(timenow, timestart);
						
						//average_round_trip = (average_round_trip + diff_time)/2.0;
						average_round_trip += diff_time;
						
						if(diff_time > worst_round_trip){
							worst_round_trip = diff_time;
						}
			
						if(best_round_trip > diff_time){
							best_round_trip = diff_time;
						}
						
						/* get one way delay from VM's 2 response */ 
						memcpy(&diff_time, message_buffer, sizeof(uint32_t));
						
						/* calc one way average, worst and best cases. */
						average_one_way += diff_time;
						
						if(diff_time > worst_one_way){
							worst_one_way = diff_time;
						}
			
						if(best_one_way > diff_time){
							best_one_way = diff_time;
						}
						
					}
				}
			}
		}
		printf("\n");
		/* show results and move the next message size. */
		printf("\nRound trip latency for messages %d bytes long.\n\tAverage %0.2f us\n\tWorst %d us\n\tBest %d us", 
			message_size, 
			CPU_TICK_TO_US((float)average_round_trip/NUMBER_OF_ROUNDS), 
			CPU_TICK_TO_US(worst_round_trip), 
			CPU_TICK_TO_US(best_round_trip));
		
		printf("\nOne way latency for messages %d bytes long.\n\tAverage %0.2f us\n\tWorst %d us\n\tBest %d us\n", 
			message_size, 
			CPU_TICK_TO_US((float)average_one_way/NUMBER_OF_ROUNDS), 
			CPU_TICK_TO_US(worst_one_way), 
			CPU_TICK_TO_US(best_one_way));
			
		puts("\n");
		
		message_size = (message_size + 64) > 256? 64 : message_size + 64;
	}
	
	return 0;
}

