/**
 * @brief Semaphore data structure.
 */
struct sem {
	struct queue *sem_queue;			/*!< queue for tasks waiting on the semaphore */
	int32_t count;					/*!< semaphore counter */
};

typedef volatile struct sem sem_t;

int32_t os_seminit(sem_t *s, int32_t value);
int32_t os_semdestroy(sem_t *s);
void os_semwait(sem_t *s);
void os_sempost(sem_t *s);
