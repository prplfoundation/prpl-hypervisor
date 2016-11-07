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


#include <stdarg.h>

/*
constants, tests and transformations
*/
#define NULL			((void *)0)
#define isprint(c)		(' '<=(c)&&(c)<='~')
#define isspace(c)		((c)==' '||(c)=='\t'||(c)=='\n'||(c)=='\r')
#define isdigit(c)		('0'<=(c)&&(c)<='9')
#define islower(c)		('a'<=(c)&&(c)<='z')
#define isupper(c)		('A'<=(c)&&(c)<='Z')
#define isalpha(c)		(islower(c)||isupper(c))
#define isalnum(c)		(isalpha(c)||isdigit(c))
#define min(a,b)		((a)<(b)?(a):(b))
#define ntohs(A)		(((A)>>8) | (((A)&0xff)<<8))
#define ntohl(A)		(((A)>>24) | (((A)&0xff0000)>>8) | (((A)&0xff00)<<8) | ((A)<<24))

/*
custom C library
*/
int8_t *strcpy(int8_t *dst, const int8_t *src);
int8_t *strncpy(int8_t *s1, int8_t *s2, int32_t n);
int8_t *strcat(int8_t *dst, const int8_t *src);
int8_t *strncat(int8_t *s1, int8_t *s2, int32_t n);
int32_t strcmp(const int8_t *s1, const int8_t *s2);
int32_t strncmp(int8_t *s1, int8_t *s2, int32_t n);
int8_t *strstr(const int8_t *string, const int8_t *find);
int32_t strlen(const int8_t *s);
int8_t *strchr(const int8_t *s, int32_t c);
int8_t *strpbrk(int8_t *str, int8_t *set);
int8_t *strsep(int8_t **pp, int8_t *delim);
int8_t *strtok(int8_t *s, const int8_t *delim);
void *memcpy(void *dst, const void *src, uint32_t n);
void *memmove(void *dst, const void *src, uint32_t n);
int32_t memcmp(const void *cs, const void *ct, uint32_t n);
void *memset(void *s, int32_t c, uint32_t n);
int32_t strtol(const int8_t *s, int8_t **end, int32_t base);
int32_t atoi(const int8_t *s);
float atof(const int8_t *p);
int8_t *itoa(int32_t i, int8_t *s, int32_t base);
int32_t puts(const int8_t *str);
int8_t *gets(int8_t *s);
int32_t abs(int32_t n);
int32_t random(void);
void srand(uint32_t seed);
int32_t printf(const int8_t *fmt, ...);
int32_t sprintf(int8_t *out, const int8_t *fmt, ...);


