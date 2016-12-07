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
#ifndef _LIBC_H
#define _LIBC_H

#include <types.h>
/*
constants, tests and transformations
*/
#define NULL			((void *)0)
#define USED			1
#define TRUE			1
#define FALSE			0
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

/* FIXME: picoTCP includes stdlib which conflicts with the following 
 * definitions.
 */
#ifndef PICOTCP
extern int32_t serial_select(uint32_t serial_number);
extern void putchar(int32_t value);
extern int32_t kbhit(void);
extern uint32_t getchar(void);
extern char *strcpy(char *dst, const char *src);
extern char *strncpy(char *s1, char *s2, int32_t n);
extern char *strcat(char *dst, const char *src);
extern char *strncat(char *s1, char *s2, int32_t n);
extern int32_t strcmp(const char *s1, const char *s2);
extern int32_t strncmp(char *s1, char *s2, int32_t n);
extern char *strstr(const char *string, const char *find);
extern int32_t strlen(const char *s);
extern char *strchr(const char *s, int32_t c);
extern char *strpbrk(char *str, char *set);
extern char *strsep(char **pp, char *delim);
extern char *strtok(char *s, const char *delim);
extern void *memcpy(void *dst, const void *src, uint32_t n);
extern void *memmove(void *dst, const void *src, uint32_t n);
extern int32_t memcmp(const void *cs, const void *ct, uint32_t n);
extern void *memset(void *s, int32_t c, uint32_t n);
extern int32_t strtol(const char *s, char **end, int32_t base);
extern int32_t atoi(const char *s);
extern float atof(const int8_t *p);
extern char *itoa(int32_t i, char *s, int32_t base);
extern int32_t puts(const char *str);
extern char *gets(char *s);
extern int32_t abs(int32_t n);
extern int32_t random(void);
extern void srand(uint32_t seed);
extern int32_t printf(char *fmt, ...);
extern int32_t sprintf(char *out, const char *fmt, ...);
extern void udelay(uint32_t usec);
extern uint32_t calc_diff_time(uint32_t now, uint32_t old);
#endif

typedef volatile unsigned int mutex_t;

void lock(mutex_t *mutex);
void spinlock(mutex_t *mutex);
void unlock(mutex_t *mutex);

#define UART2   2
#define UART6   6


/* IEEE single-precision definitions */
#define SNG_EXPBITS	8
#define SNG_FRACBITS	23
#define SNG_EXP_BIAS	127
#define SNG_EXP_INFNAN	255
#define EXCESS		126
#define SIGNBIT		0x80000000
#define HIDDEN		(1 << 23)
#define SIGN(fp)	((fp) & SIGNBIT)
#define EXP(fp)		(((fp) >> 23) & 0xFF)
#define MANT(fp)	(((fp) & 0x7FFFFF) | HIDDEN)
#define PACK(s,e,m)	((s) | ((e) << 23) | (m))

union float_long{
	float f;
	int32_t l;
};

#endif
