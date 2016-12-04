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

/**
 * @file driver.c
 * 
 * @section DESCRIPTION
 * 
 * Driver support implementation. Implements the driver_init() call. 
 *
 */


#include <types.h>
#include <driver.h>

/* Symbols defined in the linker script indicating the start and the end of the driver's function table. */
extern uint32_t __drivers_table_init_start;
extern uint32_t __drivers_table_init_end;

/**
 * @brief Walk in the driver's function table calling all initialization functions. 
 */
void drivers_initialization(){
    uint32_t i;
    for(i=(uint32_t)(&__drivers_table_init_start); i<(int)(&__drivers_table_init_end); i+=sizeof(uint32_t*)){
        ((initcall_t*)*(uint32_t*)i)();
    }
}
