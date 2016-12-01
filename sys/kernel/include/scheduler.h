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

#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include <linkedlist.h>
#include <vcpu.h>

/**
 * Keeps the virtual machine and VCPU lists. Additionally, it 
 * keeps a pointer to the VCPU in execution and to the next VCPU
 * (in case of fast interrupt delivery policy).
 */
struct scheduler_info_t{
	struct list_t *vcpu_ready_list;
	struct list_t *virtual_machines_list;
	struct list_t *vcpu_executing_nd;
	struct list_t *next_vcpu;
};

void fast_interrupt_delivery(struct list_t *target);
void run_scheduler();
struct list_t* get_fast_int_vcpu_node(uint32_t fast_int);
vcpu_t* get_vcpu_from_id(uint32_t id, struct list_t** vcpu_node);

#endif /* __SCHEDULER_H */
