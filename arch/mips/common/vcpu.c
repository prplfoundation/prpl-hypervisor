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
 * @file vcpu.c
 * 
 * @section DESCRIPTION
 * 
 * VCPU related function calls (context save, restore and instruction emulation).
 */

#include <vcpu.h> 
#include <types.h>
#include <libc.h>
#include <globals.h>
#include <hal.h>
#include <mips_cp0.h>
#include <scheduler.h>
#include <guest_interrupts.h>

/**
 * @brief Save the VCPU context. Saving only the necessary registers for 
 * the supported OSs. 
 */
void contextSave(){
	vcpu_t *vcputosave;
	
	if (!is_vcpu_executing){
		return;
	}
	
	vcputosave = vcpu_in_execution;
	
	if (vcputosave->init == 0){
		/* already Initialized VCPU - save the context */
		
		/* TODO: Guest timer is not being saved. The guests have
		 * absolute time view. This may not be the correct behavior
		 * for all type of guests. Timer virtualization should be implemented
		 * using the guest offset register.
		 *		
		 *		vcputosave->cp0_registers[9][0] = guestcount;
		 *		vcputosave->rootcount = counter;
		 */
		 
		 /* Save only necessary guest.cp0 registers. All CP0 registers
		  * write by the guests must be saved. 
		  */
		 vcputosave->cp0_registers[0] = mfgc0(4,0);	
		 vcputosave->cp0_registers[1] = mfgc0(6,0);
		 vcputosave->cp0_registers[2] = mfgc0(8,0);
		 vcputosave->cp0_registers[3] = mfgc0(11,0);
		 vcputosave->cp0_registers[4] = mfgc0(12,0);
		 vcputosave->cp0_registers[5] = mfgc0(12,1);	
		 vcputosave->cp0_registers[6] = mfgc0(12,2);
		 vcputosave->cp0_registers[7] = mfgc0(12,3);
		 vcputosave->cp0_registers[8] = mfgc0(13,0);
		 vcputosave->cp0_registers[9] = mfgc0(14,0);
		 vcputosave->cp0_registers[10] = mfgc0(14,2);
		 vcputosave->cp0_registers[11] = mfgc0(15,1);
		 vcputosave->cp0_registers[12] = mfgc0(16,0);
		 vcputosave->cp0_registers[13] = mfgc0(17,0);
		 vcputosave->cp0_registers[14] = mfgc0(16,3);
		 vcputosave->cp0_registers[15] = mfgc0(30,0);

		 vcputosave->guestclt2 |= getGuestCTL2();

		 vcputosave->pc = getEPC();
	}
}

/**
 * @brief The hypervisor performs this routine when there is 
 * 	no VCPU read to execute. In this case the hypervisor 
 * 	will wait by the next interrupt event. 
 */ 
static void cpu_idle(){
	while(1){
		/* No VCPU read for execution. Wait by the next interrupt. */
		asm("wait");
	}
}

/** 
 * @brief  Configure the processor to enter in idle mode. 
 * 
 */
static void config_idle_cpu(){
		setGuestID(0); /* Root guest ID */
		setPreviousShadowSet(0); /* Hypervisor GPR shadow page  */
		clearGuestMode(); /* keep the processor in root mode. */
		setEPC((uint32_t)cpu_idle);
}

/**
 * @brief  Restore the VCPU context on context switch. 
 */
void contextRestore(){
	
	vcpu_t *vcpu = vcpu_in_execution;
	
	/* 
	/* There are not VCPUs ready to execute. Put CPU in adle mode. */
	if(!vcpu){
		config_idle_cpu();
		return;
	}
	
	if (vcpu->gprshadowset){
            setPreviousShadowSet(vcpu->gprshadowset);
	
            setLowestGShadow(vcpu->gprshadowset);	
        }
        
	setGuestID(vcpu->id);
	
	setGuestMode();
	
	/* Mark the VCPU as initialized. */
	if(vcpu_in_execution->init){
		vcpu_in_execution->init = 0;
	}
	
	mtgc0(4, 0, vcpu->cp0_registers[0]);
	mtgc0(6, 0, vcpu->cp0_registers[1]);
	mtgc0(8, 0, vcpu->cp0_registers[2]);
	mtgc0(12, 0, vcpu->cp0_registers[4]);
	mtgc0(12, 1, vcpu->cp0_registers[5]);
	mtgc0(12, 2, vcpu->cp0_registers[6]);
	mtgc0(12, 3, vcpu->cp0_registers[7]);
	mtgc0(13, 0, vcpu->cp0_registers[8]);
	mtgc0(14, 0, vcpu->cp0_registers[9]);
	mtgc0(14, 2, vcpu->cp0_registers[10]);	
	mtgc0(15, 1, vcpu->cp0_registers[11]);	
	mtgc0(16, 0, vcpu->cp0_registers[12]);
	mtgc0(17, 0, vcpu->cp0_registers[13]);
	mtgc0(16, 3, vcpu->cp0_registers[14]);
	mtgc0(30, 0, vcpu->cp0_registers[15]);
	
	setGuestCTL2(vcpu->guestclt2);
	
	/* clear timer int. */
	vcpu->guestclt2 &= ~(GUEST_TIMER_INT << GUESTCLT2_GRIPL_SHIFT);
	
	setEPC(vcpu->pc);
}
