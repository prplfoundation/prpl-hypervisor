/*
 * Copyright (c) 2016, prpl Foundation
 * 
 * Permission to use, copy, modify, and/or distribute this software for any purpose with or without 
 * fee is hereby granted, provided that the above copyright notice and this permission notice appear 
 * in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE 
 * FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * This code was written by Sergio Johann at Embedded System Group (GSE) at PUCRS/Brazil.
 * 
 */


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


