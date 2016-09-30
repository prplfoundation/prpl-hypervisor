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
 * @file hal.c
 * 
 * @section DESCRIPTION
 * 
 * Functions for access and configuration of the CP0 and Guest CP0. 
 * Supports the M5150 processor core and initial hypervisor configuration. 
 */

#include <libc.h>
#include <types.h>
#include <hal.h>
#include <globals.h>
#include <mips_cp0.h>

/* HEAP size as calculated by the linker script. */
extern _heap_size;

/* Stringfy compiler parameters. */
#define STR(x) #x
#define STR_VALUE(x) STR(x)


/**
 * @brief Early boot message. 
 * 	Print to the stdout usefull hypervisor information.
 */
static void print_config(void)
{
	printf("\n===========================================================");
	printf("\nprplHypervsior %s [%s, %s]", STR_VALUE(HYPVERSION), __DATE__, __TIME__);
	printf("\nCopyright (c) 2016, prpl Foundation");
	printf("\n===========================================================");
	printf("\nCPU ID:        %s", CPU_ID);
	printf("\nARCH:          %s", CPU_ARCH);
	printf("\nSYSCLK:        %dMHz", CPU_SPEED/1000000);
	printf("\nHeap Size:     %dKbytes", (int)(&_heap_size)/1024);
	printf("\nScheduler      %dms", QUANTUM_SCHEDULER);
	printf("\nVMs:           %d\n", NVMACHINES);
}



/**
 * @brief C code entry. This is the first C code performed during 
 * hypervisor initialization. Called from early boot stage to perform 
 * overall hypervisor configuration. 
 * 
 * The hypervisor should never return form this call. The start_timer()
 * call should configure the system timer and wait by the first 
 * timer interrupt.
 * 
  */
void hyper_init(){
	
	/* Specific board configuration. */
	early_platform_init();    
	
	/* early boot messages with hypervisor configuration. */
	print_config();
	
	/* Processor inicialization */
	if(LowLevelProcInit()){
		CRITICAL("Low level processor initialization error.");
	}
	
	/* Configure the HEAP space on the allocator */ 
	init_mem();
	
	/*Initialize VCPUs and virtual machines*/
	initializeMachines();
	
	/* Initialize device drivers */    
	drivers_initialization();
	
	/* Start system timer. Should not return from this call.
	 *	   This call will wait for the first timer interrupt. */
	start_timer();
	
	/* Should never reach this point !!! */
	CRITICAL("Hypervisor initialization error.");
}


/**
 * @brief Verify if the processor is in root-kernel mode. 
 * @return 1 for root-kernel mode, 0 otherwise.
 */
int32_t isRootMode(){
        int guestctl0 = mfc0(CP0_GUESTCTL0, 6);
        int status = mfc0(CP0_STATUS, 0);
	if( !(guestctl0 & GUESTCTL0_GM) || 
		( (guestctl0 & GUESTCTL0_GM) &&  !( !(mfc0(CP0_DEBUG2, 0) & DEBUG_DM)
		&& !(status & STATUS_ERL) && !(status & STATUS_EXL)))){
		return 1;
	}

	return 0;
}


/**
 * @brief Verify if the processor implements the VZ module . 
 * @return 1 VZ module available, 0 otherwise.
 */
int32_t hasVZ(){
	int32_t config3 = mfc0(CP0_CONFIG3, 3);
	
	if( !(config3 & CONFIG3_VZ) ){
		return 0;
	}
	
	return 1;
}


/**
 * @brief Set the GPR Shadow Bank. 
 *      The hypervisor uses the highest GPR shadow page. The others GPR shadows keep the VM's context.
 * 
 * @return 0 for error or 1 for success.
 */
int32_t ConfigureGPRShadow(){
	int32_t srsclt_reg;
	int32_t guestsrsclt_reg;
	int32_t num_shadow_gprs = 0;

	/* Configure the GPR Shadow. The hypervisor will use the highest shadow page. 
	   Still, the hypervisor needs at least one GPR Shadow.
        */
	srsclt_reg = mfc0(CP0_SRSCTL, 2);
	num_shadow_gprs = (srsclt_reg & SRSCTL_HSS) >> SRSCTL_HSS_SHIFT;
	if(num_shadow_gprs == 0){
		/* panic */
		return 1;
	}
	
	/* Set the ESS field and PSS */
	/* PSS is set to the highest page. The processor will change to the new GPR on the next eret */
        mtc0(CP0_SRSCTL, 2, num_shadow_gprs << SRSCTL_ESS_SHIFT | num_shadow_gprs << SRSCLT_PSS_SHIFT);
	
	/* Set the SRSMap register */
	mtc0(CP0_SRSMAP, 3, num_shadow_gprs << SRSMAP_SSV7_SHIFT |
			num_shadow_gprs << SRSMAP_SSV6_SHIFT |
			num_shadow_gprs << SRSMAP_SSV5_SHIFT |
			num_shadow_gprs << SRSMAP_SSV4_SHIFT |
			num_shadow_gprs << SRSMAP_SSV3_SHIFT |
			num_shadow_gprs << SRSMAP_SSV2_SHIFT |
			num_shadow_gprs << SRSMAP_SSV1_SHIFT |
			num_shadow_gprs << SRSMAP_SSV0_SHIFT );
	
	/* Set the SRSMap2 */
	mtc0(CP0_SRSMAP2, 5, num_shadow_gprs << SRSMAP2_SSV8_SHIFT |
			 num_shadow_gprs << SRSMAP2_SSV9_SHIFT );
			 
    
        /* No virtual shadow registers to guests */
        mtc0(CP0_GUESTCTL3, 6, 0); 
	
	/* Configure $sp and $gp registers in the hypervisor shadow page. 
	   The hypervisor will move to the highest shadow page after the first 
	   interrupt. */
	hal_config_hyper_gpr_shadow();
	
    return 0;
}


/**
 * @brief Low level processor initialization. 
 *      Called once during hypervisor Initialization.
 * 
 * @return 1 for error or 0 for success.
 */
int32_t LowLevelProcInit(){

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
        
	mtc0(CP0_INTCTL, 1, mfc0(CP0_INTCTL, 1) | (INTCTL_VS << INTCTL_VS_SHIFT));
	
	/* Initializing guestCtl0 options
	GUESTCTL0_CP0 Allow guest access to some CP0 registers
	GUESTCTL0_GT Allow guest read acess to count and compare registers
	GUESTCTL0_CF Allow guest to read and write config registers */
	mtc0(CP0_GUESTCTL0, 6, mfc0(CP0_GUESTCTL0, 6) | GUESTCTL0_CP0 | GUESTCTL0_GT | GUESTCTL0_CF |  GUESTCTL0_CG);
	
	mtc0(CP0_GUESTCLT0EXT, 4, mfc0(CP0_GUESTCLT0EXT, 4) | GUESTCTL0EXT_CGI);
	
	/* Disabling Exceptions when guest modifies own CP0 registers */
	mtc0(CP0_GUESTCLT0EXT, 4, mfc0(CP0_GUESTCLT0EXT, 4) | GUESTCTL0EXT_FCD);
	
	if (ConfigureGPRShadow()){
		return 1;
	}
		
        setGTOffset(0);
	
	return 0;
}


/**
 * @brief Extract the execCode field from cause register
 * 
 * @return ExecCode value.
 */
uint32_t getCauseCode(){
	uint32_t execcode = mfc0(CP0_CAUSE, 0);
    
	return (execcode & CAUSE_EXECCODE) >> CAUSE_EXECCODE_SHIFT;
}


/**
 * @brief Extract the hypercall code from hypercall instruction. 
 * 
 * @return Hypercall code.
 */
uint32_t getHypercallCode(){
	uint32_t hypcode = mfc0(CP0_BADVADDR, 1);
	
	return (hypcode & HYPCODE) >> HYPCODE_SHIFT;	
}


/**
 * @brief Checks if the exception happend on a branch delay slot. 
 * 
 * @return 0 for non branch delay slot, otherwise greater than 0.
 */
uint32_t getCauseBD(){
	return mfc0(CP0_CAUSE, 0) & CAUSE_BD;
}


/**
 * @brief Extracts the  Guest ExecCode field from guestctl0 register. 
 * 
 * @return Guest ExecCode.
 */
uint32_t getGCauseCode(){
	uint32_t execcode = mfc0(CP0_GUESTCTL0, 6);
	
	return (execcode & CAUSE_EXECCODE) >> CAUSE_EXECCODE_SHIFT;
}


/**
 * @brief Extracts the interrupt pending bits (IP0:IP9) from cause register.
 * 
 * @return Interrupt pending bits.
 */
uint32_t getInterruptPending(){
	uint32_t pending = mfc0(CP0_CAUSE, 0);
	
	if(((pending>>CAUSE_PCI_SHIFT) & 0x1)==1)
		return PERFORMANCE_COUNTER_INT;
	else	
		return (pending & CAUSE_IP);
}	


/**
 * @brief Set the Compare register to the next timer interruption.
 * 
 */
void confTimer(uint32_t quantum){
	uint32_t count = mfc0(CP0_COUNT, 0);
	
	if(0xFFFFFFFF - count >= quantum){
		count += quantum;
	}else{
		count =  quantum - (0xFFFFFFFF - count);
	}
	
	mtc0(CP0_COMPARE, 0, count);
}


/**
 * @brief Set the IM bits on the status reg.
 * 
 */
void setInterruptMask(uint32_t im){
	mtc0(CP0_STATUS, 0, mfc0 (CP0_STATUS, 0)| im);
}


/**
 * @brief Clear the IM bits on the status reg .
 * 
 */
void clearInterruptMask(uint32_t im){
	mtc0(CP0_STATUS, 0, mfc0(CP0_STATUS, 0) & (~im));
}


/**
 * @brief Enable global interrupt. IE bit in status register.
 * 
 */
void enableIE(){
	mtc0(CP0_STATUS, 0, mfc0(CP0_STATUS, 0) | STATUS_IE);
}


/**
 * @brief Verify if the processor implements GuestID field.
 * 
 * @return 1 for guestid supported, 0 otherwise.
 */
uint32_t hasGuestID(){
	if (mfc0(CP0_GUESTCTL0, 6) & GUESTCTL0_G1){
		return 1;
	}
	return 0;
}


/**
 * @brief Verify if the processor allow direct root mode.
 * 
 * @return 1 for direct root supported, 0 otherwise.
 */
uint32_t isDirectRoot(){
	if (mfc0(CP0_GUESTCTL0, 6) & GUESTCTL0_DRG){
		return 1;
	}
	
	return 0;
}


/**
 * @brief Set CP0 EPC. 
 * 
 */
void setEPC(uint32_t epc){
    mtc0(CP0_EPC, 0, epc);
}


/**
 * @brief Get CP0 EPC. 
 * 
 * @return EPC address.
 */
uint32_t getEPC(){
	return mfc0(CP0_EPC, 0);
}


/**
 * @brief Set root GuestID mode. 
 * 
 */
void setGuestRID(uint32_t guestrid){
	uint32_t reg;
	reg = (mfc0(CP0_GUESTCTL1, 4) & (~GUESTCTL1_RID)) | (guestrid << GUESTCTL1_RID_SHIFT);
	mtc0(CP0_GUESTCTL1, 4, reg);
}


/**
 * @brief Set GuestID. 
 * 
 */
void setGuestID(uint32_t guestid){
	mtc0(CP0_GUESTCTL1, 4, (mfc0(CP0_GUESTCTL1, 4) & (~GUESTCTL1_ID)) | guestid);
}


/**
 * @brief Get GuestID. 
 * 
 */
uint32_t getGuestID(void){
	return (mfc0(CP0_GUESTCTL1, 4) & GUESTCTL1_ID);
}


/**
 * @brief Check if the processor uses root ASID. 
 * 
 * @return 1 for root ASID supported, 0 otherwise
 */
uint32_t isRootASID(){
	if (mfc0(CP0_GUESTCTL0, 6) & GUESTCTL0_RAD){
		return 1;
	}
	
	return 0;
}


/**
 * @brief Set the processor to Guest Mode. 
 * 
 */
void setGuestMode(){
	
	mtc0(CP0_GUESTCTL0, 6, mfc0(CP0_GUESTCTL0, 6) | GUESTCTL0_GM | GUESTCTL0_CP0 | GUESTCTL0_GT | 1 << 12);
	
}

/**
 * @brief Set specific bits on CP0 STATUS reg.. 
 * 
 */
void setStatusReg(uint32_t bits){
	mtc0(CP0_STATUS, 0, mfc0(CP0_STATUS, 0) | bits);
}


/**
 * @brief Check if the processor supports 1k page size. 
 * 
 * @return 0 for not supported, greather than 0 for supported. 
 */
uint32_t has1KPageSupport(){
	return  mfc0(CP0_CONFIG3, 3) & CONFIG3_SP;
}


/**
 * @brief Disable 1K page support to keep compatibility with 4K page size. 
 * 
 */
void Disable1KPageSupport(){
	mtc0(CP0_PAGEGRAIN, 1, mfc0(CP0_PAGEGRAIN, 1) & ~PAGEGRAIN_ESP);
}


/**
 * @brief Check if the processor will enter in guest mode on next eret instruction.
 * @return 1 is entering in guest mode, 0 no entering in guest mode. 
 */
int32_t isEnteringGuestMode(){
	uint32_t status;
	
	status = mfc0(CP0_STATUS, 0);
	
	if ( (mfc0(CP0_GUESTCTL0, 6) & GUESTCTL0_GM) &&
	     !(status & STATUS_ERL) && 
	     (status & STATUS_EXL) &&
	     !(mfc0(CP0_DEBUG2, 0) & DEBUG_DM) ){
		return 1;
	}
	
	return 0;
}


/**
 * @brief Get bad instruction address .
 * @return Bad instruction address. 
 */
uint32_t getBadVAddress(){
	return mfc0(CP0_BADVADDR, 0);
}


/**
 * @brief Read Reg from preview GPR Shadow .
 * @return GPR value. 
 */
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


/**
 * @brief Write value to Reg on preview GPR Shadow.
 * @return GPR value. 
 */
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


/**
 * @brief Set the Lowest GPR Shadow.
 * @return GPR value. 
 */
void setLowestGShadow(uint32_t lowestshadow){
	mtc0(CP0_GUESTCTL3, 6, lowestshadow & 0xF);
}


/**
 * @brief Get the Lowest GPR Shadow.
 * @return Lowest guest GPR shadow. 
 */
uint32_t getLowestGShadow(void){
	return (mfc0(CP0_GUESTCTL3, 6)&0xF);
}


/**
 * @brief Set the previous shadow set.
 */
void setPreviousShadowSet(uint32_t shadow_set){
	uint32_t srsclt = mfc0(CP0_SRSCTL, 2);
	srsclt = (srsclt & ~SRSCTL_PSS) | (shadow_set << SRSCLT_PSS_SHIFT);
	mtc0(CP0_SRSCTL, 2, srsclt);
}

/**
 * @brief Get the previous  GPR Shadow.
 * @return Previous guest GPR shadow. 
 */
uint32_t getPreviousShadowSet(){
	uint32_t srsclt = mfc0(CP0_SRSCTL, 2);
	return ((srsclt&SRSCTL_PSS)>>SRSCLT_PSS_SHIFT);
}


/**
 * @brief Return the CP0 COUNTER.
 * @return CP0 COUNTER. 
 */
uint32_t getCounter(void){
	return mfc0(CP0_COUNT, 0);
}


/**
 * @brief Set CP0 GTOffset .
 */
void setGTOffset(int32_t gtoffset){
    mtc0(CP0_GTOOFFSET, 0, gtoffset);
}


/**
 * @brief Set CP0 GuestCLT2 .
 */
void setGuestCTL2(uint32_t guestclt2){
    mtc0(CP0_GUESTCTL2, 5, guestclt2);
}

/**
 * @brief Get CP0 GuestCLT2 .
 * 
 * @return guestctl2 register value.
 */
uint32_t getGuestCTL2(){
    return mfc0(CP0_GUESTCTL2, 5);
}


/**
 * @brief Get CP0 Random .
 * @return Random value.
 */
uint32_t getRandom(){
    return mfc0(CP0_RANDOM, 0);
}



