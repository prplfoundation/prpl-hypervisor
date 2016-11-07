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

This code was written by Sergio Johann at Embedded System Group (GSE) at PUCRS/Brazil.

*/

#include <os.h>

extern uint32_t krln_free;
static mutex_t krnl_malloc;
static mem_header_t base;
static mem_header_t *freep = 0;
static uint32_t pool_free_pos = 0;

static void alloc_update_free(void)
{
	mem_header_t *q;
	
	q = (mem_header_t *)krnl_heap;
	krnl_free = HEAP_SIZE;
	while (q < (mem_header_t *) (krnl_heap + pool_free_pos)){
		q += q->s.size;
		krnl_free -= q->s.size * sizeof(mem_header_t);
	}
}

static void free2(void *ptr)
{
	mem_header_t *block;
	mem_header_t *p;

	block = ((mem_header_t *)ptr) - 1;

	for (p = freep; !(block > p && block < p->s.next); p = p->s.next){
		if (p >= p->s.next && (block > p || block < p->s.next))
			break;
	}

	if (block + block->s.size == p->s.next){
		block->s.size += p->s.next->s.size;
		block->s.next = p->s.next->s.next;
	}else{
		block->s.next = p->s.next;
	}

	if (p + p->s.size == block){
		p->s.size += block->s.size;
		p->s.next = block->s.next;
	}else{
		p->s.next = block;
	}

	freep = p;
}

void free(void *ptr)
{
	os_mtxlock(&krnl_malloc);
	free2(ptr);
	os_mtxunlock(&krnl_malloc);
}

static mem_header_t *morecore(uint32_t nquantas){
	uint32_t total_req_size;
	mem_header_t *h;

	if (nquantas < MIN_POOL_ALLOC_QUANTAS)
		nquantas = MIN_POOL_ALLOC_QUANTAS;
	total_req_size = nquantas * sizeof(mem_header_t);

	if (pool_free_pos + total_req_size <= HEAP_SIZE){
		h = (mem_header_t *)(krnl_heap + pool_free_pos);
		h->s.size = nquantas;
		free2((void*)(h + 1));
		pool_free_pos += total_req_size;
	}else{
		return 0;
	}

	return freep;
}

void *malloc(uint32_t size)
{
	mem_header_t *p;
	mem_header_t *prevp;

	uint32_t nquantas = (size + sizeof(mem_header_t) - 1) / sizeof(mem_header_t) + 1;
	
	os_mtxlock(&krnl_malloc);

	if ((prevp = freep) == 0){
		base.s.next = freep = prevp = &base;
		base.s.size = 0;
	}

	for (p = prevp->s.next; ; prevp = p, p = p->s.next){
		if (p->s.size >= nquantas){
			if (p->s.size == nquantas){
				prevp->s.next = p->s.next;
			}else{
				p->s.size -= nquantas;
				p += p->s.size;
				p->s.size = nquantas;
			}
			freep = prevp;
			alloc_update_free();
			os_mtxunlock(&krnl_malloc);
			
			return (void*) (p + 1);
		}
		if (p == freep){
			if ((p = morecore(nquantas)) == 0){
				os_mtxunlock(&krnl_malloc);
				return 0;
			}
		}
	}
}

void heapinit(void *heap, uint32_t len)
{
	base.s.next = 0;
	base.s.size = 0;
	freep = 0;
	pool_free_pos = 0;
	krnl_free = HEAP_SIZE;
	os_mtxinit(&krnl_malloc);
}

void *calloc(uint32_t qty, uint32_t type_size)
{
	void *buf;
	
	buf = (void *)malloc((qty * type_size));
	if (buf)
		memset(buf, 0, (qty * type_size));

	return (void *)buf;
}

void *realloc(void *ptr, uint32_t size){
	void *buf;

	if ((int32_t)size < 0) return NULL;
	if (ptr == NULL)
		return (void *)malloc(size);

	buf = (void *)malloc(size);
	if (buf){
		memcpy(buf, ptr, size);
		free(ptr);
	}

	return (void *)buf;
}

