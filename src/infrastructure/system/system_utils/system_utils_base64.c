/*****************************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "broadview.h"
#include "openapps_log_api.h"
#include "system_utils_base64.h"

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static char decoding_table[256] = {0};
static int mod_table[] = {0, 2, 1};
static bool firstTime = true; 

/******************************************************************
 * @brief     Encodes given data to base64.
 *
 * @param[in]   data           input buffer
 *              input_length   size of the input buffer
 *              encoded_data   output buffer
 *              max_len        size of the output buffer  
 *              output_length  size of the encoded data
 *
 * @retval   0   When encoding is successful
 *           -1  When length of the output is more than the buffer length
 * @note     
 *********************************************************************/
BVIEW_STATUS system_base64_encode(const unsigned char *data,
                 unsigned int input_length,
                 char *encoded_data, unsigned int max_len, unsigned int *output_length) 
{

    int  i, j;

    *output_length = (4 * ((input_length + 2) / 3));
    if (max_len < *output_length)
    {
       LOG_POST (BVIEW_LOG_ERROR,"base64_encode: Output buffer size %d is not enough to hold encoded %d bytes\n", 
	                                               max_len, *output_length);
       return BVIEW_STATUS_FAILURE;
    }

    for (i = 0, j = 0; i < input_length;) {

        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[*output_length - 1 - i] = '=';

    return BVIEW_STATUS_SUCCESS;
}
/******************************************************************
 * @brief      Build decoding table for base64 decoding
 *
 * @param[in]   none
 *
 * @retval      none
 * @note     
 *********************************************************************/
static void system_build_decoding_table() {

    int i;

    for (i = 0; i < 64; i++)
        decoding_table[(unsigned char) encoding_table[i]] = i;
}

/******************************************************************
 * @brief     Decodes given base64 encoded data.
 *
 * @param[in]   data           input buffer
 *              input_length   size of the input buffer
 *              decoded_data   output buffer
 *              max_len        size of the output buffer  
 *              output_length  size of the decoded data
 *
 * @retval   BVIEW_STATUS_SUCCESS  when decoding is successful
 *           BVIEW_STATUS_FAILURE  When length of the output is more than the buffer length
 * @note     
 *********************************************************************/
BVIEW_STATUS system_base64_decode(const char *data,
    unsigned int input_length, char *decoded_data, unsigned int max_len,
    unsigned int *output_length) 
{

  int i, j; 

  if (firstTime == true) 
  {
    firstTime = false;
    system_build_decoding_table();
  }

  if (decoded_data == NULL) return -1;

  if (input_length % 4 != 0) return -1;

  *output_length = input_length / 4 * 3;
  if (max_len < *output_length)
  {
    LOG_POST (BVIEW_LOG_ERROR,"base64_decode: Output buffer size %d is not enough to hold decoded %d bytes\n", 
        max_len, *output_length);
    return BVIEW_STATUS_FAILURE;
  }
  if (data[input_length - 1] == '=') (*output_length)--;
  if (data[input_length - 2] == '=') (*output_length)--;

  for (i = 0, j = 0; i < input_length;) {

    uint32_t tempdata_a = data[i] == '=' ? 0 & i++ : decoding_table[(unsigned int) data[i++]];
    uint32_t tempdata_b = data[i] == '=' ? 0 & i++ : decoding_table[(unsigned int) data[i++]];
    uint32_t tempdata_c = data[i] == '=' ? 0 & i++ : decoding_table[(unsigned int) data[i++]];
    uint32_t tempdata_d = data[i] == '=' ? 0 & i++ : decoding_table[(unsigned int) data[i++]];

    uint32_t triple = (tempdata_a << 3 * 6)
      + (tempdata_b << 2 * 6)
      + (tempdata_c << 1 * 6)
      + (tempdata_d << 0 * 6);

    if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
    if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
    if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
  }

  return BVIEW_STATUS_SUCCESS;
}




