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


#include <types.h>
#include <hal.h>
#include <config.h>
#include <kernel.h>
#include <tlb.h>
#include <malloc.h>
#include <libc.h>
#include <vcpu.h>
#include <dispatcher.h>
#include <globals.h>
#include <hypercall.h>
#include <common.h>


static uint32_t counttimerInt = 0;
static uint32_t guestexit = 0;

/** Hardware interrupt handle */
uint32_t InterruptHandler(){
	uint32_t ret;
	
        /*TODO: Only timer interrupt supported. This must be rewrite due to EIC support. */
    
        ret = timer_int_handler();
        
        /* Check for sw1 button pressed*/
        if (!(PORTB & (1 << 12))) {
            SoftReset();
        }

	return ret;
}

/** Handle guest exceptions */
uint32_t GuestExitException(){
	uint32_t guestcause = getGCauseCode();
	uint32_t epc = getEPC();
	uint32_t ret = SUCEEDED;
	
	switch (guestcause) {
	    case 0x0:	
			ret =  InstructionEmulation(epc);
	    	break;
	    case 0x2:
			ret = HypercallHandler();			
			break;
		default:
			break;
	}
	
	/* Determines the next EPC */
	epc = CalcNextPC(epc);	
	curr_vcpu->pc = epc;
	return ret;
		
}

/** Determine the cause and invoke the correct handler */
uint32_t HandleExceptionCause(){
	uint32_t CauseCode = getCauseCode();

	switch (CauseCode){
		/* Interrupt */
	case 	0:	
			return InterruptHandler();
		/* GuestExit */
	case	0x1b:	
			return GuestExitException();
	/* TLB load, store or fetch exception */
	case	0x3:						
	case 	0x2:
			Warning("\nTLB miss: VCPU: %d EPC 0x%x", curr_vcpu->id, getEPC());
			return ERROR;
        
        /*FIXME: The processors is with strange case code after bootloader initialization. 
         The cause is cleaned after the first timer interruption. */
        case    0x1d: return SUCEEDED;
        case    0x15: return SUCEEDED;
	default:
		/* panic */
		Warning("VM will be stopped due to error Cause Code 0x%x, EPC 0x%x, VCPU ID 0x%x", CauseCode, getEPC(), curr_vcpu->id);
		return ERROR;
	}
}

void configureGuestExecution(uint32_t exCause){
	
	uint32_t count;
	uint32_t currentCount;
	uint32_t elapsedTime;
    
	if(exCause == RESCHEDULE || exCause == CHANGE_TO_TARGET_VCPU){
		dispatcher();
    }

	
    contextRestore();
}


/** First routine executed after an exception or after the sucessfull execution of the main() routine.
    @param init	flag to signalize first execution after main();
*/
int32_t initialize_RT_services(int32_t init, uint32_t counter){
	uint32_t ret = 0;
	uint32_t runNextRtInitMachine = 0;
	
	if(!init){
		switch(HandleExceptionCause()){
			case PROGRAM_ENDED:
				//Remove vcpu from vm list
				ll_remove(curr_vm->vcpus.head);				
				free(curr_vcpu);				
				runNextRtInitMachine = 1;			
				break;
			case SUCEEDED:
				runNextRtInitMachine = 0;
				break;								
			case ERROR:
				//PANIC
				Critical("Error on RT services initialization.");
				Critical("Hypervisor execution stopped.");
				WaitforReset();
				break;						
			default:
				break;
		}
	}else{
		setStatusReg(STATUS_EXL);
		runNextRtInitMachine = 1;	
	}
	
	//Check if there are RT machines to be initialized
	if(runNextRtInitMachine){		
		if(rt_services_init_vcpu_list.count>0){
			curr_vcpu = rt_services_init_vcpu_list.head->ptr;
			ll_remove(rt_services_init_vcpu_list.head);
			exceptionHandler_addr = initialize_RT_services;	
		}else{
			//If there`s no RT Init VM remaining			
			runScheduler();		
			exceptionHandler_addr = exceptionHandler;
		}
	}
	
	configureGuestExecution(RESCHEDULE);	

	return ret;
}


int32_t exceptionHandler(int32_t init, uint32_t counter, uint32_t guestcounter){
	uint32_t ret, temp=0;
        
        contextSave(NULL, counter, guestcounter);	
        ret = HandleExceptionCause();
        
        
        
	switch(ret){
		case SUCEEDED:
			break;
		case RESCHEDULE:				
			runScheduler();
			break;
		case CHANGE_TO_TARGET_VCPU:
			curr_vcpu=target_vcpu;
			break;
		case PROGRAM_ENDED:		
			if(remove_vm_and_runBestEffortScheduler()<0){
				//printPerformanceCounters();		
				Warning("The last VM ended!");
				Warning("No more VMs to execute.");
			}
			ret = RESCHEDULE;
			
			break;
        case ERROR:                        
		default:
                        Critical("Critical error ocurred. Hypervisor stopped.");
                        dumpCP0();
                        WaitforReset();
			break;
	}
	
	configureGuestExecution(ret);

	return 0;
}
