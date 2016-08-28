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

*/

/* Simple UART Bare-metal application sample */

#include <pic32mz.h>
#include <libc.h>
#include <usb_lib.h>

#define IDPRODUCT 0
#define IDVENDOR 0x1267

struct descriptor_decoded descriptor;

void irq_timer(){
    return;
}

int32_t start_sequence_flag = 0;

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

unsigned char owi_arm_up[] = { 0x08 | 0x10 | 0x40, 0x02, 0x00 };
unsigned char owi_arm_down[] = { 0x04 | 0x20 | 0x80, 0x00, 0x00 };

unsigned char owi_wrist_shoulder_up[] = { 0x08 | 0x10, 0x00, 0x00 };
unsigned char owi_wrist_shoulder_down[] = { 0x04 | 0x20, 0x00, 0x00 };

unsigned char owi_shoulder_elbow_up[] = { 0x10 | 0x40, 0x00, 0x00 };
unsigned char owi_shoulder_elbow_down[] = { 0x20 | 0x80, 0x00, 0x00 };

unsigned char owi_light_on[] = { 0x00, 0x00, 0x01 };

unsigned char owi_stop[] = { 0x00, 0x00, 0x00 };

struct owi_command
{
	unsigned char *command;
	uint32_t duration_us;
};

struct owi_command left_right [] = {
	{ .command = owi_base_counter_clockwise, .duration_us = 2000000 },
	{ .command = owi_base_clockwise, .duration_us = 2000000 }
};

struct owi_command sequence1 [] = {

	{ .command = owi_pincher_close, .duration_us = 1500000 },

	{ .command = owi_shoulder_down, .duration_us = 1000000 },

	{ .command = owi_wrist_shoulder_down, .duration_us = 2000000 },

	{ .command = owi_wrist_up, .duration_us = 1000000 },

	{ .command = owi_base_clockwise, .duration_us = 3000000 },

	{ .command = owi_arm_up, .duration_us = 1500000 },

	{ .command = owi_wrist_shoulder_up, .duration_us = 1000000 },

	{ .command = owi_base_counter_clockwise, .duration_us = 1485000 },

	{ .command = owi_wrist_down, .duration_us = 500000 },

	{ .command = owi_pincher_open, .duration_us = 1500000 }

};

struct owi_command sequence2 [] = {
    
    { .command = owi_pincher_close, .duration_us = 1500000 },
    
    { .command = owi_shoulder_down, .duration_us = 1000000 },
    
    { .command = owi_wrist_shoulder_down, .duration_us = 1000000 },
    
    { .command = owi_wrist_up, .duration_us = 1500000 },
    
    { .command = owi_base_clockwise, .duration_us = 2000000 },
    
    { .command = owi_arm_up, .duration_us = 1500000 },
    
    { .command = owi_wrist_shoulder_up, .duration_us = 1500000 },
    
    { .command = owi_base_counter_clockwise, .duration_us = 1485000 },
    
    { .command = owi_pincher_open, .duration_us = 1500000 }
    
};


/**
 * return:
 *   0:   no message received
 *   >= 1:   message received
 *   < 0: error
 */
int get_message(unsigned char *message)
{
    uint32_t source;

	return ReceiveMessage(&source, message, 1, 0);
}

int process_message()
{
	unsigned char message;
    
	if (get_message(&message))
	{
		switch (message)
		{
		// start/resume the sequence
		case '1':
            printf("\nVM#3: Moving the Robotic ARM.");
			start_sequence_flag = 1;
			break;

		// stop the sequence
		case '2':
            printf("\nVM#3: Stoping the Robotic ARM.");
			start_sequence_flag = 0;
			break;

		// return to 0;
		case '3':
			break;

		default:
			break;
		}
	}

	return 0;
}


void send_owi_command(unsigned char byte1, unsigned char byte2, unsigned char byte3)
{
	unsigned char tx[3];
    
    tx[0] = byte1;
    tx[1] = byte2;
    tx[2] = (unsigned char)start_sequence_flag;
    
    usb_send_data(tx, 3);
}

void process_owi_command(unsigned char byte1, unsigned char byte2, unsigned char byte3, uint32_t duration_us)
{
	send_owi_command(byte1, byte2, byte3);
	udelay(duration_us);
}

void stop_sequence()
{
	send_owi_command(owi_stop[0], owi_stop[1], owi_stop[2]);
}

void start_sequence(struct owi_command *commands, int num_commands)
{
	int32_t index = 0;

	for (index = 0; index < num_commands; index++)
	{
		struct owi_command *command = &commands[index];

		process_owi_command(command->command[0],
				command->command[1],
				command->command[2],
				command->duration_us);

		stop_sequence();
		udelay(500000);

		process_message();
	}
}

/*
 * main
 */
int main() {
    int32_t ret;
    char message;
    uint32_t guest_id = hyp_get_guest_id();

    serial_select(UART2);
    
	init_network();

	stop_sequence();
    
    /* register this VM for USB interrupts */
    hyper_usb_vm_register(guest_id);
    
    printf("\nVM#3: Starting Robotic Arm Control.");

    wait_device(&descriptor, sizeof(descriptor));
    
    printf("\nVM#3: USB Device connected: idVendor 0x%04x idProduct 0x%04x", descriptor.idVendor, descriptor.idProduct);
    
    if(descriptor.idVendor != IDVENDOR || descriptor.idProduct != IDPRODUCT){
        printf("\nVM#3: Warning! This device not recognized.");
    }
    
    while(1) {

    	process_message();
        
		if (start_sequence_flag == 1) {
            start_sequence(sequence1, sizeof(sequence1) / sizeof(struct owi_command));

			// pause for 3 seconds after the cycle completes to allow the user to
			// stop the cycle
			udelay(3000000);
		}
    }
    
    return 0;
}

