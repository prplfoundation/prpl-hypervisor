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
 * @file mailbox.c
 * @author Sergio Johann Filho
 * @date January 2016
 * 
 * @section DESCRIPTION
 * 
 * Mailbox IPC primitives.
 */

#include <os.h>

/**
 * @brief Initializes a mailbox.
 * 
 * @param mbox is a pointer to a mailbox structure.
 * @param n_waiting_tasks is the number of tasks that will wait for messages on the mailbox.
 */
void os_mboxinit(mail_t *mbox, uint16_t n_waiting_tasks)
{
	uint32_t status;

	status = _di();
	mbox->msg = NULL;
	mbox->n_waiting_tasks = n_waiting_tasks;
	mbox->count = n_waiting_tasks;
	os_seminit(&mbox->msend, 1);
	os_seminit(&mbox->mrecv, 0);
	_ei(status);
}

/**
 * @brief Posts a message on a mailbox.
 * 
 * @param mbox is a pointer to a mailbox structure.
 * @param msg is a pointer to a message buffer.
 * 
 * The number of messages sent depends on the number of waiting tasks in
 * the mailbox.
 */
void os_mboxsend(mail_t *mbox, void *msg)
{
	uint32_t i;

	for(i = 0; i < mbox->n_waiting_tasks; i++){
		os_semwait(&mbox->msend);
		mbox->msg = msg;
		os_sempost(&mbox->mrecv);
	}
}

/**
 * @brief Gets a message from a mailbox.
 * 
 * @param mbox is a pointer to a mailbox structure.
 * 
 * @return pointer to a message buffer.
 * 
 * A message is received and the counter controlling the number of messages
 * left in this mailbox is decremented. If the count reaches zero, it is
 * reinitialized.
 */
void *os_mboxrecv(mail_t *mbox)
{
	void *msg;

	os_semwait(&mbox->mrecv);
	msg = mbox->msg;
	mbox->count--;
	if (mbox->count == 0){
		mbox->msg = NULL;
		mbox->count = mbox->n_waiting_tasks;
	}
	os_sempost(&mbox->msend);

	return msg;
}

/**
 * @brief Tries to gets a message from a mailbox.
 * 
 * @param mbox is a pointer to a mailbox structure.
 * 
 * @return pointer to a message buffer.
 * 
 * This is the non-blocking version of os_mboxrecv();
 */
void *os_mboxaccept(mail_t *mbox)
{
	uint32_t status;
	void *msg;

	status = _di();
	msg = mbox->msg;
	if (msg != NULL){
		mbox->count--;
		if (mbox->count == 0){
			mbox->msg = NULL;
			mbox->count = mbox->n_waiting_tasks;
		}
		os_sempost(&mbox->msend);
	}
	_ei(status);

	return msg;
}
