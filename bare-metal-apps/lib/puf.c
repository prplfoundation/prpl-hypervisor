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

#define GETUINT32(address)          (((uint32_t)(address)[0] << 24) ^ ((uint32_t)(address)[1] << 16) ^ ((uint32_t)(address)[2] << 8) ^ ((uint32_t)(address)[3]))

return_t PUF_GetSoftwareVersion(uint8_t * const majorVersion, uint8_t * const minorVersion) {
	puf_message_t puf_message;
	int32_t ret;
	int32_t source;
	return_t retVal;

	puf_message.command = PUF_GETSOFTWAREVERSION;

	ret = SendMessage(2, (void *)&puf_message, sizeof(puf_message_t));

	if (ret <= 0) {
		print_net_error(ret);
	} else {
		ret = ReceiveMessage(&source, (char *)&puf_message, 1);

		if (ret) {
			retVal = puf_message.response;
			*majorVersion = puf_message.puf_struct.puf_getsoftwareversion.majorVersion;
			*minorVersion = puf_message.puf_struct.puf_getsoftwareversion.minorVersion;
		}
	}

	return retVal;
}

return_t PUF_Stop() {
	puf_message_t puf_message;
	int32_t ret;
	int32_t source;
	return_t retVal;

	puf_message.command = PUF_STOP;

	ret = SendMessage(2, (void *)&puf_message, sizeof(puf_message_t));

	if (ret <= 0) {
		print_net_error(ret);
	} else {
		ret = ReceiveMessage(&source, (char *)&puf_message, 1);

		if (ret) {
			retVal = puf_message.response;
		}
	}

	return retVal;
}

return_t PUF_WrapKey(
	const uint8_t  * const key,
	const uint8_t  * const label,
	const uint8_t  * const context,
		  uint16_t		   keySize,
		  uint16_t		   keyProperties,
		  uint8_t		   keyIndex,
		  uint8_t  * const keyCode)
{
	puf_message_t puf_message;
	int32_t ret;
	int32_t source;
	return_t retVal;

	puf_message.command = PUF_WRAPKEY;

	memcpy(puf_message.puf_struct.puf_wrapkey.key, key, keySize);
	memcpy(puf_message.puf_struct.puf_wrapkey.label, label, 6);
	memcpy(puf_message.puf_struct.puf_wrapkey.context, context, 6);
	puf_message.puf_struct.puf_wrapkey.keySize = keySize;
	puf_message.puf_struct.puf_wrapkey.keyProperties = keyProperties;
	puf_message.puf_struct.puf_wrapkey.keyIndex = keyIndex;

	ret = SendMessage(2, (void *)&puf_message, sizeof(puf_message_t));

	if (ret <= 0) {
		print_net_error(ret);
	} else {
		ret = ReceiveMessage(&source, (char *)&puf_message, 1);

		if (ret) {
			memcpy(keyCode, puf_message.puf_struct.puf_wrapkey.keyCode, keySize + 40);

			retVal = puf_message.response;
		}
	}

	return retVal;
}

return_t PUF_UnwrapKey(
		const uint8_t  * const keyCode,
		const uint8_t  * const label,
		const uint8_t  * const context,
			  uint16_t *	   keySize,
			  uint8_t  *	   keyIndex,
			  uint8_t  * const key)
{
	puf_message_t puf_message;
	int32_t ret;
	int32_t source;
	uint16_t _keySize;
	return_t retVal;

	puf_message.command = PUF_UNWRAPKEY;

	_keySize = GETUINT32(&keyCode[28]) / 4;

	memcpy(puf_message.puf_struct.puf_unwrapkey.keyCode, keyCode, _keySize + 40);
	memcpy(puf_message.puf_struct.puf_unwrapkey.label, label, 6);
	memcpy(puf_message.puf_struct.puf_unwrapkey.context, context, 6);

	ret = SendMessage(2, (void *)&puf_message, sizeof(puf_message_t));

	if (ret <= 0) {
		print_net_error(ret);
	} else {
		ret = ReceiveMessage(&source, (char *)&puf_message, 1);

		if (ret) {
			memcpy(label, puf_message.puf_struct.puf_unwrapkey.label, 6);
			memcpy(context, puf_message.puf_struct.puf_unwrapkey.context, 6);
			*keySize = puf_message.puf_struct.puf_unwrapkey.keySize;
			*keyIndex = puf_message.puf_struct.puf_unwrapkey.keyIndex;
			memcpy(key, puf_message.puf_struct.puf_unwrapkey.key, puf_message.puf_struct.puf_unwrapkey.keySize);

			retVal = puf_message.response;
		}
	}

	return retVal;
}
