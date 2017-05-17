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
 * This code was written by Carlos Moratelli at Embedded System Group (GSE) at PUCRS/Brazil.
 * 
 */

#ifndef _INSTRUCTION_EMULATION
#define _INSTRUCTION_EMULATION

/* Extract instructions fields */
#define OPCODE(i) (i >> 26)
#define FUNC(i) (i & 0x3F)
#define RS(i) ((i >> 21) & 0x1f)
#define RT(i) ((i >> 16) & 0x1f)
#define RD(i) ((i >> 11) & 0x1f)
#define SEL(i) (i & 0x7)
#define IMED(i) (i & 0xFFFF)
#define SIMED(i) (IMED(i) & 0x8000 ? 0xFFFF0000 | IMED(i) : IMED(i))
#define JT(i) ((x & 0x3FFFFFF) << 2)
#define UPPERPC(i) (i & 0xF0000000)
#define CO(i) ((i >> 25) & 0x1)

uint32_t __instruction_emulation(uint32_t epc);

#endif 