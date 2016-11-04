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

#ifndef  __HYPERCALLS_H
#define  __HYPERCALLS_H

#include <hypercall_defines.h>

/* Read from privileged address  */
#define readio(addr) ({ int32_t __value; \
asm volatile (                                          \
"move	$a0, %z1 \n\
hypcall   %2 \n\
move	%0, $v0" \
: "=r" (__value) :  "r" ((uint32_t) (&addr)), "I" (HCALL_READ_ADDRESS): "a0", "v0");               \
__value; })

/* Write to privileged address */
#define writeio(reg, value) ({ int32_t __ret; \
asm volatile (                 \
"move $a0, %z1 \n \
 move $a1, %z2 \n \
 hypcall %3 \n \
 move %0, $v0" \
 : "=r" (__ret) : "r" ((uint32_t) (&reg)), "r" ((uint32_t) (value)), "I" (HCALL_WRITE_ADDRESS) : "a0", "a1", "v0"); \
 __ret; })

/* interVM send message  */
#define ipc_send(targed_id, msg, size) ({ int32_t __ret; \
asm volatile (                    \
"move $a0, %z1 \n \
 move $a1, %z2 \n \
 move $a2, %z3 \n \
 hypcall %4 \n\
 move %0, $v0" \
 : "=r" (__ret) : "r" ((uint32_t) (target_id)), "r" ((uint32_t) (msg)), "r" ((uint32_t) (size)), "I" (HCALL_IPC_SEND_MSG) : "a0", "a1", "a2", "v0"); \
 __ret; })
 
 /* interVM recv message  */
#define ipc_recv(source_id, msg) ({ int32_t __ret; \
asm volatile (                    \
"move $a0, %z2 \n\
 hypcall %3 \n\
 sw $a0, 0(%z1)\n \
 move %0, $v0 " \
 : "=r" (__ret) : "r" ((uint32_t) (source_id)), "r" ((uint32_t) (msg)), "I" (HCALL_IPC_RECV_MSG) : "a0", "v0"); \
 __ret; })
 
/* Get own guest ID  */
#define get_guestid() ({ int32_t __ret; \
asm volatile (                    \
"hypcall %1 \n\
 move %0, $v0 " \
 : "=r" (__ret) : "I" (HCALL_GET_VM_ID) : "a0", "v0"); \
 __ret; })
 
 /* Ethernert link checker */
#define eth_watch() ({ int32_t __ret; \
asm volatile (                    \
 "hypcall %1 \n\
  move %0, $v0 " \
  : "=r" (__ret) : "I" (HCALL_ETHERNET_WATCHDOG) : "v0"); \
 __ret; })
 
/* Ethernert get mac */
#define eth_mac(msg) asm volatile (                    \
"move $a0, %z0 \n\
 hypcall %1" \
 : : "r" ((uint32_t) (msg)), "I" (HCALL_ETHERNET_GET_MAC) : "a0");
 

/* Ethernet send message  */
#define eth_send_frame(msg, size) ({ int32_t __ret; \
asm volatile (                    \
"move $a0, %z1 \n \
 move $a1, %z2 \n \
 hypcall %3 \n\
 move %0, $v0" \
 : "=r" (__ret) : "r" ((uint32_t) (msg)), "r" ((uint32_t) (size)), "I" (HCALL_ETHERNET_SEND) : "a0", "a1", "v0"); \
 __ret; })
 
 /* Ethernet recv message  */
#define eth_recv_frame(msg) ({ int32_t __ret; \
 asm volatile (                    \
 "move $a0, %z1 \n\
 hypcall %2 \n\
 move %0, $v0 " \
 : "=r" (__ret) : "r" ((uint32_t) (msg)), "I" (HCALL_ETHERNET_RECV) : "a0", "v0"); \
 __ret; })
 
/* USB get device descriptor   */
#define usb_get_device_descriptor(descriptor, size) ({ int32_t __ret; \
asm volatile (                    \
"move $a0, %z1 \n \
 move $a1, %z2 \n \
 hypcall %3 \n\
 move %0, $v0" \
 : "=r" (__ret) : "r" ((uint32_t) (descriptor)), "r" ((uint32_t) (size)), "I" (HCALL_USB_GET_DESCRIPTOR) : "a0", "a1", "v0"); \
 __ret; })
 
/* USB polling. Updates the USB state machines.   */
#define usb_polling() ({ int32_t __ret; \
asm volatile (                    \
"hypcall %1 \n\
 move %0, $v0 " \
 : "=r" (__ret) : "I" (HCALL_USB_POLLING) : "v0"); \
 __ret; })
 
/* USB send data  */
#define usb_control_send(msg, size) ({ int32_t __ret; \
asm volatile (                    \
"move $a0, %z1 \n \
 move $a1, %z2 \n \
 hypcall %3 \n\
 move %0, $v0" \
 : "=r" (__ret) : "r" ((uint32_t) (msg)), "r" ((uint32_t) (size)), "I" (HCALL_USB_SEND_DATA) : "a0", "a1", "v0"); \
 __ret; })

/* Re-enable an interrupt */
#define reenable_interrupt(irq) ({ int32_t __ret; \
asm volatile (                    \
"move $a0, %z1 \n \
 hypcall %2 \n \
 move %0, $v0" \
 : "=r" (__ret) : "r" ((uint32_t) (irq)), "I" (HCALL_REENABLE_INTERRUPT) : "a0", "v0"); \
 __ret; })
 
/* Read 1K byte from flash - PUF specific function.  */
#define read_1k_data_flash(buf) ({ int32_t __ret; \
 asm volatile (                    \
 "move $a0, %z1 \n \
 hypcall %2 \n\
 move %0, $v0" \
 : "=r" (__ret) : "r" ((uint32_t) (buf)), "I" (HCALL_FLASH_READ) : "a0", "v0"); \
 __ret; })
 
/* Write 1K byte to flash - PUF specific function.  */
#define write_1k_data_flash(buf) ({ int32_t __ret; \
 asm volatile (                    \
 "move $a0, %z1 \n \
 hypcall %2 \n\
 move %0, $v0" \
 : "=r" (__ret) : "r" ((uint32_t) (buf)), "I" (HCALL_FLASH_WRITE) : "a0", "v0"); \
 __ret; })
 
 
 
 
#endif

