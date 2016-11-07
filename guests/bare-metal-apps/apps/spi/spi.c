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

/* Simple SPI write application sample */

#include <arch.h>
#include <libc.h>


volatile int32_t t2 = 0;

void irq_timer(){
 t2++;     
}


uint8_t tx_buffer[] = "abcdefghijklmnopqstuvxz";


/* SP1 configuration */
void setupSPI1(){
    uint32_t rData;
    
    SPI1CON = 0;
    rData=SPI1BUF;
    SPI1BRG = 4; 
    SPI1STATCLR=0x40;
    SPI1CON = 0x8120; /* enable SPI / master mode / data transition from high to low clk */
   
}


void SPI_write_byte(uint8_t c){
    while(SPI1STAT & SPISTAT_SPITBF);
    SPI1BUF=c;
}

void SPI_write_buffer(uint8_t *buffer, uint32_t size){
    uint32_t i;
    LATBCLR= 8; /* enable CS */
    
    for(i=0;i<sizeof(tx_buffer);i++){
        SPI_write_byte(tx_buffer[i]);
    }

    /* Make sure that SPI is not busy before disable CS */
    while(SPI1STAT & SPISTAT_SPIBUSY);
    
    LATBSET= 8; /* disable CS */
}


int main() {
    uint32_t i;
    /* Select output serial 2 = UART2, 6 = UART6 */
    serial_select(UART2);
    
    printf("\nConfiguring SPI.");
    
    setupSPI1();
    
    while (1){
        SPI_write_buffer(tx_buffer, sizeof(tx_buffer));
   }
    
    return 0;
}
