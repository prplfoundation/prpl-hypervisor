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
 * @brief Mailbox data structure.
 */
struct mailbox {
	void *msg;					/*!< pointer to a message buffer */
	uint16_t n_waiting_tasks;			/*!< number of waiting tasks in the mailbox */
	uint16_t count;					/*!< number of elements on the mailbox */
	sem_t msend;					/*!< synchronization semaphore for mail send */
	sem_t mrecv;					/*!< synchronization semaphore for mail receive */
};

typedef volatile struct mailbox mail_t;			/*!< mailbox type definition */

void os_mboxinit(mail_t *mbox, uint16_t n_waiting_tasks);
void os_mboxsend(mail_t *mbox, void *msg);
void *os_mboxrecv(mail_t *mbox);
void *os_mboxaccept(mail_t *mbox);
