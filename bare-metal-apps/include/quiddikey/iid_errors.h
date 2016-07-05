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

#ifndef _IIDERRORS_H_
#define _IIDERRORS_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*! \defgroup QuiddikeyErrorCode Error codes
    \ingroup Quiddikey
    \brief The possible error codes returned by the Quiddikey functions.
*/

/*@{*/

/*! \brief Successful execution
    \details Value indicating the successful execution of the called function.
*/
#define IID_SUCCESS                 0x00

/*! \brief Error base
    \details Value used internally as the base value for all error codes.
*/
#define IID_ERROR_BASE              0x01


/*! \brief Quiddikey error base
    \details Value used internally as the base value for all Quiddikey error codes.
*/
#define IID_QK_ERROR_BASE           0x20

/************************************ Generic Errors ***************************************************/
/*! \brief State error
    \details Value indicating that the function call is not allowed in the current state of the Quiddikey module.
*/
#define IID_ERROR_STATE             (IID_ERROR_BASE + 0x00)

/*! \brief Parameter error
    \details Value indicating that one or more of the parameters used during the call to the function is invalid.
*/
#define IID_ERROR_PARM              (IID_ERROR_BASE + 0x01)

/***************************** Quiddikey Specific Errors ******************************************/

/*! \brief Zeroed memory found
    \details Zeroed memory has been found during \ref QK_Enrollment function.
    Re-power the device and try again.
*/
#define IID_ERROR_ZEROED_MEM        (IID_QK_ERROR_BASE + 0x00)

/*! \brief Invalid Activation Code is used
    \details An invalid activation code has been supplied to the \ref QK_Start function.
*/
#define IID_ERROR_INVALID_AC        (IID_QK_ERROR_BASE + 0x01)

/*! \brief Invalid Key Code is used
    \details An invalid key code has been supplied to the \ref QK_Wrap, \ref QK_Unwrap or
	\ref QK_GetAuthenticationResponse functions.
*/
#define IID_ERROR_INV_KEYCODE       (IID_QK_ERROR_BASE + 0x02)

/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* _IIDERRORS_H_ */
