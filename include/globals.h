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

#include <vm.h>
#include <vcpu.h>
#include <config.h>

#define CRITICAL(X,...) printf("\nCRITICAL: "X, ##__VA_ARGS__); wait_for_reset();

#ifdef WARNINGS
#define WARNING(X,...) printf("\nWARNING: "X, ##__VA_ARGS__);
#else
#define WARNING(A,...) do{}while(0);
#endif

#ifdef ERRORS
#define ERROR(X,...) printf("\nERROR: "X, ##__VA_ARGS__);
#else
#define ERROR(X,...) do{}while(0);
#endif

#ifdef INFOS
#define INFO(X,...) printf("\n"X, ##__VA_ARGS__);
#else
#define INFO(X,...) do{}while(0);
#endif

extern struct scheduler_info_t scheduler_info;

#define is_vcpu_executing (((vcpu_t*)scheduler_info.vcpu_executing_nd) ? ((vcpu_t*)scheduler_info.vcpu_executing_nd->elem) : NULL)
#define vcpu_executing ((vcpu_t*)scheduler_info.vcpu_executing_nd->elem)
#define vm_executing   ((vm_t*)vcpu_executing->vm)

#define MICROSECOND (MILISECOND/1000)

/* Interval for interrupt injection on guests */
#define GUEST_QUANTUM (SYSTEM_TICK_US * MICROSECOND)




#endif