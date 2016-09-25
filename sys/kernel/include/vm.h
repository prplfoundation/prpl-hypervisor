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

/**
 * @file vm.h
 * 
 * @section DESCRIPTION
 * 
 * Data structures and definitions associated with the virtual machine abstraction layer. 
 */

#ifndef __VM_H
#define __VM_H

#include <linkedlist.h>
#include <tlb.h>
#include <vcpu.h>

#define VM_NAME_SZ 16

/* Identification for different operating system types */
#define GENERIC		0
#define BAREOS		1
#define BAREOS_RT   2
#define	HELLFIRE	3
#define LINUX		4
#define IDLEVCPU	5

struct vcpu_t;
typedef struct vcpu_t vcpu_t;


/* TLB entry description used in the config.h file. Used only for initialization purposes. */
struct tlb_entries{
    uint32_t entrylo0;
    uint32_t entrylo1;
    uint32_t pagemask;
    uint32_t entryhi;
    uint32_t coherency;
};

/* VM description used in the config.h file. Used only for initialization purposes. */
struct vmconf_t{
    char vm_name[VM_NAME_SZ];
    uint32_t ram_base;
    uint32_t num_tlb_entries;
    uint32_t os_type;
    uint32_t vm_entry;
    uint32_t fast_int_sz;
    uint32_t *fast_interrupts;
    const struct tlb_entries const *tlb;
};    

/* Holds information about a Virtual machine on runtime.  */
typedef struct vm_t {
	char *vm_name;
	unsigned int id;
	unsigned int base_addr;
	struct list_t *vcpus;
	uint32_t os_type;
	uint32_t ntlbent;
	uint32_t init;	
	uint32_t fast_int_sz;
	uint32_t *fast_interrupts;
	struct tlbentry *tlbentries;
}vm_t;

vm_t *create_vm(const struct vmconf_t const *vm);
vcpu_t *create_vcpu(vm_t *vm, unsigned int entry_point);


#endif

