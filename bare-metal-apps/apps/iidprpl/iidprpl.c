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
 * This code was written by Pierpaolo Bagnasco at Intrinsic-ID.
 * 
 */

/* IID_PRPL VM */

#include <arch.h>
#include <libc.h>
#include <network.h>
#include <hypercalls.h>

#include "include/puf.h"
#include "include/iid_errors.h"
#include "include/iidprplpuf.h"

#define IID_CHECK_AC_CONSTANT	0xA5FECD39

extern uint32_t _puf_address;

static return_t readAndCheckAC(uint8_t * activationCode) {
	read_1k_data_flash(activationCode);
	
	if (IID_CHECK_AC_CONSTANT == *((uint32_t *)&activationCode[IID_PRPL_AC_SIZE_BYTES])) {
		return IID_PRPL_SUCCESS; /* If AC exists */
	}
	
	return 1;/* Whatever value, if AC doesn't exist */
}

static return_t storeAC(const uint8_t * const activationCode) {
	*(uint32_t *)&activationCode[IID_PRPL_AC_SIZE_BYTES] = IID_CHECK_AC_CONSTANT;
	
	write_1k_data_flash(activationCode);
	
	return IID_PRPL_SUCCESS;
}


int main() {
	uint32_t source;
	int32_t ret;
	return_t retVal;
	puf_message_t puf_message;
	
	uint8_t * pufSram = (uint8_t *)&_puf_address;
	/* Last KB of SRAM.
	 * NOTE: remember to reserve it by changing the linker script!
	 */
	printf("\n0x%x", pufSram);
	PRE_HIS_ALIGN uint8_t activationCode_buffer[1024] POST_HIS_ALIGN = { 0 }; /* This has to be 32-bits aligned! */
	
	retVal = IID_PRPL_Init(pufSram, 1024);
	
	if (IID_PRPL_SUCCESS == retVal) {
		retVal = readAndCheckAC(activationCode_buffer);
		
		if (IID_PRPL_SUCCESS != retVal) { /* If the AC doesn't exist, generate and create it */
			retVal = IID_PRPL_Enroll(activationCode_buffer);
			printf("\nVM#2: IID_PRPL Enroll executed");
			
			if (IID_PRPL_SUCCESS == retVal) {
				retVal = storeAC(activationCode_buffer); /* Store the new AC */
				
				if (IID_PRPL_SUCCESS != retVal) {
					printf("\nVM#2: Storage of Activation Code failed with error: %02X", retVal);
				}
			} else {
				printf("\nVM#2: IID_PRPL Enroll failed with error: %02X", retVal);
			}
		}
		
		if (IID_PRPL_SUCCESS == retVal) {
			retVal = IID_PRPL_Start(activationCode_buffer);
			
			if (retVal != IID_PRPL_SUCCESS) {
				printf("\nVM#2: IID_PRPL Start failed with error: %02X", retVal);
			} else {
				printf("\nVM#2: IID_PRPL started successfully");
			}
		}
	} else {
		printf("\nVM#2: IID_PRPL Init failed with error: %02X", retVal);
	}
	
	while(1) {
		ret = ReceiveMessage(&source, (char *)&puf_message, sizeof(puf_message_t), 1);
		
		if (ret) {
			switch(puf_message.command) {
				case PUF_GETSOFTWAREVERSION:{
					puf_message.response = IID_PRPL_GetSoftwareVersion(&puf_message.puf_struct.puf_getsoftwareversion.majorVersion,
											   &puf_message.puf_struct.puf_getsoftwareversion.minorVersion
					);
					break;
				}
				
				case PUF_STOP:{
					puf_message.response = IID_PRPL_Stop();
					break;
				}
				
				case PUF_WRAPKEY:{
					puf_message.response = IID_PRPL_WrapKey(puf_message.puf_struct.puf_wrapkey.key,
										puf_message.puf_struct.puf_wrapkey.label,
					     puf_message.puf_struct.puf_wrapkey.context,
					     puf_message.puf_struct.puf_wrapkey.keySize,
					     puf_message.puf_struct.puf_wrapkey.keyProperties,
					     puf_message.puf_struct.puf_wrapkey.keyIndex,
					     puf_message.puf_struct.puf_wrapkey.keyCode
					);
					break;
				}
				
				case PUF_UNWRAPKEY:{
					puf_message.response = IID_PRPL_UnwrapKey(puf_message.puf_struct.puf_unwrapkey.keyCode,
										  puf_message.puf_struct.puf_unwrapkey.label,
					       puf_message.puf_struct.puf_unwrapkey.context,
					       &puf_message.puf_struct.puf_unwrapkey.keySize,
					       &puf_message.puf_struct.puf_unwrapkey.keyIndex,
					       puf_message.puf_struct.puf_unwrapkey.key
					);
					break;
				}
				
				case PUF_GETAUTHRESPONSE:{
					puf_message.response = IID_PRPL_GetAuthenticationResponse(puf_message.puf_struct.puf_getauthresponse.label,
												  puf_message.puf_struct.puf_getauthresponse.context,
							       puf_message.puf_struct.puf_getauthresponse.keyCode,
							       puf_message.puf_struct.puf_getauthresponse.challenge,
							       puf_message.puf_struct.puf_getauthresponse.challengeSize,
							       puf_message.puf_struct.puf_getauthresponse.response
					);
					break;
				}
				
				default:{
					puf_message.response = PUF_INVALID_COMMAND;
					break;
				}
			}
			
			SendMessage(source, (void *)&puf_message, sizeof(puf_message));
		}
	}
	
	return 0;
}

