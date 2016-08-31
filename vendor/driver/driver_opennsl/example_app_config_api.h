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

#ifndef EXAMPLE_APP_CONFIG_API_H 
#define EXAMPLE_APP_CONFIG_API_H 

#include "opennsl/types.h"
#include "opennsl/error.h"
#include "opennsl/port.h"
#include "opennsl/vlan.h"
#include "opennsl/trunk.h"
#include "opennsl/l3.h"


typedef struct example_intf_s {
  int ingress_intf;
  int egress_intf;
  unsigned int ipaddr;
  unsigned int subnet_ip;
  unsigned int next_hop;
  unsigned int vid;
  unsigned int port;
  bool in_use;
} example_intf_t;

typedef struct example_route_s {
  unsigned int subnet_ip;
  unsigned int subnet_mask;
  unsigned int next_hop;
  int egr_intf;
  int egr_ecmp_intf;
  bool in_use;
} example_route_t;

typedef struct example_ecmp_route_s {
  unsigned int subnet_ip;
  unsigned int subnet_mask;
  int ecmp_egr_intf;
  int member_count;
  bool in_use;
} example_ecmp_route_t;

typedef struct example_arp_s {
  unsigned int ipaddr;
  int port;
  int egr_intf;
  int ingress_intf;
  bool in_use;
} example_arp_t;




/**************************************************************************//**
* \brief Adds a port to vlan 
*
* \param    unit [IN]    Unit number.
* \param    vid [IN]     vlan id 
* \param    port [IN]    member port 
* \param    isUntagged [IN]   1- if the port is untagged member 0 - tagged member. 
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int example_app_opennsl_vlan_member_add(unsigned int unit, unsigned int vid, unsigned int port);


/**************************************************************************//**
* \brief removes a port from vlan 
*
* \param    unit [IN]    Unit number.
* \param    vid [IN]     vlan id 
* \param    port [IN]    member port 
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int example_app_opennsl_vlan_member_remove(unsigned int unit, unsigned int vid, unsigned int port);


/**************************************************************************//**
* \brief  example app routine to create l3 interface 
*
* \param    unit [IN]  
* \param    vid [IN]  
* \param    my_mac_addr [IN]  
* \param    ipaddr [IN]  
* \param    mask [IN]  
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int example_app_opennsl_l3_intf_create(unsigned int unit, unsigned int vid, unsigned int port, macAddr_t *my_mac_addr, 
                                        unsigned int ipaddr, unsigned int mask);

/**************************************************************************//**
* \brief  example app routine to l3 interface 
*
* \param    vid [IN]  
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int example_app_opennsl_l3_intf_delete(unsigned int vid);

/**************************************************************************//**
* \brief  example app routine to create route 
*
* \param    subnet_ip [IN]  
* \param    subnet_mask [IN]  
* \param    next_hop [IN]  
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int example_app_opennsl_route_add(unsigned int subnet_ip, unsigned int subnet_mask, unsigned int next_hop);

/**************************************************************************//**
* \brief  example app routine to delete route 
*
* \param    subnet_ip [IN]  
* \param    subnet_mask [IN]  
* \param    next_hop [IN]  
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int example_app_opennsl_route_delete(unsigned int subnet_ip, unsigned int subnet_mask, unsigned int next_hop);

/**************************************************************************//**
* \brief  delets the member to the trunk id 
*
* \param    unit [IN]    Unit number.
* \param    lag_id [IN]  trunk id   
* \param    port [IN]    member port 
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int example_app_opennsl_trunk_member_delete(unsigned int unit, unsigned int lag_id, int port);

/**************************************************************************//**
* \brief  adds the member to the trunk id 
*
* \param    unit [IN]    Unit number.
* \param    lag_id [IN]  trunk id   
* \param    port [IN]    member port 
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int example_app_opennsl_trunk_member_add(unsigned int unit, unsigned int lag_id, int port);

/**************************************************************************//**
* \brief creates the trunk id 
*
* \param    unit [IN]    Unit number.
* \param    lag_id [IN]  trunk id   
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int example_app_opennsl_trunk_create(unsigned int unit, unsigned int lag_id);

/**************************************************************************//**
* \brief deletes the trunk id 
*
* \param    unit [IN]    Unit number.
* \param    lag_id [IN]  trunk id   
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int example_app_opennsl_trunk_delete(unsigned int unit, unsigned int lag_id);
/**************************************************************************//**
* \brief  example app routine to add arp 
*
* \param    ipaddr [IN]  
* \param    next_hop_mac_addr [IN]  
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int example_app_opennsl_arp_add(unsigned int unit, unsigned int ipaddr, 
                                        macAddr_t *next_hop_mac_addr);
/**************************************************************************//**
* \brief  example app routine to delete arp 
*
* \param    ipaddr [IN]  
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int example_app_opennsl_arp_delete(unsigned int unit, unsigned int ipaddr) ;

#define EXAMPLE_APP_VLAN_CREATE           opennsl_vlan_create
#define EXAMPLE_APP_VLAN_DESTROY          opennsl_vlan_destroy
#define EXAMPLE_APP_TRUNK_DESTROY         example_app_opennsl_trunk_delete
#define EXAMPLE_APP_TRUNK_MEMBER_ADD      example_app_opennsl_trunk_member_add
#define EXAMPLE_APP_TRUNK_MEMBER_DELETE   example_app_opennsl_trunk_member_delete
#define EXAMPLE_APP_TRUNK_CREATE          example_app_opennsl_trunk_create
#define EXAMPLE_APP_VLAN_MEMBER_ADD       example_app_opennsl_vlan_member_add
#define EXAMPLE_APP_VLAN_MEMBER_DELETE    example_app_opennsl_vlan_member_remove
#define EXAMPLE_APP_L3_INTF_ADD           example_app_opennsl_l3_intf_create
#define EXAMPLE_APP_L3_INTF_DELETE        example_app_opennsl_l3_intf_delete
#define EXAMPLE_APP_L3_ROUTE_ADD          example_app_opennsl_route_add
#define EXAMPLE_APP_L3_ROUTE_DELETE       example_app_opennsl_route_delete
#define EXAMPLE_APP_L3_ARP_ADD            example_app_opennsl_arp_add
#define EXAMPLE_APP_L3_ARP_DELETE         example_app_opennsl_arp_delete

#endif


