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
