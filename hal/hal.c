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

#include "libc.h"
#include "types.h"
#include "boot.h"
#include "hal.h"
#include "kernel.h"
#include<globals.h>

void* exceptionHandler_addr = exceptionHandler;


static void print_config(void)
{
        printf("\n===========================================================");
        printf("\nprplHypervsior %s [%s, %s]", KERNEL_VER, __DATE__, __TIME__);
        printf("\nCopyright (c) 2016, prpl Foundation");
        printf("\n===========================================================");
        printf("\nCPU ID:        %s", CPU_ID);
        printf("\nARCH:          %s", CPU_ARCH);
        printf("\nSYSCLK:        %dMHz", CPU_SPEED/1000000);
        printf("\nHeap size:     %dKbytes", HEAP_SIZE/1024);
        printf("\nScheduler      %dms", QUANTUM/100000);
        printf("\nVMs:           %d\n", NVMACHINES);
}


/** C code entry. Called from hal/$(BOARD)/boot.S */
int32_t main(char * _edata, char* _data, char* _erodata){
    

    /* Specific hardware configuration. */
    hardware_config();
    
    /* UART start */
    init_uart(115200, 9600, 200000000);
    
    print_config();
    
    /* First some paranoic checks!! */
    
    /* Verify if the processor implements the VZ module */
    if(!hasVZ()){
        /* panic */
        return 1;
    }

    /* is it in root mode ?  */
    if(!isRootMode()){
        /* panic */
        return 1;
    }
    
    /* This implementation relies on the GuestID field  */
    if(isRootASID()){ 
        return -1;
    }
    
    /* This implementation relies on the GuestID field */
    if(!hasGuestID()){
        /* panic */
        return 1;
    }

    if(has1KPageSupport()){
        /* Self Protection agains a variant that may implements 1K PageSupport. */
        Disable1KPageSupport();     
    }


    /* Now inialize the hardware */
    /* Processor inicialization */
    if(LowLevelProcInit()){
        //panic
        return 1;
    }
            
    /* Initialize memory */
    /* Register heap space on the allocator */ 
     if(init_mem()){        
        return 1;
    }
    
    /*Initialize processor structure*/
    if(initProc()){
        return 1;
    } 
    
    if(initializeShedulers()){
        return 1;
    }
    
    /*Initialize vcpus and virtual machines*/
    initializeMachines();
    
    /* TODO: Enable configuration and execution of RT VCPUs*/
    /*if(initializeRTMachines()){
        return 1;
    }*/

    /* Run scheduler .*/
    runScheduler();     
    
    hal_start_hyper();

    /* configure system timer */
    configure_timer();
    
    /* Should never reach this point !!! */
    return 0;
}


/** Verify if the processor is in root mode */
int32_t isRootMode(){
	if( !(hal_lr_guestctl0() & GUESTCTL0_GM) || 
		( (hal_lr_guestctl0() & GUESTCTL0_GM) &&  !( !(hal_lr_rdebug() & DEBUG_DM)
		&& !(hal_lr_rstatus() & STATUS_ERL) && !(hal_lr_rstatus() & STATUS_EXL)))){
		return 1;
	}

	return 0;
}

/** Verify if the processor implements the VZ module */
int32_t hasVZ(){
	int32_t config3;
	
	config3 = hal_lr_rconfig3();
	
	if( !(config3 & CONFIG3_VZ) ){
		return 0;
	}
	
	return 1;
}

/** Set the GPR Shadow Bank.
  * The next exception will be handle in the new GPR.
  */   
int32_t ConfigureGPRShadow(){
	int32_t srsclt_reg;
	int32_t guestsrsclt_reg;
	int32_t num_shadow_gprs = 0;

	/* Configure the GPR Shadow. The hypervisor will use the highest shadow page. 
	   Still, the hypervisor needs at least one GPR Shadow.
	 */
    
	srsclt_reg = hal_lr_srsclt();
	num_shadow_gprs = (srsclt_reg & SRSCTL_HSS) >> SRSCTL_HSS_SHIFT;
	if(num_shadow_gprs == 0){
		/* panic */
		return 1;
	}
	/* Set the ESS field and PSS */
	/* PSS is set to the highest page. The processor will change to the
	   new GPR on the next eret*/
	hal_sr_srsclt( num_shadow_gprs << SRSCTL_ESS_SHIFT | num_shadow_gprs << SRSCLT_PSS_SHIFT);
	
	/* Set the SRSMap register */
	hal_sr_srsmap( num_shadow_gprs << SRSMAP_SSV7_SHIFT |
			num_shadow_gprs << SRSMAP_SSV6_SHIFT |
			num_shadow_gprs << SRSMAP_SSV5_SHIFT |
			num_shadow_gprs << SRSMAP_SSV4_SHIFT |
			num_shadow_gprs << SRSMAP_SSV3_SHIFT |
			num_shadow_gprs << SRSMAP_SSV2_SHIFT |
			num_shadow_gprs << SRSMAP_SSV1_SHIFT |
			num_shadow_gprs << SRSMAP_SSV0_SHIFT );
	
	/* Set the SRSMap2 */
	hal_sr_srsmap2( num_shadow_gprs << SRSMAP2_SSV8_SHIFT |
			 num_shadow_gprs << SRSMAP2_SSV9_SHIFT );
			 
    
    //No virtual shadow registers to guests
    hal_sr_guestctl3(0); //Shadow 0

    return 0;
}

/**Low level Processor inicialization */
int32_t LowLevelProcInit(){
	uint32_t status;
	/* Processor User Guide 9.2.4 - Coprocessor 0 inicialization */
        /* FIXME: This should be done before load data to SRAM. */
       // hal_sr_rconfig( ((hal_lr_rconfig() & ~CONFIG_K0)) | 4);

	/* enable kseg0 cache for guest coprocessor 0 */
	//MoveToGuestCP0(16, 0, (MoveFromGuestCP0(16, 0) & ~0x7) | 4);
	hal_sr_intctl(hal_lr_intctl() | (INTCTL_VS << INTCTL_VS_SHIFT));
	
	//Initializing some flags on guestCtl0
	//GUESTCTL0_CP0 Allow guest access to some CP0 registers
	//GUESTCTL0_GT Allow guest read acess to count and compare registers
	//GUESTCTL0_CF Allow guest to read and write config registers
	
	hal_sr_guestctl0(hal_lr_guestctl0() | GUESTCTL0_CP0 | GUESTCTL0_GT | GUESTCTL0_CF |  GUESTCTL0_CG);
	
	hal_sr_guestctl0Ext(hal_lr_guestctl0Ext() | GUESTCTL0EXT_CGI);
	
	//Disabling Exceptions when guest modifies own CP0 registers
	hal_sr_guestctl0Ext(hal_lr_guestctl0Ext() | GUESTCTL0EXT_FCD);
	
	if (ConfigureGPRShadow()){
	
		return 1;
	}
		
        setGTOffset(0);
	
	return 0;
}

/** Return execCode field from cause register */
uint32_t getCauseCode(){
	uint32_t execcode = hal_lr_rcause();
    
	return (execcode & CAUSE_EXECCODE) >> CAUSE_EXECCODE_SHIFT;
}

/** Return the Hypercall code */
uint32_t getHypercallCode(){
	uint32_t hypcode = hal_lr_badinstr();
	
	return (hypcode & HYPCODE) >> HYPCODE_SHIFT;	
}

uint32_t getCauseBD(){
	return hal_lr_rcause() & CAUSE_BD;
}

/** Return GExecCode field from guestctl0 register */
uint32_t getGCauseCode(){
	uint32_t execcode = hal_lr_guestctl0();
	
	return (execcode & CAUSE_EXECCODE) >> CAUSE_EXECCODE_SHIFT;
}


/** Return the interrupt pending bits IP0:IP9 from cause register */
uint32_t getInterruptPending(){
	uint32_t pending = hal_lr_rcause();
	
	if(((pending>>CAUSE_PCI_SHIFT) & 0x1)==1)
		return PERFORMANCE_COUNTER_INT;
	else	
		return (pending & CAUSE_IP);
}	

/** Set compare register for generate time interruption  */
void confTimer(uint32_t quantum){
	uint32_t count = hal_lr_rcount();
	
	if(0xFFFFFFFF - count >= quantum){
		count += quantum;
	}else{
		count =  quantum - (0xFFFFFFFF - count);
	}
	
	
	hal_sr_rcompare(count);
}

/** Set the IM bits on the status reg */
void setInterruptMask(uint32_t im){
	hal_sr_rstatus(hal_lr_rstatus() | im);
}

/** Clear the IM bits on the status reg */
void clearInterruptMask(uint32_t im){
	hal_sr_rstatus(hal_lr_rstatus() & (~im));
}

/** Enable global interrupt. IE bit in status register */
void enableIE(){
	hal_sr_rstatus(hal_lr_rstatus() | STATUS_IE);
}

/** Verify if the processor implements GuestID field */
uint32_t hasGuestID(){
	if (hal_lr_guestctl0() & GUESTCTL0_G1){
		return 1;
	}
	return 0;
}

/** Verify if the processor allow direct root mode */
uint32_t isDirectRoot(){
	if (hal_lr_guestctl0() & GUESTCTL0_DRG){
		return 1;
	}
	
	return 0;
}

/** set epc */
void setEPC(uint32_t epc){
	hal_sr_epc(epc);
}

/** get epc */
uint32_t getEPC(){
	return hal_lr_epc();
}


/** Set root GuestID*/
void setGuestRID(uint32_t guestrid){
	uint32_t reg;
	reg = (hal_lr_guestclt1() & (~GUESTCTL1_RID)) | (guestrid << GUESTCTL1_RID_SHIFT);
	hal_sr_guestclt1(reg);
}

/** Set GuestID */
void setGuestID(uint32_t guestid){
	hal_sr_guestclt1((hal_lr_guestclt1() & (~GUESTCTL1_ID)) | guestid);
}

/** Get GuestID */
uint32_t getGuestID(void){
	return (hal_lr_guestclt1() & GUESTCTL1_ID);
}


uint32_t isRootASID(){
	if (hal_lr_guestctl0() & GUESTCTL0_RAD){
		return 1;
	}
	
	return 0;
}

/** Set the processor to Guest Mode */
void setGuestMode(){
	
	hal_sr_guestctl0(hal_lr_guestctl0() | GUESTCTL0_GM | GUESTCTL0_CP0 | GUESTCTL0_GT | 1 << 12);
	
}

void setStatusReg(uint32_t bits){
	hal_sr_rstatus(hal_lr_rstatus() | bits);
}

uint32_t has1KPageSupport(){
	return  hal_lr_rconfig3() & CONFIG3_SP;
}

void Disable1KPageSupport(){
	hal_sr_pagegrain(hal_lr_pagegrain() & ~PAGEGRAIN_ESP);
}

uint32_t isEnteringGuestMode(){
	uint32_t status;
	
	if(curr_vcpu == idle_vcpu)
		return 1;
	
	status = hal_lr_rstatus();
	
	if ( (hal_lr_guestctl0() & GUESTCTL0_GM) &&
	     !(status & STATUS_ERL) && 
	     (status & STATUS_EXL) &&
	     !(hal_lr_rdebug() & DEBUG_DM) ){
		return 1;
	}
	
	return 0;
}

uint32_t getBadVAddress(){
	return hal_lr_badVAaddr();
}

/** Read Reg from preview GPR Shadow */
uint32_t MoveFromPreviousGuestGPR(uint32_t reg){
	uint32_t temp;
	
	switch(reg){
		case 0: asm volatile ("rdpgpr %[temp], $0": [temp] "=r"(temp) :);
			break;
		case 1: asm volatile ("rdpgpr %[temp], $1": [temp] "=r"(temp) :);
			break;
		case 2: asm volatile ("rdpgpr %[temp], $2": [temp] "=r"(temp) :);
			break;
		case 3: asm volatile ("rdpgpr %[temp], $3": [temp] "=r"(temp) :);
			break;
		case 4: asm volatile ("rdpgpr %[temp], $4": [temp] "=r"(temp) :);
			break;
		case 5: asm volatile ("rdpgpr %[temp], $5": [temp] "=r"(temp) :);
			break;
		case 6: asm volatile ("rdpgpr %[temp], $6": [temp] "=r"(temp) :);
			break;
		case 7: asm volatile ("rdpgpr %[temp], $7": [temp] "=r"(temp) :);
			break;
		case 8: asm volatile ("rdpgpr %[temp], $8": [temp] "=r"(temp) :);
			break;
		case 9: asm volatile ("rdpgpr %[temp], $9": [temp] "=r"(temp) :);
			break;
		case 10: asm volatile ("rdpgpr %[temp], $10": [temp] "=r"(temp) :);
			break;
		case 11: asm volatile ("rdpgpr %[temp], $11": [temp] "=r"(temp) :);
			break;
		case 12: asm volatile ("rdpgpr %[temp], $12": [temp] "=r"(temp) :);
			break;
		case 13: asm volatile ("rdpgpr %[temp], $13": [temp] "=r"(temp) :);
			break;
		case 14: asm volatile ("rdpgpr %[temp], $14": [temp] "=r"(temp) :);
			break;
		case 15: asm volatile ("rdpgpr %[temp], $15": [temp] "=r"(temp) :);
			break;
		case 16: asm volatile ("rdpgpr %[temp], $16": [temp] "=r"(temp) :);
			break;
		case 17: asm volatile ("rdpgpr %[temp], $17": [temp] "=r"(temp) :);
			break;
		case 18: asm volatile ("rdpgpr %[temp], $18": [temp] "=r"(temp) :);
			break;
		case 19: asm volatile ("rdpgpr %[temp], $19": [temp] "=r"(temp) :);
			break;
		case 20: asm volatile ("rdpgpr %[temp], $20": [temp] "=r"(temp) :);
			break;
		case 21: asm volatile ("rdpgpr %[temp], $21": [temp] "=r"(temp) :);
			break;
		case 22: asm volatile ("rdpgpr %[temp], $22": [temp] "=r"(temp) :);
			break;
		case 23: asm volatile ("rdpgpr %[temp], $23": [temp] "=r"(temp) :);
			break;
		case 24: asm volatile ("rdpgpr %[temp], $24": [temp] "=r"(temp) :);
			break;
		case 25: asm volatile ("rdpgpr %[temp], $25": [temp] "=r"(temp) :);
			break;
		case 26: asm volatile ("rdpgpr %[temp], $26": [temp] "=r"(temp) :);
			break;
		case 27: asm volatile ("rdpgpr %[temp], $27": [temp] "=r"(temp) :);
			break;
		case 28: asm volatile ("rdpgpr %[temp], $28": [temp] "=r"(temp) :);
			break;
		case 29: asm volatile ("rdpgpr %[temp], $29": [temp] "=r"(temp) :);
			break;
		case 30: asm volatile ("rdpgpr %[temp], $30": [temp] "=r"(temp) :);
			break;
		case 31: asm volatile ("rdpgpr %[temp], $31": [temp] "=r"(temp) :);
			break;
		default:
			debugs("Register not found.\n");
			break;
	}
	
	return temp;
}

/** Write value to Reg on preview GPR Shadow */
void MoveToPreviousGuestGPR(uint32_t reg, uint32_t value){
	uint32_t temp = value;

	switch(reg){
		case 1: asm volatile ("wrpgpr $1, %[temp]": : [temp] "r"(temp) );
			break;
		case 2: asm volatile ("wrpgpr $2, %[temp]": : [temp] "r"(temp) );
			break;
		case 3: asm volatile ("wrpgpr $3, %[temp]": : [temp] "r"(temp) );
			break;
		case 4: asm volatile ("wrpgpr $4, %[temp]": : [temp] "r"(temp) );
			break;
		case 5: asm volatile ("wrpgpr $5, %[temp]": : [temp] "r"(temp) );
			break;
		case 6: asm volatile ("wrpgpr $6, %[temp]": : [temp] "r"(temp) );
			break;
		case 7: asm volatile ("wrpgpr $7, %[temp]": : [temp] "r"(temp) );
			break;
		case 8: asm volatile ("wrpgpr $8, %[temp]": : [temp] "r"(temp) );
			break;
		case 9: asm volatile ("wrpgpr $9, %[temp]": : [temp] "r"(temp) );
			break;
		case 10: asm volatile ("wrpgpr $10, %[temp]": : [temp] "r"(temp) );
			break;
		case 11: asm volatile ("wrpgpr $11, %[temp]": : [temp] "r"(temp) );
			break;
		case 12: asm volatile ("wrpgpr $12, %[temp]": : [temp] "r"(temp) );
			break;
		case 13: asm volatile ("wrpgpr $13, %[temp]": : [temp] "r"(temp) );
			break;
		case 14: asm volatile ("wrpgpr $14, %[temp]": : [temp] "r"(temp) );
			break;
		case 15: asm volatile ("wrpgpr $15, %[temp]": : [temp] "r"(temp) );
			break;
		case 16: asm volatile ("wrpgpr $16, %[temp]": : [temp] "r"(temp) );
			break;
		case 17: asm volatile ("wrpgpr $17, %[temp]": : [temp] "r"(temp) );
			break;
		case 18: asm volatile ("wrpgpr $18, %[temp]": : [temp] "r"(temp) );
			break;
		case 19: asm volatile ("wrpgpr $19, %[temp]": : [temp] "r"(temp) );
			break;
		case 20: asm volatile ("wrpgpr $20, %[temp]": : [temp] "r"(temp) );
			break;
		case 21: asm volatile ("wrpgpr $21, %[temp]": : [temp] "r"(temp) );
			break;
		case 22: asm volatile ("wrpgpr $22, %[temp]": : [temp] "r"(temp) );
			break;
		case 23: asm volatile ("wrpgpr $23, %[temp]": : [temp] "r"(temp) );
			break;
		case 24: asm volatile ("wrpgpr $24, %[temp]": : [temp] "r"(temp) );
			break;
		case 25: asm volatile ("wrpgpr $25, %[temp]": : [temp] "r"(temp) );
			break;
		case 26: asm volatile ("wrpgpr $26, %[temp]": : [temp] "r"(temp) );
			break;
		case 27: asm volatile ("wrpgpr $27, %[temp]": : [temp] "r"(temp) );
			break;
		case 28: asm volatile ("wrpgpr $28, %[temp]": : [temp] "r"(temp) );
			break;
		case 29: asm volatile ("wrpgpr $29, %[temp]": : [temp] "r"(temp) );
			break;
		case 30: asm volatile ("wrpgpr $30, %[temp]": : [temp] "r"(temp) );
			break;
		case 31: asm volatile ("wrpgpr $31, %[temp]": : [temp] "r"(temp) );
			break;
		default:
			debugs("Register not found.\n");
			break;
	}
}

/** Read Reg from previous Guest CP0 */
uint32_t MoveFromGuestCP0(uint32_t reg, uint32_t sel){
	uint32_t temp;

	switch(reg){
		case 4:
			asm volatile ("mfgc0 %[temp], $4, 0": [temp] "=r"(temp) :);
			break;
		case 5:
			asm volatile ("mfgc0 %[temp], $5, 0": [temp] "=r"(temp) :);
			break;
		case 6:
			asm volatile ("mfgc0 %[temp], $6, 0": [temp] "=r"(temp) :);
			break;
		case 8:
			asm volatile ("mfgc0 %[temp], $8, 0": [temp] "=r"(temp) :);
			break;
			
		case 9:
			asm volatile ("mfgc0 %[temp], $9, 0": [temp] "=r"(temp) :);
			break;
		case 10:
			switch(sel){
				case 4: asm volatile ("mfgc0 %[temp], $10, 4": [temp] "=r"(temp) :);
					break;
			}
			break;
		case 11:
			switch(sel){
				case 0: asm volatile ("mfgc0 %[temp], $11, 0": [temp] "=r"(temp) :);
					break;
			}
			break;
		case 12: 
			switch(sel){
				case 0: asm volatile ("mfgc0 %[temp], $12, 0": [temp] "=r"(temp) :);
					break;
				case 1: asm volatile ("mfgc0 %[temp], $12, 1": [temp] "=r"(temp) :);
					break;
				case 2: asm volatile ("mfgc0 %[temp], $12, 2": [temp] "=r"(temp) :);
					break;
				case 3: asm volatile ("mfgc0 %[temp], $12, 3": [temp] "=r"(temp) :);
					break;
				case 6: asm volatile ("mfgc0 %[temp], $12, 6": [temp] "=r"(temp) :);
					break;
			}
			break;
		case 13: 
			switch(sel){
				case 0: asm volatile ("mfgc0 %[temp], $13, 0": [temp] "=r"(temp) :);
					break;
			}
			break;
		case 14: 
			switch(sel){
				case 0: asm volatile ("mfgc0 %[temp], $14, 0": [temp] "=r"(temp) :);
					break;
				case 2: asm volatile ("mfgc0 %[temp], $14, 2": [temp] "=r"(temp) :);
					break;					
			}
			break;
		case 15: 
			switch(sel){
				case 1: asm volatile ("mfgc0 %[temp], $15, 1": [temp] "=r"(temp) :);
					break;
			}
			break;
			
		case 16: 
			switch(sel){
				case 0: asm volatile ("mfgc0 %[temp], $16, 0": [temp] "=r"(temp) :);
					break;
				case 3: asm volatile ("mfgc0 %[temp], $16, 3": [temp] "=r"(temp) :);
					break;
					
			}
			break;
		case 17: 
			switch(sel){
				case 0: asm volatile ("mfgc0 %[temp], $17, 0": [temp] "=r"(temp) :);
					break;
			}
			break;
		case 28: 
			switch(sel){
				case 0: asm volatile ("mfgc0 %[temp], $28, 0": [temp] "=r"(temp) :);
					break;
			}
			break;
		case 30: 
			switch(sel){
				case 0: asm volatile ("mfgc0 %[temp], $30, 0": [temp] "=r"(temp) :);
					break;
			}
			break;
		default:
			printf("Register not found. mfgc0 %d\n", reg);
			break;
	}
	return temp;
}

/** Write value to GestCP0 */
void MoveToGuestCP0(uint32_t reg, uint32_t sel, uint32_t value){
	uint32_t temp = value;
	
	switch(reg){
		case 4: 
			switch(sel){
				case 0: asm volatile ("mtgc0 %[temp], $4, 0": : [temp] "r"(temp) );
					break;
			}
			break;
		case 5: 
			switch(sel){
				case 0: asm volatile ("mtgc0 %[temp], $5, 0": : [temp] "r"(temp) );
					break;
			}
			break;
		case 6: 
			switch(sel){
				case 0: asm volatile ("mtgc0 %[temp], $6, 0": : [temp] "r"(temp) );
					break;
			}
			break;
		case 8: 
			switch(sel){
				case 0: asm volatile ("mtgc0 %[temp], $8, 0": : [temp] "r"(temp) );
					break;
			}
			break;
		case 9: 
			switch(sel){
				case 0: asm volatile ("mtgc0 %[temp], $9, 0": : [temp] "r"(temp) );
					break;
			}
			break;
		case 10: 
			switch(sel){
				case 4: asm volatile ("mtgc0 %[temp], $10, 4": : [temp] "r"(temp) );
					break;
			}
			break;
			
		case 11: 
			switch(sel){
				case 0: asm volatile ("mtgc0 %[temp], $11, 0": : [temp] "r"(temp) );
					break;
			}
			break;
		case 12: 
			switch(sel){
				case 0: asm volatile ("mtgc0 %[temp], $12, 0": : [temp] "r"(temp) );
					break;
				case 1: asm volatile ("mtgc0 %[temp], $12, 1": : [temp] "r"(temp) );
					break;
				case 2: asm volatile ("mtgc0 %[temp], $12, 2": : [temp] "r"(temp) );
					break;
				case 3: asm volatile ("mtgc0 %[temp], $12, 3": : [temp] "r"(temp) );
					break;
				case 6: asm volatile ("mtgc0 %[temp], $12, 6": : [temp] "r"(temp) );
					break;
					
			}
			
			break;
		case 13: 
			switch(sel){
				case 0: asm volatile ("mtgc0 %[temp], $13, 0": : [temp] "r"(temp) );
					break;
			}
			break;
		case 14: 
			switch(sel){
				case 0: asm volatile ("mtgc0 %[temp], $14, 0": : [temp] "r"(temp) );
					break;
				case 2: asm volatile ("mtgc0 %[temp], $14, 2": : [temp] "r"(temp) );
					break;
					
			}
			break;
			
		case 15: 
			switch(sel){
				case 1: asm volatile ("mtgc0 %[temp], $15, 1": : [temp] "r"(temp) );
					break;
			}
			break;
		case 16: 
			switch(sel){
				case 0: asm volatile ("mtgc0 %[temp], $16, 0": : [temp] "r"(temp) );
					break;
				case 3: asm volatile ("mtgc0 %[temp], $16, 3": : [temp] "r"(temp) );
					break;
					
			}
			break;
		case 17: 
			switch(sel){
				case 0: asm volatile ("mtgc0 %[temp], $17, 0": : [temp] "r"(temp) );
					break;
			}
			break;
			
		case 28: 
			switch(sel){
				case 0: asm volatile ("mtgc0 %[temp], $28, 0": : [temp] "r"(temp) );
					break;
			}
			break;
		case 30: 
			switch(sel){
				case 0: asm volatile ("mtgc0 %[temp], $30, 0": : [temp] "r"(temp) );
					break;
			}
			break;
		default:
			printf("Register not found mtgc0 %d.\n", reg );
			break;
	}

}

/** Set the Guest Lowest GPR Shadow */
void setGLowestGShadow(uint32_t lowestshadow){
	hal_sr_guestctl3(lowestshadow & 0xF);
}

/** Get the Guest Lowest GPR Shadow */
uint32_t getGLowestGShadow(void){
	return (hal_lr_guestctl3()&0xF);
}

/**Set the Guest previous shadow set**/
void setGuestPreviousShadowSet(uint32_t shadow_set){
	uint32_t srsclt = hal_lr_srsclt();
	srsclt = (srsclt & ~SRSCTL_PSS) | (shadow_set << SRSCLT_PSS_SHIFT);
	hal_sr_srsclt(srsclt);
}

uint32_t getGuestPreviousShadowSet(){
	uint32_t srsclt = hal_lr_srsclt();
	return ((srsclt&SRSCTL_PSS)>>SRSCLT_PSS_SHIFT);
}
/** Write value to Reg on GPR Shadow register*/
uint32_t MoveFromGuestGPR(uint32_t gpr_id, uint32_t reg){
	
	uint32_t value;
	uint32_t shadow_set = getGuestPreviousShadowSet();
	
	setGuestPreviousShadowSet(gpr_id);
	
	value = MoveFromPreviousGuestGPR(reg);
	
	setGuestPreviousShadowSet(shadow_set);
	
	return value;
}

/** Write value to Reg on GPR Shadow register*/
void MoveToGuestGPR(uint32_t gpr_id, uint32_t reg, uint32_t value){
	
	uint32_t shadow_set = getGuestPreviousShadowSet();
	
	setGuestPreviousShadowSet(gpr_id);
	
	MoveToPreviousGuestGPR(reg,value);
	
	setGuestPreviousShadowSet(shadow_set);	
}

/** Returns the root.count */
uint32_t getCounter(void){
	return hal_lr_rcount();
}

/* Set GTOffset*/
void setGTOffset(int32_t gtoffset){
	hal_sr_gtoffset(gtoffset);
}

void setGuestCTL2(uint32_t guestclt2){
	hal_sr_guestctl2(guestclt2);
}

uint32_t getRandom(){
	return hal_lr_random();
}


/*Invoked when there is not ready VCPU to perform.*/
void idlevcpu(){
    while(1){};
}

/* Critical error ocurred. Waiting for reset */
void WaitforReset(){
    while(1){
        if (!(PORTB & (1 << 12))) {
            SoftReset();
        }
    }
}

