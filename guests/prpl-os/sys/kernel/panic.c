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

void panic(int32_t cause)
{
	_di();
	kprintf("\nKERNEL: panic [task %d] - ", krnl_current_task);
	switch(cause){
	case PANIC_ABORTED:		kprintf("execution aborted"); break;
	case PANIC_GPF:			kprintf("general protection fault"); break;
	case PANIC_STACK_OVERFLOW:	kprintf("stack overflow"); break;
	case PANIC_NO_TASKS_LEFT:	kprintf("no more tasks left to dispatch"); break;
	case PANIC_OOM:			kprintf("out of memory"); break;
	case PANIC_NO_TASKS_RUN:	kprintf("no tasks on run queue"); break;
	case PANIC_NO_TASKS_DELAY:	kprintf("no tasks on delay queue"); break;
	case PANIC_UNKNOWN_TASK_STATE:	kprintf("task in unknown state"); break;
	case PANIC_CANT_PLACE_RUN:	kprintf("can't place task on run queue"); break;
	case PANIC_CANT_PLACE_DELAY:	kprintf("can't place task on delay queue"); break;
	case PANIC_CANT_SWAP:		kprintf("can't swap tasks on queue"); break;
	case PANIC_NUTS_SEM:		kprintf("insane semaphore"); break;
	default:			kprintf("unknown error"); break;
	}
	printf(" -> system halted.\n");
	for(;;);
}
