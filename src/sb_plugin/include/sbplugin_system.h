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

#ifndef INCLUDE_SBPLUGIN_SYSTEM_H
#define INCLUDE_SBPLUGIN_SYSTEM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "broadview.h"
#include "sbplugin.h"

    /** The System Southbound Feature Definition */

    typedef struct _system_sb_feature_
    {
        /** parent object */
        BVIEW_SB_FEATURE_t feature;

        /** what ASIC can this sbplugin support ? */
        int numSupportedAsics;
        BVIEW_ASIC_t *asicList[BVIEW_MAX_ASICS_ON_A_PLATFORM];

        /** Obtain System Name. Must return the name as a null-terminated string, 
         * not exceeding length characters */
        BVIEW_STATUS(*system_name_get_cb)(char *buffer, int length);

        /** Obtain System (northbound) MAC address */
        BVIEW_STATUS(*system_mac_get_cb)(uint8_t *buffer, int length);

        /** Obtain System (northbound) IPv4 address */
        BVIEW_STATUS(*system_ip4_get_cb)(uint8_t *buffer, int length);

        /** Obtain Current Time*/
        BVIEW_STATUS(*system_time_get_cb)(time_t *time);

        /** Get ASIC from ASIC String*/
        BVIEW_STATUS(*system_asic_translate_from_notation_cb) (char *src, int *asic);

        /** Get ASIC string from ASIC*/
        BVIEW_STATUS(*system_asic_translate_to_notation_cb) (int asic, char *dst);

        /** Get Port Number from Port String*/
        BVIEW_STATUS(*system_port_translate_from_notation_cb) (char *src, int *port);

        /** Get Port String from Port Number*/
        BVIEW_STATUS(*system_port_translate_to_notation_cb) (int asic, int port, char *dst);        

    } BVIEW_SB_SYSTEM_FEATURE_t;

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SBPLUGIN_SYSTEM_H */

