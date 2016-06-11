#include <types.h>
#include <config.h>
#include "pic32mz.h"


void init_uart(uint32_t baudrate, uint32_t sysclk){


    U2MODE = 0;         // disable autobaud, TX and RX enabled only, 8N1, idle=HIGH
    U2STA = 0x1400;     // enable TX and RX
    U2BRG = ((int)( ((sysclk/2) / (16*baudrate)) -1)) + 1;
    U2MODESET = 0x8840;
    
}



void putchar(char c){   
    while(U2STA&UTXBF);
    U2TXREG = c;   
}



void blink_led() {
   uint32_t i;
    
   LATH = 0x3333;
   TRISHCLR = 0xFFFF;
   ANSELDCLR = 0x8000;
   TRISDCLR = 0x8000;

   while(1) {
      i++;
      if((i&0x1FFFFFL)==0)
      {
          LATHINV = 0xFFFF;
          LATDINV = 0x8000;
      }
    }

}
