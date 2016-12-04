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

#ifndef __VCPU_H
#define __VCPU_H

#include <types.h>
#include <vm.h>

#define NUM_ASIDS 64

/* Extract instructions fields */
#define OPCODE(x) (x >> 26)
#define FUNC(x) (x & 0x3F)
#define RS(x) ((x >> 21) & 0x1f)
#define RT(x) ((x >> 16) & 0x1f)
#define RD(x) ((x >> 11) & 0x1f)
#define SEL(x) (x & 0x7)
#define IMED(x) (x & 0xFFFF)
#define SIMED(x) (IMED(x) & 0x8000 ? 0xFFFF0000 | IMED(x) : IMED(x))
#define JT(x) ((x & 0x3FFFFFF) << 2)
#define UPPERPC(x) (x & 0xF0000000)
#define CO(x) ((x >> 25) & 0x1)

#define ESPECIAL 	0x00
#define JR 		0x08
#define JALR 		0x09
#define ESPECIAL1 	0x01
#define BLTZ 		0x00
#define BGEZ 		0x01
#define BLTZAL 		0x10
#define BGEZAL 		0x11
#define J 		0x02
#define JAL 		0x03
#define BEQ 		0x04
#define BNE 		0x05
#define BLEZ 		0x06
#define BGTZ 		0x07
#define CP0 		0x10
#define CP1 		0x11
#define CP2 		0x12
#define CP3 		0x13
#define BEQL 		0x14
#define BNEL 		0x15
#define BLEZL 		0x16
#define BGTZL 		0x17
#define LB 		0x20
#define LH 		0x21
#define LW 		0x23
#define LBU 		0x24
#define LHU 		0x25
#define SB 		0x28
#define SH 		0x29
#define SW 		0x2B
#define MFC 		0x00
#define MTC 		0x04
#define EXT16 		0x10
#define ERET 		0x18
#define CACHE		0x2f
#define WAIT 		0x20


/* Mask bits for instruction emulation */
#define STATUS_MASK	0x005FFFFF

/** Return values for communication */
#define MESSAGE_VCPU_NOT_FOUND	-1
#define MESSAGE_FULL		-2
#define MESSAGE_TOO_BIG		-3
#define MESSAGE_EMPTY		-4
#define MESSAGE_VCPU_NOT_INIT -5

#define MESSAGELIST_SZ	5
#define MESSAGE_SZ 	255

#define NETWORK_VINTERRUPT	14

struct vm_t;
typedef struct vm_t vm_t;


/** Struct for message exchange 
    It is a circular buffer. Message_list is a char matrix statically allocated.
 */

typedef struct{
	uint32_t source_id;
	uint32_t size; /* size of each message in message_list */
	uint8_t message[MESSAGE_SZ];
} message_t;

typedef struct {
	uint32_t in;
	uint32_t out;
	uint32_t num_messages;
	message_t message_list[MESSAGELIST_SZ];
} message_buffer_t;


typedef struct vcpu_t {  
	uint32_t id;
	uint32_t priority;
	uint32_t priority_rem;
	uint32_t critical;
	uint32_t bgjobs;
	uint32_t gprshadowset;
	uint32_t cp0_registers[16];
	uint32_t guestclt2;
	vm_t *vm;
	uint32_t pc;
	uint32_t init;	
	message_buffer_t messages;
}vcpu_t;

typedef struct {
  unsigned int lastTime;
  vcpu_t *curr_vcpu;
} processor_t;


extern processor_t* proc; 
extern vcpu_t *idle_vcpu;

void contextSave();
uint32_t calculateGTOffset(uint32_t savedcounter, uint32_t currentCount);
void contextRestore();


#endif
