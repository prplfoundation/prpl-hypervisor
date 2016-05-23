#include <types.h>

#define PBDIV2 (*((uint32_t *) 0xBF801310))
#define PBDIV2CLR (*((uint32_t *) (0xBF801314)))
#define PBDIV2SET (*((uint32_t *) (0xBF801318)))

#define PBDIVON (1<<15)
#define PBDIV 0x7

#define SYSKEY (*((uint32_t *) 0xBF800030))

#define OSCCON (*((uint32_t *) 0xBF801200))
#define OSCCONCLR (*((uint32_t *) 0xBF801204))
#define OSCCONSET (*((uint32_t *) 0xBF801208))

#define NOSC (7<<8)
#define OSWEN 1

#define CONFCON (*((uint32_t *) 0xBF800000))
#define IOLOCK (1<<13)



void freq_config(){
    
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    PBDIV2 &= 0xFFFFFF80 | 0x00000000;
    SYSKEY = 0x33333333;
    while(OSCCON&OSWEN);

    
    /*SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    CONFCON &= (1<<13);
    SYSKEY = 0x33333333;
    while(OSCCON&OSWEN);*/
    
}
