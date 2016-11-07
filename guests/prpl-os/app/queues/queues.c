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

void show_queue(struct queue *q)
{
	int32_t i;
	int32_t *p;
	
	printf("\nshowing the queue...");
	for (i = 0; i < os_queue_count(q); i++){
		p = os_queue_get(q, i);
		if (p)
			printf("\nnode %d: %d", i, *p);
		else
			printf("\nnode %d: (null)", i);
	}
}

void task(void)
{
	int32_t i, a, b, c, d;
	struct queue *q;
	
	for (;;){
		q = os_queue_create(20);
		a = 10;
		b = 15;
		c = 2;
		d = 45;
		printf("\nadding 4 elements to the queue");
		if (os_queue_addtail(q, &a)) printf("FAIL");
		if (os_queue_addtail(q, &b)) printf("FAIL");
		if (os_queue_addtail(q, &c)) printf("FAIL");
		if (os_queue_addtail(q, &d)) printf("FAIL");
		show_queue(q);
		printf("\nremoving the 3rd node");
		if (!os_queue_rem(q, 2)) printf("FAIL");
		show_queue(q);
		panic(0);
	}
}

void app_main(void)
{
	os_spawn(task, "task", 2048);
}

