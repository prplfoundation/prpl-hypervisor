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

/* interrupts inserted by the hypervisor */
void _irq_handler(uint32_t status, uint32_t cause)
{
    uint32_t temp_CP0;
    temp_CP0 = mfc0(CP0_CAUSE, 0);
    temp_CP0 &= ~0x3FC00;
    mtc0(CP0_CAUSE, 0, temp_CP0);
    t2++;
}

void udelay(uint32_t usec){
    uint32_t now = mfc0 (CP0_COUNT, 0);
    uint32_t final = now + usec * (CPU_SPEED / 1000000) / 2;

    for (;;) {
        now = mfc0 (CP0_COUNT, 0);
        if ((int32_t) (now - final) >= 0) break;
    }
}

void putchar(int32_t value){
    while(U2STA & USTA_UTXBF);
    U2TXREG = value;
}

int32_t kbhit(void){
        return (U2STA & USTA_URXDA);
}

int32_t getchar(void){
    while(!kbhit());
    return (uint8_t)U2RXREG;
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
    unsigned int temp_CP0;
    int i, j;

   /* configure the interrupt controller to compatibility mode */
    asm volatile("di");         /* Disable all interrupts */
    mtc0 (CP0_EBASE, 1, 0x9d000000);    /* Set an EBase value of 0x9D000000 */
    temp_CP0 = mfc0(CP0_CAUSE, 0);      /* Get Cause */
    temp_CP0 |= CAUSE_IV;           /* Set Cause IV */
    mtc0(CP0_CAUSE, 0, temp_CP0);       /* Update Cause */
 
    temp_CP0 = mfc0(CP0_STATUS, 0);     /* Get Status */
    temp_CP0 &= ~STATUS_BEV;        /* Clear Status IV */
    mtc0(CP0_STATUS, 0, temp_CP0);      /* Update Status */
    
    temp_CP0 = mfc0(12,1); /* intCTL IV must be different of 0 to allow EIC mode. */
    temp_CP0 |= 8<<5;
    mtc0(12, 1, temp_CP0);

    asm volatile ("ei");
    
	stop_sequence();

    while(1) {
    	start_sequence(action_light_on, sizeof(action_light_on) / sizeof(struct owi_command));
    }
    
    return 0;
}

