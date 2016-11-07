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

#define Q_SIZE	10

struct queue *q;
mutex_t m;
	
void sender(void)
{
	int32_t i = 0;
	int8_t *buf;
	
	for(;;){
		if (os_queue_count(q) < Q_SIZE){
			os_mtxlock(&m);
			buf = malloc(sizeof(int8_t) * 100);
			if (buf){
				sprintf(buf, "hello from task %d, counting %d", os_selfid(), i++);
				os_queue_addtail(q, buf);
			}else{
				printf("malloc() failed!\n");
			}
			os_mtxunlock(&m);
		}
//		delay_ms(5);
	}
}

void receiver(void)
{
	int8_t *b;
	
	for(;;){
		if (os_queue_count(q)){
			os_mtxlock(&m);
			b = os_queue_remhead(q);
			if (b){
				printf("task %d -> %s\n", os_selfid(), b);
				free(b);
			}
			os_mtxunlock(&m);
		}
//		delay_ms(20);
	}
}

void log(void)
{
	for(;;){
		printf("queue: %d\n", os_queue_count(q));
		os_yield();
	}
}

void app_main(void){
	os_mtxinit(&m);
	q = os_queue_create(Q_SIZE);

	os_spawn(sender, "sender 1", 1024);
	os_spawn(sender, "sender 2", 1024);
	os_spawn(receiver, "receiver 1", 1024);
	os_spawn(receiver, "receiver 2", 1024);
	os_spawn(receiver,  "receiver 3", 1024);
	os_spawn(log, "log", 512);
}

