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

This code was written by Sergio Johann Filho at Embedded System Group (GSE) at PUCRS/Brazil.

*/

#ifndef _QUEUE_H
#define _QUEUE_H

#include <types.h>
/**
 * @brief Queue data structure.
 */
struct queue_t {
    int32_t size;                   /*!< queue size (maximum number of elements) */
    int32_t elem;                   /*!< number of elements queued */
    int32_t head;                   /*!< first element of the queue */
    int32_t tail;                   /*!< last element of the queue */
    void **data;                    /*!< pointer to an array of pointers to node data */
};
 
struct queue_t *queue_create(int32_t size);
int32_t queue_destroy(struct queue_t *q);
int32_t queue_count(struct queue_t *q);
int32_t queue_addtail(struct queue_t *q, void *ptr);
void *queue_remhead(struct queue_t *q);
void *queue_remtail(struct queue_t *q);
void *queue_get(struct queue_t *q, int32_t elem);
int32_t queue_set(struct queue_t *q, int32_t elem, void *ptr);
int32_t queue_swap(struct queue_t *q, int32_t elem1, int32_t elem2);

#endif