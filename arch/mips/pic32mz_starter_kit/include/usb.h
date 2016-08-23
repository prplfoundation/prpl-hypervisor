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

/* The "descriptor types" are listed in Table 9-5 of the USB 2.0 specification. */

#define USB_DESCRIPTOR_DEVICE           0x01    // bDescriptorType for a Device Descriptor.
#define USB_DESCRIPTOR_CONFIGURATION    0x02    // bDescriptorType for a Configuration Descriptor.
#define USB_DESCRIPTOR_STRING           0x03    // bDescriptorType for a String Descriptor.
#define USB_DESCRIPTOR_INTERFACE        0x04    // bDescriptorType for an Interface Descriptor.
#define USB_DESCRIPTOR_ENDPOINT         0x05    // bDescriptorType for an Endpoint Descriptor.
#define USB_DESCRIPTOR_DEVICE_QUALIFIER 0x06    // bDescriptorType for a Device Qualifier.
#define USB_DESCRIPTOR_OTHER_SPEED      0x07    // bDescriptorType for a Other Speed Configuration.
#define USB_DESCRIPTOR_INTERFACE_POWER  0x08    // bDescriptorType for Interface Power.
#define USB_DESCRIPTOR_OTG              0x09    // bDescriptorType for an OTG Descriptor. 
#define USB_DESCRIPTOR_INTERFACE_ASSOCIATION  0x0b 
#define USB_DESCRIPTOR_BOS              0x0F    // bDescriptorType for a BOS Descriptor.
#define USB_DESCRIPTOR_DEVICE_CAPABILITY 0x10   // bDescriptorType for a Device Capability Descriptor.


/* The "standard device requests" are listed in table 9-3 of USB 2.0 spec. */

#define USB_REQUEST_GET_STATUS                  0       // Standard Device Request - GET STATUS
#define USB_REQUEST_CLEAR_FEATURE               1       // Standard Device Request - CLEAR FEATURE
#define USB_REQUEST_SET_FEATURE                 3       // Standard Device Request - SET FEATURE
#define USB_REQUEST_SET_ADDRESS                 5       // Standard Device Request - SET ADDRESS
#define USB_REQUEST_GET_DESCRIPTOR              6       // Standard Device Request - GET DESCRIPTOR
#define USB_REQUEST_SET_DESCRIPTOR              7       // Standard Device Request - SET DESCRIPTOR
#define USB_REQUEST_GET_CONFIGURATION           8       // Standard Device Request - GET CONFIGURATION
#define USB_REQUEST_SET_CONFIGURATION           9       // Standard Device Request - SET CONFIGURATION
#define USB_REQUEST_GET_INTERFACE               10      // Standard Device Request - GET INTERFACE
#define USB_REQUEST_SET_INTERFACE               11      // Standard Device Request - SET INTERFACE
#define USB_REQUEST_SYNCH_FRAME                 12      // Standard Device Request - SYNCH FRAME


/* bmRequestType field of setup data as described in Table 9-2 of the USB 2.0 specification.*/

#define USB_SETUP_DIRN_HOST_TO_DEVICE               0x00    // Setup request direction is Host -> Device
#define USB_SETUP_DIRN_DEVICE_TO_HOST               0x80    // Setup request direction is Device -> Host
#define USB_SETUP_TYPE_STANDARD                     0x00    // Setup request type is "Standard"
#define USB_SETUP_TYPE_CLASS                        0x20    // Setup request type is "Class"
#define USB_SETUP_TYPE_VENDOR                       0x40    // Setup request type is "Vendor"
#define USB_SETUP_TYPE_RESERVED                     0x60    // Reserved
#define USB_SETUP_RECIPIENT_DEVICE                  0x00    // Recipient is Device
#define USB_SETUP_RECIPIENT_INTERFACE               0x01    // Recipient is interface
#define USB_SETUP_RECIPIENT_ENDPOINT                0x02    // Recipient is endpoint
#define USB_SETUP_RECIPIENT_OTHER                   0x03    // Recipient is other

#define DESCRIPTOR_SZ                               18


enum USB_OPERATION_MODE {
    HOST_MODE,
    DEVICE_MODE
};

enum USB_SPEED {
    FULL_SPEED = 0,
    HIGH_SPEED =1,
    LOW_SPEED = 2
};

enum USB_STATE{
    IDLE,
    DEBOUNCE,
    USB_RESET,
    POST_RESET_DELAY,
    HANDSHAKE,
    REQUEST_DESCRIPTOR,
    WAIT_TX_INT,
    WAIT_DESCRIPTOR,
    READ_DESCRIPTOR,
    RUNNING,
    VBUSERROR,
    GENERALERROR
};

enum USB_TRANSFER_STATE{
    TRANSFER_IDLE,
    TRANSFER_START,
    TRANSFER_SENDING,
    TRANSFER_WAIT_TX_INT,
    TRANSFER_DONE
};

struct usb_transfer_status_t{
    volatile uint32_t state;
    uint8_t pkt_sz;
};


struct usb_status_t{
    volatile uint32_t state;
    uint32_t connected;
    uint32_t debounce;
    uint32_t wait_debounce;
    uint32_t speed;
};

struct usb_setup_packet_t{
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;    
};

struct descriptor_receive{
    uint8_t descriptor[18];
    uint32_t received;
    uint32_t descr_sz;
    uint32_t tranfer_sz;
};




struct descriptor_decoded{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
};

#define DEBOUNCE_MS 500
#define RESET_MS    100
#define POST_RESET_DELAY_MS 100 


#define USB_DEVCONN_INT (1<<4)
#define USB_VBUSERR_INT (1<<7)
#define USB_DEVDISCONN_INT (1<<5)

#define ENABLE_POWER_SUPPLY TRISBCLR = (1<<5); LATBSET = (1<<5)
#define DISABLE_POWER_SUPPLY LATBCLR = (1<<5)
#define OVERCURRENT_PIN_ENABLE ANSELBCLR = (1<<2); CNPUBSET = (1<<2); TRISBSET = (1<<2)
#define IS_OVERCURRENT (!(PORTB & (1<<2)))


/* USB ID pull down */
#define USBID_PIN_PULL_DOWN_F CNCONFSET = (1<<15); ANSELFCLR = (1<<3); TRISFSET = (1<<3); CNPDFSET = (1<<3)

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

/* disable session */
#define SESSION_DISABLE USBOTGbits.SESSION = 0

/* Device connected */
#define IS_DEVICE_CONNECTED (USBCSR2bits.CONNIF == 1)

/* USB device reset */
#define USB_RESET_ON (USBCSR0bits.RESET = 1)
#define USB_RESET_OFF (USBCSR0bits.RESET = 0)

/* HIGH or FULL speed detected */
#define HS_FS_DEVICE (USBOTGbits.FSDEV == 1)

/* Low speed detected */
#define LS_DEVICE (USBOTGbits.LSDEV == 1)

void update_state_machine();
uint32_t calc_wait_time(uint32_t time, uint32_t ms_delay);
void read_descriptor(struct descriptor_receive *descr);

#endif

