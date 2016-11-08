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

/* C type extensions */
typedef unsigned char			uint8_t;
typedef char				int8_t;
typedef unsigned short int		uint16_t;
typedef short int			int16_t;
typedef unsigned int			uint32_t;
typedef int				int32_t;
typedef unsigned long long		uint64_t;
typedef long long			int64_t;
typedef unsigned int			size_t;
typedef void				(*funcptr)();

/* disable interrupts, return previous int status / enable interrupts */
#define _di()				_interrupt_set(0)
#define _ei(S)				_interrupt_set(S)

#include <pic32mz.h>

#define STACK_MAGIC			0xb00bb00b
typedef uint32_t context[20];

/* hardware dependent stuff */
int32_t _interrupt_set(int32_t s);
void _restoreexec(context env, int32_t val, int32_t ctask);

/* hardware dependent C library stuff */
int32_t setjmp(context env);
void longjmp(context env, int32_t val);
void putchar(int32_t value);
int32_t kbhit(void);
int32_t getchar(void);

/* hardware dependent stuff */
void delay_ms(uint32_t msec);
void delay_us(uint32_t usec);

/* hardware dependent basic kernel stuff */
void _hardware_init(void);
void _vm_init(void);
void _task_init(void);
void _sched_init(void);
void _timer_init(void);
void _irq_init(void);
void _device_init(void);
void _set_task_sp(uint16_t task, uint32_t stack);
uint32_t _get_task_sp(uint16_t task);
void _set_task_tp(uint16_t task, void (*entry)());
void *_get_task_tp(uint16_t task);
void _timer_reset(void);
void _cpu_idle(void);
uint32_t _readcounter(void);

void _restoreexec(context env, int32_t val, int32_t ctask);
void _irq_handler(uint32_t status, uint32_t cause);
void _except_handler(uint32_t epc, uint32_t opcode);
void _irq_register(uint32_t mask, funcptr ptr);
