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
 * @brief Queue data structure.
 */
struct queue {
	int32_t size;					/*!< queue size (maximum number of elements) */
	int32_t elem;					/*!< number of elements queued */
	int32_t head;					/*!< first element of the queue */
	int32_t tail;					/*!< last element of the queue */
	void **data;					/*!< pointer to an array of pointers to node data */
};

struct queue *os_queue_create(int32_t size);
int32_t os_queue_destroy(struct queue *q);
int32_t os_queue_count(struct queue *q);
int32_t os_queue_addtail(struct queue *q, void *ptr);
void *os_queue_remhead(struct queue *q);
void *os_queue_remtail(struct queue *q);
void *os_queue_get(struct queue *q, int32_t elem);
int32_t os_queue_set(struct queue *q, int32_t elem, void *ptr);
int32_t os_queue_swap(struct queue *q, int32_t elem1, int32_t elem2);
