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

#include <linkedlist.h>
#include <libc.h>
#include <malloc.h>


void ll_init(linkedlist_t* ll){
	ll->head = NULL;
	ll->tail = NULL;
	ll->count = 0;	
}

void ll_append(linkedlist_t* ll, ll_node_t* n) {
  ll_node_t **a;
  n->next=NULL;

  if(ll && n) {
       
    for(a = &(ll->tail); *a && ((*a)->priority > n->priority); a = &((*a)->prev));
    
    n->prev = *a;
    *a = n;

    if(n->prev) {
      n->next = n->prev->next;
      n->prev->next = n;
    } else {
      n->next = ll->head;
      ll->head = n;
    }

    ll->count++;
    n->list = ll;
    
  }
}

void ll_remove(ll_node_t *n) {
  linkedlist_t *ll;

  if(n && (ll = n->list)) {
    
    if(n->list != ll) {      
      return;
    }

    ll->count--;

    if(n->prev)
      n->prev->next = n->next;
    else
      ll->head = n->next;

    if(n->next)
      n->next->prev = n->prev;
    else
      ll->tail = n->prev;

    n->list = NULL;
  }
}

ll_node_t *ll_get(linkedlist_t* ll, unsigned int i) {
  ll_node_t *ret;
  
  if(ll) {
    for(ret = ll->head ; ret && i; i--, ret = ret->next);

    return ret;
  }

  return NULL;
}
