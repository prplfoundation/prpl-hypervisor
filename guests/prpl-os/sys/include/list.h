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
