/*
 * Copyright (c) 2016, prpl Foundation
 * 
 * Permission to use, copy, modify, and/or distribute this software for any purpose with or without 
 * fee is hereby granted, provided that the above copyright notice and this permission notice appear 
 * in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE 
 * FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * This code was written by Sergio Johann at Embedded System Group (GSE) at PUCRS/Brazil.
 * 
 */

/**
 * @file mutex.c
 * @author Sergio Johann Filho
 * @date February 2016
 * 
 * @section DESCRIPTION
 * 
 * Mutex synchronization primitives.
 * 
 */

#include <os.h>

static int32_t tsl(mutex_t *m)
{
	volatile int32_t status, init;
	
	status = _di();
	init = m->lock;
	m->lock = 1;
	_ei(status);

	return init;
}

/**
 * @brief Initializes a mutex, defining its initial value.
 * 
 * @param s is a pointer to a mutex.
 */
void os_mtxinit(mutex_t *m)
{
	m->lock = 0;
}

/**
 * @brief Locks a mutex.
 * 
 * @param s is a pointer to a mutex.
 * 
 * If the mutex is not locked, the calling task continues execution. Otherwise,
 * the task spins.
 */
void os_mtxlock(mutex_t *m)
{
	while (tsl(m) == 1);
}

/**
 * @brief Unlocks a mutex.
 * 
 * @param s is a pointer to a mutex.
 */
void os_mtxunlock(mutex_t *m)
{
	m->lock = 0;
}
