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

#define N_READERS	3
#define N_WRITERS	1

sem_t mtx_rc, mtx_wc, mtx, w_db, r_db;
int32_t rc = 0, wc = 0, reads = 0, writes = 0;

void reader(void){
	int32_t i;

	i = os_selfid();
	while(1){
		os_semwait(&mtx);
		os_semwait(&r_db);
		os_semwait(&mtx_rc);
		rc++;
		if (rc == 1) os_semwait(&w_db);
		os_sempost(&mtx_rc);
		os_sempost(&r_db);
		os_sempost(&mtx);
		reads++;
		printf("(R) thread %d reading the database... (%d readers, %d reads, %d writes)\n", i, rc, reads, writes);
		os_semwait(&mtx_rc);
		rc--;
		if (rc == 0) os_sempost(&w_db);
		os_sempost(&mtx_rc);
		printf("(R) thread %d using data...\n", i);
	}
}

void writer(void){
	int32_t i;

	i = os_selfid();
	while(1){
		os_semwait(&mtx_wc);
		wc++;
		if (wc == 1) os_semwait(&r_db);
		os_sempost(&mtx_wc);
		printf("(W) thread %d preparing data...\n", i);
		os_semwait(&w_db);
		writes++;
		printf("(W) thread %d writing to the database... (%d reads, %d writes)\n", i, reads, writes);
		os_sempost(&w_db);
		os_semwait(&mtx_wc);
		wc--;
		if (wc == 0) os_sempost(&r_db);
		os_sempost(&mtx_wc);
	}
}

void app_main(void){
	int32_t i;

	os_seminit(&mtx_rc, 1);
	os_seminit(&mtx_wc, 1);	
	os_seminit(&mtx, 1);
	os_seminit(&w_db, 1);
	os_seminit(&r_db, 1);

	for(i = 0; i < N_READERS; i++)
		os_spawn(reader, "reader", 2048);

	for(i = 0; i < N_WRITERS; i++)
		os_spawn(writer, "writer", 2048);

}

