/*
Copyright (c) 2016, prpl Foundation

Permission to use, copy, modify, and/or distribute this software for any purpose with or without 
fee is hereby granted, provided that the above copyright notice and this permission notice appear 
in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE 
FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

This code was written by Sergio Johann at Embedded System Group (GSE) at PUCRS/Brazil.

*/


#include <os.h>

void show_list(struct list *l)
{
	int32_t i;
	int32_t *p;
	
	printf("\nshowing the list...");
	for (i = 0; i < os_list_count(l); i++){
		p = os_list_get(l, i);
		if (p)
			printf("\nnode %d: %d", i, *p);
		else
			printf("\nnode %d: (null)", i);
	}
}

void task(void)
{
	int32_t i, a, b, c, d;
	struct list *l;
	
	for (;;){
		l = os_list_init();
		printf("\nadding 20 nodes to a list...");
		for (i = 0; i < 20; i++)
			if(os_list_append(l, NULL)) printf("FAIL!");
		printf("\nthe list has %d nodes", os_list_count(l));
		a = 10;
		b = 15;
		c = 2;
		d = 45;
		printf("\nfilling the 4th, 5th and 17th nodes");
		if (os_list_set(l, &a, 3)) printf("FAIL");
		if (os_list_set(l, &b, 4)) printf("FAIL");
		if (os_list_set(l, &c, 16)) printf("FAIL");
		show_list(l);
		printf("\ninserting a node on the 7th position");
		if (os_list_insert(l, &d, 6)) printf("FAIL");
		show_list(l);
		printf("\nremoving the 6th, 10th, 11th and 17th nodes");
		if (os_list_remove(l, 5)) printf("FAIL");
		if (os_list_remove(l, 9)) printf("FAIL");
		if (os_list_remove(l, 10)) printf("FAIL");
		if (os_list_remove(l, 16)) printf("FAIL");
		show_list(l);
		panic(0);
	}
}

void app_main(void)
{
	os_spawn(task, "task", 2048);
}
