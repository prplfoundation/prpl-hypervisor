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
#include <libc.h>


#define MESSAGELIST_SZ  5
#define MESSAGE_SZ      128


/** Return values for inter-vm communication hypercalls  */
#define MESSAGE_VCPU_NOT_FOUND  -1
#define MESSAGE_FULL            -2
#define MESSAGE_TOO_BIG         -3
#define MESSAGE_EMPTY           -4


/** Struct for message exchange 
    It is a circular buffer. Message_list is a char matrix statically allocated.
 */
struct message_t{
        uint32_t source_id;
        uint32_t size; /* size of each message in message_list */
        uint8_t message[MESSAGE_SZ];
};


struct message_list_t{
        uint32_t in;
        uint32_t out;
        volatile uint32_t num_messages;
        struct message_t messages[MESSAGELIST_SZ];
};


void init_network();
int ReceiveMessage(int *source, char *message, int bufsz, int block);
int SendMessage(unsigned target_id, void* message, unsigned size);
void network_int_handler();

#endif

