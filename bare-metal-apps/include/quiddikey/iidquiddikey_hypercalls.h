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

#ifndef IIDQUIDDIKEY_HYPERCALLS_H_
#define IIDQUIDDIKEY_HYPERCALLS_H_

//#include <stdint.h> // pic32mz.h already defines all the types

#ifdef __cplusplus
extern "C"
{
#endif

/*! \brief The function return type
\details
*/
typedef uint8_t return_t;


#define MAX_KEY_SIZE                     88
#define MAX_KEYCODE_SIZE                 128
#define MAX_CHALLENGE_SIZE               64


/*! \brief Indicates a key without special key properties.
    \details This define indicates that the key that will be wrapped has no special properties.
*/
#define KEY_PROP_NONE                           0x0000

/*! \brief Indicates a key that will be used as an authentication key.
    \details This define indicates that the key that will be wrapped is an authentication key.
    This kind of key can only be used in authentication functions and cannot be unwrapped separately.
*/
#define KEY_PROP_AUTHENTICATION                 0x0001


/*! \brief Get the software version.
    \details Get the software version of this Quiddikey module.
    \param[out] majorVersion Pointer to a byte for holding the major software version.
    \param[out] minorVersion Pointer to a byte for holding the minor software version.
    \returns \ref IID_SUCCESS
*/
extern return_t QK_GetSoftwareVersion(uint8_t * const majorVersion, uint8_t * const minorVersion);

/*! \brief Wrap the given key.
    \details The function QK_WrapKey wraps the given key and returns it as a key code.
    \par
    A symmetric key is secured by Quiddikey using the Wrap functionality.
    \par
    IK has to be available in memory when calling Wrap.
	\param[in] key A buffer that holds the key to wrap.
    \param[in] label A buffer that holds the an API identifier
    \param[in] context A buffer that holds the Application Identifier.
    \param[in] keySize The size in bytes of the key that will be wrapped. This has to be a multiple of 4 bytes.
    \param[in] keyProperties The properties of the key (key type, protection, etc).
    \param[in] keyIndex The index of the key that will be wrapped.
    \param[out] keyCode A buffer that will hold the Key Code. This has to be aligned to 32 bits.
	\pre \ref QK_Start has to be called before \ref QK_WrapKey may be called.
    \returns \ref IID_SUCCESS if success.
*/
extern return_t QK_WrapKey(
	const uint8_t  * const key,
	const uint8_t  * const label,
	const uint8_t  * const context,
		  uint16_t		   keySize,
		  uint16_t		   keyProperties,
		  uint8_t		   keyIndex,
		  uint8_t  * const keyCode);

/*! \brief Unwrap the given key code.
    \details With QK_WrapKey, a previously wrapped key is retrieved from a key code.
    \par
    Keys that are stored securely can be retrieved from their key code using the Unwrap functionality.
    \par
    Intrinsic Key has to be available in memory when calling Unwrap.
	\param[in] keyCode A buffer that holds the key code. This has to be aligned to 32 bits.
    \param[in] label A buffer that holds the an API identifier.
    \param[in] context A buffer that holds the Application Identifier.
    \param[out] keySize A buffer that will hold the size of the unwrapped key.
    \param[out] keyIndex A buffer that will hold the index of the unwrapped key.
    \param[out] key A buffer that will hold the unwrapped key.
	\pre \ref QK_Start has to be called before \ref QK_UnwrapKey may be called.
    \returns \ref IID_SUCCESS if success.
    \returns \ref IID_ERROR_INV_KEYCODE if keyCode is corrupted or manipulated.
	\note Only non-authentication keys can be unwrapped.
*/
extern return_t QK_UnwrapKey(
	const uint8_t  * const keyCode,
	const uint8_t  * const label,
	const uint8_t  * const context,
		  uint16_t *	   keySize,
		  uint8_t  *	   keyIndex,
		  uint8_t  * const key);

/*! \brief Calculate the response to a given challenge.
    \details For authentication of this software, a response will be calculated based on the authentication key
    which has been wrapped into a key code and a challenge.
    \param[in] label A buffer that holds the an API identifier
    \param[in] context A buffer that holds the Application Identifier.
    \param[in] keyCode A buffer that holds the key code of the authentication key. This has to be aligned to 32 bits.
    \param[in] challenge A buffer holding the challenge to calculate the response for.
    \param[in] challengeSize The size of the challenge in bytes. This must be a multiple of 16 bytes.
    \param[out] response A buffer for holding the response. This buffer has to be 16 bytes long and aligned to 32 bits.
	\pre \ref QK_Start has to be called before \ref QK_GetAuthenticationResponse may be called.
	\returns \ref IID_SUCCESS if success.
	\returns \ref IID_ERROR_INV_KEYCODE if keyCode is corrupted or manipulated.
*/
extern return_t QK_GetAuthenticationResponse(
    const uint8_t  * const label,
    const uint8_t  * const context,
    const uint8_t  * const keyCode,
    const uint8_t  * const challenge,
          uint16_t         challengeSize,
          uint8_t  * const response);

/*@}*/


/*! \addtogroup Platform
*/
/*@{*/

#ifdef _MSC_VER
/*! \brief Macro to force memory alignment
    \details Macro to force memory alignment.
*/
#define PRE_HIS_ALIGN           __declspec(align(4))

/*! \brief Macro to force memory alignment
    \details Macro to force memory alignment.
*/
#define POST_HIS_ALIGN

#else

/*! \brief Macro to force memory alignment
    \details Macro to force memory alignment.
*/
#define PRE_HIS_ALIGN

/*! \brief Macro to force memory alignment
    \details Macro to force memory alignment.
*/
#define POST_HIS_ALIGN          __attribute__ ((aligned (4)))
#endif /* _MSC_VER */

/*@}*/


/*! \addtogroup Structures
*/
/*@{*/

// TODO: check the alignments. Should we align the union?

typedef struct qk_getsoftwareversion {
		            return_t         retVal;
	                uint8_t          majorVersion;
	                uint8_t          minorVersion;
} qk_getsoftwareversion_t;

typedef struct qk_wrapkey {
		            return_t         retVal;
	                uint8_t          key[MAX_KEY_SIZE];
	                uint8_t          label[6];
	                uint8_t          context[6];
		            uint16_t		 keySize;
		            uint16_t		 keyProperties;
		            uint8_t		     keyIndex;
PRE_HIS_ALIGN       uint8_t          keyCode[MAX_KEYCODE_SIZE] POST_HIS_ALIGN;
} qk_wrapkey_t;

typedef struct qk_unwrapkey {
		            return_t         retVal;
PRE_HIS_ALIGN       uint8_t          keyCode[128] POST_HIS_ALIGN;
	                uint8_t          label[6];
	                uint8_t          context[6];
		            uint16_t	     keySize;
		            uint8_t  	     keyIndex;
		            uint8_t          key[MAX_KEY_SIZE];
} qk_unwrapkey_t;

typedef struct qk_getauthresponse {
		            return_t         retVal;
	                uint8_t          label[6];
	                uint8_t          context[6];
PRE_HIS_ALIGN       uint8_t          keyCode[MAX_KEYCODE_SIZE] POST_HIS_ALIGN;
	                uint8_t          challenge[MAX_CHALLENGE_SIZE];
	                uint16_t         challengeSize;
PRE_HIS_ALIGN       uint8_t          response[16] POST_HIS_ALIGN;
} qk_getauthresponse_t;

typedef union qk_union {
	qk_getsoftwareversion_t qk_getsoftwareversion;
	qk_wrapkey_t qk_wrapkey;
	qk_unwrapkey_t qk_unwrapkey;
	qk_getauthresponse_t qk_getauthresponse;
} qk_union_t;

/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* IIDQUIDDIKEY_HYPERCALLS_H_ */
