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
 * @file condvar.c
 * @author Sergio Johann Filho
 * @date May 2016
 * 
 * @section DESCRIPTION
 * 
 * Condition variable synchronization primitives.
 * 
 */

#include <os.h>

/**
 * @brief Initializes a condition variable
 * 
 * @param c is a pointer to a condition variable.
 *
 * @return OS_OK on success and OS_ERROR if the condition variable could not be allocated in memory.
 */
int32_t os_condinit(cond_t *c)
{
	volatile uint32_t status;

	status = _di();
	c->cond_queue = os_queue_create(MAX_TASKS);
	if (c->cond_queue == NULL){
		_ei(status);
		return OS_ERROR;
	}else{
		_ei(status);
		return OS_OK;
	}
}

/**
 * @brief Destroys a condition variable.
 * 
 * @param c is a pointer to a condition variable.
 * 
 * @return OS_OK on success and OS_ERROR if the condition variable could not be
 * removed from memory.
 */
int32_t os_conddestroy(cond_t *c)
{
	volatile uint32_t status;

	status = _di();
	if (os_queue_destroy(c->cond_queue)){
		_ei(status);
		return OS_ERROR;
	}else{
		_ei(status);
		return OS_OK;
	}
}

/**
 * @brief Wait on a condition variable.
 * 
 * @param c is a pointer to a condition variable.
 * @param m is a pointer to a mutex.
 * 
 * Implements the atomic condition wait operation. The call should always be invoked
 * with the mutex locked. The current task is put in a queue on the condition variable,
 * its state is set to blocked and unlocks the mutex atomically, then yields the
 * processor. When woke up (by a signalling task), the task locks the mutex and returns.
 */
void os_condwait(cond_t *c, mutex_t *m)
{
	volatile uint32_t status;
	struct tcb_entry *krnl_task2;

	status = _di();
	krnl_task2 = &krnl_tcb[krnl_current_task];
	if (os_queue_addtail(c->cond_queue, krnl_task2))
		panic(PANIC_NUTS_SEM);
	else
		krnl_task2->state = TASK_BLOCKED;
	os_mtxunlock(m);
	_ei(status);
	os_yield();
	os_mtxlock(m);
}

/**
 * @brief Signal a condition variable.
 * 
 * @param c is a pointer to a condition variable.
 * 
 * Implements the condition signal operation for one waiting task. The call removes a
 * task from the waiting queue and unblocks it. If no tasks are waiting for the condition,
 * the signal is lost.
 */
void os_condsignal(cond_t *c)
{
	volatile uint32_t status;
	struct tcb_entry *krnl_task2;

	status = _di();
	krnl_task2 = os_queue_remhead(c->cond_queue);
	if (krnl_task2)
		krnl_task2->state = TASK_READY;
	_ei(status);
}

/**
 * @brief Signal (broadcast) a condition variable.
 * 
 * @param c is a pointer to a condition variable.
 * 
 * Implements the condition signal broadcast operation for all waiting tasks. The call
 * unblocks and removes all tasks from the waiting queue. If no tasks are waiting for the
 * condition, the signal is lost.
 */
void os_condbroadcast(cond_t *c)
{
	volatile uint32_t status;
	struct tcb_entry *krnl_task2;
		
	status = _di();
	while (os_queue_count(c->cond_queue)){
		krnl_task2 = os_queue_remhead(c->cond_queue);
		if (krnl_task2)
			krnl_task2->state = TASK_READY;
	}
	_ei(status);
}
