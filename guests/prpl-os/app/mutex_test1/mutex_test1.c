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

mutex_t m;

int32_t shared;

void task(void){
	int32_t itr = 0;

	while(1){
		os_mtxlock(&m);
		shared = random();
		printf("\nTask %d on critical region.. shared: %d itr: %d", os_selfid(), shared, itr++);
		os_mtxunlock(&m);
		delay_ms(1);			// do not hog the CPU!
	}
}


void app_main(void){
	os_mtxinit(&m);

	os_spawn(task, "task a", 1024);
	os_spawn(task, "task b", 1024);
	os_spawn(task, "task c", 1024);
	os_spawn(task, "task d", 1024);
	os_spawn(task, "task e", 1024);
	os_spawn(task, "task f", 1024);
}
