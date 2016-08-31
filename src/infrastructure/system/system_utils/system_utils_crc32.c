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

#define  BVIEW_CRC_TABLE_SIZE     256

#define swapInt(value) \
  ( (((value) >> 24) & 0x000000FF) | (((value) >> 8) & 0x0000FF00) | \
    (((value) << 8) & 0x00FF0000) | (((value) << 24) & 0xFF000000) )


static int crc32_table_lookup[BVIEW_CRC_TABLE_SIZE];

/*********************************************************************
* @brief        Creates CRC Lookup table
*
* @param[in]    NA
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/
void crc32_lookup_init (void)
{
  static int first = 1;
  unsigned int val = 0;
  unsigned int i = 0;
  unsigned int j = 0;

  if (first != 0)
  {
    for (i = 0; i < BVIEW_CRC_TABLE_SIZE; i++)
    {
      val = i;

      for (j = 0; j < 8; j++)
      {
        if ((val & 1) != 0)
        {
          val = 0xedb88320 ^ (val >> 1);
        }
        else
        {
          val = val >> 1;
        }
      }

      crc32_table_lookup[i] = val;
    }
    first = 0;
  }
}

/*********************************************************************
* @brief        Calculated 32 bit CRC
*
* @param[in]    buffer Starting location of the buffer for which
*               CRC is needed
* @param[in]    Length of buffer
*
* @retval       CRC
*
* @note         NA
*
* @end
*********************************************************************/
unsigned int crc32_calculate (unsigned char * buffer, int length)
{
  unsigned int result = ~0;
  unsigned int i = 0;
  crc32_lookup_init ();

  for (i = 0; i < length; i++)
  {
    result = crc32_table_lookup[(result ^ buffer[i]) & 0xff] ^ (result >> 8);
  }

  result = (~result) & 0xffffffff;
  result = swapInt(result);
  return result;
}


