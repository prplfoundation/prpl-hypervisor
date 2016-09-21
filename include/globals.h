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

#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <types.h>
#include <vm.h>
#include <vcpu.h>
//#include <pic32mz.h>

#define CRITICAL(x) printf(x); while(1){};

#ifdef WARNINGS
#define Warning(A,...) printf("WARNING: "A"\n",##__VA_ARGS__);
#else
#define Warning(A,...)
#endif

#ifdef INFOS
#define Info(A,...) printf(A"\n",##__VA_ARGS__);
#else
#define Info(A,...)
#endif

#ifdef CRITICALS
#define Critical(A,...) printf("CRITICAL: "A"\n",##__VA_ARGS__);
#else
#define Critical(A,...)
#endif

void print_here(void);

/* Exception handler addr */
extern void* exceptionHandler_addr;
extern uint32_t lastguestcount;

/* Processor and vcpu */
extern processor_t* proc; 
extern vcpu_t *idle_vcpu;
extern vcpu_t *target_vcpu;
extern vcpu_t *vcpu_sgpr[8];

/* Best effort vcpu list */
extern linkedlist_t be_vcpu_list;
extern linkedlist_t virtualmachines;


#define curr_vcpu proc->curr_vcpu
#define curr_vm   curr_vcpu->vm

#endif