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


/**
 * @file main.c
 * @author Sergio Johann Filho
 * @date January 2016
 * 
 * @section DESCRIPTION
 * 
 * The Prpl-OS realtime operating system kernel.
 * 
 */
 
#include <os.h>

static void print_config(void)
{
	kprintf("\n===========================================================");
	kprintf("\nPrpl-OS %s [%s, %s]", KERN_VER, __DATE__, __TIME__);
	kprintf("\n===========================================================\n");
	kprintf("\ncpu id:        %d", CPU_ID);
	kprintf("\narch:          %s", CPU_ARCH);
	kprintf("\nsys clk:       %d kHz", CPU_SPEED/1000);
	if (TIME_SLICE != 0)
		kprintf("\ntime slice:    %d us", TIME_SLICE);
	kprintf("\nheap size:     %d bytes", sizeof(krnl_heap));
	kprintf("\nmax tasks:     %d\n", MAX_TASKS);
}

static void clear_tcb(void)
{
	uint16_t i, j;
	
	for(i = 0; i < MAX_TASKS; i++){
		krnl_task = &krnl_tcb[i];
		krnl_task->id = -1;
		for (j = 0; j < sizeof(krnl_task->name); j++)
			krnl_task->name[j] = 0;
		krnl_task->state = TASK_IDLE;
		krnl_task->jobs = 0;
		krnl_task->ptask = NULL;
		krnl_task->pstack = NULL;
		krnl_task->stack_size = 0;
		krnl_task->other_data = 0;
	}

	krnl_tasks = 0;
	krnl_current_task = 0;
	krnl_schedule = 0;
}

static void init_queues(void)
{
	krnl_run_queue = os_queue_create(MAX_TASKS);
	if (krnl_run_queue == NULL) panic(PANIC_OOM);
	krnl_delay_queue = os_queue_create(MAX_TASKS);
	if (krnl_delay_queue == NULL) panic(PANIC_OOM);
}

static void idletask(void)
{
	os_schedlock(0);
	for (;;){
		_cpu_idle();
	}
}

/**
 * @internal
 * @brief Prpl-OS kernel entry point and system initialization.
 * 
 * @return should not return.
 * 
 * We assume that the following machine state has been already set
 * before this routine.
 *	- Kernel BSS section is filled with 0.
 *	- Kernel stack is configured.
 *	- All interrupts are disabled.
 *	- Minimum page table is set. (MMU systems only)
 */
int main(void)
{
	static uint32_t oops=0xbaadd00d;
	
	_hardware_init();
	os_schedlock(1);
	_di();
	if (oops == 0xbaadd00d){
		oops = 0;
		print_config();
		_vm_init();
		clear_tcb();
		init_queues();
		_sched_init();
		_device_init();
		_irq_init();
		_timer_init();
		os_spawn(idletask, "idle task", 1024);
		_di();
		_task_init();
		app_main();
		sched_be();
		kprintf("\nfree heap: %d bytes", krnl_free);
		kprintf("\nKERNEL: Prpl-OS is running\n");
		_restoreexec(krnl_tcb[0].task_context, 1, 0);
		panic(PANIC_ABORTED);
	}else{
		panic(PANIC_GPF);
	}
	
	return 0;
}
