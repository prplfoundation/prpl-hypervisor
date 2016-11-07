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

sem_t s1, s2, s3, s4;

void task0(void){
	int i;

	for(;;){
		for(i = 1; i <= 200; i++)
			printf("%d ", i);
		os_sempost(&s1);
		for(;;);
	}
}

void task1(void){
	int i;

	for(;;){
		os_semwait(&s1);
		for(i = 200; i <= 400; i++)
			printf("%d ", i);
		os_sempost(&s2);
		for(;;);
	}
}

void task2(void){
	int i;

	for(;;){
		os_semwait(&s2);
		for(i = 401; i <= 600; i++)
			printf("%d ", i);
		os_sempost(&s3);
		for(;;);
	}
}

void task3(void){
	int i;

	for(;;){
		os_semwait(&s3);
		for(i = 601; i <= 800; i++)
			printf("%d ", i);
		os_sempost(&s4);
		for(;;);
	}
}

void task4(void){
	int i;

	for(;;){
		os_semwait(&s4);
		for(i = 801; i <= 1000; i++)
			printf("%d ", i);
		for(;;);
	}
}

void app_main(void){
	os_seminit(&s1, 0);
	os_seminit(&s2, 0);
	os_seminit(&s3, 0);
	os_seminit(&s4, 0);

	os_spawn(task0, "task a", 1024);
	os_spawn(task1, "task b", 1024);
	os_spawn(task2, "task c", 1024);
	os_spawn(task3, "task d", 1024);
	os_spawn(task4, "task e", 1024);
}
