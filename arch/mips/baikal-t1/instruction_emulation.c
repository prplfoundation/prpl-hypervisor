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

/**
 * @file instruction_emulation.c
 * 
 * @section DESCRIPTION Functions to perform instruction emulation. This should be part of the vcpu.c file.
 * 	However, not all platforms will require it. For example, platorms that only perform baremetal applications 
 * 	do not require instruction emulation. Platforms that virtualize Linux instances will require. 
 */


#include <libc.h>
#include <vcpu.h>
#include <mips_cp0.h>
#include <gpr_context.h>
#include <globals.h>
#include <instruction_emulation.h>

/**
 * @brief Emulate instructions with CO field.
 * 
 * @param badinstr Instruction that caused the trap.
 * @return 0 on success, otherwise error.
 */
static uint32_t co_instructions(uint32_t badinstr){
	switch (FUNC(badinstr)){
		case WAIT: /* wait instruction. */
			return 1;
		default:
			return 1;
	}
	return 0;
}

/**
 * @brief Emulate mfc0 instructions.
 * 
 * @param badinstr Instruction that caused the trap.
 * @return 0 on success, otherwise error.
 */
static uint32_t mfc0_instructions(uint32_t badinstr){
	uint32_t rd = RD(badinstr);
	uint32_t rt = RT(badinstr);
	uint32_t regvalue;
	
	switch (rd){
		case 12:
			if(SEL(badinstr) == 2){
				/* read from COP0 (12, 2) SRSCTL */
				regvalue = mfc0(CP0_SRSCTL, 2);
				regvalue = regvalue & ~SRSCLT_HSS;
				MoveToPreviousGuestGPR(rt, regvalue);
				return 0;
			}
			return 1;
		case 15: 
			if (SEL(badinstr) == 0){
				/* read from COP0 (15, 0) PRID */
				//regvalue = (mfc0(CP0_PRID, 0) & ~0xff00) | 0x8000;
				regvalue = mfc0(CP0_PRID, 0);
				MoveToPreviousGuestGPR(rt, regvalue);
				return 0;
			}
			return 1;
		case 18: 
			if (SEL(badinstr) == 0){
				/* read from COP0 (18, 0) WATCHLo0 */
				regvalue = mfgc0(CP0_WATCHLO, 0);
				MoveToPreviousGuestGPR(rt, regvalue);
				return 0;
			}else if (SEL(badinstr) == 1){
				/* read from COP0 (18, 0) WATCHLo1 */
				MoveToPreviousGuestGPR(rt, 0);
				return 0;
			}
			return 1;
		case 19: 
			if (SEL(badinstr) == 0){
				/* read from COP0 (18, 0) WATCHHi0 */
				regvalue = mfgc0(CP0_WATCHHI, 0);
				MoveToPreviousGuestGPR(rt, regvalue);
				return 0;
			}else if (SEL(badinstr) == 1){
				/* read from COP0 (18, 0) WATCHHi1 */
				MoveToPreviousGuestGPR(rt, 0);
				return 0;
			}
			return 1;
			
		default:
			return 1;
	}
	return 0;
}

/**
 * @brief Emulate mtc0 instructions.
 * 
 * @param badinstr Instruction that caused the trap.
 * @return 0 on success, otherwise error.
 */
static uint32_t mtc0_instructions(uint32_t badinstr){
	uint32_t rd = RD(badinstr);
	uint32_t rt = RT(badinstr);
	uint32_t regvalue;
	
	switch (rd){
		case 18: 
			if (SEL(badinstr) == 0){
				/* write to COP0 (18, 0) WATCHLo0 */
				regvalue = MoveFromPreviousGuestGPR(rd);
				mtgc0(CP0_WATCHLO, 0, regvalue);
				return 0;
			}else if (SEL(badinstr) == 1){
				/* write to COP0 (18, 1) WATCHLo1 */
				return 0;
			}
			return 1;
		case 19: 
			if (SEL(badinstr) == 0){
				/* write to COP0 (19, 0) WATCHHi0 */
				regvalue = MoveFromPreviousGuestGPR(rd);
				mtgc0(CP0_WATCHHI, 0, regvalue);
				return 0;
			}else if (SEL(badinstr) == 1){
				/* write to COP0 (19, 1) WATCHHi1 */
				return 0;
			}
			return 1;
			
		default:
			return 1;
	}
	return 0;
}



/** 
 * @brief Instruction emulation entry.
 * 
 */
uint32_t __instruction_emulation(uint32_t epc){
	uint32_t badinstr = getBadInstruction();
	uint32_t rs;

	switch(OPCODE(badinstr)){
		/* COP0 instructions */
		case CP0:
			switch (CO(badinstr)){
				case	0x1: 
					if(co_instructions(badinstr)){
						goto instr_not_supported;
					}
					break;
				case 	0:
					rs = RS(badinstr);
					switch (rs){
						case MTC:
							if(mtc0_instructions(badinstr)){
								goto instr_not_supported;
							}
							break;
						case MFC:
							if(mfc0_instructions(badinstr)){
								goto instr_not_supported;
							}
							break;
						default:
							goto instr_not_supported;
					}
					break;
				default:
					goto instr_not_supported;
			}
			break;
		/* Cache instructions */
		case CACHE:
			goto instr_not_supported;
		default:
			goto instr_not_supported;
	}
	
	return 0;

instr_not_supported:
	WARNING("Instruction 0x%x emulation at 0x%x not supported.", badinstr, epc);
	return 1;
	
}