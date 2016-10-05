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

#ifndef _MALLOC_H_
#define _MALLOC_H_

#include <types.h>

#define NULL            ((void *)0)

#if 0
typedef int32_t word_align;

union header{    /* block header */
struct{
union header *ptr;  /* mnext block if on free list */
uint32_t size;   /* size of this block */
} s;
word_align x;    /* force block alignment */
};

typedef union header mem_header;

#else

#define align4(x) ((((x) + 3) >> 2) << 2)

typedef uint32_t size_t;

struct mem_block {
	struct mem_block *next;		/* pointer to the next block */
	size_t size;			/* aligned block size. the LSB is used to define if the block is used */
};

#endif 

#endif
