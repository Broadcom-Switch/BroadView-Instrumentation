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

#ifndef INCLUDE_SB_FEATURE_H
#define	INCLUDE_SB_FEATURE_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include "broadview.h"
#include "openapps_feature.h"

    /** The following structure defines a generic South-Bound feature 
    * Specific Plugins add multiple callback declarations 
    * underneath the single parameter */

    typedef struct _bview_sb_feature_
    {
        /** Feature ID   */
        BVIEW_FEATURE_ID featureId;
        /** Mask of asics that support this feature */
        BVIEW_ASIC_TYPE  supportedAsicMask;
    } BVIEW_SB_FEATURE_t;


#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_SB_FEATURE_H */

