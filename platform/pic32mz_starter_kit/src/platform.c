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
 * @file platform.c
 * 
 * @section DESCRIPTION
 * 
 * Specific platform initialization.
 */

#include<pic32mz.h>
#include<config.h>

/**
 * @brief Called on early hypervisor initialization and responsable for basic the
 * platform configuration. 
 */
void early_platform_init(){
    init_uart(115200, 9600, CPU_FREQ);
    
    /* SPI1 pin map */
    ANSELBCLR = 0x0008; /* pin B3 used as output for CS */
    TRISBCLR = 0x0008;
    TRISDCLR = 0x0002;  /* pin D1 used as output for SCLK */
    TRISFCLR = 0x0020;  /* pin F5 used as output for MOSI */
    TRISFSET = 0x0010;  /* pin F4 used as input for MISO */
    LATFSET = 0x0010;

    SDI1R = 2;  /* pin F4 as SPI1 data input */
    RPF5R = 5;  /* pin F5 as SPI1 data output */

    /* SPI config settings */

    SPI1BRG = 4; /* Set clock divider to selected_clock/10: selected_clk/(2*(4+1)) */
    SPI1CON = 0x8120; /* enable SPI / master mode / data transition from high to low clk */
}


