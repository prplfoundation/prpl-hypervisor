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
 * @brief Configures UART2 as stdout and UART6 as alternative. 
 * @param baudrate_u2 UART2 baudrate.
 * @param baudrate_u6 UART6 baudrate.
 * @param sysclk System clock. 
 */
void init_uart(uint32_t baudrate_u2, uint32_t baudrate_u6, uint32_t sysclk){

    U2RXR = 1;  /* RPG6 to UART2 RX */
    RPB14R = 2; /* RPB14R to UART2 TX */

    U2STA = 0;
    U2BRG = ((int)( ((sysclk/2) / (16*baudrate_u2)) -1)) + 1;    
    U2MODE = UMODE_PDSEL_8NPAR |            /* 8-bit data, no parity */
                UMODE_ON;                       /* UART Enable */
    U2STASET = USTA_URXEN | USTA_UTXEN;     /* RX / TX Enable */
    
    U6RXR = 3; /* RPD0 to UART6 RX (pin 11) */
    RPF2R = 4; /* RPF2 to UART6 TX (pin 12)*/

    U6STA = 0;
	U6BRG = ((int)( ((sysclk/2) / (16*baudrate_u6)) -1)) + 1;    
    U6MODE = UMODE_PDSEL_8NPAR |            /* 8-bit data, no parity */
                UMODE_ON;                       /* UART Enable */
    U6STASET = USTA_URXEN | USTA_UTXEN;     /* RX / TX Enable */
}


/** 
 * @brief Write char to UART2. 
 * @param c Character to be writed. 
 */
void putchar(uint8_t c){   
    while(U2STA&USTA_UTXBF);
    U2TXREG = c;   
}


/** 
 * @brief Block and wait for a character. 
 * @return Read character. 
 */
uint32_t getchar(void){
	while(!(U2STA & USTA_URXDA));
    return (uint32_t)U2RXREG;
}

