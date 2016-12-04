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

#ifndef _LIBC_H_
#define _LIBC_H_

#include <types.h>
#include <stdarg.h>

#define min(a,b)		((a)<(b)?(a):(b))
#define max(a,b)        ((a)>(b)?(a):(b))

void *memset(void *dst, int c, unsigned long bytes);
void *memcpy(void *dst, const void *src, unsigned long bytes);
int32_t puts(const char *str);
char *itoa(int i, char *s, int base);
int32_t printf(const char *format, ...);
int32_t sprintf(char *out, const char *format, ...);
int32_t strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
uint32_t strlen(const char *str);
uint32_t hash(unsigned char *str);


#endif
