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
 * @file driver.h
 * 
 * @section DESCRIPTION
 * 
 * Driver definitions. 
 * The macro driver_init() must be declared for all device drivers indicating its initialization function. The initialization function will be 
 * called during hypervisor startup. 
 */

#ifndef __DRIVER_H_
#define __DRIVER_H_

#include <types.h>

/**
 * @brief Allocate a function pointer in a table of device drivers init calls. 
 * @param fn Init function name.
 */
#define driver_init(fn) static void (*__initcall_t_##fn)() __attribute__ ((section(".__drivers_table_init"))) __attribute__ ((__used__))= fn

typedef void initcall_t();

#endif

