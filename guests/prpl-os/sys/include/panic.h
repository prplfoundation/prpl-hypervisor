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


/* generic kernel panic definitions */
#define PANIC_ABORTED			0x00
#define PANIC_GPF			0x01
#define PANIC_STACK_OVERFLOW		0x02
#define PANIC_NO_TASKS_LEFT		0x03
#define PANIC_OOM			0x04
#define PANIC_NO_TASKS_RUN		0x05
#define PANIC_NO_TASKS_DELAY		0x07
#define PANIC_UNKNOWN_TASK_STATE	0x08
#define PANIC_CANT_PLACE_RUN		0x09
#define PANIC_CANT_PLACE_DELAY		0x0a
#define PANIC_CANT_SWAP			0x0c
#define PANIC_NUTS_SEM			0x0d
#define PANIC_UNKNOWN			0xff

void panic(int32_t cause);
