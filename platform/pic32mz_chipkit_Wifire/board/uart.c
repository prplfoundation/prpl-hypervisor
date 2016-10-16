#include <config.h>
#include <types.h>
#include "pic32mz.h"


void init_uart(uint32_t baudrate_u4, uint32_t baudrate_u1, uint32_t sysclk){
	U4BRG = BRG_BAUD (CPU_SPEED / 2, baudrate_u4);
	U4STA = 0;
	U4MODE = UMODE_PDSEL_8NPAR |		/* 8-bit data, no parity */
		UMODE_ON;			/* UART Enable */
	U4STASET = USTA_URXEN |	USTA_UTXEN;	/* RX / TX Enable */
	
	/*TODO: configure UART1 */

}



void putchar(char c){   
	while(U4STA&USTA_UTXBF);
	U4TXREG = c;   
}


int32_t kbhit(void){
        return (U4STA & USTA_URXDA);
}

uint32_t getchar(void){
    while(!kbhit());
    return (uint32_t)U4RXREG;
}

