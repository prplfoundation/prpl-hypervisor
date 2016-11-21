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
 * This is the dhrystone benchmark. 
 * 
 * 
 */

#include <arch.h>
#include <libc.h>
#include <hypercalls.h>
#include <guest_interrupts.h>
#include <platform.h>
#include <io.h>

#define RUNS 5000000

#define CPU_TICK_TO_MS(ticks) ((ticks)/((CPU_SPEED/2)/1000))

volatile uint64_t tick_passed = 0;

void irq_timer(){
	static uint32_t tick_old = 0;
	uint32_t tick_now = mfc0 (CP0_COUNT, 0);
	uint32_t diff_time;
	
	if (tick_old == 0){
		tick_old = tick_now;
		return;
	}
	
	if (tick_now >= tick_old){
		diff_time = tick_now - tick_old;
	}else{
		diff_time = 0xffffffff - (tick_old - tick_now);
	}
	
	tick_old = tick_now;
	
	tick_passed += diff_time;
}

long time(long *tm){
	uint32_t seconds = CPU_TICK_TO_MS(tick_passed)/1000;
	if (tm){
		*tm=seconds;
	}
	
	return seconds;
}


int main() {
	perf_control_t perf0, perf1;
	uint32_t perfcount[2];
	
	interrupt_register(irq_timer, GUEST_TIMER_INT);
	
	/* peformance counter 0 - D-Cache Misses */
	perf0.w = 0;
	//perf0.m = 1;
	perf0.ec = 2;
	perf0.event = 11;
	perf0.ie = 0;
	perf0.u = 0;
	perf0.k = 1;
	perf0.exl = 0;

	/* peformance counter 1 - I-Cache Misses */
	perf1.w = 0;
	//perf1.m = 1;
	perf1.ec = 2;
	perf1.event = 9;
	perf1.ie = 0;
	perf1.u = 0;
	perf1.k = 1;
	perf1.exl = 0;
	
	performance_counter_start(perf0.w, perf1.w);
	
	main_dhry(RUNS);    

	performance_counter_stop(perfcount);
	
	printf("\nD-Cache Misses: %d", perfcount[0]);
	printf("\nI-Cache Misses: %d", perfcount[1]);
	
	
	return 0;
}

