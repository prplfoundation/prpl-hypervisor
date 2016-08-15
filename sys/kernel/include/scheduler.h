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

This code was written by Carlos Moratelli at Embedded System Group (GSE) at PUCRS/Brazil.

*/

#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include "types.h"
#include "linkedlist.h"

struct vcpu_t;
typedef struct vcpu_t vcpu_t;

typedef struct task_t{  
  
  /**Identification**/
  char* unique_name;
  uint32_t unique_name_hash;

  /**IPC**/
  uint8_t  message_pending_out;
  uint32_t message_target_id;
  uint32_t message_size_out;
  char*    message_ptr_out;
  
  uint8_t  message_pending_in;
  uint32_t message_source_id;
  char*    message_ptr_in;    
    
  /**Scheduling parameters*/
  uint32_t deadline;
  uint32_t relative_deadline;
  uint32_t release_time;
  uint32_t period;
  
  uint32_t used_period_time;
  
  uint32_t wcet;
  uint32_t wcet_counter;
  uint32_t deadline_counter;
  uint32_t deadline_misses;
  
  //~ 
  //~ unsigned int run_time;
  //~ unsigned int lost_deadlines;
}task_t;

void runBestEffortScheduler();
int32_t remove_vm_and_runBestEffortScheduler();
ll_node_t* get_vcpu_node_from_task_name(char* unique_name, linkedlist_t* vcpu_list);

#endif /* !__SCHEDULER_H */
