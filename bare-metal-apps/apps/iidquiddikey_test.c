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
#include <quiddikey/iidquiddikey_hypercalls.h>


volatile int32_t t2 = 0;

void irq_timer(){
 t2++;
}


int main() {
//	qk_union_t qk_struct;
//	qk_union_t qk_struct_2;
//	qk_union_t qk_struct_3;
//
    while(1) {
//    	// Test QK_GetSoftwareVersion
//    	hyp_qk_getsoftwareversion((uint32_t)&qk_struct);
//    	printf("version: %x.%x\n", qk_struct.qk_getsoftwareversion.majorVersion, qk_struct.qk_getsoftwareversion.minorVersion);
//
//    	// Test QK_WrapKey
//    	memset(qk_struct.qk_wrapkey.key, 0x12, 16);
//    	memset(qk_struct.qk_wrapkey.label, 0xAB, 6);
//    	memset(qk_struct.qk_wrapkey.context, 0xCD, 6);
//    	qk_struct.qk_wrapkey.keySize = 16;
//    	qk_struct.qk_wrapkey.keyProperties = KEY_PROP_NONE;
//    	qk_struct.qk_wrapkey.keyIndex = 1;
//
//    	hyp_qk_wrapkey((uint32_t)&qk_struct);
//
//    	if (IID_SUCCESS != qk_struct.qk_wrapkey.retVal) {
//    		printf("Error QK_WrapKey: %x\n", qk_struct.qk_wrapkey.retVal);
//    	} else {
//    		printf("keyCode[32] - %02x\n", qk_struct.qk_wrapkey.keyCode[32]);
//    	}
//
//    	// Test QK_UnwrapKey
//    	memcpy(qk_struct_2.qk_unwrapkey.keyCode, qk_struct.qk_wrapkey.keyCode, 16 + 40);
//    	memset(qk_struct_2.qk_unwrapkey.label, 0xAB, 6);
//    	memset(qk_struct_2.qk_unwrapkey.context, 0xCD, 6);
//
//    	hyp_qk_unwrapkey((uint32_t)&qk_struct_2);
//
//		if (IID_SUCCESS != qk_struct.qk_wrapkey.retVal) {
//			printf("Error QK_UnwrapKey: %x\n", qk_struct.qk_wrapkey.retVal);
//		} else {
//			if (0 == memcmp(qk_struct.qk_wrapkey.key, qk_struct_2.qk_unwrapkey.key, 16)) {
//				printf("Keys match after QK_UnwrapKey!\n");
//			} else {
//				printf("Error QK_UnwrapKey: keys don't match.\n");
//			}
//		}
//
//    	// Test QK_GetAuthenticationResponse
//		qk_struct.qk_wrapkey.keyProperties = KEY_PROP_AUTHENTICATION;
//		hyp_qk_wrapkey((uint32_t)&qk_struct);
//
//    	memset(qk_struct_3.qk_getauthresponse.label, 0xAB, 6);
//    	memset(qk_struct_3.qk_getauthresponse.context, 0xCD, 6);
//    	memcpy(qk_struct_3.qk_getauthresponse.keyCode, qk_struct.qk_wrapkey.keyCode, 16 + 40);
//    	memset(qk_struct_3.qk_getauthresponse.challenge, 0x45, 16);
//    	qk_struct_3.qk_getauthresponse.challengeSize = 16;
//
//		hyp_qk_getauthresponse((uint32_t)&qk_struct_3);
//
//		if (IID_SUCCESS != qk_struct_3.qk_getauthresponse.retVal) {
//			printf("Error QK_GetAuthenticationResponse: %x\n", qk_struct_3.qk_getauthresponse.retVal);
//		} else {
//			printf("response[0] - %02x\n", qk_struct_3.qk_getauthresponse.response[0]);
//		}

    	//

    	printf("test\n");
    	udelay(1000000);
    }

    return 0;
}

