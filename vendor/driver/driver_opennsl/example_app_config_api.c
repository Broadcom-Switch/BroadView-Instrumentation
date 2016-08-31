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
#include <stdbool.h>
#include <string.h>
#include "driver_util_menu.h"
#include "example_server.h"
#include "opennsl/types.h"
#include "opennsl/error.h"
#include "opennsl/port.h"
#include "opennsl/vlan.h"
#include "opennsl/trunk.h"
#include <opennsl/l2.h>
#include "opennsl/l3.h"
#include "example_app_config_api.h"


example_intf_t l3_intf_table[EXAMPLE_APP_MAX_TABLE_SIZE+1];
example_route_t l3_route_table[EXAMPLE_APP_MAX_TABLE_SIZE+1];
example_ecmp_route_t l3_ecmp_route_table[EXAMPLE_APP_MAX_TABLE_SIZE+1];
example_arp_t l3_arp_table[EXAMPLE_APP_MAX_TABLE_SIZE+1];

int example_app_lag_id_map[EXAMPLE_APP_MAX_LAG] = {0xFFFFFFFF};


/* debug prints */
int example_app_debug = 0;


/**************************************************************************//**
* \brief Adds a port to vlan 
*
* \param    unit [IN]    Unit number.
* \param    vid [IN]     vlan id 
* \param    port [IN]    member port 
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/

int example_app_opennsl_vlan_member_add(unsigned int unit, unsigned int vid, unsigned int port)
{
  opennsl_pbmp_t pbmp;
  opennsl_pbmp_t ubmp;
  int rv;

  /* Add the test ports to the vlan */
  OPENNSL_PBMP_CLEAR(pbmp);
  OPENNSL_PBMP_CLEAR(ubmp);

  /* add the new port */
  OPENNSL_PBMP_PORT_ADD(pbmp, port);
  /* programme the same */
 rv = (opennsl_vlan_port_add(unit, vid, pbmp, ubmp));
 if (OPENNSL_E_NONE != rv)
 {
    return -1;
 }

  return 0;
}
/**************************************************************************//**
* \brief removes a port from vlan 
*
* \param    unit [IN]    Unit number.
* \param    vid [IN]     vlan id 
* \param    port [IN]    member port 
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int example_app_opennsl_vlan_member_remove(unsigned int unit, unsigned int vid, unsigned int port)
{
  opennsl_pbmp_t pbmp;
  opennsl_pbmp_t ubmp;

  /* Add the test ports to the vlan */
  OPENNSL_PBMP_CLEAR(pbmp);
  OPENNSL_PBMP_CLEAR(ubmp);


  OPENNSL_PBMP_PORT_ADD(pbmp, port);
  /* programme the same */
  if (OPENNSL_E_NONE != (opennsl_vlan_port_remove(unit, vid, pbmp)))
    return -1;


  OPENNSL_PBMP_PORT_ADD(pbmp, port);
  OPENNSL_PBMP_PORT_ADD(ubmp, port);

  /* programme the same */
  if (OPENNSL_E_NONE != (opennsl_vlan_port_add(unit, 1, pbmp, ubmp)))
    return -1;
    return 0;
}
/**************************************************************************//**
* \brief creates the trunk id 
*
* \param    unit [IN]    Unit number.
* \param    lag_id [IN]  trunk id   
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int example_app_opennsl_trunk_create(unsigned int unit, unsigned int lag_id)
{
  unsigned int flags = 0;
  int id = 0;
  int rv = 0;

  flags = OPENNSL_TRUNK_FLAG_WITH_ID;

   rv = opennsl_trunk_create(unit, flags, &id);
   if ((OPENNSL_E_NONE != rv) && (OPENNSL_E_EXISTS != rv))
   { 
     return -1;
   }
   example_app_lag_id_map[lag_id-1] = id;
    return 0;
}
/**************************************************************************//**
* \brief deletes the trunk id 
*
* \param    unit [IN]    Unit number.
* \param    lag_id [IN]  trunk id   
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int example_app_opennsl_trunk_delete(unsigned int unit, unsigned int lag_id)
{
  int id = 0;

  id = example_app_lag_id_map[lag_id];
  if (OPENNSL_E_NONE != (opennsl_trunk_destroy(unit, id)))
    return -1;

    return 0;
}
/**************************************************************************//**
* \brief  adds the member to the trunk id 
*
* \param    unit [IN]    Unit number.
* \param    lag_id [IN]  trunk id   
* \param    port [IN]    member port 
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int example_app_opennsl_trunk_member_add(unsigned int unit, unsigned int lag_id, int port)
{
  int id = 0;
  int gport = 0;
  opennsl_trunk_member_t lag_member;
  int rv = 0;


  memset(&lag_member, 0, sizeof(opennsl_trunk_member_t));


   if (OPENNSL_E_NONE != (opennsl_port_gport_get(unit, port, &gport)))
   {
     return -1;
   }

  id = example_app_lag_id_map[lag_id];

  lag_member.gport = gport;

   rv = opennsl_trunk_member_add(unit, id, &lag_member);
   if ((OPENNSL_E_NONE != rv) && (OPENNSL_E_EXISTS != rv))
   { 
     return -1;
   }

    return 0;
}

/**************************************************************************//**
* \brief  delets the member to the trunk id 
*
* \param    unit [IN]    Unit number.
* \param    lag_id [IN]  trunk id   
* \param    port [IN]    member port 
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int example_app_opennsl_trunk_member_delete(unsigned int unit, unsigned int lag_id, int port)
{
  int id = 0;
  int gport = 0;
  opennsl_trunk_member_t lag_member;


  memset(&lag_member, 0, sizeof(opennsl_trunk_member_t));


  if (OPENNSL_E_NONE != (opennsl_port_gport_get(unit, port, &gport)))
    return -1;

  id = example_app_lag_id_map[lag_id];

  lag_member.gport = gport;

  if (OPENNSL_E_NONE != (opennsl_trunk_member_delete(unit, id, &lag_member)))
    return -1;

    return 0;
}
/**************************************************************************//**
* \brief Add host address to the routing table
*
* \param    unit [IN]    Unit number.
* \param    addr [IN]    32 bit IP address value
* \param    intf [IN]    egress object created using example_create_l3_egress
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int add_host(int unit, unsigned int addr, int intf, opennsl_mac_t *mac_addr) {
  int rc;
  opennsl_l3_host_t l3host;
  opennsl_l3_host_t_init(&l3host);

  l3host.l3a_flags = 0;
  l3host.l3a_ip_addr = addr;
  l3host.l3a_intf = intf;

  rc = opennsl_l3_host_add(unit, &l3host);
  if (rc != OPENNSL_E_NONE) {
    printf ("opennsl_l3_host_add failed: %x \n", rc);
  }

  return rc;
}



/**************************************************************************//**
* \brief creates l3 interface  
*
* \param    unit [IN]    Unit number.
* \param    vlan [IN]    vlan number.
* \param    mac_l3_ingress [IN]   mac address  
* \param    intf [IN]    ingress object created using example_create_l3_egress
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/

int
create_l3_intf(int unit, int vlan, int port, opennsl_mac_t *mac_l3_ingress, int *intf)
{
  int rc;
  opennsl_pbmp_t pbmp;
  opennsl_pbmp_t ubmp;
  opennsl_l3_intf_t l3if, l3if_ori;

  rc = opennsl_vlan_create (unit, vlan);
  if ((OPENNSL_E_NONE != rc) && (OPENNSL_E_EXISTS != rc))
  {
    printf("opennsl_vlan_create %d: %s\n", rc, opennsl_errmsg(rc));
    return -1;
  }


  /* Add the test ports to the vlan */
  OPENNSL_PBMP_CLEAR(pbmp);
  OPENNSL_PBMP_CLEAR(ubmp);

  rc = (opennsl_vlan_port_add(unit, 1, pbmp, ubmp));
  if (OPENNSL_E_NONE != rc)
  {
    return -1;
  }

  /* add the new port */
  OPENNSL_PBMP_PORT_ADD(pbmp, port);
  rc = (opennsl_vlan_port_add(unit, vlan, pbmp, ubmp));
  if (OPENNSL_E_NONE != rc)
  {
    return -1;
  }


  /* create L3 interface on the vlan */
  opennsl_l3_intf_t_init(&l3if);

  /* before creating L3 INTF, check whether L3 INTF already exists*/
  opennsl_l3_intf_t_init(&l3if_ori);
  l3if_ori.l3a_intf_id = vlan;
  rc = opennsl_l3_intf_get(unit, &l3if_ori);
  if (rc == OPENNSL_E_NONE) {
    /* if L3 INTF already exists, replace it*/
    l3if.l3a_flags = OPENNSL_L3_REPLACE | OPENNSL_L3_WITH_ID;
    l3if.l3a_intf_id = vlan;
  }

  memcpy(l3if.l3a_mac_addr, mac_l3_ingress, 6);
  l3if.l3a_vid = vlan;
  l3if.l3a_flags |= OPENNSL_L3_ADD_TO_ARL;

  rc = opennsl_l3_intf_create (unit, &l3if);
  if (OPENNSL_FAILURE(rc)) {
    printf("opennsl_l3_intf_create %d: %s\n", rc, opennsl_errmsg(rc));
    return -1;
  }

  if (example_app_debug)
  {
    printf("L3 interface created, MAC 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x on VLAN 0x%x\n",
        l3if.l3a_mac_addr[0],
        l3if.l3a_mac_addr[1],
        l3if.l3a_mac_addr[2],
        l3if.l3a_mac_addr[3],
        l3if.l3a_mac_addr[4],
        l3if.l3a_mac_addr[5], vlan);

  }
  *intf = l3if.l3a_intf_id;

  return rc;
}

/**************************************************************************//**
* \brief creates l3 egress interface  
*
* \param    unit [IN]    Unit number.
* \param    vlan [IN]    vlan number.
* \param    ingress_intf [IN]    ingress intf number.
* \param    mac_l3_ingress [IN]   mac address  
* \param    intf [out]    egress object created
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int
create_l3_egress(int unit, uint32 flags, int vlan, int ingress_intf, opennsl_mac_t *mac_l3_egress, int *intf, int *encap_id, int port)
{
  opennsl_l3_egress_t l3eg;
  int rc;
  int mod = 0;

  unit = 0;
  opennsl_if_t l3egid;

  opennsl_l3_egress_t_init(&l3eg);
  l3eg.intf = ingress_intf;
  memcpy(l3eg.mac_addr, mac_l3_egress, 6);
  l3eg.vlan = vlan;
  l3eg.module = mod;
  l3eg.port = port;

  l3egid = *intf;

  rc = opennsl_l3_egress_create(unit, flags, &l3eg, &l3egid);
  if (OPENNSL_FAILURE(rc)) {
    printf("OPENNSL FAIL %d: %s\n", rc, opennsl_errmsg(rc));
    return -1;
  }

  if(example_app_debug)
  {
    printf("L3 egress created, MAC 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x on VLAN 0x%x\n",
        l3eg.mac_addr[0],
        l3eg.mac_addr[1],
        l3eg.mac_addr[2],
        l3eg.mac_addr[3],
        l3eg.mac_addr[4],
        l3eg.mac_addr[5], vlan);
  }
  *intf = l3egid;

  return rc;
}

/**************************************************************************//**
* \brief adds route to the routing interface  
*
* \param    unit [IN]  
* \param    intf [IN]  
* \param    route [IN]   
* \param    subnet_mask [IN]  
* \param    set_ecmp [IN]
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int
add_route (int unit, opennsl_if_t intf, unsigned int route, unsigned int subnet_mask, bool set_ecmp)
{
  opennsl_l3_route_t ecmp_l3rt;
  int rc, i;

  opennsl_l3_route_t_init(&ecmp_l3rt);

  if (true == set_ecmp)
  {
    ecmp_l3rt.l3a_flags = OPENNSL_L3_MULTIPATH;
  }
  ecmp_l3rt.l3a_subnet = route;
  ecmp_l3rt.l3a_ip_mask = subnet_mask;
  ecmp_l3rt.l3a_intf = intf;

  rc = opennsl_l3_route_add(unit, &ecmp_l3rt);
  if (OPENNSL_FAILURE(rc)) {
    printf("OPENNSL FAIL %d: %s\n", rc, opennsl_errmsg(rc));
    return -1;
  }
  if (example_app_debug)
  {
    printf("L3 multipath route created, 0x%08x (0x%08x)\n", ecmp_l3rt.l3a_subnet, ecmp_l3rt.l3a_ip_mask);
  }
  for (i = 0; i < EXAMPLE_APP_MAX_TABLE_SIZE; i++)
  {
    if (false == l3_route_table[i].in_use)
    {
      break;
    }
  }

  if (EXAMPLE_APP_MAX_TABLE_SIZE == i)
  {
    printf("can't add route entry.. Maximum routes exceeded\n");
    return -1;
  }

  return rc;
}

/**************************************************************************//**
* \brief find the ingress interface and vid in the l3 intf table using next hop  
*
* \param    unit [IN]  
* \param    nexthop [IN]  
* \param    ing_if [out]   
* \param    vid [out]   
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int nexthop_to_ingress_intf_get(unsigned int nexthop, int *ing_if, unsigned int *vid)
{
  int i = 0;

  for (i = 0; i < EXAMPLE_APP_MAX_TABLE_SIZE; i++)
  {
    if (true == l3_intf_table[i].in_use)
    {
      if (example_app_debug)
      {
        printf("l3_intf_table[i].subnet_ip %d, nexthop %d, (l3_intf_table[i].subnet_ip & nexthop) %d\n",
            l3_intf_table[i].subnet_ip,nexthop,(l3_intf_table[i].subnet_ip & nexthop));
      }
      if (l3_intf_table[i].subnet_ip == (l3_intf_table[i].subnet_ip & nexthop))
      {
        *ing_if = l3_intf_table[i].ingress_intf;
        *vid = l3_intf_table[i].vid;
        return 0;
      }
    }
  }

  return -1;
}


/**************************************************************************//**
* \brief find the ingress interface and vid in the l3 intf table using next hop  
*
* \param    unit [IN]  
* \param    nexthop [IN]  
* \param    ing_if [out]   
* \param    vid [out]   
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int nexthop_to_egress_intf_get(unsigned int nexthop, int *ing_if, unsigned int *vid)
{
  int i = 0;

  for (i = 0; i < EXAMPLE_APP_MAX_TABLE_SIZE; i++)
  {
    if (true == l3_arp_table[i].in_use)
      {
      if (l3_arp_table[i].ipaddr == nexthop)
      {
        *ing_if = l3_arp_table[i].egr_intf;
        return 0;
      }
    }
  }

  return -1;
}

/**************************************************************************//**
* \brief find the table index using interface number
*
* \param    unit [IN]  
* \param    type [IN] - table type 
* \param    inf [in]   
* \param    id [out]   
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int intf_to_table_entry_index_get(EXAMPLE_L3_TABLE_TYPE_t type, int inf, int *id)
{
  int i = 0;

  if (L3_INTF == type)
  {
    for (i = 0; i  < EXAMPLE_APP_MAX_TABLE_SIZE; i++)
    {
      if (inf == l3_intf_table[i].ingress_intf)
      {
        *id = i;
        return 0;
      }
    }
  }
  if (L3_ROUTING == type)
  {
    for (i = 0; i  < EXAMPLE_APP_MAX_TABLE_SIZE; i++)
    {
      if (l3_route_table[i].egr_intf == inf)
      {
        *id = i;
        return 0;
      }
    }
  }
  if (L3_ECMP_ROUTING == type)
  {
    for (i = 0; i  < EXAMPLE_APP_MAX_TABLE_SIZE; i++)
    {
      if (l3_ecmp_route_table[i].ecmp_egr_intf == inf)
      {
        *id = i;
        return 0;
      }
    }
  }
  return -1;
}
/**************************************************************************//**
* \brief find the table index using ip address
*
* \param    unit [IN]  
* \param    type [IN] - table type 
* \param    ipaddr [in]   
* \param    id [out]   
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int ip_to_table_index_get(EXAMPLE_L3_TABLE_TYPE_t type, unsigned int ipaddr, int *id)
{
  int i = 0;

  if (L3_INTF == type)
  {
    for (i = 0; i  < EXAMPLE_APP_MAX_TABLE_SIZE; i++)
    {
      if (ipaddr == l3_intf_table[i].ipaddr)
      {
        *id = i; 
        return 0;
      }
    }
  }
  if (L3_ROUTING == type)
  {
    for (i = 0; i  < EXAMPLE_APP_MAX_TABLE_SIZE; i++)
    {
      if (l3_route_table[i].next_hop == ipaddr)
      {
        *id = i;
        return 0;
      }
    }
  }
  if (L3_ECMP_ROUTING == type)
  {
    for (i = 0; i  < EXAMPLE_APP_MAX_TABLE_SIZE; i++)
    {
      if (l3_ecmp_route_table[i].subnet_ip == ipaddr)
      {
        *id = i;
        return 0;
      }
    }
  }
  return -1;
}
/**************************************************************************//**
* \brief find the table free index
*
* \param    unit [IN]  
* \param    type [IN] - table type 
* \param    id [out]   
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int table_free_entry_index_get(EXAMPLE_L3_TABLE_TYPE_t type, int *id)
{
  int i = 0;

  if (L3_INTF == type)
  {
    for (i = 0; i  < EXAMPLE_APP_MAX_TABLE_SIZE; i++)
    {
      if (false == l3_intf_table[i].in_use)
      {
        *id = i;
        return 0;
      }
    }
  }
  if (L3_ROUTING == type)
  {
    for (i = 0; i  < EXAMPLE_APP_MAX_TABLE_SIZE; i++)
    {
      if (l3_route_table[i].in_use == false)
      {
        *id = i;
        return 0;
      }
    }
  }
  if (L3_ECMP_ROUTING == type)
  {
    for (i = 0; i  < EXAMPLE_APP_MAX_TABLE_SIZE; i++)
    {
      if (l3_ecmp_route_table[i].in_use == false)
      {
        *id = i;
        return 0;
      }
    }
  }
  if (L3_ARP == type)
  {
    for (i = 0; i  < EXAMPLE_APP_MAX_TABLE_SIZE; i++)
    {
      if (l3_arp_table[i].in_use == false)
      {
        *id = i;
        return 0;
      }
    }
  }

  return -1;
}
/**************************************************************************//**
* \brief checks if the l3 intf exists for the vlan 
*
* \param    vid [IN]  
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int is_l3_intf_exist(unsigned int vid)
{
  int i;

  for (i = 0; i < EXAMPLE_APP_MAX_TABLE_SIZE; i++)
  {
    if ((l3_intf_table[i].vid == vid) && (true == l3_intf_table[i].in_use))
      return true;
  }
  return false;
}
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
int example_app_opennsl_l3_intf_create(unsigned int unit, unsigned int vid, unsigned int port,
                                        macAddr_t *my_mac_addr, 
                                        unsigned int ipaddr, unsigned int mask)
{
  int l3_intf = 0, egr_intf = 0, id =0;
  opennsl_mac_t my_mac, zero_mac;
  unsigned int flags = 0;

  unit = 0;
  memset(&zero_mac,0, 6);

  if (true == is_l3_intf_exist(vid))
  {
    printf("vlan interface already exists !!\n");
    return 0;
  }
  /* Set L3 Egress Mode */
  if (OPENNSL_E_NONE != opennsl_switch_control_set(unit, opennslSwitchL3EgressMode, 1))
  {
    return -1;
  }
  if(example_app_debug >= 3) {
    printf("\nL3 Egress mode is set succesfully\n");
  }

  /* remove the port from VLAN 1 and add as member if the vlan */


  memcpy (&my_mac, my_mac_addr, 6);
  if (OPENNSL_E_NONE !=  create_l3_intf(unit, vid, port, &my_mac, &l3_intf))
  {
    return -1;
  }
  if (OPENNSL_E_NONE != create_l3_egress(unit, flags, vid, l3_intf, &my_mac, &egr_intf, &id, 0))
  {
    return -1;
  }

  if (OPENNSL_E_NONE != add_host(unit, ipaddr, egr_intf, &zero_mac))
  {
    return -1;
  }

   /* add entry defipt tabble */
  if (OPENNSL_E_NONE != add_route(unit, egr_intf, ipaddr, mask, 0))
  {
    return -1;
  }


  if (0 != table_free_entry_index_get(L3_INTF, &id))
    return -1;


  l3_intf_table[id].ingress_intf = l3_intf;
  l3_intf_table[id].egress_intf = egr_intf;
  l3_intf_table[id].ipaddr = ipaddr;
  l3_intf_table[id].subnet_ip = (ipaddr & mask);
  l3_intf_table[id].vid = vid;
  l3_intf_table[id].port = port;
  l3_intf_table[id].in_use = true;
  return 0;
}
/**************************************************************************//**
* \brief  example app routine to create route 
*
* \param    subnet_ip [IN]  
* \param    subnet_mask [IN]  
* \param    next_hop [IN]  
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int example_app_opennsl_route_add(unsigned int subnet_ip, unsigned int subnet_mask, unsigned int next_hop)
{
  int egr_intf[EXAMPLE_APP_MAX_TABLE_SIZE];
  int i, count = 0, egr_ecmp_intf = 0, egr_if, l3_intf;
  int rc, id;
  opennsl_l3_egress_ecmp_t ecmp;
  opennsl_mac_t my_mac;
  unsigned int vid;
 /* unsigned int flags = 0; */
  opennsl_l3_route_t  route_info;

  memset(&my_mac, 1, sizeof(opennsl_mac_t));

  /* check if the arp entry for the next hop is resolved */
  for (i = 0; i < EXAMPLE_APP_MAX_TABLE_SIZE; i++)
  {
    if (l3_arp_table[i].ipaddr == next_hop)
    {
      break;
    }
  }

  if (i == EXAMPLE_APP_MAX_TABLE_SIZE)
  {
    /* next hop not resolved
        Allow to add route only
       after adding the arp entry */

    printf("Can't add route . Arp for next hop not resolved!!\n");
    return 0;
  }


  /* Check if already route is already present */
  for (i = 0; i < EXAMPLE_APP_MAX_TABLE_SIZE; i++)
  {
    if ((l3_route_table[i].subnet_ip == subnet_ip) && 
        (l3_route_table[i].subnet_mask == subnet_mask))
    {
      if (l3_route_table[i].next_hop == next_hop)
        return 0;

      egr_intf[count] = l3_route_table[i].egr_intf;
      if (0 != l3_route_table[i].egr_ecmp_intf)
      {
        egr_ecmp_intf = l3_route_table[i].egr_ecmp_intf;
      }
      count++;
    }
  }

  /* new route */
  /* find the corresponding ingress interface */
  if (0 != nexthop_to_egress_intf_get(next_hop, &l3_intf, &vid))
  {
    printf("failed to find the ingress interface \n");
    return -1;
  }

      egr_if = l3_intf;

  if (0 == count)
  {
    /* not an ecmp route */
    if ( 0 == add_route(0, l3_intf, subnet_ip, subnet_mask, 0))
    {
      i = 0xFF;
      if (0 == table_free_entry_index_get(L3_ROUTING, &i))
      {
        l3_route_table[i].subnet_ip = subnet_ip;
        l3_route_table[i].subnet_mask = subnet_mask;
        l3_route_table[i].next_hop = next_hop;
        l3_route_table[i].egr_intf = l3_intf;
        l3_route_table[i].in_use = true;
        l3_route_table[i].egr_ecmp_intf = 0;
        return 0;
      }
      else
      {
        printf("Unable to find free index in software routing table \n");
        return -1;
      }
    }
  }
  else
  {
    /* ecmp */
    if ( 1 == count)
    {
      egr_intf[count] = egr_if;

      /* delete the existing route entry and move the same to ecmp group */
      opennsl_l3_route_t_init(&route_info);
      route_info.l3a_intf = egr_intf[0];
      route_info.l3a_subnet = subnet_ip;
      route_info.l3a_ip_mask = subnet_mask;
      if (0 != opennsl_l3_route_delete(0, &route_info))
      {
        printf("failed to delete, 0x%08x (0x%08x)\n", route_info.l3a_subnet, route_info.l3a_ip_mask);
      }

      /* create the ecmp interface and add the existing and new routes */
      opennsl_l3_egress_ecmp_t_init(&ecmp);

      if (example_app_debug)
      {
        for (i = 0; i< count+1; i++)
          printf(" intf id = %d\n",egr_intf[i]);
        printf("count = %d\n", count+1);
        printf("egr_ecmp_intf = %d\n", egr_ecmp_intf);
      }

      ecmp.max_paths = EXAMPLE_APP_MAX_MULTIPATHS;
      /* create ECMP L3 egress */
      rc = opennsl_l3_egress_ecmp_create(0, &ecmp, count+1, egr_intf);

      if (OPENNSL_FAILURE(rc)) {
        printf("opennsl_l3_egress_multipath_create\n");
        printf("OPENNSL FAIL %d: %s\n", rc, opennsl_errmsg(rc));
        return -1;
      }
      if (example_app_debug)
      {
        printf ("L3 egress multipath created: %d\n", ecmp.ecmp_intf);
      }

      /* add route to ecmp */
      rc = add_route(0,ecmp.ecmp_intf, subnet_ip, subnet_mask, 1);

      if (rc!=OPENNSL_E_NONE)
      {
        printf("add_route failed\n");
        return rc;
      }

      count++;
      for (i = 0; i < count-1; i++)
      {
        if (0 != intf_to_table_entry_index_get(L3_ROUTING, egr_intf[i], &id))
          return -1;
        l3_route_table[id].egr_ecmp_intf = ecmp.ecmp_intf;
      }
      id = 0xFF;
      if (0 == table_free_entry_index_get(L3_ROUTING, &id))
      {
        l3_route_table[id].subnet_ip = subnet_ip;
        l3_route_table[id].subnet_mask = subnet_mask;
        l3_route_table[id].next_hop = next_hop;
        l3_route_table[id].egr_intf = egr_if;
        l3_route_table[id].in_use = true;
        l3_route_table[id].egr_ecmp_intf = ecmp.ecmp_intf;
      }
      if (0 == table_free_entry_index_get(L3_ECMP_ROUTING, &id))
      {
        l3_ecmp_route_table[id].subnet_ip = subnet_ip;
        l3_ecmp_route_table[id].subnet_mask = subnet_mask;
        l3_ecmp_route_table[id].ecmp_egr_intf = ecmp.ecmp_intf;
        l3_ecmp_route_table[id].in_use = true;
        l3_ecmp_route_table[id].member_count = count+1;
      }
    }
    else 
    {
      if (0 != ip_to_table_index_get(L3_ECMP_ROUTING, subnet_ip, &id))
      {
        printf("failed to find ecmp egress if\n");
      }

      ecmp.ecmp_intf = l3_ecmp_route_table[id].ecmp_egr_intf;
      ecmp.max_paths = EXAMPLE_APP_MAX_MULTIPATHS;

      if (count >= EXAMPLE_APP_MAX_MULTIPATHS)
      {
        return -1;
      }
      rc =  opennsl_l3_egress_ecmp_add(0, &ecmp, egr_if);
      if (OPENNSL_E_NONE != rc)
      {
        printf("rc: %d, opennsl_l3_egress_ecmp_add \n", rc);
        return -1;
      }

      l3_ecmp_route_table[id].member_count = count+1;

      if (0 == table_free_entry_index_get(L3_ROUTING, &id))
      {
        l3_route_table[id].subnet_ip = subnet_ip;
        l3_route_table[id].subnet_mask = subnet_mask;
        l3_route_table[id].next_hop = next_hop;
        l3_route_table[id].egr_intf = egr_if;
        l3_route_table[id].in_use = true;
        l3_route_table[id].egr_ecmp_intf = ecmp.ecmp_intf;
      }
    }
  }

  return 0;
}
/**************************************************************************//**
* \brief  example app routine to add arp 
*
* \param    ipaddr [IN]  
* \param    next_hop_mac_addr [IN]  
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int example_app_opennsl_arp_add(unsigned int unit, unsigned int ipaddr, 
                                        macAddr_t *next_hop_mac_addr)
{
  int i, id =0;
  int egr_intf, l3_intf;
  opennsl_mac_t next_hop_mac, zero_mac;
  unsigned int flags = 0;
  int port = 0;

  unit = 0;

  memcpy(&next_hop_mac, next_hop_mac_addr, 6);
  memset(&zero_mac, 0, 6);
  /* Check if already route is already present */
  for (i = 0; i < EXAMPLE_APP_MAX_TABLE_SIZE; i++)
  {
    if ((l3_intf_table[i].subnet_ip & ipaddr) == l3_intf_table[i].subnet_ip)
   /* if (l3_route_table[i].next_hop == ipaddr) */
    {
      egr_intf = l3_intf_table[i].egress_intf;
      l3_intf = l3_intf_table[i].ingress_intf;
      port = l3_intf_table[i].port;
      break;
    }
  }

  if (i >= EXAMPLE_APP_MAX_TABLE_SIZE)
  {
    printf ("Corresponding local interface not present\n");
    return -1;
  }

  if (OPENNSL_E_NONE != create_l3_egress(0, flags, 0, l3_intf, &next_hop_mac, &egr_intf, &id,port))
  {
    printf("%s %d, failed to create egress interface \n", __func__, __LINE__);
    return -1;
  }

  if (OPENNSL_E_NONE != add_host(unit, ipaddr, egr_intf, &zero_mac))
  {
    return -1;
  }

  /* get the free index in the arp table */
  if (-1 == table_free_entry_index_get(L3_ARP, &id))
    printf("Arp Table full\n");


  /* update the entry */
  l3_arp_table[id].ipaddr = ipaddr;
  l3_arp_table[id].port = port;
  l3_arp_table[id].ingress_intf = l3_intf;
  l3_arp_table[id].egr_intf = egr_intf;
  l3_arp_table[id].in_use = true;

  return 0;
}
/**************************************************************************//**
* \brief  example app routine to delete arp 
*
* \param    ipaddr [IN]  
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int example_app_opennsl_arp_delete(unsigned int unit, unsigned int ipaddr) 
{
  int l3_intf = 0, egr_intf = 0, id = 0;
  opennsl_l3_host_t host_info;
  unsigned int vid, flags = 0;
  opennsl_mac_t next_hop_mac;

  unit = 0;
  memset(&next_hop_mac, 0, sizeof(opennsl_mac_t));

  /* Check if already route is already present */
  /* find the corresponding egress interface */
  if (0 != nexthop_to_ingress_intf_get(ipaddr, &l3_intf, &vid))
  {
    printf("failed to find the ingress interface \n");
    return -1;
  }

  flags = (OPENNSL_L3_WITH_ID |  OPENNSL_L3_REPLACE);
  if (OPENNSL_E_NONE != create_l3_egress(0, flags, vid, l3_intf, &next_hop_mac, &egr_intf, &id, 0))
  {
    printf("%s %d, failed to create egress interface \n", __func__, __LINE__);
    return -1;
  }


  if (0 == ip_to_table_index_get(L3_ROUTING, ipaddr, &id))
  {
    l3_intf = l3_route_table[id].egr_intf;
    opennsl_l3_host_t_init (&host_info);
    host_info.l3a_intf = l3_intf;
    host_info.l3a_ip_addr = ipaddr;

    if (0  != opennsl_l3_host_delete(0, &host_info))
    {
      printf("failed to delete the host\n");
      return -1;
    }
  }
  return 0;
}
/**************************************************************************//**
* \brief  example app routine to delete route 
*
* \param    subnet_ip [IN]  
* \param    subnet_mask [IN]  
* \param    next_hop [IN]  
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int example_app_opennsl_route_delete(unsigned int subnet_ip, unsigned int subnet_mask, unsigned int next_hop)
{
  opennsl_l3_route_t  route_info;
  int i, id;
  int rc,egr_if, ecmp_egr_intf;
  opennsl_l3_egress_ecmp_t ecmp;

  /* check if the subnet ip is ecmp route */
  if (0 == ip_to_table_index_get(L3_ECMP_ROUTING, subnet_ip, &id))
  {
    ecmp_egr_intf = l3_ecmp_route_table[id].ecmp_egr_intf;
    ecmp.ecmp_intf = ecmp_egr_intf;
    ecmp.max_paths = EXAMPLE_APP_MAX_MULTIPATHS;
    if (0 != next_hop)
    {
      /* get the egress interface */
      if (0 != ip_to_table_index_get(L3_ROUTING, next_hop, &id))
      {
        printf("Couldn't find the route with next hop\n");
        return -1;
      }
      egr_if = l3_route_table[id].egr_intf;
      rc = opennsl_l3_egress_ecmp_delete(0, &ecmp, egr_if);
      if (OPENNSL_E_NONE != rc)
      {
        printf ("rc = %d, failed to delete the member from ecmp multipath\n", rc);
        return -1;
      }

      memset (&l3_route_table[id], 0, sizeof(example_route_t));
      ecmp.ecmp_intf = ecmp_egr_intf;
      ecmp.max_paths = EXAMPLE_APP_MAX_MULTIPATHS;

    }
    else
    {
      /* destroy the egress ecmp interface */
      rc = opennsl_l3_egress_ecmp_destroy(0, &ecmp);
      if (OPENNSL_E_NONE != rc)
      {
        printf ("rc = %d, failed to destroy the ecmp\n", rc);
        return -1;
      }

      /* update the l3 ecmp table
       */
      if (0 != ip_to_table_index_get(L3_ECMP_ROUTING, subnet_ip, &id))
      {
        printf("Couldn't find the ecmp route entry\n");
        return -1;
      }
      memset(&l3_ecmp_route_table[id], 0 , sizeof(example_ecmp_route_t));
      for (i = 0; i < EXAMPLE_APP_MAX_TABLE_SIZE; i++)
      {
        if (l3_route_table[i].egr_ecmp_intf == ecmp_egr_intf)
        {
          memset(&l3_route_table[id], 0 , sizeof(example_route_t));
        }
      }
    }
  }
  else
  {
    /* not an ecmp route */
    /* Check if already route is already present */
    for (i = 0; i < EXAMPLE_APP_MAX_TABLE_SIZE; i++)
    {
      if ((l3_route_table[i].subnet_ip == subnet_ip) && 
          (l3_route_table[i].subnet_mask == subnet_mask))
      {
        opennsl_l3_route_t_init(&route_info);
        route_info.l3a_intf = l3_route_table[i].egr_intf;
        route_info.l3a_subnet = subnet_ip;
        route_info.l3a_ip_mask = subnet_mask;
        if (0 != opennsl_l3_route_delete(0, &route_info))
        {
          printf("failed to delete, 0x%08x (0x%08x)\n", route_info.l3a_subnet, route_info.l3a_ip_mask);
        }
        /* Delete the egress interface */
        if ( 0 != opennsl_l3_egress_destroy (0, l3_route_table[i].egr_intf))
        {
          printf("failed to delete egress interface\n");
          return -1;
        }
        memset(&l3_route_table[i], 0, sizeof(example_route_t));
        return 0;
      }
    }
  }
  return 0;
}



/**************************************************************************//**
* \brief  example app routine to l3 interface 
*
* \param    vid [IN]  
*
* \return      OPENNSL_E_xxx  opennsl API return code
*****************************************************************************/
int example_app_opennsl_l3_intf_delete(unsigned int vid)
{
  int i, l3_intf;
  opennsl_l3_intf_t ingress_intf;


  for (i = 0; i < EXAMPLE_APP_MAX_TABLE_SIZE; i++)
  {
    if (vid == l3_intf_table[i].vid)
    {
      opennsl_l3_intf_t_init(&ingress_intf);
      l3_intf = l3_intf_table[i].egress_intf;

      /* Delete the egress interface */
      if ( 0 != opennsl_l3_egress_destroy (0, l3_intf))
      {
        printf("failed to delete egress interface\n");
        return -1;
      }
      ingress_intf.l3a_intf_id = l3_intf_table[i].ingress_intf;

      if (0 != opennsl_l3_intf_delete(0, &ingress_intf))
      {
        printf("failed to destroy the interface for %d\n", vid);
      }
      memset(&l3_intf_table[i], 0, sizeof(example_intf_t));
    }
  }
  return 0;
}

void example_app_data_init()
{
  memset (l3_intf_table, 0, sizeof(l3_intf_table));
  memset (l3_route_table, 0, sizeof(l3_route_table));
  memset (l3_ecmp_route_table, 0, sizeof(l3_ecmp_route_table));
  memset(l3_arp_table, 0, sizeof(l3_arp_table));
}
