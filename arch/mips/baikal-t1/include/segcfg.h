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

/*
 *  Baikal-T (P5600) address segments macro
 *  (c)2014 Baikal Electronics OJSC
 *
 */

#ifndef _ASM_SEGCFG_H
#define _ASM_SEGCFG_H

#define MIPS_SEGCFG_PA_SHIFT	9
#define MIPS_SEGCFG_PA		((127) << MIPS_SEGCFG_PA_SHIFT)
#define MIPS_SEGCFG_AM_SHIFT	4
#define MIPS_SEGCFG_AM		((7) << MIPS_SEGCFG_AM_SHIFT)
#define MIPS_SEGCFG_EU_SHIFT	3
#define MIPS_SEGCFG_EU		((1) << MIPS_SEGCFG_EU_SHIFT)
#define MIPS_SEGCFG_C_SHIFT	0
#define MIPS_SEGCFG_C		((7) << MIPS_SEGCFG_C_SHIFT)

#define MIPS_SEGCFG_UUSK	(7)
#define MIPS_SEGCFG_USK		(5)
#define MIPS_SEGCFG_MUSUK	(4)
#define MIPS_SEGCFG_MUSK	(3)
#define MIPS_SEGCFG_MSK		(2)
#define MIPS_SEGCFG_MK		(1)
#define MIPS_SEGCFG_UK		(0)

/*
 * For Legacy Mode should be next configuration
 *
 * Segment   Virtual    Size   Access Mode   Physical   Caching   EU
 * -------   -------    ----   -----------   --------   -------   --
 *    0      e0000000   512M      MK            UND         U      0
 *    1      c0000000   512M      MSK           UND         U      0
 *    2      a0000000   512M      UK            000         2      0
 *    3      80000000   512M      UK            000         3      0
 *    4      40000000    1G       MUSK          UND         U      1
 *    5      00000000    1G       MUSK          UND         U      1
 */

#define MIPS_LEGACY_SEG0	((MIPS_SEGCFG_MK << MIPS_SEGCFG_AM_SHIFT) | \
				((3) << MIPS_SEGCFG_C_SHIFT))
#define MIPS_LEGACY_SEG1	((MIPS_SEGCFG_MSK << MIPS_SEGCFG_AM_SHIFT) | \
				((3) << MIPS_SEGCFG_C_SHIFT))
#define MIPS_LEGACY_SEG2	((MIPS_SEGCFG_UK << MIPS_SEGCFG_AM_SHIFT) | \
				((2) << MIPS_SEGCFG_C_SHIFT))
#define MIPS_LEGACY_SEG3	((MIPS_SEGCFG_UK << MIPS_SEGCFG_AM_SHIFT) | \
				((3) << MIPS_SEGCFG_C_SHIFT))
#define MIPS_LEGACY_SEG4	((MIPS_SEGCFG_MUSK << MIPS_SEGCFG_AM_SHIFT) | \
				((3) << MIPS_SEGCFG_C_SHIFT) | \
				((1) << MIPS_SEGCFG_EU_SHIFT))
#define MIPS_LEGACY_SEG5	((MIPS_SEGCFG_MUSK << MIPS_SEGCFG_AM_SHIFT) | \
				((3) << MIPS_SEGCFG_C_SHIFT) | \
				((1) << MIPS_SEGCFG_EU_SHIFT))

#define MIPS_LEGACY_SEGCFG0	((MIPS_LEGACY_SEG1 << 16) | MIPS_LEGACY_SEG0)
#define MIPS_LEGACY_SEGCFG1	((MIPS_LEGACY_SEG3 << 16) | MIPS_LEGACY_SEG2)
#define MIPS_LEGACY_SEGCFG2	((MIPS_LEGACY_SEG5 << 16) | MIPS_LEGACY_SEG4)

#endif /* _ASM_SEGCFG_H */
