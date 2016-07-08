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

/* Debug and info UART prints */
#define DEBUG
#define WARNINGS
#define INFOS
#define CRITICALS
#define MICROCHIP
#define VERBOSE

/* Hypervisor version and board info */
#define KERNEL_VER "v0.5.0"
#define CPU_ID "M5150"
#define CPU_ARCH "Microchip Starter Kit"
#define CPU_FREQ  200000000
#define UART_SPEED 115200

/* Timer and scheduler configuration */

/* 200MHz core - counter at 100Mhz */
#define MILISECOND    (100000000 / 1000)
/* Scheduler Quantum */
#define QUANTUM     (1 * MILISECOND)

/* Heap configuration */

/* Heap Size */
#define HEAP_SIZE 0x8000 
/* Heap Address */ 
#define HEAP_ADDRESS  0x80002000


#define NVMACHINES 3
#define STATICTLB
#define VMCONF_NUMCOLUNS 6


#define VMCONF {0x80010000, 0x4000,     5,              BAREOS,         0,        0x9d001000, \
                1,              0x1d010,        0x1d020,        PAGEMASK_64KB,  0x1d000,    2, \
                1,              0x00010,        0x00020,        PAGEMASK_64KB,   0x00000,    2, \
                1,              0x1f822,        0,              PAGEMASK_4KB,   0x1f822,    2, \
                1,              0,              0x1f821,        PAGEMASK_4KB,   0x1f821,    2, \
                1,              0x1f860,        0,              PAGEMASK_4KB,   0x1f860,    2, \
                0x80030000,     0x4000,   3,              BAREOS,         0,        0x9d001000, \
                2,              0x1d030,        0x1d040,        PAGEMASK_64KB,   0x1d000,    2, \
                2,              0x00030,        0x00040,              PAGEMASK_64KB,   0x00000,    2, \
                2,              0x1f822,        0,              PAGEMASK_4KB,   0x1f822,    2, \
                0x80050000,     0x4000,   3,              BAREOS,         0,        0x9d001000, \
                3,              0x1d050,        0x1d060,        PAGEMASK_64KB,   0x1d000,    2, \
                3,              0x00050,        0x00060,        PAGEMASK_64KB,   0x00000,    2, \
                3,              0x1f822,        0,              PAGEMASK_4KB,   0x1f822,    2, \
                0,           0,        0,              0,              0,        0  }



#define VMCONF_RT {0}

