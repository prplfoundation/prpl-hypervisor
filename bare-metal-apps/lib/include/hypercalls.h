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
#define read(addr) ({ int32_t __value; \
asm volatile (                                          \
"move	$a0, %z1 \n\
hypcall   %2 \n\
move	%0, $v0" \
: "=r" (__value) :  "r" ((uint32_t) (&addr)), "I" (HCALL_READ_ADDRESS): "a0", "v0");               \
__value; })

/* Write to privileged address */
#define write(reg, value) asm volatile (                    \
"move $a0, %z0 \n \
 move $a1, %z1 \n \
 hypcall %2" \
 : : "r" ((uint32_t) (&reg)), "r" ((uint32_t) (value)), "I" (HCALL_WRITE_ADDRESS) : "a0", "a1")

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
 
 /* interVM send message  */
#define ipc_recv(source_id, msg) ({ int32_t __ret; \
asm volatile (                    \
"move $a0, %z2 \n\
 hypcall %3 \n\
 sw $a0, 0(%z1)\n \
 move %0, $v0 " \
 : "=r" (__ret) : "r" ((uint32_t) (source_id)), "r" ((uint32_t) (msg)), "I" (HCALL_IPC_RECV_MSG) : "a0", "v0"); \
 __ret; })
 
  
#endif

