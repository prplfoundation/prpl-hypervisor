/**
 * @brief Mutex data structure.
 */
struct mtx {
	int32_t lock;					/*!< mutex lock, atomically modified */
};

typedef volatile struct mtx mutex_t;

void os_mtxinit(mutex_t *m);
void os_mtxlock(mutex_t *m);
void os_mtxunlock(mutex_t *m);
