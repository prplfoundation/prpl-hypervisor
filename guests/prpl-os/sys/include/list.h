/**
 * @brief List data structure.
 */
struct list {
	void *elem;					/*!< pointer to list node data */
	struct list *next;				/*!< pointer to the next list node */
};

struct list *os_list_init(void);
int32_t os_list_append(struct list *lst, void *item);
int32_t os_list_insert(struct list *lst, void *item, int32_t pos);
int32_t os_list_remove(struct list *lst, int32_t pos);
void *os_list_get(struct list *lst, int32_t pos);
int32_t os_list_set(struct list *lst, void *item, int32_t pos);
int32_t os_list_count(struct list *lst);
