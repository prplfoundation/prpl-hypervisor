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

#include <pic32mz.h>
#include <libc.h>
#include <puf.h>
#include <network.h>


volatile int32_t t2 = 0;

void irq_timer(){
 t2++;
}

#define BUFFER_RX_SIZE        255 /* This has to be greater than (MAX_KEY_SIZE + KEYCODE_OVERHEAD = 128) */

uint8_t buffer[BUFFER_RX_SIZE];

int main() {
	uint8_t i;
	return_t retVal;

	/* Key configuration */
#define KEYSIZE 16

	uint16_t keySize = KEYSIZE;
	uint8_t key[KEYSIZE] = { // This is the key that has to be wrapped using PUF
							 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
							 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
						   };

	uint8_t key_unwrapped[KEYSIZE]; // This will contain the unwrapped key
	uint8_t label[LABEL_SIZE] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
	uint8_t context[CONTEXT_SIZE] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
	uint16_t keyProperties = KEY_PROP_NONE;
	uint8_t keyIndex = 0;
	uint8_t keyCode[KEYSIZE + KEYCODE_OVERHEAD]; // This will contain the wrapped key


	serial_select(UART2);


	while (guest_is_up(2) == MESSAGE_VCPU_NOT_INIT) {
		printf("\nWaiting guest to be ready!\n");
	}

	udelay(2000000);

	retVal = PUF_WrapKey(key, label, context, keySize, keyProperties, keyIndex, keyCode);

	if (IID_PRPL_SUCCESS != retVal) {
		printf("Error PUF_WrapKey: %x\n", retVal);
	} else {
		printf("keyCode: ");
		for (i = 0; i < KEYSIZE + KEYCODE_OVERHEAD; ++i) {
			printf("%02x", keyCode[i]);
		}
		printf("\n");
	}

	while (1) {
		for (i = 0; i < BUFFER_RX_SIZE; ++i) {
			buffer[i] = getchar();

			if (buffer[i] == '\n' || buffer[i] == '\r') {
				break;
			}
		}

		retVal = PUF_UnwrapKey(buffer, label, context, &keySize, &keyIndex, key_unwrapped);

		if (IID_PRPL_SUCCESS != retVal) {
			printf("Invalid key! Robotic arm cannot be controlled.\n");
		} else {
			printf("Valid key! Robotic arm control is enabled.\n");
		}
	}

    return 0;
}

