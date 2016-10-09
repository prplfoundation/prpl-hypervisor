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

#include <libc.h>
#include <config.h>
#include <stdarg.h>
#include <uart.h>

#define PAD_RIGHT 1
#define PAD_ZERO 2

void *memset(void *dst, int c, unsigned long bytes){
	unsigned char *Dst = (unsigned char *)dst;

	while((int)bytes-- > 0)
		*Dst++ = (unsigned char)c;
	
	return dst;
}

void *memcpy(void *dst, const void *src, unsigned long bytes){
	if(((unsigned int)dst | (unsigned int)src | bytes) & 3){
		unsigned char *Dst = (unsigned char *)dst, *Src = (unsigned char *)src;
		while((int)bytes-- > 0)
			*Dst++ = *Src++;
	}else{
	
		unsigned int *Dst32 = (unsigned int *)dst, *Src32 = (unsigned int *)src;
		bytes >>= 2;
		while((int)bytes-- > 0)
			*Dst32++ = *Src32++;
	}
	return dst;
}

int32_t puts(const char *str){
	while(*str){
		if(*str == '\n')
			putchar('\r');
		putchar(*str++);
	}
  return 0;
}

char *itoa(int i, char *s, int base){
	char c;
	char *p = s;
	char *q = s;
	unsigned int h;

	if (base == 16){
		h = (unsigned int)i;
		do{
			*q++ = '0' + (h % base);
		} while (h /= base);
	
		for (*q = 0; p <= --q; p++){
			(*p > '9')?(c = *p + 39):(c = *p);
			(*q > '9')?(*p = *q + 39):(*p = *q);
			*q = c;
		}
	}else{
		if (i >= 0){
			do{
				*q++ = '0' + (i % base);
			} while (i /= base);
		}else{
			*q++ = '-';
			p++;
			do{
				*q++ = '0' - (i % base);
			} while (i /= base);
		}
		for (*q = 0; p <= --q; p++){
			c = *p;
			*p = *q;
			*q = c;
		}
	}
	return s;
}

static void printchar(char **str, int c)
{		
	if (str) {
		**str = c;
		++(*str);
	}
	else (void)putchar(c);
}

static int32_t prints(char **out, const char *string, int width, int pad)
{
	register int pc = 0, padchar = ' ';

	if (width > 0) {
		register int len = 0;
		register const char *ptr;
		for (ptr = string; *ptr; ++ptr) ++len;
		if (len >= width) width = 0;
		else width -= len;
		if (pad & PAD_ZERO) padchar = '0';
	}
	if (!(pad & PAD_RIGHT)) {
		for ( ; width > 0; --width) {
			printchar (out, padchar);
			++pc;
		}
	}
	for ( ; *string ; ++string) {
		printchar (out, *string);
		++pc;
	}
	for ( ; width > 0; --width) {
		printchar (out, padchar);
		++pc;
	}

	return pc;
}

static int32_t printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{
	char print_buf[12];
	register char *s;
	register int t, neg = 0, pc = 0;
	register unsigned int u = i;

	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints (out, print_buf, width, pad);
	}

	if (sg && b == 10 && i < 0) {
		neg = 1;
		u = -i;
	}

	s = print_buf + 11;
	*s = '\0';

	while (u) {
		t = u % b;
		if( t >= 10 )
			t += letbase - '0' - 10;
		*--s = t + '0';
		u /= b;
	}

	if (neg) {
		if( width && (pad & PAD_ZERO) ) {
			printchar (out, '-');
			++pc;
			--width;
		}
		else {
			*--s = '-';
		}
	}

	return pc + prints (out, s, width, pad);
}

static int32_t print(char **out, const char *format, va_list args )
{
	register int width, pad;
	register int pc = 0;
	char scr[2];

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			width = pad = 0;
			if (*format == '\0') break;
			if (*format == '%') goto out;
			if (*format == '-') {
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
			for ( ; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			if( *format == 's' ) {
				register char *s = (char *)va_arg( args, int );
				pc += prints (out, s?s:"(null)", width, pad);
				continue;
			}
			if( *format == 'd' ) {
				pc += printi (out, va_arg( args, int ), 10, 1, width, pad, 'a');
				continue;
			}
			if( *format == 'x' ) {
				pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'X' ) {
				pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'A');
				continue;
			}
			if( *format == 'u' ) {
				pc += printi (out, va_arg( args, int ), 10, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'c' ) {
				/* char are converted to int then pushed on the stack */
				scr[0] = (char)va_arg( args, int );
				scr[1] = '\0';
				pc += prints (out, scr, width, pad);
				continue;
			}
		}
		else {
			
			if (*format == '\n')
				printchar(out, '\r');
			
		out:
			printchar (out, *format);
			++pc;
		}
	}
	if (out) **out = '\0';
	va_end( args );
	return pc;
}

/*Based on Georges Menie Version
contributed by Christian Ettinger*/
int32_t printf(const char *format, ...)
{
	va_list args;
        
	va_start( args, format );
	
	return print( 0, format, args );
}

int32_t sprintf(char *out, const char *format, ...)
{
	va_list args;
        
	va_start( args, format );
	
	return print( &out, format, args );
}

//Some string manipulation functions
int32_t strcmp(const char *s1, const char *s2)
{
	int ret = 0;
	while	(!(ret = *(unsigned char *) s1 - *(unsigned char *) s2) && *s2) ++s1, ++s2;
	if (ret < 0)
		ret = -1;
	else if (ret > 0)
		ret = 1 ;
	return ret;
}

char *strcpy(char *dest, const char *src)
{
	char *save = dest;
	while(*dest++ = *src++);
	return save;
}

uint32_t strlen(const char *str)
{
	const char *s;
	for (s = str; *s; ++s);
	return (s - str);
}

uint32_t hash(unsigned char *str) {
	uint32_t hash = 5381;
	int c;

	while (c = *str++) {
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}

	return hash;
}
