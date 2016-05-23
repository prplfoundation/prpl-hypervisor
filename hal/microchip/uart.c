#include <types.h>


#define U4MODE (*((uint32_t *) 0xBF822600))
#define U4MODECLR (*((uint32_t *) (0xBF822604)))
#define U4MODESET (*((uint32_t *) (0xBF822608)))

#define U4STAT (*((uint32_t *) 0xBF822610))
#define U4STATCLR (*((uint32_t *) (0xBF822614)))
#define U4STATSET (*((uint32_t *) (0xBF822618)))

#define U4TX (*((uint32_t *) 0xBF822620))

#define U4BRG (*((uint32_t *) 0xBF822640))
#define U4BRGCLR (*((uint32_t *) (0xBF822644)))
#define U4BRGSET (*((uint32_t *) (0xBF822648)))

#define UTXBF (1<<9)
#define UTXEN (1<<10)
#define RTSMD (1<<11)
#define URXEN (1<<12)
#define UTXINV (1<<13)
#define FRZ (1<<14)
#define ON (1<<15)

#define RPD9R (*((uint32_t *) 0xBF8015E4))
#define RPG6R (*((uint32_t *) 0xBF801698))
#define RPB8R (*((uint32_t *) 0xBF801560))
#define RPB15R (*((uint32_t *) 0xBF80157C))
#define RPD4 (*((uint32_t *) 0xBF8015D0))
#define RPB0R (*((uint32_t *) 0xBF801540))
#define RPE3 (*((uint32_t *) 0xBF80160c))
#define RPB7R (*((uint32_t *) 0xBF80155c))
#define RPF12R (*((uint32_t *) 0xBF801670))
#define RPD12R (*((uint32_t *) 0xBF8015f0))
#define RPF8R (*((uint32_t *) 0xBF801660))
#define RPB9R (*((uint32_t *) 0xBF801564))
#define RPF2R (*((uint32_t *) 0xBF801648))


void init_uart(uint32_t baudrate, uint32_t sysclk){
    uint32_t stat;

    RPF8R = 2;
    U4MODE = 0;          
    U4STAT = 0x1400;     
    U4BRG = ((int)( (sysclk / (16*baudrate)) -1)) + 1;
    U4MODESET = 0x8840;
    
}



void putchar(char c){   
    while(U4STAT&UTXBF);
    U4TX = c;   
}
