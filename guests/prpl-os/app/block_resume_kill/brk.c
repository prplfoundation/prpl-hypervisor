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

void task(void){
	int32_t jobs;
	
	for(;;){
		jobs = krnl_task->jobs;
		printf("\n%s (%d)[%d]", krnl_task->name, krnl_task->id, krnl_task->jobs);
		while (jobs == krnl_task->jobs);
	}
}

void task2(void){
	int32_t jobs;
	
	for(;;){
		printf("\nblocking task 2");
		jobs = krnl_task->jobs;
		os_block(2);
		while (jobs == krnl_task->jobs);
		delay_ms(100);
		printf("\nunblocking task 2");
		jobs = krnl_task->jobs;
		os_resume(2);
		while (jobs == krnl_task->jobs);
		delay_ms(100);
		printf("\nkilling task 2");
		jobs = krnl_task->jobs;
		os_kill(2);
		while (jobs == krnl_task->jobs);
		delay_ms(100);
		printf("\nspawning task 2");
		jobs = krnl_task->jobs;
		os_spawn(task, "task b", 2048);
		os_delay(2, 1000);
		while (jobs == krnl_task->jobs);
		printf("\nend of life");
		os_kill(os_selfid());
	}
}

void app_main(void){
	os_spawn(task, "task a", 2048);
	os_spawn(task, "task b", 2048);
	os_spawn(task, "task c", 2048);
	os_spawn(task, "task d", 2048);
	os_spawn(task2, "task", 2048);
}
