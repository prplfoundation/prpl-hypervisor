#include <config.h>
#include <types.h>
#include <pic32mz.h>
#include <uart.h>

/**
 * @file uart.c
 * 
 * @section DESCRIPTION
 * 
 * UART initialization and low level UART functions. 
 * 
 */

/** 
 * @brief Configures UART2 as stdout alternative. 
 * @param baudrate_u2 UART2 baudrate.
 * @param baudrate_u6 UART6 baudrate.
 * @param sysclk System clock. 
 */
void init_uart(uint32_t baudrate_u2, uint32_t baudrate_u6, uint32_t sysclk){

	U1RXR = 0b0011; /* // RPD10 -> U1RX */
	RPD15R = 0b0001; /*  RPD15 -> U1TX */

	U1STA = 0;
	U1BRG = ((int)( ((sysclk/2) / (16*baudrate_u2)) -1)) + 1;    
	U1MODE = UMODE_PDSEL_8NPAR |            /* 8-bit data, no parity */
		UMODE_ON;                       /* UART Enable */
	U1STASET = USTA_URXEN | USTA_UTXEN;     /* RX / TX Enable */
	
	baudrate_u6 = 0; /* Only UART1 used for now. */
}


/** 
 * @brief Write char to UART2. 
 * @param c Character to be writed. 
 */
void putchar(uint8_t c){   
	while(U1STA&USTA_UTXBF);
	U1TXREG = c;   
}


/** 
 * @brief Block and wait for a character. 
 * @return Read character. 
 */
uint32_t getchar(void){
	while(!(U1STA & USTA_URXDA));
	return (uint32_t)U1RXREG;
}

