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
 * @file queue.c
 * @author Sergio Johann Filho
 * @date February 2016
 * 
 * @section DESCRIPTION
 * 
 * Queue manipulation primitives and auxiliary functions. Queue structures are allocated
 * only on the creation of queues, so little additional overhead regarding memory management
 * is incurred at runtime.
 */

#include <os.h>

/**
 * @brief Creates a queue of specified size.
 * 
 * @param size is the maximum number of elements.
 * 
 * @return pointer to the queue on success and NULL otherwise.
 */
struct queue *os_queue_create(int32_t size)
{
	struct queue *q = malloc(sizeof(struct queue));
	
	if (q==NULL){
		return NULL;
	}
	q->size = size + 1;
	q->data = malloc(q->size * sizeof(void *));
	if (q->data == NULL){
		free(q);
		return NULL;
	}
	q->head = q->tail = 0;
	q->elem = 0;
	return q;
}

/**
 * @brief Destroys a queue.
 * 
 * @param q is a pointer to a queue structure.
 * 
 * @return 0 when successful and -1 otherwise.
 */
int32_t os_queue_destroy(struct queue *q)
{
	if (q->head == q->tail){
		free(q->data);
		free(q);
		return 0;
	}
	return -1;
}

/**
 * @brief Counts the number of nodes in a queue.
 * 
 * @param q is a pointer to a queue structure.
 * 
 * @return the number of nodes.
 */
int32_t os_queue_count(struct queue *q)
{
	return q->elem;
}

/**
 * @brief Adds a node to the tail of the queue.
 * 
 * @param q is a pointer to a queue structure.
 * @param ptr a pointer to data belonging to the queue node.
 * 
 * @return 0 when successful and -1 otherwise.
 */
int32_t os_queue_addtail(struct queue *q, void *ptr)
{
	int32_t next;

	if (q->size == 0) return -1;
	next = (q->tail + 1) % q->size;
	if (next == q->head) return -1;
	q->data[q->tail] = ptr;
	q->tail = next;
	q->elem++;
	return 0;
}

/**
 * @brief Removes a node from the head of the queue.
 * 
 * @param q is a pointer to a queue structure.
 * 
 * @return pointer to node data on success and 0 otherwise.
 */
void *os_queue_remhead(struct queue *q)
{
	void *ret;
	
	if (q->size == 0) return NULL;
	if (q->head == q->tail) return NULL;
	ret = q->data[q->head];
	q->head = (q->head + 1) % q->size;
	q->elem--;
	return ret;
}

/**
 * @brief Removes a node from the tail of the queue.
 * 
 * @param q is a pointer to a queue structure.
 * 
 * @return pointer to node data on success and 0 otherwise.
 */
void *os_queue_remtail(struct queue *q)
{
	void *ret;
	
	if (q->size == 0) return NULL;
	if (q->head == q->tail) return NULL;
	ret = q->data[q->tail];
	q->tail = (q->tail - 1) % q->size;
	q->elem--;
	return ret;
}

/**
 * @brief Returns a node from the queue.
 * 
 * @param q is a pointer to a queue structure.
 * @param elem is the n-th element from the queue.
 * 
 * @return pointer to node data success and 0 otherwise.
 */
void *os_queue_get(struct queue *q, int32_t elem)
{
	void *ret;

	if (q->size == 0 || q->elem <= elem || q->head == q->tail) return 0;
	ret = q->data[(q->head + elem) % q->size];
	return ret;
}

/**
 * @brief Updates a node on the queue.
 * 
 * @param q is a pointer to a queue structure.
 * @param elem is the n-th element from the queue.
 * @param ptr a pointer to data belonging to the queue node. 
 * 
 * @return 0 success and -1 otherwise.
 */
int32_t os_queue_set(struct queue *q, int32_t elem, void *ptr)
{
	if (q->size == 0 || q->elem <= elem || q->head == q->tail) return -1;
	q->data[(q->head + elem) % q->size] = ptr;
	return 0;
}

/**
 * @brief Swap the position of two nodes in the queue.
 * 
 * @param q is a pointer to a queue structure.
 * @param elem1 is the first n-th element from the queue.
 * @param elem2 is the second n-th element from the queue.
 * 
 * @return 0 when successful and -1 otherwise.
 */
int32_t os_queue_swap(struct queue *q, int32_t elem1, int32_t elem2)
{
	void *t;
	
	if (q->size == 0 || q->elem <= elem1 || q->elem <= elem2 || q->head == q->tail) return -1;
	t = q->data[(q->head + elem1) % q->size];
	q->data[(q->head + elem1) % q->size] = q->data[(q->head + elem2) % q->size];
	q->data[(q->head + elem2) % q->size] = t;
	return 0;
}
