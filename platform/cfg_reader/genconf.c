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
 * @file genconf.c
 * @author Carlos Moratelli
 * @brief This application generates the config.h include file with the VMs' configuration used during
 * the hypervisor compilation. It makes easier to configure the hypervisor through readably 
 * cfg files (see samples_cfg directory)
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>

#if (LIBCONFIG_VER_MAJOR <= 1 && LIBCONFIG_VER_MINOR < 5)
#define config_setting_lookup config_lookup_from
#endif 

#define STRSZ 128
#define LARGESTR 1024
#define VMS_INFO_FILE "include/vms.info"
#define OUTFILE "include/config.h"
#define INCLUDE_DIR "include"
#define DEBUG_COMMENT "/* Debug UART prints */\n"
#define SYSTEM_COMMENT "/* Hypervisor kernel configuration */\n"
#define VM_MAP_COMMENT "/* VMs mapping */\n"

/* Virtual address for VM's RAM */
#define VMS_RAM_VIRTUAL_BASE_ADDRESS  0x80000000

#ifndef BAIKAL_T1

/* Intermediate Physical address of the first VM on the RAM */
#define VMS_DATA_INTERMEDIATE_BASE_ADDRESS 0x80008000

/* Intermediate Physical address of the first VM on the FLASH */
#define VMS_CODE_INTERMEDIATE_BASE_ADDRESS  0x9D008000

/* Virtual address for VM's FLASH */
#define VMS_CODE_VIRTUAL_BASE_ADDRESS  0x9D000000

/* Number of TLB entries available for use. */
#define TOTAL_TLB_ENTRIES      15

#else

/* Intermediate Physical address of the first VM on the RAM */
#define VMS_DATA_INTERMEDIATE_BASE_ADDRESS 0x80010000

/* Intermediate Physical address of the first VM on the FLASH */
#define VMS_CODE_INTERMEDIATE_BASE_ADDRESS  0x80010000

/* Virtual address for VM's FLASH */
#define VMS_CODE_VIRTUAL_BASE_ADDRESS  0x80000000

/* Number of TLB entries available for use. */
#define TOTAL_TLB_ENTRIES      63

#endif


#define DEBUG
#ifdef DEBUG
#define debug(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
#else
#define debug(...) do{ }while(0)
#endif

/* Struct for translation between string names and its respective values */
struct mem_sizes_def{
	unsigned int mask;
	unsigned int value;
	char name[32];
};



/* Pages mask definition used by the hypervisor */
const struct mem_sizes_def PageSizes[] = {
	{name: "PAGEMASK_4KB",    value: 4096,      mask: 0xFFF},
	{name: "PAGEMASK_16KB",   value: 16384,     mask: 0x3FFF},
	{name: "PAGEMASK_64KB",   value: 65536,     mask: 0xFFFF},
	{name: "PAGEMASK_256KB",  value: 262144,    mask: 0x3FFFF},
	{name: "PAGEMASK_1MB",    value: 1048576,   mask: 0xFFFFF},    
	{name: "PAGEMASK_4MB",    value: 4194304,   mask: 0x3FFFFF},    
	{name: "PAGEMASK_16MB",   value: 16777216,  mask: 0xFFFFFF},    
	{name: "PAGEMASK_256MB",  value: 268435456, mask: 0xFFFFFFF}
};

/* Memory areas definition used in the config file. */
const struct mem_sizes_def MemSizes[] = {
	{name: "MEM_SIZE_4KB",    value: 4096},
	{name: "MEM_SIZE_8KB",    value: 8192},
	{name: "MEM_SIZE_16KB",   value: 16384},
	{name: "MEM_SIZE_32KB",   value: 32768},
	{name: "MEM_SIZE_64KB",   value: 65536},
	{name: "MEM_SIZE_128KB",  value: 131072},    
	{name: "MEM_SIZE_256KB",  value: 262144},    
	{name: "MEM_SIZE_512KB",  value: 524288},    
	{name: "MEM_SIZE_1MB",    value: 1048576}
};  

/**
 * @brief Find a string name on a array of struct mem_sizes_def returning it corresponding size.
 * @param mdef A array of struct mem_sizes_def.
 * @param num_ele Num of elements in the array.
 * @param str_name Name to be find. 
 * @return Corresponding positive value or 0 if the name was not found. 
 */
int get_value_from_str(struct mem_sizes_def *mdef, int num_el, char *str_name){
	int i;
	for(i=0;i<num_el;i++){
		if (!strcmp(mdef[i].name, str_name)){
			return mdef[i].value;
		}
	}
	return 0;
}

/**
 * @brief Write to the configuration file.
 * @param f output file.
 * @param str string containing the output text.
 * @return 0 when sucessfull or EXIT_FAILURE in case of error. 
 */
int write_to_conf_file(FILE *f, char* str){ 
	int size = strlen(str);
	if (fwrite(str, sizeof(char), size, f) != size){
		perror("fwrite(): ");
		return EXIT_FAILURE;
	}
	return 0;
}

/**
 * @brief Generate initial message on top of the config.h file.
 * @param f output file.
 * @param conf_name Input configuration file name.
 * @return 0 when sucessfull or EXIT_FAILURE in case of error. 
 */
int initial_msg(FILE *f, char * conf_name){
	if (write_to_conf_file(f, "/* THIS FILE IS AUTOMATICALLY GENERATED. DO NOT MODIFY IT. */\n"))
		return EXIT_FAILURE;
    
	if(write_to_conf_file(f, "/* See the input xml in "))
		return EXIT_FAILURE;
    
	if (write_to_conf_file(f, conf_name))
		return EXIT_FAILURE;
    
	if (write_to_conf_file(f, " */\n\n"))
		return EXIT_FAILURE;
    
	if (write_to_conf_file(f, "#ifndef __CONFIG_H\n#define __CONFIG_H\n\n"))
		return EXIT_FAILURE;
    
	if (write_to_conf_file(f, "#include <vm.h>\n\n"))
		return EXIT_FAILURE;

	if (write_to_conf_file(f, "#include <arch.h>\n\n"))
		return EXIT_FAILURE;
    
	return 0;
}

/**
 * @brief Concatenate a list of strings
 * @param dest Destination string.
 * @param size Size of the destination string.
 * @param ... Variable number of strings .
 * @return Pointer to the destination string. 
 */
char* strings_cat(char *dest, int size, ...){
	va_list arguments;
	char *c;
	int total = 0;
    
	memset(dest, 0, size);
    
	va_start(arguments, size); 
    
	while( (c = va_arg(arguments, char*)) ){
		total+=strlen(c);
		if (total > size)
			break;
		strcat(dest, c);
	}
    
	va_end(arguments);
    
	return dest;
}


/**
 * @brief Insert blank line in the ouput file.
 * @param f output file.
 * @return 0 if sucessfull or EXIT_FAILURE in case of error. 
 */
int insert_blank_line(FILE *f){
	if ( write_to_conf_file(f, "\n")) {
		return EXIT_FAILURE;
	}
	return 0;
}

/**
 * @brief Generate general system configuration 
 * @param cfg Input configuration.
 * @param outfile Output file
 * @return 0 if sucessfull or EXIT_FAILURE in case of error. 
 */
int gen_system_configuration(config_t cfg, FILE* outfile){
	config_setting_t *setting;
	int num_el;
	int ret;
	char str[STRSZ];
	char auxstr[STRSZ];
	const char *auxstrp;
	int value;
    
	/* get debug flags . */
	setting = config_lookup(&cfg, "system.debug");
	if(setting){
		num_el = config_setting_length(setting);
		int i;
        
		if ( (ret = write_to_conf_file(outfile, DEBUG_COMMENT)) ) {
			return ret;
		}
        
		for(i = 0; i < num_el; ++i){
			const char* debug_flag = config_setting_get_string_elem(setting, i);
			strings_cat(str, STRSZ, "#define ", debug_flag, "\n", NULL);
			if ( (ret = write_to_conf_file(outfile, str)) ) {
				return ret;
			}
		}
	}
    
	if ( (ret = insert_blank_line(outfile)) ){
		return ret;
	}
    
	/* Insert system configuration comment */
	if ( (ret = write_to_conf_file(outfile, SYSTEM_COMMENT)) ) {
		return ret;
	}
	
	/* MILISECOND */
	strings_cat(str, STRSZ, "#define MILISECOND ", "(", "(CPU_FREQ/2)", "/ 1000)", "\n", NULL);
	if ( (ret = write_to_conf_file(outfile, str)) ) {
		return ret;
	}
	
	/* UART speed */
	if (config_lookup_int(&cfg, "system.uart_speed", &value)){
		snprintf(auxstr, STRSZ, "%d", value);
		strings_cat(str, STRSZ, "#define UART_SPEED ", auxstr, "\n", NULL);
		if ( (ret = write_to_conf_file(outfile, str)) ) {
			return ret;
		}
	}
       
	/* scheduler_quantum_ms  */
	if (config_lookup_int(&cfg, "system.scheduler_quantum_ms", &value)){
		if(value<3){
			printf("Minimal scheduler_quantum_ms is 3.\n\n");
			return -1;
		}
		snprintf(auxstr, STRSZ, "%d", value);
		strings_cat(str, STRSZ, "#define QUANTUM_SCHEDULER_MS ", auxstr, " \n", NULL);
		if ( (ret = write_to_conf_file(outfile, str)) ) {
			return ret;
		}
	}
	
	/* system_tick_us  */
	if (config_lookup_int(&cfg, "system.guest_quantum_ms", &value)){
		snprintf(auxstr, STRSZ, "%d", value);
		strings_cat(str, STRSZ, "#define GUEST_QUANTUM_MS ", auxstr, " \n", NULL);
		if ( (ret = write_to_conf_file(outfile, str)) ) {
			return ret;
		}
	}
	
       
	if ( (ret = insert_blank_line(outfile)) ){
		return ret;
	}
    
	return 0;
}

/**
 * @brief Find a TLB mask that fits the memory mapping size. 
 * @param mem_size Memory mapping size in bytes.
 * @param page_size Array that contains all TLB possible mask sizes. 
 * @param num_el Number of elements in the array. 
 * @param dual_entry Indicates if a dual TLB entry must be used. 
 * @return A pointer to the element in the array or NULL if no match.  
 */
const struct mem_sizes_def* select_page_mask(int mem_size, const struct mem_sizes_def *page_sizes, int num_el, int *dual_entry){
	int i;
	int half_sz = mem_size/2;
    
	*dual_entry = 0;
    
	/* check if the memory mapping size fits a dual tlb entry  */
	for(i=0; i<num_el; i++){
		if(page_sizes[i].value == half_sz){
			*dual_entry = 1;
			return &page_sizes[i];
		}
	}
    
	/* dual entry can't be used  */
	for(i=0; i<num_el; i++){
		if(page_sizes[i].value == mem_size){
			return &page_sizes[i];
		}
	}
    
	return NULL;
}

/**
 * @brief Convert Kseg0 addresses to physical addresses in TLB format (last 12 bits shifted).  
 * @param addr Kseg0 address
 * @return Physical address in TLB format
 */
unsigned int kseg0_addr_to_physical(unsigned int addr){
	return (addr & 0x1FFFFFFF) >> 12;
}

/**
 * @brief Calculate page alignment based on page size.
 * @param page_size Page size to be used.
 * @param mem_base Memory base
 * @return Intermediate memory address where the mapping must take place. 
 */

int calculate_mem_base(const struct mem_sizes_def* page_size, unsigned int mem_base){
	if (mem_base % page_size->value){
		return (mem_base + page_size->value) & (~page_size->mask);
	}
	return mem_base;
}

/**
 * @brief Process a TLB entry writing the output file.
 * @param vm_number Used as TLB ID. 
 * @param mem_size Size of the mapped area.
 * @param mem_base Intermediate physical address where the mapped area starts. 
 * @param va Virtual address to be mapped.  
 * @return 0 if sucessfull or error code in case of fail. 
 */
int process_tlb_entry(int vm_number, 
                      unsigned int mem_size, 
                      unsigned int *mem_base, 
                      unsigned int va, 
		      unsigned char *coherency,
                      FILE* outfile){
    
	char str[STRSZ];
	char straux[STRSZ];
	const struct mem_sizes_def* page_size;
	int dual_entry, ret;
    
	if ( (ret = write_to_conf_file(outfile, "\t\t\t{\n")) ) {
		return ret;
	}

	if ( (page_size = select_page_mask(mem_size, (const struct mem_sizes_def *)&PageSizes, sizeof(PageSizes)/sizeof(struct mem_sizes_def), &dual_entry)) == NULL){
		fprintf(stderr, "The memory size 0x%x does not fit any TLB entry size.\n", mem_size);
		return EXIT_FAILURE;
	}
    
	*mem_base = calculate_mem_base(page_size, *mem_base);
    
	/* Write intermediate physical addresses entrylo0 and entrylo1 */
	if(dual_entry){
		sprintf(straux, "0x%05x", kseg0_addr_to_physical(*mem_base));
		strings_cat(str, STRSZ, "\t\t\t\tentrylo0: ", straux, ",\n", NULL);
		if ( (ret = write_to_conf_file(outfile, str)) ) {
			return ret;
		}

		sprintf(straux, "0x%05x", kseg0_addr_to_physical(*mem_base+(mem_size/2)));
		strings_cat(str, STRSZ, "\t\t\t\tentrylo1: ", straux, ",\n", NULL);        
		if ( (ret = write_to_conf_file(outfile, str)) ) {
			return ret;
		}
	}else {
		sprintf(straux, "0x%05x", kseg0_addr_to_physical(*mem_base));
		strings_cat(str, STRSZ, "\t\t\t\tentrylo0: ", straux, ",\n", NULL);
		if ( (ret = write_to_conf_file(outfile, str)) ) {
			return ret;
		}
        
		if ( (ret = write_to_conf_file(outfile, "\t\t\t\tentrylo1: 0,\n")) ) {
			return ret;
		}	
	}

	/* pagemask */
	snprintf(straux, STRSZ, "%s", page_size->name);
	strings_cat(str, STRSZ, "\t\t\t\tpagemask: ", straux, ",\n", NULL);
	if ( (ret = write_to_conf_file(outfile, str)) ) {
		return ret;
	}
    
	/* Virtual address */
	sprintf(straux, "0x%05x", kseg0_addr_to_physical(va));        
	strings_cat(str, STRSZ, "\t\t\t\tentryhi: ", straux, ",\n", NULL);
	if ( (ret = write_to_conf_file(outfile, str)) ) {
		return ret;
	}

	strings_cat(str, STRSZ, "\t\t\t\tcoherency: ", coherency, "\n", NULL);
	if ( (ret = write_to_conf_file(outfile, str))) {
		return ret;
	}
    
	/* Close the TLB entry group. */
	if ( (ret = write_to_conf_file(outfile, "\t\t\t},\n"))) {
		return ret;
	}
    
	return 0;
}

/**
 * @brief Generate the output configuration for the VMs mapping.
 * @param cfg libconfig input.
 * @param outfile Output config file. 
 * @param app_list Returns the list of VMs.
 * @param vm_count Number of virtual machines.
 * @return 0 if sucessfull or EXIT_FAILURE in case of error. 
 */
int gen_conf_vms(config_t cfg, FILE* outfile, char *app_list, int* vm_count, char* vms_info){
	int vm_number = 1;
	unsigned int vm_data_inter_addr = VMS_DATA_INTERMEDIATE_BASE_ADDRESS;
	unsigned int vm_code_inter_addr = VMS_CODE_INTERMEDIATE_BASE_ADDRESS;
	int i, num_el, ret, aux, ram_size, flash_size, j, num_mm;
	unsigned int value;
	char auxstr[STRSZ], str[STRSZ], app_name[STRSZ];
	const char *auxstrp;
	config_setting_t *setting;
	int total_tlb_entries = 0;
	unsigned int priority = 0;
    
	/* make sure app_list and vm_info are an empty str */
	strcpy(app_list, "");
    
	/* Generates a list of VM's flash and RAM sizes */
#ifndef BAIKAL_T1	
	strcpy(vms_info, "VM name \tflash_size \tram_size \t address_start\n");
#else
	strcpy(vms_info, "VM name \tcode_size \tram_size \t address_start\n");
#endif	
    
	/* Write comment */
	if ( (ret = write_to_conf_file(outfile, VM_MAP_COMMENT)) ) {
		return ret;
	}

	setting = config_lookup(&cfg, "virtual_machines");
	if(!setting){
		fprintf(stderr, "Missing virtual_machines group configuration on the input file.\n");
		return EXIT_FAILURE;
	}
    
	if ( (ret = write_to_conf_file(outfile, "static const struct vmconf_t const VMCONF[] = {\n")) ) {
		return ret;
	}
    
	num_el = config_setting_length(setting);
	*vm_count = num_el;
	for (i=0;i<num_el;i++){
        
		snprintf(str, STRSZ, "\t{ /* VM#%d */\n", vm_number);
		if ( (ret = write_to_conf_file(outfile, str)) ) {
		return ret;
		}
        
		config_setting_t *vm_conf = config_setting_get_elem(setting, i);
        
		/* get app_name */
		if( !config_setting_lookup_string(vm_conf, "app_name", &auxstrp)){
			fprintf(stderr, "Missing app_name proprierty on virtual_machines group.\n");
			return EXIT_FAILURE;
		}
		strncat(app_list, auxstrp, STRSZ);
		strncat(app_list, " ", STRSZ);
		strncpy(app_name, auxstrp, STRSZ);
	
		strings_cat(str, STRSZ, "\t\tvm_name: \"", app_name, "\",\n", NULL);
		if ( (ret = write_to_conf_file(outfile, str)) ) {
			return ret;
		}
        
		/* get OS type  */
		if( !config_setting_lookup_string(vm_conf, "os_type", &auxstrp)){
			fprintf(stderr, "Missing os_type proprierty on virtual_machines group.\n");
			return EXIT_FAILURE;
		}	
		strings_cat(str, STRSZ, "\t\tos_type: ", auxstrp, ",\n", NULL);
		if ( (ret = write_to_conf_file(outfile, str)) ) {
			return ret;
		}
			
		/* priority  */
		if (!config_setting_lookup_int(vm_conf, "priority", &priority)){
			fprintf(stderr, "Missing priority proprierty on virtual_machines group. Using default: 100.\n");
			priority = 100;
		}else{
			if (priority > 255 || priority < 0){
				fprintf(stderr, "Priority value %d is not valid. Using default: 100.\n", priority);
				priority = 100;
			}
		}
		snprintf(auxstr, STRSZ, "%d", priority);
		strings_cat(str, STRSZ, "\t\tpriority: ", auxstr, ",\n", NULL);
		if ( (ret = write_to_conf_file(outfile, str)) ) {
			return ret;
		}
        
		/* Device Mapping array  */
		config_setting_t * device_mapping_setting = config_setting_lookup(vm_conf, "device_mapping");
		if(device_mapping_setting){
			int int_sz = config_setting_length(device_mapping_setting);
			int i;
			/* get fast_int_sz */
			snprintf(str, STRSZ, "\t\tdevices_mapping_sz: %d,\n", int_sz);
			if ( (ret = write_to_conf_file(outfile, str)) ) {
				return ret;
			}

			if ( (ret = write_to_conf_file(outfile, "\t\tdevices: (const struct device_mapping_t const []) {\n")) ) {
				return ret;
			}

			for(i = 0; i < int_sz; ++i){
				if ( (ret = write_to_conf_file(outfile, "\t\t\t{\n")) ) {
					return ret;
				}
					
				const char* device_name = config_setting_get_string_elem(device_mapping_setting, i);
				
				strings_cat(str, STRSZ, "\t\t\t\tstart_addr: ", device_name, "_BASE,\n", NULL);
				if ( (ret = write_to_conf_file(outfile, (char*)str)) ) {
					return ret;
				}
				
				strings_cat(str, STRSZ, "\t\t\t\tsize: ", device_name, "_SIZE,\n", NULL);
				if ( (ret = write_to_conf_file(outfile, (char*)str)) ) {
					return ret;
				}
				
				if ( (ret = write_to_conf_file(outfile, "\t\t\t},\n")) ) {
					return ret;
				}
				
				
			}

			if ( (ret = write_to_conf_file(outfile, "\t\t},\n")) ) {
				return ret;
			}
		}
		
		/* fast_interrupts array  */
		config_setting_t * fast_int_setting = config_setting_lookup(vm_conf, "fast_interrupts");
		if(fast_int_setting){
			int int_sz = config_setting_length(fast_int_setting);
			int i;
			/* get fast_int_sz */
			snprintf(str, STRSZ, "\t\tfast_int_sz: %d,\n", int_sz);
			if ( (ret = write_to_conf_file(outfile, str)) ) {
				return ret;
			}
			
			if ( (ret = write_to_conf_file(outfile, "\t\tfast_interrupts: (uint32_t []) {")) ) {
				return ret;
			}
			
			for(i = 0; i < int_sz; ++i){
				if(i>0){
					if ( (ret = write_to_conf_file(outfile, ", ")) ) {
						return ret;
					}
				}
				const char* fast_int = config_setting_get_string_elem(fast_int_setting, i);
				if ( (ret = write_to_conf_file(outfile, (char*)fast_int)) ) {
					return ret;
				}
			}
			
			if ( (ret = write_to_conf_file(outfile, " },\n")) ) {
				return ret;
			}
		}
		
		/* interrupt_redirect array  */
		config_setting_t * int_redirect_setting = config_setting_lookup(vm_conf, "interrupt_redirect");
		if(int_redirect_setting){
			int int_red_sz = config_setting_length(int_redirect_setting);
			int i;
			/* get fast_int_sz */
			snprintf(str, STRSZ, "\t\tinterrupt_redirect_sz: %d,\n", int_red_sz);
			if ( (ret = write_to_conf_file(outfile, str)) ) {
				return ret;
			}
			
			if ( (ret = write_to_conf_file(outfile, "\t\tinterrupt_redirect: (uint32_t []) {")) ) {
				return ret;
			}
			
			for(i = 0; i < int_red_sz; ++i){
				if(i>0){
					if ( (ret = write_to_conf_file(outfile, ", ")) ) {
						return ret;
					}
				}
				const char* int_red = config_setting_get_string_elem(int_redirect_setting, i);
				if ( (ret = write_to_conf_file(outfile, (char*)int_red)) ) {
					return ret;
				}
			}
			
			if ( (ret = write_to_conf_file(outfile, " },\n")) ) {
				return ret;
			}
		}
		
		/* write num of tlb entries */
		config_setting_t *mem_maps = config_setting_lookup(vm_conf, "memory_maps");
		aux = mem_maps? config_setting_length(mem_maps) : 0;
#ifndef BAIKAL_T1		
		/* RAM and FLASH mapping requires 2 additional TLB entries 
		 * for code on flash and data on RAM.
		 */
		aux += 2;
#else
		/* Data and code running on RAM, only one TLB entry required. */
		aux += 1;
#endif
		snprintf(auxstr, STRSZ, "\t\tnum_tlb_entries: 0x%x,\n", aux);
		if ( (ret = write_to_conf_file(outfile, auxstr)) ) {
			return ret;
		}
        
		/* The current hypervisor implementation uses static tlb configuration.	
		Only 15 TLB entries are available, 1 is reserved for interVM communication. 
		Stop compilation if more then 15 TLB entries are used. */
		total_tlb_entries += aux;
		if(total_tlb_entries > TOTAL_TLB_ENTRIES){
			fprintf(stderr, "You are using more than %d TLB entries.\n", TOTAL_TLB_ENTRIES);
			return EXIT_FAILURE;
		}
		
		/* Generate the TLB entries to the current VM's configuration */
		if ( (ret = write_to_conf_file(outfile, "\t\ttlb: (const struct tlb_entries const []){\n")) ) {
			return ret;
		}
        
		/* get RAM size */
		if( !config_setting_lookup_string(vm_conf, "RAM_size_bytes", &auxstrp)){
			fprintf(stderr, "Missing RAM_size_bytes proprierty on virtual_machines group.\n");
			return EXIT_FAILURE;
		}
		
		if ( (ram_size = get_value_from_str((struct mem_sizes_def*)MemSizes, sizeof(MemSizes)/sizeof(struct mem_sizes_def), (char*)auxstrp)) == 0){
			fprintf(stderr, "Invalide value for RAM_size_bytes: %s.\n", auxstrp);
			return EXIT_FAILURE;
		}
        
		/* Create a TLB entry to the RAM memory */
		if ((ret = process_tlb_entry(vm_number, ram_size, &vm_data_inter_addr, VMS_RAM_VIRTUAL_BASE_ADDRESS, "WRITE_BACK", outfile))){
			return ret;
		}
		
		/* get FLASH size */
		if( !config_setting_lookup_string(vm_conf, "flash_size_bytes", &auxstrp)){
#ifndef BAIKAL_T1			
			fprintf(stderr, "Missing flash_size_bytes proprierty on virtual_machines group.\n");
			return EXIT_FAILURE;
#endif			
		}else{
			if ( (flash_size = get_value_from_str((struct mem_sizes_def*)MemSizes, sizeof(MemSizes)/sizeof(struct mem_sizes_def), (char*)auxstrp)) == 0){
				fprintf(stderr, "Invalide value for flash_size_bytes: %s.\n", auxstrp);
				return EXIT_FAILURE;
			}
		
			/* Create a TLB entry to the FLASH memory */
			if ((ret = process_tlb_entry(vm_number, flash_size, &vm_code_inter_addr, VMS_CODE_VIRTUAL_BASE_ADDRESS, "WRITE_BACK", outfile))){
				return ret;
			}
		}
		
		/* process the additional memory mapping */
		mem_maps = config_setting_lookup(vm_conf, "memory_maps");
		num_mm = mem_maps? config_setting_length(mem_maps) : 0;
		for(j=0; j<num_mm; j++){
			unsigned int page_size;
			unsigned int base_addr;
			config_setting_t *mm = config_setting_get_elem(mem_maps, j);

			/* get base addr */
			if( !config_setting_lookup_int(mm, "base_addr", (int*)&base_addr)){
				fprintf(stderr, "Missing base_addr proprierty on virtual_machines.memory_maps group.\n");
				return EXIT_FAILURE;
			}
            
			/* get page size */
			if( !config_setting_lookup_string(mm, "page_size", &auxstrp)){
				fprintf(stderr, "Missing page_size proprierty on virtual_machines.memory_maps group.\n");
				return EXIT_FAILURE;
			}
			if ( (page_size = get_value_from_str((struct mem_sizes_def*)MemSizes, sizeof(MemSizes)/sizeof(struct mem_sizes_def), (char*)auxstrp)) == 0){
				fprintf(stderr, "Invalide value for flash_size_bytes: %s.\n", auxstrp);
				return EXIT_FAILURE;
			}
			
			/* get cache coherency */
			if( !config_setting_lookup_string(vm_conf, "coherency", &auxstrp)){
				fprintf(stderr, "Missing cache coherency property. Setting default to uncached.\n");
				strcpy(auxstr, "UNCACHED");
			}else{
				strcpy(auxstr, auxstrp);
			}
            
			/* Create a TLB entry to this mapping */
			if ((ret = process_tlb_entry(vm_number, page_size, &base_addr, base_addr, auxstr, outfile))){
				return ret;
			}
            
		}
        
		/* Close the TLB array group  */
		if ( (ret = write_to_conf_file(outfile, "\t\t},\n")) ) {
			return ret;
		}
		
		/* write the a ddress where the VM is in the RAM as seeing by the hypervisor (physical intermediate address) */
		snprintf(auxstr, STRSZ, "\t\tram_base: 0x%x\n", vm_data_inter_addr);
		if ( (ret = write_to_conf_file(outfile, auxstr)) ) {
			return ret;
		}
		
#ifndef BAIKAL_T1        
		snprintf(auxstr, STRSZ, "%d \t%d \t0x%x\n", flash_size, ram_size, vm_code_inter_addr);
#else
		snprintf(auxstr, STRSZ, "%d \t%d \t0x%x\n", ram_size, ram_size, vm_code_inter_addr);
#endif		
		strings_cat(str, STRSZ, app_name, " \t", auxstr, NULL);
		strcat(vms_info, str);
		
		/* Increment to the intermediate address of the next VM */
		vm_data_inter_addr += ram_size;
		
		/* Increment to the intermediate address of the next VM */
#ifndef BAIKAL_T1		
		vm_code_inter_addr += flash_size;
#else
		vm_code_inter_addr += ram_size;
#endif		
        
		vm_number++;
        
		/* Close the VM group  */
		if ( (ret = write_to_conf_file(outfile, "\t},\n")) ) {
			return ret;
		}
		
	}

	/* Close VM's conf. */
	if ( (ret = write_to_conf_file(outfile, "};\n")) ) {
		return ret;
	}
    
	return 0;
}


/**
 * @brief Write the number of VMs to the config file. 
 * @param vm_count Number of VMs.
 * @param app_list Name of the VMs.
 * @param outfile Output config file. 
 * @return 0 if sucessfull or EXIT_FAILURE in case of error. 
 */
int write_vm_number(int vm_count, char* app_list, FILE* outfile){
	char str[STRSZ];
	char straux[STRSZ];
	int ret;
    
	if ( (ret = insert_blank_line(outfile)) ){
		return ret;
	}
    
	strings_cat(str, STRSZ, "/* Virtual Machine names: ", app_list, " */\n", NULL);
	if ( (ret = write_to_conf_file(outfile, str)) ) {
		return ret;
	}
    
	snprintf(straux, STRSZ, "%d", vm_count);
	strings_cat(str, STRSZ, "#define NVMACHINES ", straux, "\n", NULL);
	if ( (ret = write_to_conf_file(outfile, str)) ) {
		return ret;
	}
    
	return 0;
}

/**
 * @brief Write empty RT-VM list.
 * @param outfile Output config file. 
 * @return 0 if sucessfull or EXIT_FAILURE in case of error. 
 */
int rt_vm_list(FILE* outfile){
	int ret;
    
	if ( (ret = insert_blank_line(outfile)) ){
		return ret;
	}
    
	if ( (ret = write_to_conf_file(outfile, "#endif\n\n")) ) {
		return ret;
	}
    
	return 0;
}

/**
 * @brief Write a str to a file
 * @param str Input string.
 * @param fname Ouput file name
 * @return 0 if sucessfull or EXIT_FAILURE in case of error. 
 */

int write_str_to_file(char* str, char*fname){
	FILE*f;
	int size;
    
	if( (f=fopen(fname, "w")) == NULL){
		perror("fopen: ");
		return EXIT_FAILURE;
	}
    
	size = strlen(str);
	if (fwrite(str, sizeof(char), size, f) != size){
		perror("fwrite: ");
		return EXIT_FAILURE;
	}
    
	fclose(f);
    
	return 0;
}

int main(int argc, char **argv)
{
	config_t cfg;
	FILE* outfile;
	char app_list[STRSZ];
	int vm_count;
	char* vms_info;
	struct stat st = {0};
    
	if (argc<2){
		fprintf(stderr, "Usage: %s <config file path>\n", argv[0]);
		return EXIT_FAILURE;
	}
        
	config_init(&cfg);
    
	/* Read the file. If there is an error, report it and exit. */
	if(! config_read_file(&cfg, argv[1]))
	{
		fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
			config_error_line(&cfg), config_error_text(&cfg));
		config_destroy(&cfg);
		return(EXIT_FAILURE);
	}
    
	if (stat(INCLUDE_DIR, &st) == -1) {
		mkdir(INCLUDE_DIR, 0700);
	}
	
	/* output file (config.h) */
	if(NULL == (outfile = fopen(OUTFILE, "w"))){
		fprintf(stderr, "Error creating %s file.", OUTFILE);
		config_destroy(&cfg);
		return(EXIT_FAILURE);
	}
    
	if (initial_msg(outfile, argv[1])){
		config_destroy(&cfg);
		fclose(outfile);
		return(EXIT_FAILURE);
	}
    
	if (gen_system_configuration(cfg, outfile)){
		config_destroy(&cfg);
		fclose(outfile);
		return(EXIT_FAILURE);
	}
	
	vms_info = (char*)malloc(LARGESTR);
	if (gen_conf_vms(cfg, outfile, app_list, &vm_count, vms_info)){
		config_destroy(&cfg);
		fclose(outfile);
		return(EXIT_FAILURE);
	}

	if(write_str_to_file(vms_info, VMS_INFO_FILE)){
		config_destroy(&cfg);
		fclose(outfile);
		return(EXIT_FAILURE);
	}
    
	free(vms_info);
    
	if (write_vm_number(vm_count, app_list, outfile)){  
		config_destroy(&cfg);
		fclose(outfile);
		return(EXIT_FAILURE);
	}

	if (rt_vm_list(outfile)){  
		config_destroy(&cfg);
		fclose(outfile);
		return(EXIT_FAILURE);
	}
    
	printf("%s\n", app_list);
    
	fclose(outfile);
	config_destroy(&cfg);
	return(EXIT_SUCCESS);
}


