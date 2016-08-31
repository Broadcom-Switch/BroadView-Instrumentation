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

#ifndef INCLUDE_SBPLUGIN_API_H
#define	INCLUDE_SBPLUGIN_API_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include "broadview.h"
#include "sbplugin.h"

/*********************************************************************
* @brief    South bound plugin init
*
* @retval   BVIEW_STATUS_SUCCESS if BST feature is
*                                initialized successfully.
* @retval   BVIEW_STATUS_FAILURE if initialization is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_common_init ();

#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_SBPLUGIN_API_H */

