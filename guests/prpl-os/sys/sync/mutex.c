/**
 * @file mutex.c
 * @author Sergio Johann Filho
 * @date February 2016
 * 
 * @section DESCRIPTION
 * 
 * Mutex synchronization primitives.
 * 
 */

#include <os.h>

static int32_t tsl(mutex_t *m)
{
	volatile int32_t status, init;
	
	status = _di();
	init = m->lock;
	m->lock = 1;
	_ei(status);

	return init;
}

/**
 * @brief Initializes a mutex, defining its initial value.
 * 
 * @param s is a pointer to a mutex.
 */
void os_mtxinit(mutex_t *m)
{
	m->lock = 0;
}

/**
 * @brief Locks a mutex.
 * 
 * @param s is a pointer to a mutex.
 * 
 * If the mutex is not locked, the calling task continues execution. Otherwise,
 * the task spins.
 */
void os_mtxlock(mutex_t *m)
{
	while (tsl(m) == 1);
}

/**
 * @brief Unlocks a mutex.
 * 
 * @param s is a pointer to a mutex.
 */
void os_mtxunlock(mutex_t *m)
{
	m->lock = 0;
}
