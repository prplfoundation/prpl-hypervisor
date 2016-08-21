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

#ifndef _USB_H_
#define _USB_H_

enum USB_OPERATION_MODE {
    HOST_MODE,
    DEVICE_MODE
};

enum USB_SPEED {
    FULL_SPEED = 0,
    HIGH_SPEED =1
};

#define USB_DEVCONN_INT (1<<4)
#define USB_VBUSERR_INT (1<<7)
#define USB_DEVDISCONN_INT (1<<5)

#define ENABLE_POWER_SUPPLY TRISBCLR = (1<<5); LATBSET = (1<<5)
#define DISABLE_POWER_SUPPLY LATBCLR = (1<<5)
#define OVERCURRENT_PIN_ENABLE ANSELBCLR = (1<<2); CNPUBSET = (1<<2); TRISBSET = (1<<2)
#define IS_OVERCURRENT (!(PORTB & (1<<2)))


/* USB ID pull down */
#define USBID_PIN_PULL_DOWN_F CNCONFSET = (1<<15); ANSELFCLR = (1<<3); TRISFSET = (1<<3); CNPDFSET = (1<<3)
#define USBID_PIN_PULL_DOWN_C CNCONCSET = (1<<15); ANSELCCLR = (1<<3); TRISCSET = (1<<3); CNPDCSET = (1<<3) 

/* Clear USB interrupt */
#define USB_INTERRUPT_CLEAR IFSCLR(4) = (1<<4)

/* Enable USB interrupts */
#define USB_INTERRUPT_ENABLE  OFF(132) = 0x200; IPCSET(33) = 0x1F; IECSET(4) = (1<<4)

/* Clear USB DMA interrupt */
#define USB_DMA_INTERRUPT_CLEAR IFSCLR(4) = (1<<5)

/* Enable USB DMA interrupts */
#define USB_DMA_INTERRUPT_ENABLE OFF(133) = 0x200; IPCSET(33) = 0x1F00; IECSET(4) = (1<<5)

/*Check for device mode */
#define IS_B_MODE (USBOTGbits.BDEV & 0x80)

/* check for valid VBUS */
#define IS_VALID_VBUS (USBOTGbits.VBUS == 3)

/* enable session */
#define SESSION_ENABLE USBOTGbits.SESSION = 1

/* Device connected */
#define IS_DEVICE_CONNECTED (USBCSR2bits.CONNIF == 1)



#endif

