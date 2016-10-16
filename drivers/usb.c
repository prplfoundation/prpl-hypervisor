/*
 * Copyright (c) 2016, prpl Foundation
 * 
 * Permission to use, copy, modify, and/or distribute this software for any purpose with or without 
 * fee is hereby granted, provided that the above copyright notice and this permission notice appear 
 * in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE 
 * FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * This code was written by Sergio Johann at Embedded System Group (GSE) at PUCRS/Brazil.
 * 
 */

#include <config.h>
#include <types.h>
#include <pic32mz.h>
#include <globals.h>
#include <usb.h>
#include <mips_cp0.h>
#include <scheduler.h>
#include <hal.h>
#include <driver.h>
#include <hypercall_defines.h>



static struct usb_status_t usb_status;
static struct descriptor_receive descriptor_data;
static struct usb_setup_packet_t setup_packet;
static struct descriptor_decoded descriptor;
static struct usb_transfer_status_t usb_transfer_status;

uint8_t buffer_tx[64];

void usb_send_data(uint8_t* buf, uint32_t size){
    memcpy(buffer_tx, buf, size);
    usb_transfer_status.state = TRANSFER_START;   
}

uint32_t get_descriptor(){
    uint8_t *buf  = (uint8_t *)MoveFromPreviousGuestGPR(REG_A0);

    uint32_t size = MoveFromPreviousGuestGPR(REG_A1);
    
    char* frame_ptr_mapped = (char*)tlbCreateEntry((uint32_t)buf, vm_executing->base_addr, size, 0xf, CACHEABLE);
    
    memcpy(frame_ptr_mapped, &descriptor_data, sizeof(struct descriptor_receive));
    
    MoveToPreviousGuestGPR(REG_V0, size);
}

void usb_interrupt_enable(uint32_t general, uint32_t transmit, uint32_t receive){
    USBCSR1bits.TRXIE = transmit;
    USBCSR2bits.RCVIE = receive;
    USBCSR2bits.GENIE = general;
}


    

void usb_polling(){
    if(usb_status.state == RUNNING){
        update_transfer_state_machine();
        
    }else{
        update_state_machine();
    }
    
    MoveToPreviousGuestGPR(REG_V0, usb_status.descriptor_available);
}

void usb_int_handler(){
    IFSCLR(4) = (1<<4);
    
    /* USB interrupts are not persistent (clear on read) */
    uint32_t interrupts = USBCSR2bits.INTERRUPTS;
    
    if (interrupts & USB_DEVCONN_INT){
        usb_status.connected = 1;
        usb_status.state = DEBOUNCE;
        usb_status.wait_debounce = mfc0(CP0_COUNT, 0);
    }

    if (interrupts & USB_DEVDISCONN_INT){

        usb_status.state = IDLE;
        usb_transfer_status.state = TRANSFER_IDLE;
        usb_status.connected = 0;
        usb_status.descriptor_available = 0;
    }
    
    if (interrupts & USB_VBUSERR_INT){
        usb_status.state = VBUSERROR;
        usb_status.connected = 0;
        usb_status.descriptor_available = 0;
    }
    
    if(USBCSR0bits.EP0IF){
       /* This means it was an TX interrupt, so send handshake for data IN */
       if (usb_status.state == WAIT_TX_INT){
            usb_status.state = HANDSHAKE;
            
       }else if (usb_status.state == WAIT_DESCRIPTOR){
           /* Descriptor received */
           usb_status.state = READ_DESCRIPTOR;
       }else if (usb_transfer_status.state == TRANSFER_WAIT_TX_INT){
           usb_transfer_status.state = TRANSFER_SENDING;
           
       }
    }
}

uint32_t calc_wait_time(uint32_t time, uint32_t ms_delay){
    uint32_t now = mfc0(CP0_COUNT, 0);
    if ( (now - time) > (ms_delay * MILISECOND)){
        return 1;
    }
    return 0;
    
}

void usb_fill_setup_packet(struct usb_setup_packet_t *packet, 
                           uint8_t requestType, 
                           uint8_t request, 
                           uint16_t value,
                           uint16_t index,
                           uint16_t length){
    
    packet->bmRequestType = requestType;
    packet->bRequest = request ;
    packet->wValue = value ;
    packet->wIndex = index ;
    packet->wLength = length;
}



void update_state_machine(){
    static uint32_t request_count = 0;
    uint32_t sz;
    uint32_t i;
    vcpu_t* vcpu = NULL;
    
    switch (usb_status.state){
        case DEBOUNCE:
            if (calc_wait_time(usb_status.wait_debounce, DEBOUNCE_MS)){
                usb_status.state = USB_RESET;
                USB_RESET_ON;
            }
            break;
        case USB_RESET:
            if (calc_wait_time(usb_status.wait_debounce, RESET_MS)){
                usb_status.state = POST_RESET_DELAY;
                USB_RESET_OFF;
                if(HS_FS_DEVICE){
                    usb_status.speed = FULL_SPEED;
                }else if (LS_DEVICE){
                    usb_status.speed = LOW_SPEED;
                }else{
                    /* Nothing is connected ? */
                    usb_status.speed = LOW_SPEED;
                }
                usb_status.wait_debounce = mfc0(CP0_COUNT, 0);
            }
            break;
        case POST_RESET_DELAY:
            if (calc_wait_time(usb_status.wait_debounce, POST_RESET_DELAY_MS)){
                usb_status.state = REQUEST_DESCRIPTOR;   
            }
            break;
            
        case REQUEST_DESCRIPTOR:
            /* discover who is connected */
            memset(&descriptor_data, 0, sizeof(descriptor_data));
           
            usb_fill_setup_packet(&setup_packet, (  USB_SETUP_DIRN_DEVICE_TO_HOST | 
                                                    USB_SETUP_TYPE_STANDARD | 
                                                    USB_SETUP_RECIPIENT_DEVICE ),
                                                    USB_REQUEST_GET_DESCRIPTOR, 
                                                    (USB_DESCRIPTOR_DEVICE << 8), 
                                                    0, 
                                                    18 ) ;

            setup_packet_send((uint8_t*)&setup_packet, sizeof(setup_packet));
            usb_status.state = WAIT_TX_INT;
            usb_status.wait_debounce = mfc0(CP0_COUNT, 0);
            break;

        case WAIT_TX_INT:
            /* Updated to next state by the interrupt handler */
            break;

        case HANDSHAKE:
            USBENCTRL0bits.w |= 0x600000;
            usb_status.state = WAIT_DESCRIPTOR; 
            usb_status.wait_debounce = mfc0(CP0_COUNT, 0);
            break;
            
        case WAIT_DESCRIPTOR:
            if (calc_wait_time(usb_status.wait_debounce, 500)){
                if (request_count < 5){
                    request_count++;
                    usb_status.state = REQUEST_DESCRIPTOR;
                }else {
                    usb_status.state = GENERALERROR;
                }
            }
            
            break;
        case READ_DESCRIPTOR:
            read_descriptor(&descriptor_data);
            if (descriptor_data.received < descriptor_data.descr_sz){
                usb_status.state=WAIT_DESCRIPTOR;
                usb_status.wait_debounce = mfc0(CP0_COUNT, 0);
                /* Request next packet - clear STATPKT and set REQPKT */
                *(((uint8_t*)&USBENCTRL0bits)+2) = 0x20;
            }else{
                usb_status.state=RUNNING;
                memcpy(&descriptor, &descriptor_data, sizeof(descriptor));
                usb_status.descriptor_available = 1;
            }
            break;
            
        case RUNNING:
            break;

        case IDLE:
            /* No device connected. Check if the controller slipped out of host mode. */
            if (IS_B_MODE && IS_VALID_VBUS){
                SESSION_DISABLE;
                SESSION_ENABLE;
            }
            break;
            
        case VBUSERROR:
            
            break;
        
        case GENERALERROR:
            request_count = 0;
            puts("\nFailed");
            
            break;
    }
    
    if(IS_OVERCURRENT){
        DISABLE_POWER_SUPPLY;
        usb_status.state = GENERALERROR;
        usb_status.connected = 0;
    }
    
}

static uint32_t sent_sz = 0;

void update_transfer_state_machine(){
    uint32_t i;
    static uint32_t flag=0;
    vcpu_t *vcpu=NULL;
    
    switch(usb_transfer_status.state){
        case TRANSFER_INT_VM:
            /* This state means a device was connected and his descriptor was read. 
             Send an interrupt to the registered VM. */
            usb_transfer_status.state = TRANSFER_IDLE;
            
            break;
        case TRANSFER_IDLE:
            break;
        case TRANSFER_START:
            usb_fill_setup_packet(&setup_packet, (  USB_SETUP_DIRN_HOST_TO_DEVICE | 
                                                    USB_SETUP_TYPE_VENDOR | 
                                                    USB_SETUP_RECIPIENT_DEVICE ),
                                                    6, 
                                                    256, 
                                                    0, 
                                                    3 ) ;
                                                    
            usb_transfer_status.state = TRANSFER_SENDING;
            sent_sz = 0;
            setup_packet_send((uint8_t*)&setup_packet, sizeof(setup_packet));
            
            break;
            
        case TRANSFER_SENDING:
            data_packet_sent(buffer_tx, 3); 
            if (sent_sz>2){
                usb_status.wait_debounce = mfc0(CP0_COUNT, 0);
                usb_transfer_status.state = TRANSFER_DONE;
            }else{
                usb_transfer_status.state = TRANSFER_WAIT_TX_INT;
            }
            break;
            
        case TRANSFER_WAIT_TX_INT:
            break;
            
        case TRANSFER_DONE:
            usb_transfer_status.state = TRANSFER_IDLE;  
            break;
            
    }
    
}

void ep0SetSpeed(uint32_t speed){
    if (speed == LOW_SPEED){
        USBIE0CSR2bits.SPEED = 3;
    }else if (speed == FULL_SPEED){
        USBIE0CSR2bits.SPEED = 2;
    }else{
        /* high speed */
        USBIE0CSR2bits.SPEED = 1;
    }
}

void read_descriptor(struct descriptor_receive *descr){
    uint32_t i;
    uint32_t rxsz = USBIE0CSR2bits.RXCNT;
    
    for(i=0; i<rxsz;i++){
        descr->descriptor[descr->received++] = *((uint8_t*)&USBFIFO0bits + (i & 3));
    }
    
    /* Get descriptor size and max packet sz*/
    if (descr->descr_sz == 0){
        descr->descr_sz = descr->descriptor[0];
        descr->tranfer_sz = descr->descriptor[7];
    }
    
    USBENCTRL0bits.RXPKTRDY = 0;
    
    /* if the received bytes is less then the packet size the transfer is finished.*/
    if (rxsz < descr->tranfer_sz || descr->received == descr->descr_sz){
        descr->descr_sz = descr->received;
    }
}

/* USBHS_EndpointFIFOLoad_Default */
void setup_packet_send(uint8_t *data, uint32_t size){
    uint32_t i;
    
    ep0SetSpeed(usb_status.speed);
    
    for(i=0;i<size;i++){
        USBFIFO0bits.byte = *(data + i);
    }
    
    /* SETUPPKT and TXPKTRDY*/
    USBENCTRL0bits.w |= 0xA0000;
    
}


/* USBHS_EndpointFIFOLoad_Default */
void data_packet_sent(uint8_t *data, uint32_t size){
    uint32_t i;
    
    ep0SetSpeed(usb_status.speed);
    
    for(i=0;i<8 && sent_sz<size;i++){
        USBFIFO0bits.byte = *(data + sent_sz);
        sent_sz++;
    }
    
    *(((uint8_t*)&USBENCTRL0bits)+2) = 0x2;

}


void usb_start(){
    uint32_t operation_mode = HOST_MODE;
    uint32_t speed = FULL_SPEED;
    uint32_t offset;
    
    INFO("Initializing USB device in Host mode.");
    
    if (operation_mode != HOST_MODE){
        WARNING("Only Host Mode is supported");
    }
    
    offset = register_interrupt(usb_int_handler);
    OFF(132) = offset;
    
    INFO("USB interrupt vector at 0x%x", offset);
    
    if (register_hypercall(usb_polling, USB_VM_POLLING) < 0){
        ERROR("Error registering the HCALL_GET_VM_ID hypercall");
        return;
    }
    
    if (register_hypercall(get_descriptor, USB_VM_GET_DESCRIPTOR) < 0){
        ERROR("Error registering the HCALL_GET_VM_ID hypercall");
        return;
    }
    
    usb_status.state = IDLE;
    
    usb_transfer_status.state = TRANSFER_INT_VM;
    
    OVERCURRENT_PIN_ENABLE;
    
    /* USBID pin (RPF3) pin in PULL DOWN */
    USBID_PIN_PULL_DOWN_F;
    
    /* Enable USB general interrutps  */
    USBCRCONbits.USBIE = 1;
    
    /* 10ms reset USB */
    USBEOFRSTbits.SOFRST = 0x3;
    udelay(10000);
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
    
    /* Configure interrupt vectors, clear and enable interrupts */
    USB_INTERRUPT_CLEAR;
    USB_INTERRUPT_ENABLE;
    
    USB_DMA_INTERRUPT_CLEAR;
    USB_DMA_INTERRUPT_ENABLE;
    
    ENABLE_POWER_SUPPLY;
    
    usb_interrupt_enable(0xb0, 1, 0);
    
    while(!IS_VALID_VBUS);
    
    SESSION_ENABLE;
    
}


driver_init(usb_start);

