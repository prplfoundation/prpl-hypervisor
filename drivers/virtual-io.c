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
 * @file device_mapping.c
 * 
 * @section DESCRIPTION
 * 
 * This is a hypercall driver implementation to allow for virtualized I/O.
 *
 * A guest must be allowed to access memory address space. Thus, devices can be mapped 
 * to a VMs using the device_mapping proprierty list on the hypervisor's configuration
 * file. Example:
 * 
 * device_mapping = ["PORTH", "UART1"];
 * 
 */

#include <hypercall.h>
#include <globals.h>
#include <driver.h>
#include <hypercall_defines.h>
#include <mips_cp0.h>
#include <hal.h>
#include <scheduler.h>
#include <libc.h>

/**
 * @brief A local array of pointers to the vmconf_t data structure.
 * Used to avoid an excessive number of pointer redirections to read the vmconf data. 
 */
static const struct vmconf_t *vmconf_p[NVMACHINES];


/**
 * @brief Read memory address.
 * 
 * Returns the value of a memory address. The access to such address is only
 * allowed if the space address is allowed for the VM. See device_mapping VM's proprierty.
 * 
 * This hypercall does not provide access error to non-allowed addresses.
 * 
 * Hypercall convention: 
 * 
 * a0: input address.
 * v0: read value. 
*/
void read_address(){
	uint32_t i;
	uint32_t memory_addr = MoveFromPreviousGuestGPR(REG_A0);
	int32_t vcpu_index = vcpu_in_execution->id-1;
	
	/* Check if the address to be read is in the allowed memory space address. */
	for(i = 0; i < vmconf_p[vcpu_index]->devices_mapping_sz; i++){
		if(vmconf_p[vcpu_index]->devices[i].start_addr <= memory_addr &&
			vmconf_p[vcpu_index]->devices[i].start_addr + vmconf_p[vcpu_index]->devices[i].size >= memory_addr){
			MoveToPreviousGuestGPR(REG_V0, *(volatile unsigned*)memory_addr);
			return;
		}
	}
	WARNING("VM %s trying to read a non-allowed memory address.", vm_in_execution->vm_name);
}

/**
 * @brief Write memory address.
 * 
 * Write a value to an specified memory address. The access to such address is only
 * allowed if the space address is allowed for the VM. See device_mapping VM's proprierty.
 * 
 * Hypercall convention: 
 * 
 * a0: input address.
 * a1: value.
 * v0: 0 if successes or HCALL_ADDRESS_NOT_ALLOWED otherwise.
 */
void write_address(){
	uint32_t i;
	uint32_t memory_addr = MoveFromPreviousGuestGPR(REG_A0);
	uint32_t value = MoveFromPreviousGuestGPR(REG_A1);
	int32_t vcpu_index = vcpu_in_execution->id-1;
	
	/* Check if the address to be write is in the allowed memory space address. */
	for(i = 0; i < vmconf_p[vcpu_index]->devices_mapping_sz; i++){
		if(vmconf_p[vcpu_index]->devices[i].start_addr <= memory_addr &&
			vmconf_p[vcpu_index]->devices[i].start_addr + vmconf_p[vcpu_index]->devices[i].size >= memory_addr){
			*(volatile unsigned*)memory_addr = value;
			MoveToPreviousGuestGPR(REG_V0, 0);
			return;
		}
	}
	
	MoveToPreviousGuestGPR(REG_V0, HCALL_ADDRESS_NOT_ALLOWED);
	WARNING("VM %s trying to write a non-allowed memory address: 0x%x.", vm_in_execution->vm_name, memory_addr);
}


/**
 * @brief Driver init call.  Registers the hypercalls. 
 */
void virtual_io_init(){
	uint32_t i;
	vcpu_t *vcpu;
	
	if (register_hypercall(write_address, HCALL_WRITE_ADDRESS) < 0){
		ERROR("Error registering the HCALL_GET_VM_ID hypercall");
		return;
	}
    
	if (register_hypercall(read_address, HCALL_READ_ADDRESS) < 0){
		ERROR("Error registering the HCALL_GET_VM_ID hypercall");
		return;
	}
    
	/* Create a local list of pointers to the vmconf data structure to avoid
	   excessive number of pointer redirections during the hypercall execution. */
	for(i=0;i<NVMACHINES;i++){
		vcpu = get_vcpu_from_id(i+1);
		vmconf_p[i] = vcpu->vm->vmconf;
	}
    
	INFO("Device mapping hypercalls registered.");
}

driver_init(virtual_io_init);

