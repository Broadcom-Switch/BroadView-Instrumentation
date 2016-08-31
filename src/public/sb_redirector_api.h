/*! \file sb_redirector_api.h
 */

/*! @addtogroup GLUSMDB API Definitions
 *  @{
 *    @addtogroup GLBVIEW BroadView South Bound Plug-In Redirector APIs [BVIEW]
 *    @{
 */
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

#ifndef INCLUDE_SB_REDIRECTOR_API_H
#define	INCLUDE_SB_REDIRECTOR_API_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include "broadview.h"
#include "sbplugin.h"

/*****************************************************************//**
* @brief       Initialize South bound redirector's local data with defaults
*
* @retval   BVIEW_STATUS_FAILURE   if failed to initialize read-write lock
*
* @retval   BVIEW_STATUS_SUCCESS   All the data structures are sucessfully 
*                                  initialized
*
*********************************************************************/
extern BVIEW_STATUS sb_redirector_init ();

/*****************************************************************//**
* @brief       Register a south bound plug-in with
*                               south bound redirector
*
* @param[in]  sbPlugin        South bound plug-in
*
* @retval   BVIEW_STATUS_FAILURE   When validation of sb plug-in fails or
*                                  Lock acquistion fails 
*
* @retval   BVIEW_STATUS_TABLE_FULL  Number of plug-ins already registered 
*                                    are BVIEW_MAX_SUPPORTED_SB_PLUGINS.
*
* @retval   BVIEW_STATUS_SUCCESS    Successfully registered sb plug-in with 
*                                    sb redirector
*
*********************************************************************/
extern BVIEW_STATUS sb_plugin_register (BVIEW_SB_PLUGIN_t sbPlugin);

#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_SB_REDIRECTOR_API_H */

/*!  @}
 * @}
 */
