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

This code was written by Pierpaolo Bagnasco at Intrinsic-ID.

*/

/* IID Quiddikey test */

#include <pic32mz.h>
#include <libc.h>
#include <quiddikey/iid_errors.h>
#include <puf.h>


volatile int32_t t2 = 0;

void irq_timer(){
 t2++;
}


int main() {
	return_t retVal;

	//udelay(5000000);

#define KEYSIZE 16
    	uint16_t keySize = KEYSIZE;

    	uint8_t key[KEYSIZE];
    	memset(key, 0x5E, KEYSIZE); // Set the key that has to be wrapped to a fixed value

    	uint8_t key_unwrapped[KEYSIZE];

    	uint8_t label[6];
    	memset(label, 0x12, 6);

    	uint8_t context[6];
    	memset(context, 0x23, 6);

    	uint16_t keyProperties = KEY_PROP_NONE;
    	uint8_t keyIndex = 0;
    	uint8_t keyCode[KEYSIZE + 40];

    while(1) {
    	udelay(100000);
    	uint8_t i;

    	// Test QK_WrapKey
    	retVal = PUF_WrapKey(key, label, context, keySize, keyProperties, keyIndex, keyCode);

    	if (IID_SUCCESS != retVal) {
    		printf("Error QK_WrapKey: %x\n", retVal);
    	} else {
    		for (i = 0; i < 40 + 16; ++i) {
    			printf("%02x", keyCode[i]);
    		}
    		printf("\n");
    	}

    	// Test QK_UnwrapKey
    	retVal = PUF_UnwrapKey(keyCode, label, context, &keySize, &keyIndex, key_unwrapped);

		if (IID_SUCCESS != retVal) {
			printf("Error PUF_UnwrapKey: %x\n", retVal);
		} else {
			if (0 != memcmp(key, key_unwrapped, KEYSIZE)) {
				printf("Error PUF_UnwrapKey: keys don't match. Robotic arm cannot be controlled.\n");
			}
		}
    }

    return 0;
}

