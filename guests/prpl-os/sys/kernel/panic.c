#include <os.h>

void panic(int32_t cause)
{
	_di();
	kprintf("\nKERNEL: panic [task %d] - ", krnl_current_task);
	switch(cause){
	case PANIC_ABORTED:		kprintf("execution aborted"); break;
	case PANIC_GPF:			kprintf("general protection fault"); break;
	case PANIC_STACK_OVERFLOW:	kprintf("stack overflow"); break;
	case PANIC_NO_TASKS_LEFT:	kprintf("no more tasks left to dispatch"); break;
	case PANIC_OOM:			kprintf("out of memory"); break;
	case PANIC_NO_TASKS_RUN:	kprintf("no tasks on run queue"); break;
	case PANIC_NO_TASKS_DELAY:	kprintf("no tasks on delay queue"); break;
	case PANIC_UNKNOWN_TASK_STATE:	kprintf("task in unknown state"); break;
	case PANIC_CANT_PLACE_RUN:	kprintf("can't place task on run queue"); break;
	case PANIC_CANT_PLACE_DELAY:	kprintf("can't place task on delay queue"); break;
	case PANIC_CANT_SWAP:		kprintf("can't swap tasks on queue"); break;
	case PANIC_NUTS_SEM:		kprintf("insane semaphore"); break;
	default:			kprintf("unknown error"); break;
	}
	printf(" -> system halted.\n");
	for(;;);
}
