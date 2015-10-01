/*****************************************************************************
*
* (C) Copyright Broadcom Corporation 2015
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
*
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
***************************************************************************/

#ifndef INCLUDE_PTAPP_BASE64_H
#define	INCLUDE_PTAPP_BASE64_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <arpa/inet.h>



/******************************************************************
 * @brief     Encodes given data to base64.
 *
 * @param[in]   data           input buffer
 *              input_length   size of the input buffer
 *              encoded_data   output buffer
 *              max_len        size of the output buffer  
 *              output_length  size of the encoded data
 *
 * @retval   0   when encoding is successful
 *           -1  When length of the output is more than the buffer length
 * @note     
 *********************************************************************/
int ptapp_base64_encode(const unsigned char *data,
                 unsigned int input_length,
                 char *encoded_data, unsigned int max_len, unsigned int *output_length); 

/******************************************************************
 * @brief     Decodes given base64 encoded data.
 *
 * @param[in]   data           input buffer
 *              input_length   size of the input buffer
 *              decoded_data   output buffer
 *              max_len        size of the output buffer  
 *              output_length  size of the decoded data
 *
 * @retval   0   when decoding is successful
 *           -1  When length of the output is more than the buffer length
 * @note     
 *********************************************************************/
int ptapp_base64_decode(const char *data,
    unsigned int input_length, char *decoded_data, unsigned int max_len,
    unsigned int *output_length);


#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_PT_APP_H */

