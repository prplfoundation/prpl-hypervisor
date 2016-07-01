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

#ifndef NETWORK_H
#define NETWORK_H

#include <pic32mz.h>


#define MESSAGELIST_SZ  10
#define MESSAGE_SZ      128

/** Struct for message exchange 
    It is a circular buffer. Message_list is a char matrix statically allocated.
 */
typedef struct{
        uint32_t source_id;
        uint32_t size; /* size of each message in message_list */
        uint8_t message[MESSAGE_SZ];
} message_t;

typedef struct {
        uint32_t in;
        uint32_t out;
        uint32_t num_messages;
        message_t messages[MESSAGELIST_SZ];
} message_buffer_t;


void init_network();
int ReceiveMessage(int *source, char *message);
int SendMessage(unsigned target_id, void* message, unsigned size);
void network_int_handler();

#endif

