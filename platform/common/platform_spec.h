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
#ifndef INCLUDE_PLATFORM_SPEC_H
#define INCLUDE_PLATFORM_SPEC_H

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(BVIEW_CHIP_TD2)
#include "trident2/platform.h"
#endif

#if defined(BVIEW_CHIP_TOMAHAWK)
#include "tomahawk/platform.h"
#endif

#if defined(BVIEW_CHIP_PCLINUX)
#include "pclinux/platform.h"
#endif

#if defined(BVIEW_CHIP_OVSDB)
#include "UB-OVSDB/platform.h"
#endif

#endif /* INCLUDE_PLATFORM_SPEC_H */

