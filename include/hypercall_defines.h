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

#define SERVICE_NAME_SZ	32

/** RT SERVICES HYPERCALLS **/

/* CREATE SERVICE
 * a0 = Struct rt_vcpu_hypercall 
 * v0 = true/false (Succeded/failed) */
#define HCALL_RT_CREATE_SERVICE  0x100

/* LAUNCH SERVICE
 * a0 = Service name 
 * v0 = true/false (Succeded/failed)*/
#define HCALL_RT_LAUNCH_SERVICE  0x101

/** INFORMATION HYPERCALLS **/

/* GET PROCESS ID
 *  v0 = Process ID */
#define HCALL_INFO_GET_ID 0x200


/** IPC HYPERCALLS **/

/* SEND MESSAGE 
 *   a0 = Target process ID
 *   a1 = Source message pointer 
 *   a2 = Message size 
 *   v0 = true/false (Succeded/failed)*/
#define HCALL_IPC_SEND_MSG  0x300

/* RECEIVE MESSAGE 
 *   a0 = Process ID
 *   a1 = Target message pointer 
 *   v0 = Message size */
#define HCALL_IPC_RECV_MSG  0x301

#define HCALL_INTERRUPT_GUEST  0x120


/** CONTROL HYPERCALLS**/
#define HCALL_HALT_GUEST 0x001
#define HCALL_START_TIMER_GUEST 0x002
#define HCALL_GUEST_UP 0x003

/* PUF hypercalls */
#define HCALL_PUF_SHARED_MEMORY 0x150


/** FLASH hypercalls **/
#define HCALL_FLASH_READ  0x151  /* a0 = destination buffer */
#define HCALL_FLASH_WRITE 0x152  /* a0 = source buffer */


#define ETH_GET_MAC    0x153  /* a0 = destination buffer */
#define ETH_LINK_STATE 0x12
#define ETH_RECV_FRAME 0x11
#define ETH_SEND_FRAME 0x10

#endif

