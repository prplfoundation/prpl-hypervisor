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

This code was written by Carlos Moratelli at Embedded System Group (GSE) at PUCRS/Brazil.

*/

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

struct ll_node_t;
struct linkedlist_t;

typedef struct ll_node_t {
  unsigned int priority;
  void *ptr;
  struct linkedlist_t *list;
  struct ll_node_t *next;
  struct ll_node_t *prev;
} ll_node_t;

typedef struct linkedlist_t {  
  ll_node_t *head;
  ll_node_t *tail;
  unsigned int count;
} linkedlist_t;

void ll_append(linkedlist_t*, ll_node_t*);
void ll_remove(ll_node_t*);
ll_node_t* ll_get(linkedlist_t*, unsigned int);
void ll_init(linkedlist_t*);


#endif /* !LINKED_LIST_H */
