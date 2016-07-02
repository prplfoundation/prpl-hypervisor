#include <config.h>
#include <types.h>
#include "pic32mz.h"


void init_uart(uint32_t baudrate_u2, uint32_t baudrate_u4, uint32_t sysclk){

    U2RXR = 1;  // RPG6 -> U2RX
    RPB14R = 2; //RPB14R ->U2TX

    U2MODE = 0;         // disable autobaud, TX and RX enabled only, 8N1, idle=HIGH
    U2STA = 0x1400;     // enable TX and RX
    U2BRG = ((int)( ((sysclk/2) / (16*baudrate_u2)) -1)) + 1;
    U2MODESET = 0x8810;
    
    RPG6R = 0x2; /* RPG6 to UART4 TX */
    U4RXR = 0x2;  /* RPB14 to UART4 RX */
    
    U4MODE = 0;         // disable autobaud, TX and RX enabled only, 8N1, idle=HIGH
    U4STA = 0x1400;     // enable TX and RX
    U4BRG = ((int)( ((sysclk/2) / (16*baudrate_u4)) -1)) + 1;
    U4MODESET = 0x8810;
    
}



void putchar(char c){   
    while(U2STA&USTA_UTXBF);
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
