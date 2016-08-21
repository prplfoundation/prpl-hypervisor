#include <config.h>
#include <types.h>
#include <pic32mz.h>
#include <globals.h>
#include <usb.h>

void usb_interrupt_enable(uint32_t general, uint32_t transmit, uint32_t receive){
    USBCSR1bits.TRXIE = transmit;
    USBCSR2bits.RCVIE = receive;
    USBCSR2bits.GENIE = general;
}
    
uint32_t usb_start(uint32_t operation_mode, uint32_t speed){
    
    if (operation_mode != HOST_MODE){
        WARNINGS("Only Host Mode is supported");
        return 1;
    }
    
    OVERCURRENT_PIN_ENABLE;
    
    /* USBID pin (RPF3) pin in PULL DOWN */
    USBID_PIN_PULL_DOWN_F;
    USBID_PIN_PULL_DOWN_C;
    
    /* Enable USB general interrutps  */
    USBCRCONbits.USBIE = 1;
    
    /* 3 seconds reset USB */
    USBEOFRSTbits.SOFRST = 0x3;
    udelay(3000000);
    USBEOFRSTbits.SOFRST = 0;
    
    /* Interrupts disable */
    usb_interrupt_enable(0, 0, 0);
    
    /* Speed select */
    USBCSR0bits.HSEN = speed;
    
    /* USB ID override enable */
    USBCRCONbits.USBIDOVEN = 1;
    
    /* USB ID monitoring */
    USBCRCONbits.PHYIDEN = 1;
    
    /* USB ID value */ 
    USBCRCONbits.USBIDVAL = 0;
    /* Harmony code is writting twice here. Lets do the same. */
    USBCRCONbits.USBIDVAL = 0;
    
    //USBCSR0bits.SOFTCONN = 1; 
    
    /* Configure interrupt vectors, clear and enable interrupts */
    USB_INTERRUPT_CLEAR;
    USB_INTERRUPT_ENABLE;
    
    USB_DMA_INTERRUPT_CLEAR;
    USB_DMA_INTERRUPT_ENABLE;
    
    
    ENABLE_POWER_SUPPLY;

    usb_interrupt_enable(0xb0, 1, 0);
    
    while(!IS_VALID_VBUS);
    
    SESSION_ENABLE;
    
    return 0;
}

void usb_device_attach(){
    //printf("\n%d %d %d 0%x %d %d %d", IS_B_MODE, IS_VALID_VBUS, IS_DEVICE_CONNECTED, USBCSR2bits.INTERRUPTS, USBCRCONbits.USBIF, USBCRCONbits.USBIE, USBOTGbits.HOSTMODE);
    
    
    if (IS_B_MODE && IS_VALID_VBUS){
        printf("\nsession enable");
        SESSION_ENABLE;
    }
  /*  if(IS_DEVICE_CONNECTED){
        printf("device connected");
    }*/
    
    if(IS_OVERCURRENT){
        DISABLE_POWER_SUPPLY;
    }
    
}

void usb_int_handler(){
    /* USB interrupts are not persistent (clear on read) */
    uint32_t interrupts = USBCSR2bits.INTERRUPTS;
    
    if (interrupts & USB_DEVCONN_INT){
        printf("\nDevice Connected!");
    }

    if (interrupts & USB_DEVDISCONN_INT){
        printf("\nDevice Disconnected!");
    }
    
    if (interrupts & USB_VBUSERR_INT){
        printf("\nDevice VBUS error!");
    }
    
   
}


