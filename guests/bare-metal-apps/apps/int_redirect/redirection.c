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

/* This example shows how to use the hypervisor's interrupt redirection feature.
 * 
 * Interrupt redirection allows for the hypervisor to inject a virtual interrupt on a 
 * guest that desires to implements its own device driver. PIC32MZ lacks of the interrupt
 * pass-through feature. Interrupt redirection emulates the interrupt pass-through.
 * 
 * Use the valid CFG file (redirection.cfg) to configure the hypervisor to this application.
 * 
 * This application receives UART RX interrupts and read the characters storing them into an array.
 * 
 * To send characters to the application use:
 * 	echo "Hello World!" > /dev/ttyACM0
 * 
 * Substitute /dev/ttyACM0 by your serial port. 
 * 
 * See the User's Guide for more information. 
 * 
 * 
 */

#include <arch.h>
#include <libc.h>
#include <hypercalls.h>
#include <guest_interrupts.h>
#include <platform.h>
#include <io.h>

#define DATA_SZ 128

static uint8_t str[DATA_SZ];
static volatile int32_t t2 = 0;

void irq_timer(){
	t2++;
}

void irq_uart2_rx(){
	static uint32_t i = 0;
	
	while (read(U2STA) & USTA_URXDA){
		str[i%DATA_SZ] = read(U2RXREG);
		i++;
	}
	
	str[i%DATA_SZ] = '\0';
	
	reenable_interrupt(GUEST_USER_DEFINED_INT_1);
}


int main() {
	uint32_t timer = 0;
    
	interrupt_register(irq_timer, GUEST_TIMER_INT);
	
	interrupt_register(irq_uart2_rx, GUEST_USER_DEFINED_INT_1);
	
	ENABLE_LED1;
   
	while (1){
		if(wait_time(timer, 1000)) {
			printf("\nRead data: %s \nTotal of %d timer ticks.", str, t2);
        
			/* Blink Led */
			TOGGLE_LED1;
			
			timer = mfc0(CP0_COUNT, 0);
		}
	}
    
	return 0;
}

