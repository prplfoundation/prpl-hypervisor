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

#include <config.h>

#ifdef IASIM

//#define UART_BASE ((unsigned char *) 0xBFFFF000)
#define UART_BASE ((unsigned char *) 0xBF000900)

#define UART_TX   ((unsigned char *) (UART_BASE + 0))
 
void putchar(char c){
	*UART_TX = c;		
}

#else

#define UART_BASE ((unsigned char *) 0xBF000900)

/* Register offsets */
#define UART_RBR             0x00    /* receive buffer       */

#define UART_THR             0x00    /* transmit holding     */

#define UART_IER             0x01*4    /* interrupt enable     */

#define UART_IIR             0x02*4    /* interrupt identification register */

#define UART_FCR             0x03*4    /* FIFO control register */
#define UART_FCR_ENABLE_FIFO    0x01 /* Enable the FIFO */
#define UART_FCR_CLEAR_RCVR     0x02 /* Clear the RCVR FIFO */
#define UART_FCR_CLEAR_XMIT     0x04 /* Clear the XMIT FIFO */

#define UART_LCR             0x04*4    /* line control register */
#define UART_LSR_THRE        0x20 /* Transmit-hold-register empty */

#define UART_LSR             0x05*4    /* line status register */

#define UART_MSR             0x06*4    /* Data Carrier Detect */


#define UART_TX   ((unsigned char *) (UART_BASE + UART_THR))

void putchar(char c){	
	while((AsmReadByte(UART_BASE+UART_LSR)&UART_LSR_THRE)==0);		
	AsmWriteByte(UART_BASE+UART_THR, c);	
}

#endif 