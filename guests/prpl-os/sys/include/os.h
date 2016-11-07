/**
 * @file os.h
 * @author Sergio Johann Filho
 * @date February 2016
 * 
 * @section DESCRIPTION
 * 
 * Default system wide include file and error code definitions.
 * 
 */
/*
include default stuff
*/
#include <hal.h>
#include <libc.h>
#include <kprintf.h>
#include <malloc.h>
#include <queue.h>
#include <list.h>
#include <semaphore.h>
#include <mutex.h>
#include <condvar.h>
#include <mailbox.h>
#include <tcb.h>
#include <panic.h>
#include <scheduler.h>
#include <task.h>
#include <processor.h>
#include <main.h>

/*
error codes
*/
/* generic */
#define	OS_OK			0			/*!< no error */
#define OS_ERROR		-1			/*!< generic error */
/* task errors */
#define OS_INVALID_ID		-100			/*!< invalid task id number */
#define OS_INVALID_PARAMETER	-101			/*!< invalid task parameters */
#define OS_INVALID_STATE	-102			/*!< invalid task state */
#define OS_EXCEED_MAX_NUM	-103			/*!< maximum defined number of system tasks exceeded */
#define OS_OUT_OF_MEMORY	-104			/*!< out of heap memory */
#define OS_INVALID_NAME		-105			/*!< invalid task name / unknown task */


