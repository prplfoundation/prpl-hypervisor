/*
 * Copyright (c) 2016, prpl Foundation
 * 
 * Permission to use, copy, modify, and/or distribute this software for any purpose with or without 
 * fee is hereby granted, provided that the above copyright notice and this permission notice appear 
 * in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE 
 * FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * This code was written by Carlos Moratelli at Embedded System Group (GSE) at PUCRS/Brazil.
 * 
 */

/**
 * @file reset.c
 * 
 * @section DESCRIPTION
 * 
 * This is a software reset driver to the PIC32MZ Starter Kit since it lacks a reset button. 
 * 
 * 
 */

#include<pic32mz.h>
#include <driver.h>


/**
 * @brief Interrupt handler for the SW1 button.
 */
void sw1_button_handler(){
    IFSCLR(3) = 1 << 23;
    IECCLR(3) = 1<<23;
    SoftReset();
}

/**
 * @brief Driver init. Registers the interrupt handler routine and configures the RB12 pin
 * associated to the SW1 button. 
 */
void sw1_button_interrupt_init(){
    uint32_t offset;
    
    offset = register_interrupt(sw1_button_handler);
    OFF(119) = offset;
    
    CNCONBSET = 0x8000;
    CNENBSET = 1<<12;
    IPC(29) = 0x1f<<24;
    IFSCLR(3) = 1<<23;
    IECSET(3) = 1<<23;
    
    printf("Software reset interrupt (SW1) registered at offset 0x%x.", offset);
}

/**
 * @brief Performs a software reset to the board. 
 * 
 */
void SoftReset(){
    
    printf("\nReset button (sw1) pressed. Performing software reset.");
    
    /* Give time for message output before reset. */
    udelay(1000000);
    
    NVMKEY = 0x0;
    NVMKEY = 0xAA996655;
    NVMKEY = 0x556699AA;

    RSWRST |= 1;
    
    /* read RSWRST register to trigger reset */
    volatile int* p = &RSWRST;
    *p;
    
    /* prevent any unwanted code execution until reset occurs*/
    while(1) ;  
}

driver_init(sw1_button_interrupt_init);
