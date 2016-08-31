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
unsigned int crc32_calculate (unsigned char * buffer, int length);


