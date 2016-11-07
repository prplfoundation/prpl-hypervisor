/**
 * @file scheduler.c
 * @author Sergio Johann Filho
 * @date February 2016
 * 
 * @section DESCRIPTION
 * 
 * Kernel scheduler and task queue management.
 * 
 */

#include <os.h>

static void process_delay_queue(void)
{
	int32_t i, k;
	struct tcb_entry *krnl_task2;

	k = os_queue_count(krnl_delay_queue);
	for (i = 0; i < k; i++){
		krnl_task2 = os_queue_remhead(krnl_delay_queue);
		if (!krnl_task2) panic(PANIC_NO_TASKS_DELAY);
		if (--krnl_task2->delay == 0){
			if (os_queue_addtail(krnl_run_queue, krnl_task2)) panic(PANIC_CANT_PLACE_RUN);
		}else{
			if (os_queue_addtail(krnl_delay_queue, krnl_task2)) panic(PANIC_CANT_PLACE_DELAY);
		}
	}
}

/**
 * @brief The task dispatcher.
 * 
 * The job of the dispatcher is simple: save the current task context on the TCB,
 * update its state to ready and check its stack for overflow. If there are
 * tasks to be scheduled, process the delay queue and invoke the scheduler.
 * Update the scheduled task state to running and restore the context of the task.
 * 
 * Delayed tasks are in the delay queue, and are processed in the following way:
 *	- The number of elements (tasks) in queue is counted;
 *	- The a task from the head of the queue is removed and its delay is decremented;
 * 		- If the decremented delay of a task reaches 0, it is put on the run queue;
 * 		- It is put it back on the tail of the delay queue otherwise;
 *	- Repeat until the whole queue is processed;
 */
void dispatch_isr(void *arg)
{
	int32_t rc;

	_timer_reset();
	if (krnl_schedule == 0) return;
	krnl_task = &krnl_tcb[krnl_current_task];
	rc = setjmp(krnl_task->task_context);
	if (rc){
		return;
	}
	if (krnl_task->state == TASK_RUNNING)
		krnl_task->state = TASK_READY;
	if (krnl_task->pstack[0] != STACK_MAGIC)
		panic(PANIC_STACK_OVERFLOW);
	if (krnl_tasks > 0){
		process_delay_queue();
		krnl_current_task = sched_be();
		krnl_task->state = TASK_RUNNING;
		_restoreexec(krnl_task->task_context, 1, krnl_current_task);
		panic(PANIC_UNKNOWN);
	}else{
		panic(PANIC_NO_TASKS_LEFT);
	}
}

/**
 * @brief Best effort (BE) scheduler.
 * 
 * @return Best effort task id.
 * 
 * The algorithm is Lottery Scheduling.
 * 	- Take a task from the run queue, copy its entry and put it back at the tail of the run queue.
 * 	- If the task is in the blocked state (it may be simply blocked or waiting in a semaphore) or
 * its not the ticket, it is put back at the tail of the run queue and the next task is picked up.
 * 	- So, if all tasks are blocked, at least the idle task can execute (it is never
 * blocked, at least it is what we hope!).
 * 	- Tasks in the blocked state are never removed from the run queue (they are
 * ignored), although they may be in another queue waiting for a resource.
 */

int32_t sched_be(void)
{
	int32_t r, i = 0;
	
	r = random() % krnl_tasks;
	if (os_queue_count(krnl_run_queue) == 0)
		panic(PANIC_NO_TASKS_RUN);
	do {
		krnl_task = os_queue_remhead(krnl_run_queue);
		if (!krnl_task) panic(PANIC_NO_TASKS_RUN);
		if (os_queue_addtail(krnl_run_queue, krnl_task))
			panic(PANIC_CANT_PLACE_RUN);
	} while ((krnl_task->state == TASK_BLOCKED) || ((i++ % krnl_tasks) != r));
	krnl_task->jobs++;

	return krnl_task->id;
}
