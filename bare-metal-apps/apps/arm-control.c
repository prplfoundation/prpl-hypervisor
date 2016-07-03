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

/* Simple UART Bare-metal application sample */

#include <pic32mz.h>
#include <libc.h>

volatile int32_t t2 = 0;

void irq_timer(){
 t2++;
}

/*
 * OWI arm definitions
 */
unsigned char owi_pincher_close[] = { 0x01, 0x00, 0x00 };
unsigned char owi_pincher_open[] = { 0x02, 0x00, 0x00 };

unsigned char owi_wrist_up[] = { 0x04, 0x00, 0x00 };
unsigned char owi_wrist_down[] = { 0x08, 0x00, 0x00 };

unsigned char owi_elbow_up[] = { 0x10, 0x00, 0x00 };
unsigned char owi_elbow_down[] = { 0x20, 0x00, 0x00 };

unsigned char owi_shoulder_up[] = { 0x40, 0x00, 0x00 };
unsigned char owi_shoulder_down[] = { 0x80, 0x00, 0x00 };

unsigned char owi_base_clockwise[] = { 0x00, 0x01, 0x00 };
unsigned char owi_base_counter_clockwise[] = { 0x00, 0x02, 0x00 };

unsigned char owi_light_on[] = { 0x00, 0x00, 0x01 };

unsigned char owi_stop[] = { 0x00, 0x00, 0x00 };

struct owi_command
{
	unsigned char *command;
	uint32_t duration_us;
};

struct owi_command action_light_on [] = {
	{ .command = owi_light_on, .duration_us = 2000000 },
	{ .command = owi_stop, .duration_us = 2000000 }
};

struct owi_command left_right [] = {
	{ .command = owi_base_counter_clockwise, .duration_us = 2000000 },
	{ .command = owi_base_clockwise, .duration_us = 2000000 }
};

int8_t counter = 0;

void send_owi_command(unsigned char *command)
{
	counter++;

	putchar(command[0]);
	putchar(command[1]);
	putchar(command[2]);
	putchar(counter);
}

void process_owi_command(struct owi_command *command)
{
	send_owi_command(command->command);
	udelay(command->duration_us);
}

void stop_sequence()
{
	send_owi_command(owi_stop);
}

void start_sequence(struct owi_command *commands, int num_commands)
{
	int32_t index = 0;

	for (index = 0; index < num_commands; index++)
	{
		process_owi_command(&commands[index]);
	}

	stop_sequence();
}

/*
 * main
 */
int main() {

	serial_select(4);

	stop_sequence();

    while(1) {

    	send_owi_command(owi_light_on);

    	start_sequence(left_right, sizeof(left_right) / sizeof(struct owi_command));

    	udelay(2000000);
    }
    
    return 0;
}

