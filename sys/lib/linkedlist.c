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


/**
 * @file list.c
 * 
 * @section LICENSE
 *
 * @section DESCRIPTION
 * 
 * List manipulation primitives and auxiliary functions. List structures are allocated
 * dynamically at runtime, which makes them very flexible. Memory is allocated / deallocated
 * on demand, so additional memory management penalties are incurred.
 */


#include <libc.h>
#include <malloc.h>
#include <linkedlist.h>
 

/**
 * @brief Appends a new node to the end of the list.
 * 
 * @param lst is a pointer to a list structure.
 * @param item is a pointer to data belonging to the list node.
 * 
 * @return 0 when successful and -1 otherwise.
 */
int32_t list_append(struct list_t **lst, void *item)
{
	struct list_t *t1, *t2;
 
	t1 = (struct list_t *)malloc(sizeof(struct list_t));
	
	if(t1==NULL){
		return -1;
	}
	
	t1->elem = item;
	t1->next = NULL;
	
	if(*lst==NULL){
		*lst = t1;
		return 0;
	}
	
	t2 = *lst;
	while (t2->next){
		t2 = t2->next;
	}
	
	t2->next = t1;
	
	return 0;
}
 
 
/**
 * @brief Removes all elements of the list.
 * 
 * @param lst is a referece to a pointer to a list structure.
 * 
 * @return 0 when successful and -1 otherwise.
 */
int32_t list_remove_all(struct list_t **lst)
{
	struct list_t *aux;
	
	while(*lst){
		aux = *lst;
		*lst = (*lst)->next;
		free(aux->elem);
		free(aux);
	}
	
	return 0;
}
 

/**
 * @brief Returns the number of nodes in a list.
 * 
 * @param lst is a pointer to a list structure.
 * 
 * @return The number of elements in the list.
 */
int32_t list_count(struct list_t *lst)
{
	struct list_t *t1;
	int32_t i = 0;
 
	t1 = lst;
	while ((t1 = t1->next))
		i++;
 
	return i;
}

