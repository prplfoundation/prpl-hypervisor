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

/* Simple USB Bare-metal application that detects and read device's descriptors. */

#include <arch.h>
#include <libc.h>
#include <usb.h>
#include <guest_interrupts.h>
#include <hypercalls.h>
#include <network.h>
#include <platform.h>


/* OWI Robotic Arm */
#define IDPRODUCT 0
#define IDVENDOR 0x1267

/*
 * Copyright (c) 2016, prpl Foundation
 * 
 * Permission to use, copy, modify, and/or distribute this software for any purpose with or without 
 * fee is hereby granted, provided that the above copyright notice and this permission notice appear 
 * in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE 
 * FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 */

#define IDPRODUCT 0
#define IDVENDOR 0x1267

#define CMD_SEQUENCE_SIZE(x) ((sizeof(x)/sizeof(struct owi_command)))

struct descriptor_decoded descriptor;

enum arm_control_state{
	WAITING_CONNECT_ARM,
	ACK_DEVICE_BLINK_LED,
	READ_TO_USE,
	RUNNING
};


/*
 * OWI arm definitions
 */
static unsigned char owi_pincher_close[] = { 0x01, 0x00, 0x00 };
static unsigned char owi_pincher_open[]  = { 0x02, 0x00, 0x00 };
static unsigned char owi_wrist_up[]      = { 0x04, 0x00, 0x00 };
static unsigned char owi_wrist_down[]    = { 0x08, 0x00, 0x00 };
/*static unsigned char owi_elbow_up[]      = { 0x10, 0x00, 0x00 };
static unsigned char owi_elbow_down[]    = { 0x20, 0x00, 0x00 };
static unsigned char owi_shoulder_up[]   = { 0x40, 0x00, 0x00 }; 
static unsigned char owi_shoulder_down[] = { 0x80, 0x00, 0x00 };*/
static unsigned char owi_base_clockwise[]         = { 0x00, 0x01, 0x00 };
static unsigned char owi_base_counter_clockwise[] = { 0x00, 0x02, 0x00 };
/*static unsigned char owi_wrist_down_shoulder_up[] = { 0x08 | 0x40, 0x00, 0x00 };
static unsigned char owi_wrist_up_shoulder_down[] = { 0x04 | 0x80, 0x00, 0x00 };
static unsigned char owi_elbow_up_shoulder_up[]     = { 0x10 | 0x40, 0x00, 0x00 };
static unsigned char owi_elbow_down_shoulder_down[] = { 0x20 | 0x80, 0x00, 0x00 };*/
static unsigned char owi_arm_up[]   = { 0x08 | 0x10 | 0x40, 0x00, 0x00 }; // wrist down + elbow up + shoulder up
static unsigned char owi_arm_down[] = { 0x04 | 0x20 | 0x80, 0x01, 0x00 }; // wrist up + elbow down + shoulder down + base clockwise
static unsigned char owi_light_on[] = { 0x00, 0x00, 0x01 };
static unsigned char owi_light_off[] = { 0x00, 0x00, 0x00 };
static unsigned char owi_stop[] = { 0x00, 0x00, 0x00 };

struct owi_command{
	unsigned char *command;
	uint32_t duration_ms;
};


struct owi_command move_sequence [] = {
	{ .command = owi_pincher_close, .duration_ms = 1550 },
	{ .command = owi_wrist_up, .duration_ms = 1900 },
	{ .command = owi_arm_down, .duration_ms = (1800+300) },
	{ .command = owi_base_clockwise, .duration_ms = (4000-1800) },
	{ .command = owi_pincher_open, .duration_ms = 1500 },
	{ .command = owi_stop, .duration_ms = 500 },
	{ .command = owi_pincher_close, .duration_ms = 1500 },
	{ .command = owi_base_counter_clockwise, .duration_ms = (4000-300) },
	{ .command = owi_arm_up, .duration_ms = (1800+600) },
	{ .command = owi_wrist_down, .duration_ms = (1900-225) },
	{ .command = owi_pincher_open, .duration_ms = 1550 },
	{ .command = owi_stop, .duration_ms = 4000 }
};


struct owi_command ack_sequence [] = {
	{ .command = owi_light_on, .duration_ms = 500 },
	{ .command = owi_light_off, .duration_ms = 500 },
	{ .command = owi_light_on, .duration_ms = 500 },
	{ .command = owi_light_off, .duration_ms = 500 },
	{ .command = owi_light_on, .duration_ms = 500 },
	{ .command = owi_light_off, .duration_ms = 500 },
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

uint32_t process_message()
{
	unsigned char message;
	uint32_t run_sequence_flag = 0;
	
	if (get_message(&message))
	{
		switch (message)
		{
			/* start/resume the sequence */
			case '1':
				printf("\nVM#3: Start robotic arm sequence");
				run_sequence_flag = 1;
				break;
				
			/* stop/pause the sequence */
			case '2':
				printf("\nVM#3: Stop robotic arm sequence");
				run_sequence_flag = 0;
				break;
				
			default:
				break;
		}
	}
	
	return run_sequence_flag;
}

void process_owi_command(unsigned char byte1, unsigned char byte2)
{
	/* Make sure that the local buffer will be located on stack using volatile. 
	   The compiler may keep small data structures in registers. The usb_control_send() hypercall
	   requires a buffer in RAM. */
	volatile unsigned char tx[3];
	
	tx[0] = byte1;
	tx[1] = byte2;
	tx[2] = 1; /* LED on */
	
	usb_control_send(tx, 3);
}

uint32_t owi_arm_blink_led(){
	static uint32_t tm_cmd = 1;
	static uint32_t tm_next_cmd = 0;
	static uint32_t index = 0;
	
	if(wait_time(tm_cmd, tm_next_cmd)){
		usb_control_send(ack_sequence[index].command, 3);
		
		tm_next_cmd = ack_sequence[index].duration_ms;
		
		if(CMD_SEQUENCE_SIZE(ack_sequence)-1 == index){
			tm_next_cmd = 0;
			index = 0;
			tm_cmd = 1;
			return READ_TO_USE;
		}

		index = (index + 1) % CMD_SEQUENCE_SIZE(ack_sequence);		
		
		tm_cmd = mfc0(CP0_COUNT, 0);	
	}
	
	return ACK_DEVICE_BLINK_LED;
}

uint32_t waiting_for_arm(uint32_t dev_connected){
	static uint32_t old = 0;
	
	if (dev_connected != old){
		if (dev_connected){
			usb_get_device_descriptor((char*)&descriptor, sizeof(struct descriptor_decoded));
			printf("\nDevice connected: idVendor 0x%x idProduct 0x%x", descriptor.idVendor, descriptor.idProduct);
		}else{
			printf("\nDevice Disconnected");
		}
		old = dev_connected;
	}
	
	if(dev_connected){
		return ACK_DEVICE_BLINK_LED;
	}else{
		return WAITING_CONNECT_ARM;
	}
}

void perform_arm_moviment(uint32_t stop){
	static uint32_t tm_cmd = 1;
	static uint32_t tm_next_cmd = 0;
	static uint32_t index = 0;
	static uint32_t running = 0;
	
	if(stop == 2 && !running){
		return;
	}
	
	running = 1;
	
	if(wait_time(tm_cmd, tm_next_cmd)){
		
		process_owi_command(move_sequence[index].command[0],
				    move_sequence[index].command[1]);
		
		tm_next_cmd = move_sequence[index].duration_ms;
		
		if(CMD_SEQUENCE_SIZE(move_sequence)-1 == index){
			tm_next_cmd = 0;
			index = 0;
			tm_cmd = 1;
			running = 0;
			return;
		}
		
		
		
		index = (index + 1) % CMD_SEQUENCE_SIZE(move_sequence);		
		
		tm_cmd = mfc0(CP0_COUNT, 0);	
	}
	
}
	

int main() {
	/* int32_t ret; */
	/* uint32_t guest_id = get_guestid(); */
	uint32_t dev_connected = 0;
	uint32_t tm_poll = 0, tm_tst = 0;
	uint32_t stop;
	
	uint32_t state = WAITING_CONNECT_ARM;
	
	serial_select(UART2);
	
	printf("\nVM#3: Waiting for USB device ...");
	
	while(1) {
		
		if(wait_time(tm_poll, 10)){
			
			tm_poll = mfc0(CP0_COUNT, 0);
			
			dev_connected = usb_polling();	
			
			if(!dev_connected){
				state = WAITING_CONNECT_ARM;
			}
		}
			
		switch (state){
			case WAITING_CONNECT_ARM:
				state = waiting_for_arm(dev_connected);
				break;
				
			case ACK_DEVICE_BLINK_LED:
				state = owi_arm_blink_led();
				tm_tst = mfc0(CP0_COUNT, 0);
				break;
				
			case READ_TO_USE:
				if(wait_time(tm_tst, 1000)){
					state = RUNNING;
					printf("\nArm read to use!");
				}
				break;
				
			case RUNNING:
				stop = process_message();
				perform_arm_moviment(1);
				break;
		}
	}
	
	return 0;
}
