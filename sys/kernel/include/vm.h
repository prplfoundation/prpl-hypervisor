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

#ifndef __VM_H
#define __VM_H

#include <vcpu.h>
#include <linkedlist.h>
#include <tlb.h>

/* Number of colums for VM's configuratio */
#define VMCONF_NUMCOLUNS  6


/* OS types */
#define GENERIC		0
#define BAREOS		1
#define BAREOS_RT   	2
#define	HELLFIRE	3
#define LINUX		4
#define IDLEVCPU	5


static uint32_t HF_Template[][VMCONF_NUMCOLUNS] = { 	0, 		132, 		2,		0	,	0, 	0, 
						0, 		0x20000, 	0x20010,	PAGEMASK_64KB, 	0,    	0, 
						0,		0x1FFFF,	0,		PAGEMASK_4KB,  	0x40,	2 };

static uint32_t TLBIndex = 1;


/** Holds information about memory regions of a VM
 * 
 * Base guest virtual address and size are used to determine if it is allowed to a VM map a memory region. 
 */ 
typedef struct memVMMap{
	uint32_t phyGuestBase;
	uint32_t vGuestBase;
	uint32_t size;
	uint32_t coherency;
}memVMMap_t;


/**
 * Holds information about a Virtual machine
 *
 * A domain represent a virtual machine instance, it's memory map,
 * it's virtual devices state and the Virtual Processing Units within it
 */
typedef struct vm_t {
	unsigned int id;
	unsigned int base_addr;
	unsigned int size;	
	linkedlist_t vcpus;
	uint32_t os_type;
	uint32_t ntlbent;
	uint32_t init;	
	//uint32_t gprshadownum;
  
	struct tlbentry *tlbentries;
	memVMMap_t *vmmap;
}vm_t;


vcpu_t *create_vcpu(vm_t *vm, unsigned int entry_point, unsigned int arg, char* stack_pointer,  uint32_t pip, uint32_t ostype);
void delete_vcpu(vcpu_t*);
vm_t *create_vm(uint32_t vm[][VMCONF_NUMCOLUNS]);
vm_t *get_vm(unsigned int);
void delete_vm(vm_t*);

linkedlist_t virtualmachines;
linkedlist_t virtualmachines_rt;

#endif

