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

#include <vcpu.h> 
#include <types.h>
#include <libc.h>
#include <rw_regs.h>
#include <globals.h>
#include <hal.h>

processor_t *proc;
//vcpu_t *last_vcpu;
vcpu_t *idle_vcpu;
vcpu_t *target_vcpu;

//Last vcpu that used the sgpr set
vcpu_t * vcpu_sgpr[8];

void vcpu_set_pc(vcpu_t *v, uint32_t pc) {
  v->pc = pc;
}

void vcpu_set_k0(vcpu_t *v, uint32_t value) {
  //Set on guest id shadow register
}

void save_curr_vcpu_ctx(){
	
	//Save curr_vcpu context
	
	curr_vcpu->pc = getEPC();
			
}

void restore_curr_vcpu_ctx(){
	
	//Restore curr_vcpu context
	
	setEPC(curr_vcpu->pc);	
}

/** Determine the next PC when an exception occurs on the branch delay slot. */
uint32_t CalcNextPC(uint32_t epc){
	uint32_t badinstrP = hal_lr_badinstrP();
	uint32_t npc;
	uint32_t rs, rt, rd;
	
	epc+=4;
	
	if(!getCauseBD()){
		return epc;
	}
	
	printf("branch delay!!");
	
	rs = RS(badinstrP);
	rt = RT(badinstrP);
	rd = RD(badinstrP);
	
	switch(OPCODE(badinstrP)){
		case ESPECIAL:
			switch(FUNC(badinstrP)) {
				case JALR:
					debugs("jalr");
					MoveToPreviousGuestGPR(rd, epc+8);
				case JR:
					debugs("jr");
					npc = MoveFromPreviousGuestGPR(rs);
					break;
				default:
					npc = epc+4;
					break;
			}
			break;

		case ESPECIAL1:
			switch (rt) {
				case BLTZ:
					debugs("bltz");
					npc = rs < 0 ? epc + (SIMED(badinstrP) << 2) : epc + 8;
					break;
					
				case BGEZ:
					debugs("bgez");
					npc = rs >= 0 ?  epc + (SIMED(badinstrP) << 2) : epc + 8;
					break;
					
				case BLTZAL:
					debugs("bltzal");
					if (rs < 0) {
						MoveToPreviousGuestGPR(31, epc+8);
						npc = epc + (SIMED(badinstrP) << 2);
					} else{
						npc = epc + 8;
					}
					break;
					
				case BGEZAL:
					debugs("bgezal");
					if (rs >= 0) {
						MoveToPreviousGuestGPR(31, epc+8);
						npc = epc + (SIMED(badinstrP) << 2);
					}else{
						npc = epc + 8;
					}
					break;
					
					default:
						npc = epc+4;
						break;
			}
			break;
			
		case JAL:
			debugs("jal");
			MoveToPreviousGuestGPR(31, epc+8);
			
		case J:
			debugs("j");
			npc = JT(badinstrP) | UPPERPC(epc);
			break;

		case BEQ:
			debugs("beq");
			npc = rs == rt ? epc + (SIMED(badinstrP) << 2) : epc + 8;
			break;

		case BNE:
			debugs("bne");
			npc = rs != rt ? epc + (SIMED(badinstrP) << 2) : epc + 8;
			break;

		case BLEZ:
			debugs("blez");
			npc = rs <= 0 ? epc + (SIMED(badinstrP) << 2) : epc + 8;		
			break;

		case BGTZ:
			debugs("bgtz");
			npc = rs > 0 ? epc + (SIMED(badinstrP) << 2) : epc + 8;
			break;

		case BEQL:
			debugs("beql");
			if (rs == rt) {
				MoveToPreviousGuestGPR(31, epc+8);
				npc = epc + (SIMED(badinstrP) << 2);
			} else{
				npc = epc + 8;
			}
			break;

		case BNEL:
			debugs("bnel");
			if (rs != rt) {
				MoveToPreviousGuestGPR(31, epc+8);
				npc = epc + (SIMED(badinstrP) << 2);
			}else{
				npc = epc + 8;
			}
			break;

		case BLEZL:
			debugs("blezl");
			if (rs <= 0) {
				MoveToPreviousGuestGPR(31, epc+8);
				npc = epc + (SIMED(badinstrP) << 2);
			}else{
				npc = epc + 8;
			}
			break;

		case BGTZL:
			debugs("bgtzl");
			if (rs > 0) {
				MoveToPreviousGuestGPR(31, epc+8);
				npc = epc + (SIMED(badinstrP) << 2);
			}else{
				npc = epc + 8;
			}
			break;

		default:
			npc = epc+4;
			break;
	}
	
	return npc;
}

/** Implement the instruction emulation  */
/* FIXME: must emulate all necessary registers. For now, just status reg. */
uint32_t InstructionEmulation(uint32_t epc){
	uint32_t badinstr = hal_lr_badinstr();
	uint32_t regvalue;
	uint32_t currentValue;
	uint32_t rs, rt, rd;
	uint32_t opcode, flag = 0;
	uint32_t co, func, emulinstr;
	
	opcode = OPCODE(badinstr);	
	rs = RS(badinstr);
	rt = RT(badinstr);
	rd = RD(badinstr);
	co = CO(badinstr);
	func = FUNC(badinstr);
	
	switch(opcode){
		/* COP0 instructions */
		case CP0:
			switch (co){
				case	0x1: 
					switch (func){
						case WAIT: /* wait */
							Warning("Wait emulation ignored.");
							break;
						default:
							Warning("Instruction 0x%x at 0x%x not supported on VCPU 0x%x ", badinstr, epc, curr_vcpu->id);
							break;
					}
					break;
				case 	0:
					switch (rs){
						case MTC:
							regvalue = MoveFromPreviousGuestGPR(rt);
							switch (rd){
								case 0x0C:
									switch(SEL(badinstr)){
										case 0:
											currentValue = MoveFromGuestCP0(rd ,0);
											regvalue = (regvalue & STATUS_MASK) | (currentValue & !STATUS_MASK);
											MoveToGuestCP0(rd, 0, regvalue);
											flag = 1;
											break;
										case 3: /* srsctl */
											Warning("Write to CP0 SRSCTL ignored");
											break;
										default:
											Warning("Instruction 0x%x at 0x%x not supported on VCPU 0x%x ", badinstr, epc, curr_vcpu->id);
											break;
									}
								break;
								case 0x09:
									switch(SEL(badinstr)){
										case 0:
											currentValue = MoveFromGuestCP0(0x9, 0);
											currentValue = (~currentValue) + 1;
											setGTOffset(currentValue);
											break;
										default:
											Warning("Instruction 0x%x at 0x%x not supported on VCPU 0x%x ", badinstr, epc, curr_vcpu->id);
											break;
									}
								break;
								
								default:
									Warning("Instruction 0x%x at 0x%x not supported on VCPU 0x%x ", badinstr, epc, curr_vcpu->id);
									break;
							}
							break;
						case MFC:
							switch (rd){
								case 0xf: /* read from COP0 15.*/
									regvalue = (hal_lr_pid() & ~0xff00) | 0x8000;
									MoveToPreviousGuestGPR(rt, regvalue);
									break;
								case 0xc: /* read from COP0 12.*/
									switch(SEL(badinstr)){
										case 2:
											regvalue = hal_lr_srsclt();
											regvalue = regvalue & ~SRSCLT_HSS;
											MoveToPreviousGuestGPR(rt, regvalue);
											break;
										default:
											Warning("Instruction 0x%x at 0x%x not supported on VCPU 0x%x ", badinstr, epc, curr_vcpu->id);
											break;
									}
									break;
								case 0x19: /* read from COP0 25.*/
									regvalue = hal_lr_pcr();
									MoveToPreviousGuestGPR(rt, regvalue);
									break;
						
								default:
									Warning("Instruction 0x%x at 0x%x not supported on VCPU 0x%x ", badinstr, epc, curr_vcpu->id);
									break;
							}
							break;
						default:
							Warning("Instruction 0x%x at 0x%x not supported on VCPU 0x%x ", badinstr, epc, curr_vcpu->id);
							break;
					}
					break;
				default:
					Warning("Instruction 0x%x at 0x%x not supported on VCPU 0x%x ", badinstr, epc, curr_vcpu->id);
					break;
			}
			break;
			/* Cache instructions */
		case CACHE:
			Warning("Cache instruction 0x%x at 0x%x not supported on VCPU 0x%x ", badinstr, epc, curr_vcpu->id);
			break;
		default:
			Warning("Instruction 0x%x at 0x%x not supported on VCPU 0x%x", badinstr, epc, curr_vcpu->id);
			break;
	}

	return SUCEEDED;
}

/** Save the cpu context on context switch 
* For now, saving just the necessary regs. */
void contextSave(vcpu_t *vcpu, uint32_t counter, uint32_t guestcount){
	vcpu_t *vcputosave;
	
	if(vcpu){
		vcputosave = vcpu;
	}else{
		vcputosave = curr_vcpu;
	}
	
	if (vcputosave->init == 0 && vcputosave != idle_vcpu){
        /* already Initialized VCPU - save the context */
	
        vcputosave->cp0_registers[9][0] = guestcount;
	
        /* save root.count for calculate gtooffset on context restore. */
        vcputosave->rootcount = counter;
	
        /* Save the guest.cp0 registers */
        vcputosave->cp0_registers[4][0] = MoveFromGuestCP0(4,0);	
        vcputosave->cp0_registers[6][0] = MoveFromGuestCP0(6,0);
        vcputosave->cp0_registers[8][0] = MoveFromGuestCP0(8,0);
        vcputosave->cp0_registers[11][0] = MoveFromGuestCP0(11,0);
        vcputosave->cp0_registers[12][0] = MoveFromGuestCP0(12,0);
        vcputosave->cp0_registers[12][1] = MoveFromGuestCP0(12,1);	
        vcputosave->cp0_registers[12][2] = MoveFromGuestCP0(12,2);
        vcputosave->cp0_registers[12][3] = MoveFromGuestCP0(12,3);
        vcputosave->cp0_registers[13][0] = MoveFromGuestCP0(13,0);
        vcputosave->cp0_registers[14][0] = MoveFromGuestCP0(14,0);
        vcputosave->cp0_registers[14][2] = MoveFromGuestCP0(14,2);
        vcputosave->cp0_registers[15][0] = MoveFromGuestCP0(15,1);
        vcputosave->cp0_registers[16][0] = MoveFromGuestCP0(16,0);
        vcputosave->cp0_registers[17][0] = MoveFromGuestCP0(17,0);
        vcputosave->cp0_registers[16][3] = MoveFromGuestCP0(16,3);
        vcputosave->cp0_registers[30][0] = MoveFromGuestCP0(30,0);
        vcputosave->pc = getEPC();
    }
}

/** Determines the Guest Time Offset. */
uint32_t calculateGTOffset(uint32_t savedcounter, uint32_t currentCount){
	uint32_t offset;	
	
	/* check for count overflow */
	if(savedcounter > currentCount){
		offset= (0xFFFFFFFF - savedcounter) + currentCount;
	}else{
		offset= currentCount - savedcounter;
	}
	
	/* gtoffset is the two's complement of the time between save and restore the context of a VCPU.*/
	return (~offset) + 1;
}

/** Restore the cpu context on context switch */
/* FIXME: must restore all necessary registers. For now, just status reg. */
void contextRestore(){

	vcpu_t *currentVCPU = curr_vcpu;
	int32_t gtoffset;
	uint32_t guestcount;
	uint32_t currentCountRoot;
	uint32_t pending;
    
    if (currentVCPU != idle_vcpu ){
		
	MoveToGuestCP0(12, 0, currentVCPU->cp0_registers[12][0]);
	/* Restoring Guest Time. See documentation! */
	if(currentVCPU->cp0_registers[11][0]!=0){
		/* determines the new gtoffset */
		currentCountRoot = getCounter();
// 		gtoffset = calculateGTOffset(currentVCPU->rootcount,currentCountRoot);
		setGTOffset(gtoffset);
		
		/* Restore compare */
		MoveToGuestCP0(11, 0, currentVCPU->cp0_registers[11][0]);
		
		/* read guest.count */ 
		guestcount = MoveFromGuestCP0(9,0);
		
		/* Set Guest.Cause->TI.*/
		if(guestcount > currentVCPU->cp0_registers[9][0]){
			/* guest count has not wrapped */
			if(currentVCPU->cp0_registers[11][0] > currentVCPU->cp0_registers[9][0] && currentVCPU->cp0_registers[11][0] < guestcount){
				/* set timer interrupt */
				currentVCPU->cp0_registers[13][0] = currentVCPU->cp0_registers[13][0] | CAUSE_TI;
			}
		} else{
			/* guest count has wrapped */
			if(currentVCPU->cp0_registers[11][0] > currentVCPU->cp0_registers[9][0] || currentVCPU->cp0_registers[11][0] < guestcount){
				/* set timer interrupt */
				currentVCPU->cp0_registers[13][0] = currentVCPU->cp0_registers[13][0] | CAUSE_TI;
			}
		}
	}else{
		/* Restore compare */
		MoveToGuestCP0(11, 0, currentVCPU->cp0_registers[11][0]);
	}

	
	/* Finished restore guest timer. Restoring the other registers hereafter */
	//pending = getInterruptPending() & currentVCPU->pip;
	//clearInterruptMask(pending<<2);
	
	//Warning("p 0x%x", pending);
	
	MoveToGuestCP0(4, 0, currentVCPU->cp0_registers[4][0]);
	MoveToGuestCP0(6, 0, currentVCPU->cp0_registers[6][0]);
	MoveToGuestCP0(5, 0, currentVCPU->cp0_registers[5][0]);
	MoveToGuestCP0(8, 0, currentVCPU->cp0_registers[8][0]);
	MoveToGuestCP0(13, 0, currentVCPU->cp0_registers[13][0]);
	MoveToGuestCP0(12, 1, currentVCPU->cp0_registers[12][1]);
	MoveToGuestCP0(12, 2, currentVCPU->cp0_registers[12][2]);
	MoveToGuestCP0(12, 3, currentVCPU->cp0_registers[12][3]);
	MoveToGuestCP0(14, 0, currentVCPU->cp0_registers[14][0]);
	MoveToGuestCP0(14, 2, currentVCPU->cp0_registers[14][2]);	
	MoveToGuestCP0(15, 1, currentVCPU->cp0_registers[15][0]);	
	MoveToGuestCP0(16, 0, currentVCPU->cp0_registers[16][0]);
	MoveToGuestCP0(17, 0, currentVCPU->cp0_registers[17][0]);
	MoveToGuestCP0(16, 3, currentVCPU->cp0_registers[16][3]);
	MoveToGuestCP0(30, 0, currentVCPU->cp0_registers[30][0]);
	

	/* config interrupt pass-through (PIP) */
	//MoveToGuestCP0(12, 6, (MoveFromGuestCP0(12, 6) | currentVCPU->pip));
	//MoveToGuestCP0(10, 5, (MoveFromGuestCP0(10, 5) & ~(0xff << 10)) | pending);

	if(currentVCPU->guestclt2){
		setGuestCTL2(currentVCPU->guestclt2);
		currentVCPU->guestclt2 = 0;
	}else{
		setGuestCTL2(currentVCPU->guestclt2);
	}
	
    }
    
	setEPC(currentVCPU->pc);
    
}


uint32_t initProc() {
  
  int i;
  proc=(processor_t *)malloc(sizeof(processor_t));
  for(i=0;i<8;i++){
	vcpu_sgpr[i]=NULL;
  }
  
  Info("Initializing Physical Processor.");
  
  return 0;
  
}
