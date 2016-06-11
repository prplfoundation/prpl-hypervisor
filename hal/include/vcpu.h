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
#include <scheduler.h>
#include <vm.h>
#include <irq.h>

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


//It`s not clear how the decoding is done
//Verify that
//#define WAIT 		0x10

//MIPS REGISTERS

#define REG_ZERO 0
#define REG_AT   1
#define REG_V0   2
#define REG_V1   3
#define REG_A0   4
#define REG_A1   5
#define REG_A2   6
#define REG_A3   7
#define REG_T0   8
#define REG_T1   9
#define REG_T2   10
#define REG_T3   11
#define REG_T4   12
#define REG_T5   13
#define REG_T6   14
#define REG_T7   15
#define REG_S0   16
#define REG_S1   17
#define REG_S2   18
#define REG_S3   19
#define REG_S4   20
#define REG_S5   21
#define REG_S6   22
#define REG_S7   23
#define REG_T8   24
#define REG_T9   25
#define REG_K0   26 
#define REG_K1   27
#define REG_GP   28 
#define REG_SP   29
#define REG_FP   30
#define REG_RA   31


/* Mask bits for instruction emulation */
#define STATUS_MASK	0x005FFFFF


/*Return values, instruction emulation*/
#define SUCEEDED 0
#define PROGRAM_ENDED 1
#define RESCHEDULE 2
#define CHANGE_TO_TARGET_VCPU 3
#define ERROR -1

/** Return values for communication */
#define MESSAGE_VCPU_NOT_FOUND	-1
#define MESSAGE_FULL		-2
#define MESSAGE_TOO_BIG		-3
#define MESSAGE_EMPTY		 0

#define MESSAGELIST_SZ	1
#define MESSAGE_SZ 	32

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
	uint32_t rootcount;
	uint32_t offseTelapsedTime;
	uint32_t gprshadowset;
	uint32_t cp0_registers[32][4];
	uint32_t gp_registers[34];	
	uint32_t guestclt2;
	uint32_t pip; /* pending interrupt pass-through */
	vm_t *vm;
	task_t task;   
	uint32_t pc;
	uint32_t sp;
	uint32_t gp;
	uint32_t arg;
	uint32_t init;	
	message_buffer_t messages;
}vcpu_t;

typedef struct {
  //unsigned int id;
  unsigned int lastTime;
  irq_handler_t irq_handlers[8];     
  vcpu_t *curr_vcpu;
} processor_t;


extern processor_t* proc; 
extern vcpu_t *idle_vcpu;

uint32_t InstructionEmulation();

void vcpu_set_pc(vcpu_t *v, uint32_t pc);
void vcpu_set_k0(vcpu_t *v, uint32_t value);

#endif
