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

#ifndef _HYPERCALL_DEFINES_H_
#define _HYPERCALL_DEFINES_H_

typedef void hypercall_t();

#define HCALL_TABLE_SIZE 30
#define HCALL_CODE_INVALID       -1
#define HCALL_CODE_USED          -2
#define HCALL_NOT_IMPLEMENTED    -3
#define HCALL_ADDRESS_NOT_ALLOWED -4
#define HCALL_USB_BUSY 			-5


/*************************/
/* InterVM Communication */
/*************************/

/* GET VM ID 
 * v0 = identification number.
 */
#define HCALL_GET_VM_ID 0

/* RECEIVE MESSAGE 
 *   Input: 	a0 = Destination buffer.
 *   Output: 	v0 = Message size. 
 *   		a0 = Source ID.
 */
#define HCALL_IPC_RECV_MSG  1

/* SEND MESSAGE 
 *   a0 = Target process ID
 *   a1 = Source message pointer 
 *   a2 = Message size 
 *   v0 = true/false (Succeded/failed)*/
#define HCALL_IPC_SEND_MSG  2

/* Check if guest is UP. 
 * v0 = identification number.
 */
#define HCALL_GUEST_UP 3

/*************************/
/*  ETHERNET HYPERCALLS  */
/*************************/

/* Stablished and monitore ethernet state . 
 * v0 = 0 (link down) or 1 (link up).
 */
#define HCALL_ETHERNET_WATCHDOG 4

/* Send a frame throught the ethernet.
 * A0 = buffer pointer.
 * A1 = buffer size.
 */ 
#define HCALL_ETHERNET_SEND     5

/* Recv a frame from the ethernet.
 * A0 = buffer pointer.
 * V0 = buffer size.
 */ 
#define HCALL_ETHERNET_RECV     6

/* Returns the MAC address.
 * A0 = buffer pointer.
 */ 
#define HCALL_ETHERNET_GET_MAC  7

/* Keep the USB state machines updated. 
 * V0 = 1 or 0 for device descriptor available/unavailable.
 */
#define HCALL_USB_POLLING              8

/* Returns the device's descritpor 
 * A0 = buffer pointer
 * A1 = buffer size.
 * V0 = descriptor size
 */
#define HCALL_USB_GET_DESCRIPTOR       9

/* USB send data
 * A0 = buffer pointer
 * A1 = buffer size.
 */
#define HCALL_USB_SEND_DATA            10


/* Virtual IO Write Address .
 * A0 = Address to write.
 * A1 = Value to be writed.
 * V0 = 0 if success or error code otherwise.
 */ 
#define HCALL_WRITE_ADDRESS	11

/* Virtual IO Read Address .
 * A0 = Address to read.
 * V0 = Value.
 */ 
#define HCALL_READ_ADDRESS	12

/* Re-enable interrupt for the interrupt redirection feature. 
 * A0 = Interrupt to enabe.
 * V0 = 0 for success. Otherwise 1.
 */ 
#define HCALL_REENABLE_INTERRUPT	13

/* PUF FLASH hypercalls */

/* Read 1K buffer from flash. 
 *  A0 = destination buffer.
 *  V0 = greater or iqual to 0 sucess, less than zero error.
 */
#define HCALL_FLASH_READ  14 

/* Write 1K buffer to flash.
 *  A0 = Source buffer.
 *  V0 = greater or iqual to 0 sucess, less than zero error.
 */
#define HCALL_FLASH_WRITE 15  


#endif

