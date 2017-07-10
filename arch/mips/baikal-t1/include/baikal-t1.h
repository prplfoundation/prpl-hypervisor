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

/*
 *  Baikal-T (P5600) core address definitions
  *
 */

#ifndef _BAIKAL_T1_H
#define _BAIKAL_T1_H

#define UART0_BASE(a)	*(volatile unsigned*) (0xBF04A000 + (a<<2))
#define UART0_RBR	UART0_BASE(0)	
#define UART0_IER	UART0_BASE(1)
#define UART0_IIR	UART0_BASE(2)
#define UART0_LCR	UART0_BASE(3)
#define UART0_LSR	UART0_BASE(5)
#define UART0_MSR	UART0_BASE(6)	

#define UART1_BASE(a)	*(volatile unsigned*) (0xBF04B000 + (a<<2))
#define UART1_RBR	UART1_BASE(0)	
#define UART1_IER	UART1_BASE(1)
#define UART1_IIR	UART1_BASE(2)
#define UART1_LCR	UART1_BASE(3)
#define UART1_LSR	UART1_BASE(5)
#define UART1_MSR	UART1_BASE(6)	


typedef union {
    struct{
	    unsigned exl:1;
	    unsigned k:1;
	    unsigned :1;
	    unsigned u:1;
	    unsigned ie:1;
	    unsigned event:10;
	    unsigned :8;
	    unsigned ec:2;
	    unsigned :6;
	    unsigned m:1;
    };
    struct{
        unsigned w:32;
    };
} perf_control_t;


#endif /* _BAIKAL_T1_H */
