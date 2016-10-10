#ifndef _UART_H
#define _UART_H

#include <types.h>

void init_uart(uint32_t baudrate_u2, uint32_t baudrate_u6, uint32_t sysclk);
void putchar(uint8_t c);
uint32_t getchar(void);

#endif 