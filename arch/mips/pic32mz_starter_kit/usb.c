#include <config.h>
#include <types.h>
#include <pic32mz.h>
#include <globals.h>
#include <usb.h>

void usb_interrupt_enable(uint32_t transmit, uint32_t receive, uint32_t general){
    USBCSR1bits.TRXIE = transmit;
    USBCSR2bits.RCVIE = receive;
    USBCSR2bits.GENIE = general;
}
    
uint32_t usb_start(uint32_t operation_mode, uint32_t speed){
    
    if (operation_mode != HOST_MODE){
        WARNINGS("Only Host Mode is supported");
        return 1;
    }
    
    ENABLE_POWER_SUPPLY;
    
    /* USBID pin (RPF3) pin in PULL DOWN */
    USBID_PIN_PULL_DOWN;
    
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
    
    /* Configure interrupt vectors, clear and enable interrupts */
    USB_DMA_INTERRUPT_VECTOR;
    USB_INTERRUPT_VECTOR;
    
    USB_INTERRUPT_CLEAR;
    USB_INTERRUPT_ENABLE;
    
    USB_DMA_INTERRUPT_CLEAR;
    USB_DMA_INTERRUPT_ENABLE;
    
  
    return 0;
}

void usb_device_attach(){
    printf("\n%d %d %d 0%x", IS_B_MODE, IS_VALID_VBUS, IS_DEVICE_CONNECTED, USBCSR2bits.INTERRUPTS);
    
    
    if (IS_B_MODE && IS_VALID_VBUS){
        printf("\nsession enable");
        SESSION_ENABLE;
    }
    if(IS_DEVICE_CONNECTED){
        printf("device connected");
    }
    
}



