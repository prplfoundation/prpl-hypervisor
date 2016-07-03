#include <config.h>
#include <types.h>
#include "pic32mz.h"


void init_uart(uint32_t baudrate_u2, uint32_t baudrate_u4, uint32_t sysclk){

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
    U6BRG = ((int)( ((sysclk/2) / (16*baudrate_u4)) -1)) + 1;    
    U6MODE = UMODE_PDSEL_8NPAR |            /* 8-bit data, no parity */
                UMODE_ON;                       /* UART Enable */
    U6STASET = USTA_URXEN | USTA_UTXEN;     /* RX / TX Enable */
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


int32_t kbhit(void){
        return (U2STA & USTA_URXDA);
}

uint32_t getchar(void){
    while(!kbhit());
    return (uint32_t)U2RXREG;
}

