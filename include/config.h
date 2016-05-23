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

#define DEBUG
#define WARNINGS
#define INFOS
#define CRITICALS

#define VERBOSE


#define STATICTLB

/* 50MHz core - counter at 25Mhz */
#define MILISECOND    (25000000 / 1000)

/* quantum */
#define QUANTUM     (30 * MILISECOND)

/* Heap Size */
#define HEAP_SIZE 0x80000 

/* Heap Address */ 
#define HEAP_ADDRESS  0x80032000

#define VMCONF_NUMCOLUNS 6

/* Static TLB - 1 Linux 32MB VM and 1 Hellfire VM.*/
#define VMCONF {0x90000000, 	0x200000, 	3,		LINUX,		0xff, 	  0x8029d040, \
		1, 		0x10000, 	0x11000,	PAGEMASK_16MB,	0,    	  4, \
		1,		0x1F000,	0,      	PAGEMASK_1MB,  	0x1F000,  2, \
		1,		0x1b100,	0,      	PAGEMASK_4KB,  	0x1b100,  2, \
		0x91200000, 	0x100000, 	2,		HELLFIRE,	0, 	  0x800010e4, \
		2, 		0x14000, 	0,		PAGEMASK_1MB,	0,    	  4, \
		2,		0x1F000,	0,      	PAGEMASK_4KB,  	0x1F000,  2, \
		0,		      0,        0,              0,              0,        0  }

/* Static TLB - 1 Linux 32MB VM.*/

/*#define VMCONF {0x90000000, 	16777216, 	3,		LINUX,		0xff, 	  0x8029d040, \
		1, 		0x10000, 	0x11000,	PAGEMASK_16MB,	0,    	  4, \
		1,		0x1F000,	0,      	PAGEMASK_1MB,  	0x1F000,  2, \
		1,		0x1b100,	0,      	PAGEMASK_4KB,  	0x1b100,  2, \
		0,		      0,        0,              0,              0,        0  }*/


#define VMCONF_RT {0}

