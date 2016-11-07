/**
 * @file task.c
 * @author Sergio Johann Filho
 * @date March 2016
 * 
 * @section DESCRIPTION
 * 
 * Task management primitives and auxiliary functions.
 * 
 */

#include <os.h>

/**
 * @brief Get a task id by its name.
 * 
 * @param name is a pointer to an array holding the task name.
 * 
 * @return task id if the task is found and OS_INVALID_NAME otherwise.
 */
int32_t os_id(int8_t *name)
{
	int32_t i;

	for (i = 0; i < MAX_TASKS; i++){
		if (krnl_tcb[i].ptask){
			if (strcmp(krnl_tcb[i].name, name) == 0)
				return krnl_tcb[i].id;
		}
	}
	return OS_INVALID_NAME;
}

/**
 * @brief Get a task name by its id.
 * 
 * @param id is a task id number.
 * 
 * @return task name if the task is found and NULL otherwise.
 */
int8_t *os_name(uint16_t id)
{
	if (id < MAX_TASKS)
		if (krnl_tcb[id].ptask)
			return krnl_tcb[id].name;
	return NULL;
}

/**
 * @brief Get the current task id.
 * 
 * @return current task id.
 */
uint16_t os_selfid(void)
{
	return krnl_task->id;
}

/**
 * @brief Get the current task name.
 * 
 * @return current task name.
 */
int8_t *os_selfname(void)
{
	return krnl_task->name;
}

/**
 * @brief Get the current state of a task.
 * 
 * @param id is a task id number.
 * 
 * @return task state the task if found (TASK_IDLE, TASK_READY, TASK_RUNNING, TASK_BLOCKED, TASK_DELAYED or TASK_WAITING) and OS_INVALID_ID otherwise.
 */
int32_t os_state(uint16_t id)
{
	if (id < MAX_TASKS)
		if (krnl_tcb[id].ptask)
			return krnl_tcb[id].state;
	return OS_INVALID_ID;
}

/**
 * @brief Get the number of executed jobs of a task.
 * 
 * @param id is a task id number.
 * 
 * @return jobs executed by the task if found and OS_INVALID_ID otherwise.
 */
int32_t os_jobs(uint16_t id)
{
	if (id < MAX_TASKS)
		if (krnl_tcb[id].ptask)
			return krnl_tcb[id].jobs;
	return OS_INVALID_ID;
}

/**
 * @brief Spawn a new task.
 * 
 * @param task is a pointer to a task function / body.
 * @param period is the task RT period (in quantum / tick units).
 * @param capacity is the amount of work to be executed in a period (in quantum / tick units).
 * @param deadline is the task deadline to complete the work in the period (in quantum / tick units).
 * @param name is a string used to identify a task.
 * @param stack_size is the stack memory to be allocated for the task.
 * 
 * @return task id if the task is created, OS_EXCEED_MAX_NUM if the maximum number of tasks in the system
 * is exceeded, OS_INVALID_PARAMETER if impossible RT parameters are specified or OS_OUT_OF_MEMORY if
 * the system fails to allocate memory for the task resources.
 * 
 * If a task has defined realtime parameters, it is put on the RT queue, if not
 * (period 0, capacity 0 and deadline 0), it is put on the BE queue.
 * WARNING: Task stack size should be always configured correctly, considering data
 * declared on the auto region (local variables) and around 1024 of spare memory for the OS.
 * For example, if you declare a buffer of 5000 bytes, stack size should be 6000.
 */
int32_t os_spawn(void (*task)(), int8_t *name, uint32_t stack_size)
{
	volatile uint32_t status, i = 0;
	
	status = _di();
	while ((krnl_tcb[i].ptask != 0) && (i < MAX_TASKS))
		i++;
	if (i == MAX_TASKS){
		kprintf("\nKERNEL: task not added - MAX_TASKS: %d", MAX_TASKS);
		_ei(status);
		return OS_EXCEED_MAX_NUM;
	}
	krnl_tasks++;
	krnl_task = &krnl_tcb[i];
	krnl_task->id = i;
	strncpy(krnl_task->name, name, sizeof(krnl_task->name));
	krnl_task->state = TASK_IDLE;
	krnl_task->delay = 0;
	krnl_task->jobs = 0;
	krnl_task->ptask = task;
	stack_size += 3;
	stack_size >>= 2;
	stack_size <<= 2;
	krnl_task->stack_size = stack_size;
	krnl_task->pstack = (int32_t *)malloc(stack_size);
	_set_task_sp(krnl_task->id, (uint32_t)krnl_task->pstack + (stack_size - 4));
	_set_task_tp(krnl_task->id, krnl_task->ptask);
	if (krnl_task->pstack){
		krnl_task->pstack[0] = STACK_MAGIC;
		kprintf("\nKERNEL: [%s], id: %d, addr: %x, sp: %x, ss: %d bytes", krnl_task->name, krnl_task->id, krnl_task->ptask, _get_task_sp(krnl_task->id), stack_size);
		if (os_queue_addtail(krnl_run_queue, krnl_task)) panic(PANIC_CANT_PLACE_RUN);
	}else{
		krnl_task->ptask = 0;
		krnl_tasks--;
		kprintf("\nKERNEL: task not added (out of memory)");
		i = OS_OUT_OF_MEMORY;
	}
	krnl_task = &krnl_tcb[krnl_current_task];
	_ei(status);
	
	return i;
}

 /**
 * @brief Yields the current task.
 * 
 * The current task gives up execution and the best effort scheduler is invoked.
 */
void os_yield(void)
{
	int32_t rc;
	volatile int32_t status;
	
	status = _di();
	krnl_task = &krnl_tcb[krnl_current_task];
	rc = setjmp(krnl_task->task_context);
	if (rc){
		_ei(status);
		return;
	}
	if (krnl_task->state == TASK_RUNNING)
		krnl_task->state = TASK_READY;
	if (krnl_task->pstack[0] != STACK_MAGIC)
		panic(PANIC_STACK_OVERFLOW);
	if (krnl_tasks > 0){
		krnl_current_task = sched_be();
		krnl_task->state = TASK_RUNNING;
		_restoreexec(krnl_task->task_context, status, krnl_current_task);
		panic(PANIC_UNKNOWN);
	}else{
		panic(PANIC_NO_TASKS_LEFT);
	}
}

 /**
 * @brief Blocks a task.
 * 
 * @param id is a task id number.
 * 
 * @return OS_OK on success, OS_INVALID_ID if the referenced task does not exist or OS_ERROR if the task is already in the blocked state.
 * 
 * The task is marked as TASK_BLOCKED so the scheduler doesn't select it as a candidate for scheduling.
 * The blocking state is acomplished without removing the task from the run queue, reducing the cost of
 * the operation in cases where the task state is switched frequently (such as in semaphore primitives).
 */
int32_t os_block(uint16_t id)
{
	volatile uint32_t status;

	status = _di();
	if (id == 0){
		kprintf("\nKERNEL: can't block the idle task");
		_ei(status);
		return OS_INVALID_ID;
	}
	krnl_task = &krnl_tcb[id];
	if ((krnl_task->ptask == 0) || (id >= MAX_TASKS)){
		kprintf("\nKERNEL: task doesn't exist");
		krnl_task = &krnl_tcb[krnl_current_task];
		_ei(status);
		return OS_INVALID_ID;
	}
	if (krnl_task->state == TASK_BLOCKED){
		kprintf("\nKERNEL: can't block an already blocked task");
		krnl_task = &krnl_tcb[krnl_current_task];
		_ei(status);
		return OS_ERROR;
	}
	krnl_task->state = TASK_BLOCKED;
	krnl_task = &krnl_tcb[krnl_current_task];
	_ei(status);
	
	return OS_OK;
}

 /**
 * @brief Resumes a blocked task.
 * 
 * @param id is a task id number.
 * 
 * @return OS_OK on success, OS_INVALID_ID if the referenced task does not exist or OS_ERROR if the task is not in the blocked state.
 * 
 * The task must be in the TASK_BLOCKED state in order to be resumed. 
 * The task is marked as TASK_BLOCKED so the scheduler doesn't select it as a candidate for scheduling.
 * The blocking state is acomplished without removing the task from the run queue, reducing the cost of
 * the operation in cases where the task state is switched frequently (such as in semaphore primitives).
 */
int32_t os_resume(uint16_t id)
{
	volatile uint32_t status;

	status = _di();
	if (id == 0){
		kprintf("\nKERNEL: can't resume the idle task");
		_ei(status);
		return OS_INVALID_ID;
	}
	krnl_task = &krnl_tcb[id];
	if ((krnl_task->ptask == 0) || (id >= MAX_TASKS)){
		kprintf("\nKERNEL: task doesn't exist");
		krnl_task = &krnl_tcb[krnl_current_task];
		_ei(status);
		return OS_INVALID_ID;
	}
	if (krnl_task->state != TASK_BLOCKED){
		kprintf("\nKERNEL: can't resume a non blocked task");
		krnl_task = &krnl_tcb[krnl_current_task];
		_ei(status);
		return OS_ERROR;
	}
	krnl_task->state = TASK_READY;
	krnl_task = &krnl_tcb[krnl_current_task];
	_ei(status);

	return OS_OK;
}

 /**
 * @brief Kills a task.
 * 
 * @param id is a task id number.
 * 
 * @return OS_OK on success or OS_INVALID_ID if the referenced task does not exist.
 * 
 * All memory allocated during the task initialization is freed, the TCB entry is cleared and
 * the task is removed from its run queue.
 */
int32_t os_kill(uint16_t id)
{
	volatile uint32_t status;
	int32_t i, j, k;
	struct tcb_entry *krnl_task2;

	status = _di();
	if (id == 0){
		kprintf("\nKERNEL: can't kill the idle task");
		_ei(status);
		return OS_INVALID_ID;
	}
	krnl_task = &krnl_tcb[id];
	if ((krnl_task->ptask == 0) || (id >= MAX_TASKS)){
		kprintf("\nKERNEL: task doesn't exist");
		krnl_task = &krnl_tcb[krnl_current_task];
		_ei(status);
		return OS_INVALID_ID;
	}

	krnl_task->id = -1;
	krnl_task->ptask = 0;
	free(krnl_task->pstack);
	_set_task_sp(krnl_task->id, 0);
	_set_task_tp(krnl_task->id, 0);
	krnl_task->state = TASK_IDLE;
	krnl_tasks--;

	k = os_queue_count(krnl_run_queue);
	for (i = 0; i < k; i++)
		if (os_queue_get(krnl_run_queue, i) == krnl_task) break;
	if (!k || i == k) panic(PANIC_NO_TASKS_RUN);
	for (j = i; j > 0; j--)
		if (os_queue_swap(krnl_run_queue, j, j-1)) panic(PANIC_CANT_SWAP);
	krnl_task2 = os_queue_remhead(krnl_run_queue);

	if (!krnl_task2 || krnl_task2 != krnl_task) panic(PANIC_UNKNOWN_TASK_STATE);
	
	krnl_task = &krnl_tcb[krnl_current_task];
	kprintf("\nKERNEL: task died, id: %d, tasks left: %d", id, krnl_tasks);
	if (os_selfid() == id){
		_ei(status);
		os_yield();
	}else{
		_ei(status);
	}

	return OS_OK;
}

 /**
 * @brief Delays a task for an amount of time.
 * 
 * @param id is a task id number.
 * @param delay is the amount of time (in quantum / tick units).
 * 
 * @return OS_OK on success or OS_INVALID_ID if the referenced task does not exist.
 * 
 * A task is removed from its run queue and its state is marked as TASK_DELAYED. The task is put on the delay queue
 * and remains there until the dispatcher places it back to its run queue. Time is managed by the task dispatcher, which
 * counts down delays, controls the delay queue by cycling the tasks and removing them when the task delay has passed.
 */
int32_t os_delay(uint16_t id, uint32_t delay)
{
	int32_t i, j, k;
	volatile uint32_t status;
	struct tcb_entry *krnl_task2;
	
	if (delay == 0) return OS_ERROR;
	
	status = _di();
	if (id == 0){
		kprintf("\nKERNEL: can't delay the idle task");
		_ei(status);
		return OS_INVALID_ID;
	}
	krnl_task = &krnl_tcb[id];
	if ((krnl_task->ptask == 0) || (id >= MAX_TASKS)){
		kprintf("\nKERNEL: task doesn't exist");
		krnl_task = &krnl_tcb[krnl_current_task];
		_ei(status);
		return OS_INVALID_ID;
	}
	k = os_queue_count(krnl_run_queue);
	for (i = 0; i < k; i++)
		if (os_queue_get(krnl_run_queue, i) == krnl_task) break;
	if (!k || i == k) panic(PANIC_NO_TASKS_RUN);
	for (j = i; j > 0; j--)
		if (os_queue_swap(krnl_run_queue, j, j-1)) panic(PANIC_CANT_SWAP);
	krnl_task2 = os_queue_remhead(krnl_run_queue);
	
	krnl_task->state = TASK_DELAYED;
	krnl_task->delay = delay;
	if (os_queue_addtail(krnl_delay_queue, krnl_task2)) panic(PANIC_CANT_PLACE_DELAY);
	krnl_task = &krnl_tcb[krnl_current_task];
	_ei(status);
	
	return OS_OK;
}
