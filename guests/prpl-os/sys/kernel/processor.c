/**
 * @file processor.c
 * @author Sergio Johann Filho
 * @date March 2016
 * 
 * @section DESCRIPTION
 * 
 * Processor and scheduler management primitives and auxiliary functions.
 * 
 */

#include <os.h>

/**
 * @brief Enables or disables the task scheduler.
 * 
 * @param lock defines the scheduler activation (a value of 1 disables task scheduling).
 */
void os_schedlock(int32_t lock)
{
	if (lock)
		krnl_schedule = 0;
	else
		krnl_schedule = 1;
}

/**
 * @brief Returns the current cpu id number.
 * 
 * @return the current cpu id, defined by the CPU_ID macro.
 */
uint16_t os_cpuid(void)
{
	return CPU_ID;
}

/**
 * @brief Returns the number of processors in the system.
 * 
 * @return the number of cores, defined by the CPU_CORES macro.
 */
uint16_t os_ncores(void)
{
	return CPU_CORES;
}

/**
 * @brief Returns the percentage of free processor time.
 * 
 * @return a value between 0 and 99.
 */
int16_t os_freecpu(void)
{
	return 0;
}

/**
 * @brief Returns the amount of free memory.
 * 
 * @return free heap memory, in bytes.
 */
uint32_t os_freemem(void)
{
	return krnl_free;
}
