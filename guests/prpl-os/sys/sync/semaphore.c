/**
 * @file semaphore.c
 * @author Sergio Johann Filho
 * @date February 2016
 * 
 * @section DESCRIPTION
 * 
 * Semaphore synchronization primitives.
 * 
 */

#include <os.h>

/**
 * @brief Initializes a semaphore and defines its initial value.
 * 
 * @param s is a pointer to a semaphore.
 * @param value is the semaphore initial value.
 * 
 * @return OS_OK on success and OS_ERROR if the semaphore could not be allocated in
 * memory or its initial value is less than zero.
 */
int32_t os_seminit(sem_t *s, int32_t value)
{
	volatile uint32_t status;

	status = _di();
	s->sem_queue = os_queue_create(MAX_TASKS);
	if ((s->sem_queue == NULL) || (value < 0)){
		_ei(status);
		return OS_ERROR;
	}else{
		s->count = value;
		_ei(status);
		return OS_OK;
	}
}

/**
 * @brief Destroys a semaphore.
 * 
 * @param s is a pointer to a semaphore.
 * 
 * @return OS_OK on success and OS_ERROR if the semaphore could not be removed from memory.
 */
int32_t os_semdestroy(sem_t *s)
{
	volatile uint32_t status;

	status = _di();
	if (os_queue_destroy(s->sem_queue)){
		_ei(status);
		return OS_ERROR;
	}else{
		_ei(status);
		return OS_OK;
	}
}

/**
 * @brief Wait on a semaphore.
 * 
 * @param s is a pointer to a semaphore.
 * 
 * Implements the atomic P() operation. The semaphore count is decremented and
 * calling task is blocked and queued on the semaphore if the count reaches a negative
 * value. If not, the task continues its execution.
 */
void os_semwait(sem_t *s)
{
	volatile uint32_t status;
	struct tcb_entry *krnl_task2;

	status = _di();
	s->count--;
	if (s->count < 0){
		krnl_task2 = &krnl_tcb[krnl_current_task];
		if (os_queue_addtail(s->sem_queue, krnl_task2))
			panic(PANIC_NUTS_SEM);
		else
			krnl_task2->state = TASK_BLOCKED;
		_ei(status);
		os_yield();
	}else{
		_ei(status);
	}
}

/**
 * @brief Signal a semaphore.
 * 
 * @param s is a pointer to a semaphore.
 * 
 * Implements the atomic V() operation. The semaphore count is incremented and
 * the task from the head of the semaphore queue is unblocked if the count is less
 * than or equal to zero.
 */
void os_sempost(sem_t *s)
{
	volatile uint32_t status;
	struct tcb_entry *krnl_task2;

	status = _di();
	s->count++;
	if (s->count <= 0){
		krnl_task2 = os_queue_remhead(s->sem_queue);
		if (krnl_task2 == NULL)
			panic(PANIC_NUTS_SEM);
		else
			krnl_task2->state = TASK_READY;
	}
	_ei(status);
}
