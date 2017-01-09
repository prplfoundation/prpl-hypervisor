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

#include <arch.h>
#include <libc.h>
#include <hypercalls.h>

char *strcpy(char *dst, const char *src){
	char *dstSave=dst;
	int32_t c;

	do{
		c = *dst++ = *src++;
	} while(c);
	return dstSave;
}

char *strncpy(char *s1, char *s2, int32_t n){
	int32_t i;
	char *os1;

	os1 = s1;
	for (i = 0; i < n; i++)
		if ((*s1++ = *s2++) == '\0') {
			while (++i < n)
				*s1++ = '\0';
			return(os1);
		}
	return(os1);
}

char *strcat(char *dst, const char *src){
	int32_t c;
	char *dstSave=dst;

	while(*dst)
		++dst;
	do{
		c = *dst++ = *src++;
	} while(c);

	return dstSave;
}

char *strncat(char *s1, char *s2, int32_t n){
	char *os1;

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

int32_t strcmp(const char *s1, const char *s2){
	while (*s1 == *s2++)
		if (*s1++ == '\0')
			return(0);

	return(*s1 - *--s2);
}

int32_t strncmp(char *s1, char *s2, int32_t n){
	while (--n >= 0 && *s1 == *s2++)
		if (*s1++ == '\0')
			return(0);

	return(n<0 ? 0 : *s1 - *--s2);
}

char *strstr(const char *string, const char *find){
	int32_t i;
	for(;;){
		for(i = 0; string[i] == find[i] && find[i]; ++i);
		if(find[i] == 0)
			return (char*)string;
		if(*string++ == 0)
			return NULL;
	}
}

int32_t strlen(const char *s){
	int32_t n;

	n = 0;
	while (*s++)
		n++;

	return(n);
}

char *strchr(const char *s, int32_t c){
	while (*s != (char)c) 
		if (!*s++)
			return 0; 

	return (char *)s; 
}

char *strpbrk(char *str, char *set){
	while (*str != '\0'){
		if (strchr(set, *str) == 0)
			++str;
		else
			return (char *) str;
	}
	return 0;
}

char *strsep(char **pp, char *delim){
	char *p, *q;

	if (!(p = *pp))
		return 0;
	if ( (q = strpbrk (p, delim)) ){
		*pp = q + 1;
		*q = '\0';
	}else	*pp = 0;

	return p;
}

char *strtok(char *s, const char *delim){
	const char *spanp;
	int32_t c, sc;
	char *tok;
	static char *last;

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
	char *r1 = dst;
	const char *r2 = src;

	while (n--)
		*r1++ = *r2++;

	return dst;
}

void *memmove(void *dst, const void *src, uint32_t n){
	char *s = (char *)dst;
	const char *p = (const char *)src;

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
	const char *r1 = (const char *)cs;
	const char *r2 = (const char *)ct;

	while (n && (*r1 == *r2)) {
		++r1;
		++r2;
		--n;
	}

	return (n == 0) ? 0 : ((*r1 < *r2) ? -1 : 1);
}

void *memset(void *s, int32_t c, uint32_t n){
	char *p = (char *)s;

	while (n--)
		*p++ = (char)c;

	return s;
}

int32_t strtol(const char *s, char **end, int32_t base){
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

int32_t atoi(const char *s){
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

char *itoa(int32_t i, char *s, int32_t base){
	char *ptr = s, *ptr1 = s, tmp_char;
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

/**
 * Auxiliar buffer for print functions. 
 * 
 * Instead to make a hypercall for each character,
 * do a hypercall sending STRHYPERSZ characters. 
 * 
 */
#define STRHYPERSZ	1024

struct strhyper_t{
	char str[STRHYPERSZ];
	uint32_t size;
};

static struct strhyper_t strhyper = { str: {""}, size: 0 };

static void printchar(char **str, int32_t c){
	if (str){
		**str = c;
		++(*str);
	} else{
		strhyper.str[strhyper.size] = c;
		strhyper.size++;
		if (strhyper.size == STRHYPERSZ){
			uart_send(strhyper.str, strhyper.size);
			strhyper.size = 0;
		}
	}
}

int32_t puts(const char *str){
	while(*str){
		printchar(NULL, *str++);
	}
	if(strhyper.size > 0){
		uart_send(strhyper.str, strhyper.size);
		strhyper.size = 0;
	}
	return 0;
}

char *gets(char *s){
	int32_t c;
	char *cs;

	cs = s;
	while ((c = getchar()) != '\n' && c >= 0)
		*cs++ = c;
	if (c<0 && cs==s)
		return(NULL);
	*cs++ = '\0';
	return(s);
}


static int32_t prints(char **out, const char *string, int32_t width, int32_t pad){
	int32_t pc = 0, padchar = ' ';

	if (width > 0){
		int32_t len = 0;
		const char *ptr;
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

static int32_t printi(char **out, int32_t i, int32_t b, int32_t sg, int32_t width, int32_t pad, int32_t letbase){
	char print_buf[PRINT_BUF_LEN];
	char *s;
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

float atof(const int8_t *p){
	float val, power;
	int32_t i, sign;
	
	for (i = 0; isspace(p[i]); i++);
	
	sign = (p[i] == '-') ? -1 : 1;
	
	if (p[i] == '+' || p[i] == '-')
		i++;
	for (val = 0.0f; isdigit(p[i]); i++)
		val = 10.0f * val + (p[i] - '0');
	
	if (p[i] == '.')
		i++;
	for (power = 1.0f; isdigit(p[i]); i++){
		val = 10.0f * val + (p[i] - '0');
		power *= 10.0f;
	}
	
	return sign * val / power;
}

static int32_t print(int8_t **out, const int8_t *format, va_list args){
	int32_t width, pad;
	int32_t pc = 0;
	int8_t scr[2];
	int32_t i,j;
	int8_t buf[30];
	int32_t f1, precision_n = 6, precision_v = 1;
	float f;
	
	
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
				int8_t *s = (int8_t *)va_arg(args, size_t);
				pc += prints((char**)out, s?(char*)s:"(null)", width, pad);
				continue;
			}
			if(*format == 'd'){
				pc += printi((char**)out, va_arg(args, size_t), 10, 1, width, pad, 'a');
				continue;
			}
			if(*format == 'x'){
				pc += printi((char**)out, va_arg(args, size_t), 16, 0, width, pad, 'a');
				continue;
			}
			if(*format == 'X'){
				pc += printi((char**)out, va_arg(args, size_t), 16, 0, width, pad, 'A');
				continue;
			}
			if(*format == 'u'){
				pc += printi((char**)out, va_arg(args, size_t), 10, 0, width, pad, 'a');
				continue;
			}
			if(*format == 'c'){
				scr[0] = (int8_t)va_arg(args, size_t);
				scr[1] = '\0';
				pc += prints((char**)out, (char*)scr, width, pad);
				continue;
			}
			switch(*format){
				case '.':
					// decimal point: 1 to 9 places max. single precision is only about 7 places anyway.
					i = *++format - '0';
					format++;
					precision_n = i;
					precision_v = 1;
				case 'e':
				case 'E':
				case 'g':
				case 'G':
				case 'f':
					f = va_arg(args, double);
					if (f < 0.0f){
						printchar((char**)out, '-');
						f = -f;
					}
					itoa((int32_t)f,(char*)buf,10);
					j=0;
					while(buf[j]) printchar((char**)out, buf[j++]);
					printchar((char**)out, '.');
					for(j=0; j<precision_n; j++)
						precision_v *= 10;
					f1 = (f - (int32_t)f) * precision_v;
					i = precision_v / 10;
					while(i > f1){
						printchar((char**)out, '0');
						i /= 10;
					}
					itoa(f1,(char*)buf,10);
					j=0;
					if (f1 != 0)
						while(buf[j]) printchar((char**)out, buf[j++]);
						precision_n = 6;
					precision_v = 1;
					break;
			}
		}
		else {
			out:
			printchar((char**)out, *format);
			++pc;
		}
	}
	if (out) **out = '\0';
	va_end( args );
	return pc;
}

uint8_t* strdup(uint8_t* s){
	uint32_t n = strlen(s);
	uint8_t *aux = (uint8_t*)malloc(n+1);
	memcpy(aux, s, n+1);
	return aux;
}

int32_t printf(char *fmt, ...){
        va_list args;
	uint32_t size;
        
        va_start(args, fmt);
        size = print(0, (int8_t*)fmt, args);
	
	/* hypercall str send */
	uart_send(strhyper.str, strhyper.size);
	strhyper.size = 0;
	
	return size;
}

int32_t sprintf(char *out, const char *fmt, ...){
        va_list args;
        
        va_start(args, fmt);
	return print((int8_t**)&out, (int8_t*)fmt, args);
}


void mdelay(uint32_t msec){
    uint32_t now = mfc0 (CP0_COUNT, 0);
    
    while(!wait_time(now, msec));
}

uint32_t calc_diff_time(uint32_t now, uint32_t old){
	if (now >= old){
		return  now - old;
	}else{
		return 0xffffffff - (old - now);
	}
}

/*
software implementation of multiply/divide and 64-bit routines
*/
 
typedef union{
	int64_t all;
	struct{
#if LITTLE_ENDIAN
		uint32_t low;
		int32_t high;
#else
		int32_t high;
		uint32_t low;
#endif
	}s;
} dwords;
 
int32_t __mulsi3(uint32_t a, uint32_t b){
	uint32_t answer = 0;
 
	while(b){
		if(b & 1)
			answer += a;
		a <<= 1;
		b >>= 1;
	}
	return answer;
}
 
int64_t __muldsi3(uint32_t a, uint32_t b){
	dwords r;
 
	const int32_t bits_in_word_2 = (int32_t)(sizeof(int32_t) * 8) / 2;
	const uint32_t lower_mask = (uint32_t)~0 >> bits_in_word_2;
	r.s.low = (a & lower_mask) * (b & lower_mask);
	uint32_t t = r.s.low >> bits_in_word_2;
	r.s.low &= lower_mask;
	t += (a >> bits_in_word_2) * (b & lower_mask);
	r.s.low += (t & lower_mask) << bits_in_word_2;
	r.s.high = t >> bits_in_word_2;
	t = r.s.low >> bits_in_word_2;
	r.s.low &= lower_mask;
	t += (b >> bits_in_word_2) * (a & lower_mask);
	r.s.low += (t & lower_mask) << bits_in_word_2;
	r.s.high += t >> bits_in_word_2;
	r.s.high += (a >> bits_in_word_2) * (b >> bits_in_word_2);
 
	return r.all;
}
 
int64_t __muldi3(int64_t a, int64_t b){
	dwords x;
	x.all = a;
	dwords y;
	y.all = b;
	dwords r;
	r.all = __muldsi3(x.s.low, y.s.low);
	/*  r.s.high += x.s.high * y.s.low + x.s.low * y.s.high; */
	r.s.high += __mulsi3(x.s.high, y.s.low) + __mulsi3(x.s.low, y.s.high);
 
	return r.all;
}
 
uint32_t __udivmodsi4(uint32_t num, uint32_t den, int32_t modwanted){
	uint32_t bit = 1;
	uint32_t res = 0;
 
	while (den < num && bit && !(den & (1L << 31))) {
		den <<= 1;
		bit <<= 1;
	}
	while (bit){
		if (num >= den){
			num -= den;
			res |= bit;
		}
		bit >>= 1;
		den >>= 1;
	}
	if (modwanted)
		return num;
	return res;
}
 
int32_t __divsi3(int32_t a, int32_t b){
	int32_t neg = 0;
	int32_t res;
 
	if (a < 0){
		a = -a;
		neg = !neg;
	}
 
	if (b < 0){
		b = -b;
		neg = !neg;
	}
 
	res = __udivmodsi4(a, b, 0);
 
	if (neg)
		res = -res;
 
	return res;
}
 
int32_t __modsi3(int32_t a, int32_t b){
	int32_t neg = 0;
	int32_t res;
 
	if (a < 0){
		a = -a;
		neg = 1;
	}
 
	if (b < 0)
		b = -b;
 
	res = __udivmodsi4(a, b, 1);
 
	if (neg)	
		res = -res;
 
	return res;
}
 
uint32_t __udivsi3 (uint32_t a, uint32_t b){
	return __udivmodsi4(a, b, 0);
}
 
uint32_t __umodsi3 (uint32_t a, uint32_t b){
	return __udivmodsi4(a, b, 1);
}
 
int64_t __ashldi3(int64_t u, uint32_t b){
	dwords uu, w;
	uint32_t bm;
 
	if (b == 0)
		return u;
 
	uu.all = u;
	bm = 32 - b;
	
	if (bm <= 0){
		w.s.low = 0;
		w.s.high = (uint32_t) uu.s.low << -bm;
	}else{
		const uint32_t carries = (uint32_t) uu.s.low >> bm;
 
		w.s.low = (uint32_t) uu.s.low << b;
		w.s.high = ((uint32_t) uu.s.high << b) | carries;	
	}
     
	return w.all;
}
 
int64_t __ashrdi3(int64_t u, uint32_t b){
	dwords uu, w;
	uint32_t bm;
 
	if (b == 0)
		return u;
 
	uu.all = u;
	bm = 32 - b;
 
	if (bm <= 0){
		/* w.s.high = 1..1 or 0..0 */
		w.s.high = uu.s.high >> 31;
		w.s.low = uu.s.low >> -bm;
	}else{
		const uint32_t carries = (uint32_t) uu.s.high << bm;
 
		w.s.high = uu.s.high >> b;
		w.s.low = ((uint32_t) uu.s.low >> b) | carries;
	}
     
	return w.all;
}
 
int64_t __lshrdi3(int64_t u, uint32_t b){
	dwords uu, w;
	uint32_t bm;
 
	if (b == 0)
		return u;
 
	uu.all = u;
	bm = 32 - b;
 
	if (bm <= 0){
		w.s.high = 0;
		w.s.low = (uint32_t) uu.s.high >> -bm;
	}else{
		const uint32_t carries = (uint32_t) uu.s.high << bm;
     
		w.s.high = (uint32_t) uu.s.high >> b;
		w.s.low = ((uint32_t) uu.s.low >> b) | carries;
	}
     
	return w.all;
}
 
uint64_t __udivmoddi4(uint64_t num, uint64_t den, uint64_t *rem_p){
	uint64_t quot = 0, qbit = 1;
 
	if (den == 0){
		return 1 / ((uint32_t)den);
	}
 
	while ((int64_t)den >= 0){
		den <<= 1;
		qbit <<= 1;
	}
 
	while (qbit){	
		if (den <= num){
			num -= den;
			quot += qbit;
		}
		den >>= 1;
		qbit >>= 1;
	}
 
	if (rem_p)
		*rem_p = num;
	
	return quot;
}
 
uint64_t __umoddi3(uint64_t num, uint64_t den){
	uint64_t v;
 
	(void) __udivmoddi4(num, den, &v);
	return v;
}
 
uint64_t __udivdi3(uint64_t num, uint64_t den){
	return __udivmoddi4(num, den, NULL);
}
 
int64_t __moddi3(int64_t num, int64_t den){
	int minus = 0;
	int64_t v;
 
	if (num < 0){
		num = -num;
		minus = 1;
	}
	if (den < 0){
		den = -den;
		minus ^= 1;
	}
 
	(void) __udivmoddi4(num, den, (uint64_t *)&v);
	if (minus)
		v = -v;
	
	return v;
}
 
int64_t __divdi3(int64_t num, int64_t den){
	int minus = 0;
	int64_t v;
 
	if (num < 0){
		num = -num;
		minus = 1;
	}
	if (den < 0){
		den = -den;
		minus ^= 1;
	}
 
	v = __udivmoddi4(num, den, NULL);
	if (minus)
		v = -v;
 
	return v;
}

/*
 * software implementation of IEEE single-precision floating point
 */

#define FtoL(X)		(*(uint32_t *)&(X))
#define LtoF(X)		(*(float *)&(X))

/* add two floats */
float __addsf3(float a1, float a2){
	int32_t mant1, mant2;
	union float_long fl1, fl2;
	int32_t exp1, exp2;
	int32_t sign = 0;
	
	fl1.f = a1;
	fl2.f = a2;
	
	/* check for zero args */
	if (!fl1.l){
		fl1.f = fl2.f;
		goto test_done;
	}
	if (!fl2.l)
		goto test_done;
	
	exp1 = EXP(fl1.l);
	exp2 = EXP(fl2.l);
	
	if (exp1 > exp2 + 25)
		goto test_done;
	if (exp2 > exp1 + 25){
		fl1.f = fl2.f;
		goto test_done;
	}
	
	/* do everything in excess precision so's we can round later */
	mant1 = MANT (fl1.l) << 6;
	mant2 = MANT (fl2.l) << 6;
	
	if (SIGN (fl1.l))
		mant1 = -mant1;
	if (SIGN (fl2.l))
		mant2 = -mant2;
	
	if (exp1 > exp2){
		mant2 >>= exp1 - exp2;
	}else{
		mant1 >>= exp2 - exp1;
		exp1 = exp2;
	}
	mant1 += mant2;
	
	if (mant1 < 0){
		mant1 = -mant1;
		sign = SIGNBIT;
	}else{
		if (!mant1){
			fl1.f = 0;
			goto test_done;
		}
	}
	
	/* normalize up */
	while (!(mant1 & 0xE0000000)){
		mant1 <<= 1;
		exp1--;
	}
	
	/* normalize down? */
	if (mant1 & (1 << 30)){
		mant1 >>= 1;
		exp1++;
	}
	
	/* round to even */
	mant1 += (mant1 & 0x40) ? 0x20 : 0x1F;
	
	/* normalize down? */
	if (mant1 & (1 << 30)){
		mant1 >>= 1;
		exp1++;
	}
	
	/* lose extra precision */
	mant1 >>= 6;
	
	/* turn off hidden bit */
	mant1 &= ~HIDDEN;
	
	/* pack up and go home */
	fl1.l = PACK (sign, exp1, mant1);
	test_done:
	return (fl1.f);
}

/* subtract two floats */
float __subsf3(float a1, float a2){
	union float_long fl1, fl2;
	
	fl1.f = a1;
	fl2.f = a2;
	
	/* check for zero args */
	if (!fl2.l)
		return (fl1.f);
	if (!fl1.l)
		return (-fl2.f);
	
	/* twiddle sign bit and add */
	fl2.l ^= SIGNBIT;
	return __addsf3(a1, fl2.f);
}

/* compare two floats */
int32_t __cmpsf2(float a1, float a2){
	union float_long fl1, fl2;
	int32_t i = 0;
	
	fl1.f = a1;
	fl2.f = a2;
	
	if (SIGN (fl1.l) && SIGN (fl2.l)){
		fl1.l ^= SIGNBIT;
		fl2.l ^= SIGNBIT;
		i = 1;
	}
	if (i == 0){
		if (fl1.l < fl2.l)
			return (-1);
		if (fl1.l > fl2.l)
			return (1);
		return (0);
	}else{
		if (fl1.l > fl2.l)
			return (-1);
		if (fl1.l < fl2.l)
			return (1);
		return (0);
	}
}

int32_t __ltsf2(float a, float b){
	return -(__cmpsf2 (a, b) < 0);
}

int32_t __lesf2(float a, float b){
	return __cmpsf2 (a, b) > 0;
}

int32_t __gtsf2(float a, float b){
	return __cmpsf2 (a, b) > 0;
}

int32_t __gesf2(float a, float b){
	return (__cmpsf2 (a, b) >= 0) - 1;
}

int32_t __eqsf2(float a, float b){
	uint32_t c, d;
	memcpy(&c, &a, sizeof(c));
	memcpy(&d, &b, sizeof(d));
	//return !(*(int32_t *) & a == *(int32_t *) & b);
	return !(c == d);
}

int32_t __nesf2(float a, float b){
	uint32_t c, d;
	memcpy(&c, &a, sizeof(c));
	memcpy(&d, &b, sizeof(d));
//	return *(int32_t *) & a != *(int32_t *) & b;
	return c != d;
}

/* multiply two floats */
float __mulsf3(float a1, float a2){
	union float_long fl1, fl2;
	uint32_t result;
	int32_t exp;
	int32_t sign;
	
	fl1.f = a1;
	fl2.f = a2;
	
	if (!fl1.l || !fl2.l){
		fl1.f = 0;
		goto test_done;
	}
	
	/* compute sign and exponent */
	sign = SIGN (fl1.l) ^ SIGN (fl2.l);
	exp = EXP(fl1.l) - EXCESS;
	exp += EXP(fl2.l);
	
	fl1.l = MANT (fl1.l);
	fl2.l = MANT (fl2.l);
	
	/* the multiply is done as one 16x16 multiply and two 16x8 multiplies */
	result = (fl1.l >> 8) * (fl2.l >> 8);
	result += ((fl1.l & 0xFF) * (fl2.l >> 8)) >> 8;
	result += ((fl2.l & 0xFF) * (fl1.l >> 8)) >> 8;
	
	result >>= 2;
	if (result & 0x20000000){
		/* round */
		result += 0x20;
		result >>= 6;
	}else{
		/* round */
		result += 0x10;
		result >>= 5;
		exp--;
	}
	if (result & (HIDDEN<<1)){
		result >>= 1;
		exp++;
	}
	
	result &= ~HIDDEN;
	
	/* pack up and go home */
	fl1.l = PACK(sign, exp, result);
	test_done:
	return (fl1.f);
}

/* divide two floats */
float __divsf3(float a1, float a2){
	union float_long fl1, fl2;
	int32_t result;
	int32_t mask;
	int32_t exp, sign;
	
	fl1.f = a1;
	fl2.f = a2;
	
	/* subtract exponents */
	exp = EXP(fl1.l) - EXP(fl2.l) + EXCESS;
	
	/* compute sign */
	sign = SIGN(fl1.l) ^ SIGN(fl2.l);
	
	/* divide by zero??? */
	if (!fl2.l)
		/* return NaN or -NaN */
		return (sign ? 0xFFFFFFFF : 0x7FFFFFFF);
	
	/* numerator zero??? */
	if (!fl1.l)
		return (0);
	
	/* now get mantissas */
	fl1.l = MANT (fl1.l);
	fl2.l = MANT (fl2.l);
	
	/* this assures we have 25 bits of precision in the end */
	if (fl1.l < fl2.l){
		fl1.l <<= 1;
		exp--;
	}
	
	/* now we perform repeated subtraction of fl2.l from fl1.l */
	mask = 0x1000000;
	result = 0;
	while (mask){
		if (fl1.l >= fl2.l){
			result |= mask;
			fl1.l -= fl2.l;
		}
		fl1.l <<= 1;
		mask >>= 1;
	}
	
	/* round */
	result += 1;
	
	/* normalize down */
	exp++;
	result >>= 1;
	
	result &= ~HIDDEN;
	
	/* pack up and go home */
	fl1.l = PACK(sign, exp, result);
	return (fl1.f);
}

/* negate a float */
float __negsf2(float a1){
	union float_long fl1;
	
	fl1.f = a1;
	if (!fl1.l)
		return (0);
	
	fl1.l ^= SIGNBIT;
	return (fl1.f);
}

/* convert float to int32_t */
int32_t __fixsfsi(float a_fp){
	uint32_t a;
	uint32_t as;
	int32_t ae;
	int32_t af, shift;
	
	memcpy(&a, &a_fp, sizeof(a));
	as = a >> 31;
	ae = (a >> 23) & 0xff;
	af = 0x00800000 | (a & 0x007fffff);
	af <<= 7;
	shift = -(ae - 0x80 - 29);
	if(shift > 0){
		if(shift < 31) 
			af >>= shift;
		else 
			af = 0;
	}
	af = as ? -af: af;
	
	return af;
}

uint32_t __fixunssfsi(float a_fp){
	uint32_t a;
	uint32_t as;
	int32_t ae;
	int32_t af, shift;
	
	memcpy(&a, &a_fp, sizeof(a));
	as = a >> 31;
	ae = (a >> 23) & 0xff;
	af = 0x00800000 | (a & 0x007fffff);
	af <<= 7;
	shift = -(ae - 0x80 - 29);
	if(shift > 0){
		if(shift < 31) 
			af >>= shift;
		else 
			af = 0;
	}
	af = as ? -af: af;
	
	return af;
}

/* convert int32_t to float */
float __floatsisf(int32_t af){
	uint32_t a;
	uint32_t as, ae;
	float resp;
	
	as = af>=0 ? 0: 1;
	af = af>=0 ? af: -af;
	ae = 0x80 + 22;
	if(af == 0){ 
		memcpy(&resp, &af, sizeof(resp));
		return resp;
	}
	while(af & 0xff000000){
		++ae;
		af >>= 1;
	}
	while((af & 0xff800000) == 0){
		--ae;
		af <<= 1;
	}
	a = (as << 31) | (ae << 23) | (af & 0x007fffff);
	
	memcpy(&resp, &a, sizeof(resp));
	return resp;
}

float __floatunsisf(uint32_t af){
	uint32_t a;
	uint32_t as, ae;
	float resp;
	
	as = af>=0 ? 0: 1;
	af = af>=0 ? af: -af;
	ae = 0x80 + 22;
	if(af == 0) {
		memcpy(&resp, &af, sizeof(resp));
		return resp;
	}
	while(af & 0xff000000){
		++ae;
		af >>= 1;
	}
	while((af & 0xff800000) == 0){
		--ae;
		af <<= 1;
	}
	a = (as << 31) | (ae << 23) | (af & 0x007fffff);
	
	memcpy(&resp, &a, sizeof(resp));
	return resp;
}

time_t time(time_t *t){
	uint32_t aux = mfc0(CP0_COUNT, 0);
	if(t){
		*t = aux;
	}
	return aux;
}

uint32_t toupper (uint32_t ch){
	 if(('a' <= ch) && (ch <= 'z')){
		ch = 'A' + (ch - 'a');
	 }
	 
	return ch;
}

uint32_t tolower (uint32_t ch){
	if(('A' <= ch) && (ch <= 'Z')){
		ch = 'a' + (ch - 'A');	
	}
	
	return ch;	
}
