#include <config.h>
#include <types.h>
#include <baikal-t1.h>
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

}


/** 
 * @brief Write char to UART2. 
 * @param c Character to be writed. 
 */
void putchar(uint8_t c){   
	while( !(UART_LSR&0x40) );
	UART_THR = c;   
}


/** 
 * @brief Block and wait for a character. 
 * @return Read character. 
 */
uint32_t getchar(void){
	
}

