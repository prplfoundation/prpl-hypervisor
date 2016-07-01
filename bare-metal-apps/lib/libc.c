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

#include <pic32mz.h>
#include <libc.h>

int8_t *strcpy(int8_t *dst, const int8_t *src){
	int8_t *dstSave=dst;
	int32_t c;

	do{
		c = *dst++ = *src++;
	} while(c);
	return dstSave;
}

int8_t *strncpy(int8_t *s1, int8_t *s2, int32_t n){
	int32_t i;
	int8_t *os1;

	os1 = s1;
	for (i = 0; i < n; i++)
		if ((*s1++ = *s2++) == '\0') {
			while (++i < n)
				*s1++ = '\0';
			return(os1);
		}
	return(os1);
}

int8_t *strcat(int8_t *dst, const int8_t *src){
	int32_t c;
	int8_t *dstSave=dst;

	while(*dst)
		++dst;
	do{
		c = *dst++ = *src++;
	} while(c);

	return dstSave;
}

int8_t *strncat(int8_t *s1, int8_t *s2, int32_t n){
	int8_t *os1;

	os1 = s1;
	while (*s1++);
	--s1;
	while ((*s1++ = *s2++))
		if (--n < 0) {
			*--s1 = '\0';
			break;
		}
	return(os1);
}

int32_t strcmp(const int8_t *s1, const int8_t *s2){
	while (*s1 == *s2++)
		if (*s1++ == '\0')
			return(0);

	return(*s1 - *--s2);
}

int32_t strncmp(int8_t *s1, int8_t *s2, int32_t n){
	while (--n >= 0 && *s1 == *s2++)
		if (*s1++ == '\0')
			return(0);

	return(n<0 ? 0 : *s1 - *--s2);
}

int8_t *strstr(const int8_t *string, const int8_t *find){
	int32_t i;
	for(;;){
		for(i = 0; string[i] == find[i] && find[i]; ++i);
		if(find[i] == 0)
			return (char*)string;
		if(*string++ == 0)
			return NULL;
	}
}

int32_t strlen(const int8_t *s){
	int32_t n;

	n = 0;
	while (*s++)
		n++;

	return(n);
}

int8_t *strchr(const int8_t *s, int32_t c){
	while (*s != (int8_t)c) 
		if (!*s++)
			return 0; 

	return (int8_t *)s; 
}

int8_t *strpbrk(int8_t *str, int8_t *set){
	while (*str != '\0'){
		if (strchr(set, *str) == 0)
			++str;
		else
			return (int8_t *) str;
		return 0;
	}
}

int8_t *strsep(int8_t **pp, int8_t *delim){
	int8_t *p, *q;

	if (!(p = *pp))
		return 0;
	if (q = strpbrk (p, delim)){
		*pp = q + 1;
		*q = '\0';
	}else	*pp = 0;

	return p;
}

int8_t *strtok(int8_t *s, const int8_t *delim){
	const int8_t *spanp;
	int32_t c, sc;
	int8_t *tok;
	static int8_t *last;

	if (s == NULL && (s = last) == NULL)
		return (NULL);

	cont:
	c = *s++;
	for (spanp = delim; (sc = *spanp++) != 0;){
		if (c == sc)
		goto cont;
	}

	if (c == 0){
		last = NULL;
		return (NULL);
	}
	tok = s - 1;

	for(;;){
		c = *s++;
		spanp = delim;
		do{
			if ((sc = *spanp++) == c){
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				last = s;
				return (tok);
			}
		}while (sc != 0);
	}
}

void *memcpy(void *dst, const void *src, uint32_t n){
	int8_t *r1 = dst;
	const int8_t *r2 = src;

	while (n--)
		*r1++ = *r2++;

	return dst;
}

void *memmove(void *dst, const void *src, uint32_t n){
	int8_t *s = (int8_t *)dst;
	const int8_t *p = (const int8_t *)src;

	if (p >= s){
		while (n--)
			*s++ = *p++;
	}else{
		s += n;
		p += n;
		while (n--)
			*--s = *--p;
	}

	return dst;
}

int32_t memcmp(const void *cs, const void *ct, uint32_t n){
	const uint8_t *r1 = (const uint8_t *)cs;
	const uint8_t *r2 = (const uint8_t *)ct;

	while (n && (*r1 == *r2)) {
		++r1;
		++r2;
		--n;
	}

	return (n == 0) ? 0 : ((*r1 < *r2) ? -1 : 1);
}

void *memset(void *s, int32_t c, uint32_t n){
	uint8_t *p = (uint8_t *)s;

	while (n--)
		*p++ = (uint8_t)c;

	return s;
}

int32_t strtol(const int8_t *s, int8_t **end, int32_t base){
	int32_t i;
	uint32_t ch, value=0, neg=0;

	if(s[0] == '-'){
		neg = 1;
		++s;
	}
	if(s[0] == '0' && s[1] == 'x'){
		base = 16;
		s += 2;
	}
	for(i = 0; i <= 8; ++i){
		ch = *s++;
		if('0' <= ch && ch <= '9')
			ch -= '0';
		else if('A' <= ch && ch <= 'Z')
			ch = ch - 'A' + 10;
		else if('a' <= ch && ch <= 'z')
			ch = ch - 'a' + 10;
		else
			break;
		value = value * base + ch;
	}
	if(end)
		*end = (char*)s - 1;
	if(neg)
		value = -(int32_t)value;
	return value;
}

int32_t atoi(const int8_t *s){
	int32_t n, f;

	n = 0;
	f = 0;
	for(;;s++){
		switch(*s){
		case ' ':
		case '\t':
			continue;
		case '-':
			f++;
		case '+':
			s++;
		}
		break;
	}
	while(*s >= '0' && *s <= '9')
		n = n*10 + *s++ - '0';
	return(f?-n:n);
}

int8_t *itoa(int32_t i, int8_t *s, int32_t base){
	int8_t *ptr = s, *ptr1 = s, tmp_char;
	int32_t tmp_value;

	if (base < 2 || base > 36) {
		*s = '\0';
		return s;
	}
	do {
		tmp_value = i;
		i /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - i * base)];
	} while (i);
	if (tmp_value < 0)
		*ptr++ = '-';
	*ptr-- = '\0';
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return s;
}

int32_t puts(const int8_t *str){
	while(*str)
		putchar(*str++);
	putchar('\n');

	return 0;
}

int8_t *gets(int8_t *s){
	int32_t c;
	int8_t *cs;

	cs = s;
	while ((c = getchar()) != '\n' && c >= 0)
		*cs++ = c;
	if (c<0 && cs==s)
		return(NULL);
	*cs++ = '\0';
	return(s);
}

int32_t abs(int32_t n){
	return n>=0 ? n:-n;
}

static uint32_t rand1=0xbaadf00d;

int32_t random(void){
	rand1 = rand1 * 1103515245 + 12345;
	return (uint32_t)(rand1 >> 16) & 32767;
}

void srand(uint32_t seed){
	rand1 = seed;
}

/*
printf() and sprintf()
*/
#include <stdarg.h>
#define PAD_RIGHT 1
#define PAD_ZERO 2
#define PRINT_BUF_LEN 30

static void printchar(int8_t **str, int32_t c){
	if (str){
		**str = c;
		++(*str);
	} else
		(void)putchar(c);
}

static int32_t prints(int8_t **out, const int8_t *string, int32_t width, int32_t pad){
	int32_t pc = 0, padchar = ' ';

	if (width > 0){
		int32_t len = 0;
		const int8_t *ptr;
		for (ptr = string; *ptr; ++ptr) ++len;
		if (len >= width) width = 0;
		else width -= len;
		if (pad & PAD_ZERO) padchar = '0';
	}
	if (!(pad & PAD_RIGHT)){
		for ( ; width > 0; --width){
			printchar (out, padchar);
			++pc;
		}
	}
	for ( ; *string ; ++string){
		printchar (out, *string);
		++pc;
	}
	for ( ; width > 0; --width){
		printchar (out, padchar);
		++pc;
	}

	return pc;
}

static int32_t printi(int8_t **out, int32_t i, int32_t b, int32_t sg, int32_t width, int32_t pad, int32_t letbase){
	int8_t print_buf[PRINT_BUF_LEN];
	int8_t *s;
	int32_t t, neg = 0, pc = 0;
	uint32_t u = i;

	if (i == 0){
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints (out, print_buf, width, pad);
	}

	if (sg && b == 10 && i < 0){
		neg = 1;
		u = -i;
	}

	s = print_buf + PRINT_BUF_LEN-1;
	*s = '\0';

	while (u){
		t = u % b;
		if( t >= 10 )
			t += letbase - '0' - 10;
		*--s = t + '0';
		u /= b;
	}

	if (neg){
		if(width && (pad & PAD_ZERO)){
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

static int32_t print(int8_t **out, const int8_t *format, va_list args){
	int32_t width, pad;
	int32_t pc = 0;
	int8_t scr[2];

	for (; *format != 0; ++format){
		if (*format == '%'){
			++format;
			width = pad = 0;
			if (*format == '\0') break;
			if (*format == '%') goto out;
			if (*format == '-'){
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0'){
				++format;
				pad |= PAD_ZERO;
			}
			for (; *format >= '0' && *format <= '9'; ++format){
				width *= 10;
				width += *format - '0';
			}
			if(*format == 's'){
				int8_t *s = (int8_t *)va_arg(args, int32_t);
				pc += prints(out, s?s:"(null)", width, pad);
				continue;
			}
			if(*format == 'd'){
				pc += printi(out, va_arg(args, int32_t), 10, 1, width, pad, 'a');
				continue;
			}
			if(*format == 'x'){
				pc += printi(out, va_arg(args, int32_t), 16, 0, width, pad, 'a');
				continue;
			}
			if(*format == 'X'){
				pc += printi(out, va_arg(args, int32_t), 16, 0, width, pad, 'A');
				continue;
			}
			if(*format == 'u'){
				pc += printi(out, va_arg(args, int32_t), 10, 0, width, pad, 'a');
				continue;
			}
			if(*format == 'c'){
				scr[0] = (int8_t)va_arg(args, int32_t);
				scr[1] = '\0';
				pc += prints(out, scr, width, pad);
				continue;
			}
		}
		else {
		out:
			printchar(out, *format);
			++pc;
		}
	}
	if (out) **out = '\0';
	va_end( args );
	return pc;
}

int printf(const int8_t *fmt, ...){
        va_list args;
        
        va_start(args, fmt);
        return print(0, fmt, args);
}

int sprintf(int8_t *out, const int8_t *fmt, ...){
        va_list args;
        
        va_start(args, fmt);
        return print(&out, fmt, args);
}


void udelay(uint32_t usec){
    uint32_t now = mfc0 (CP0_COUNT, 0);
    uint32_t final = now + usec * (CPU_SPEED / 1000000) / 2;

    for (;;) {
        now = mfc0 (CP0_COUNT, 0);
        if ((int32_t) (now - final) >= 0) break;
    }
}

void putchar(int32_t value){
    while(U2STA & USTA_UTXBF);
    U2TXREG = value;    
}

int32_t kbhit(void){
        return (U2STA & USTA_URXDA);
}

int32_t getchar(void){
    while(!kbhit());
    return (uint8_t)U2RXREG;
}
