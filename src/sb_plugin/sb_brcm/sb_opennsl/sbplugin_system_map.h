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

#ifndef INCLUDE_SBPLUGIN_SYSTEM_MAP_H
#define INCLUDE_SBPLUGIN_SYSTEM_MAP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "sbplugin.h"
#include "sbplugin_system.h"
#include "openapps_log_api.h"
#include "sal/commdefs.h"
#include "opennsl/types.h"
#include "opennsl/init.h"
#include "opennsl/switch.h"
#include "opennsl/port.h"
#include "opennsl/cosq.h"
#include "opennsl/stat.h"
#include "opennsl/error.h"
#include "shared/pbmp.h"
#include "opennsl/trunk.h"
#include "sbplugin.h"
#include "sbplugin_system.h"

#define  SBPLUGIN_NETWORK_OS   "openNSL" 

/* TRIDENT 2      */ 
#define SB_BRCM_56850_DEVICE_ID      0xb850
#define SB_BRCM_56851_DEVICE_ID      0xb851
#define SB_BRCM_56852_DEVICE_ID      0xb852
#define SB_BRCM_56853_DEVICE_ID      0xb853
#define SB_BRCM_56854_DEVICE_ID      0xb854
#define SB_BRCM_56855_DEVICE_ID      0xb855

/* Trident 2 plus */
#define SB_BRCM_56860_DEVICE_ID      0xb860
#define SB_BRCM_56861_DEVICE_ID      0xb861
#define SB_BRCM_56862_DEVICE_ID      0xb862
#define SB_BRCM_56864_DEVICE_ID      0xb864
#define SB_BRCM_56865_DEVICE_ID      0xb865
#define SB_BRCM_56867_DEVICE_ID      0xb867
#define SB_BRCM_56760_DEVICE_ID      0xb760
#define SB_BRCM_56832_DEVICE_ID      0xb832
#define SB_BRCM_56833_DEVICE_ID      0xb833
#define SB_BRCM_56865_DEVICE_ID      0xb865
#define SB_BRCM_56833_DEVICE_ID      0xb833
/* Tomahawk */
#define SB_BRCM_56960_DEVICE_ID      0xb960
#define SB_BRCM_56961_DEVICE_ID      0xb961
#define SB_BRCM_56962_DEVICE_ID      0xb962
#define SB_BRCM_56965_DEVICE_ID      0xb965




#define SB_BRCM_IS_CHIP_TD2(_info)                                               \
                                   (((_info).device == SB_BRCM_56850_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56851_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56852_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56853_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56854_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56855_DEVICE_ID)) 


#define SB_BRCM_IS_CHIP_TD2_PLUS(_info)                                          \
                                   (((_info).device == SB_BRCM_56860_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56861_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56862_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56864_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56865_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56867_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56760_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56832_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56833_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56865_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56833_DEVICE_ID)) 

#define SB_BRCM_IS_CHIP_TH(_info)                                                \
                                   (((_info).device == SB_BRCM_56960_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56961_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56962_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56965_DEVICE_ID)) 

#define SB_BRCM_E_NONE          OPENNSL_E_NONE            

#define SB_BRCM_RV_ERROR(_rv)     ((_rv) < 0)

#define SB_BRCM_PBMP_COUNT(_port_bit_map, _count)     OPENNSL_PBMP_COUNT((_port_bit_map), (_count))

#define SB_BRCM_CPU_PORT_GET(_unit, _port_ptr)                                                \
                             {                                                              \
                                int _port = 0;                                               \
                                while(_port < OPENNSL_PBMP_PORT_MAX)                             \
                                {                                                           \
                                  if (OPENNSL_PBMP_MEMBER(sb_brcm_port_config[(_unit)].cpu, _port)) \
                                  {                                                         \
                                    *(_port_ptr) = _port;                                       \
                                    break;                                                 \
                                  }                                                         \
                                  _port++;                                                   \
                                }                                                           \
                             }

#define SB_BRCM_IS_XE_PORT(_unit, _port)                                            \
                          (OPENNSL_PBMP_MEMBER(sb_brcm_port_config[(_unit)].xe, (_port)))    
						   
#define SB_BRCM_API_PORT_GPORT_GET(_unit, _port, _gport_ptr)                        \
                                opennsl_port_gport_get((_unit), (_port), (_gport_ptr))

#define SB_BRCM_API_SWITCH_CONTROL_GET(_unit, _control_type, _arg_ptr)                 \
                             opennsl_switch_control_get((_unit), (_control_type), (_arg_ptr)) 

#define SB_BRCM_API_SWITCH_CONTROL_SET(_unit, _control_type, _arg)                     \
                             opennsl_switch_control_set((_unit), (_control_type), (_arg)) 


#define SB_BRCM_API_UNIT_INFO_GET(_unit,_info_ptr)      opennsl_info_get((_unit),(_info_ptr))


#define SOC_BRCM_API_ATTACH_MAX_UNITS_GET(_max_units_ptr)  { opennsl_attach_max((int *)_max_units_ptr);    \
                                                   *(_max_units_ptr) = (*(_max_units_ptr))+1; }


#define SB_BRCM_API_UNIT_CHECK(_unit)              \
                              opennsl_attach_check(_unit)

#define SB_BRCM_API_PORT_CONFIG_GET(_unit,_port_cfg_ptr)           \
                  opennsl_port_config_get(_unit, _port_cfg_ptr)  

#define SB_BRCM_SWITCH_EVENT_REGISTER(_unit, _cb, _user_data_ptr)     \
            opennsl_switch_event_register(_unit, _cb, _user_data_ptr)



typedef opennsl_gport_t             SB_BRCM_GPORT_t;
typedef opennsl_info_t              SB_BRCM_UNIT_INFO_t;
typedef opennsl_port_config_t       SB_BRCM_PORT_CONFIG_t;
typedef opennsl_switch_event_t      SB_BRCM_SWITCH_EVENT_t;
typedef opennsl_switch_event_cb_t   SB_BRCM_SWITCH_EVENT_CB_t;


extern SB_BRCM_PORT_CONFIG_t     sb_brcm_port_config[];
#ifdef __cplusplus
}
#endif
#endif
