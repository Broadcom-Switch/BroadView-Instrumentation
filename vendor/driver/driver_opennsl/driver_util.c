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
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sal/commdefs.h>
#include <sal/driver.h>
#include <opennsl/error.h>
#include <opennsl/cosq.h>
#include <opennsl/vlan.h>
#include <opennsl/switch.h>
#include <opennsl/link.h>
#include "example_server.h"
#include "broadview.h"
#include "asic.h"

#define opennslSwitchMcQueueSchedMode 1
#define opennslPortControlMmuTrafficEnable 1
#define opennslPortControlMmuDrain 1


#ifndef CDP_EXCLUDE
extern void bview_system_init_ph2(void *param);
#endif
#define soc_ndev 1
#define DEFAULT_VLAN 1


opennsl_gport_t c_gport [BVIEW_ASIC_MAX_PORTS];
opennsl_gport_t c_port_sched [BVIEW_ASIC_MAX_PORTS];
opennsl_gport_t c_l0_sched[BVIEW_ASIC_MAX_PORTS][5];
opennsl_gport_t c_l1_sched[BVIEW_ASIC_MAX_PORTS][10]; 
opennsl_gport_t c_Uqueue[BVIEW_ASIC_MAX_PORTS][12]; 
opennsl_gport_t c_Mqueue[BVIEW_ASIC_MAX_PORTS][48];

int c_port_child_wt [] = {1, 1, 1, 0, 0};
int c_l0_0_child_wt [] = {1, 1, 1, 1, 1, 1, 1, 1};

int c_is_l1_sp[BVIEW_ASIC_MAX_PORTS][8] = {{0}};
int c_num_l1_sp[BVIEW_ASIC_MAX_PORTS] = {0};
int c_l1_sp_offset[BVIEW_ASIC_MAX_PORTS][8] = {{0}};
int c_idx_start_at_least = 1;

int opennslDrvIsHspPort(int unit, int port)
{
    int max_speed = 0;


    if (opennsl_port_speed_max(unit, port, &max_speed) != OPENNSL_E_NONE)
    {
       return 0;
    }

    if (max_speed >= 40000)
    {
      return 1; 
    }
    return 0;
}

void AttachL1Nodes(int unit, int startPort, int endPort)
{
    int port = 0;
    int rc = 0;
    int l0_sp_idx = 2;
    int l1_sp_idx = 8;
    for(port = startPort; port <= endPort; port++)
    {
        rc = opennsl_cosq_gport_attach(unit,c_l1_sched[port][l1_sp_idx], 
                                   c_l0_sched[port][l0_sp_idx], -1);

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_attach returned error: %d on attaching c_l1_sched[port][%d]: 0x%x to c_l0_sched[port][%d]: 0x%x\r\n", rc, 
                    l1_sp_idx, c_l1_sched[port][l1_sp_idx], l0_sp_idx, 
                    c_l0_sched[port][l0_sp_idx]);

        }
        else
        {
 //           printf ("opennsl_cosq_gport_attach success on attaching c_l1_sched[port][%d]: 0x%x to c_l0_sched[port][%d]: 0x%x\r\n", 
   //                 l1_sp_idx, c_l1_sched[port][l1_sp_idx], l0_sp_idx, 
     //               c_l0_sched[port][l0_sp_idx]);
        }
    }

}
/*
 * HQoS parameters for all platforms except C9000.
 */
int c_num_l1_child = 2;
int c_num_l1_child_ctrl = 5;
int c_num_l2_uc_nodes = 12;
int c_l0_data_node_idx = 0;
int c_l1_ctrl_node_idx = 9;

int opennslCintTablesInit (void)
{
    return 0;
}

int numCpuCosQ = 12;
int opennslCintCpuHqosInit (int unit)
{
    int rc = 0;
    int cos = 0;
    int port = 0;

    /* Delete the default hierarchy of given port */ 
    rc = opennsl_port_gport_get(unit, port, &c_gport [port]);

    if (rc != OPENNSL_E_NONE)
    {
        printf ("opennsl_port_gport_get returned error: %d\r\n", rc);

        return rc;
    }
    else
    {
      //  printf ("opennsl_port_gport_get Successful. c_gport: 0x%x\r\n",  c_gport [port]);
    }

    rc = opennsl_cosq_gport_delete(unit, c_gport[port]);

    if (rc != OPENNSL_E_NONE)
    {
        printf ("opennsl_cosq_gport_delete returned error: %d\r\n", rc);

        return rc;
    }
    else
    {
     //   printf ("opennsl_cosq_gport_delete successful for c_gport: 0x%x\r\n", 
       //         c_gport[port]);
    }

    /* Create port scheduler. Specify port scheduler with 1 input */ 
    rc = opennsl_cosq_gport_add(unit, c_gport[port], 1, 0, &c_port_sched[port]);

    if (rc != OPENNSL_E_NONE)
    {
        printf ("opennsl_cosq_gport_add returned error: %d\r\n", rc);

        return rc;
    }
    else
    {
     //   printf ("opennsl_cosq_gport_add successful. c_port_sched: 0x%x\r\n", 
       //         c_port_sched[port]);
    }

    cos = 0;

    /* Create a L0 scheduler gport object with 1 input */ 
    rc = opennsl_cosq_gport_add (unit, c_gport[port], 1, 
                             OPENNSL_COSQ_GPORT_SCHEDULER, &c_l0_sched[port][cos]);

    if (rc != OPENNSL_E_NONE)
    {
        printf ("opennsl_cosq_gport_add returned error: %d\r\n", rc);

        return rc;
    }
    else
    {
     //   printf ("opennsl_cosq_gport_add Successful. c_l0_sched[port][%d]: 0x%x\r\n",
       //         cos, c_l0_sched[port] [cos]);
    }

    /* Create a L1 gport object with 12 inputs */ 
    rc = opennsl_cosq_gport_add(unit, c_gport[port], numCpuCosQ, 
                            OPENNSL_COSQ_GPORT_SCHEDULER, &c_l1_sched[port][cos]);

    if (rc != OPENNSL_E_NONE)
    {
        printf ("opennsl_cosq_gport_add returned error: %d\r\n", rc);

        return rc;
    }
    else
    {
     //   printf ("opennsl_cosq_gport_add Successful. c_l1_sched[port][%d]: 0x%x \r\n",
       //         cos, c_l1_sched[port][cos]);
    }

    /* Create 12 MC Queue Gports */ 
    for (cos = 0; cos < numCpuCosQ; cos++) 
    {
        rc = opennsl_cosq_gport_add(unit, c_gport[port], 1, 
                                OPENNSL_COSQ_GPORT_MCAST_QUEUE_GROUP, 
                                &c_Mqueue[port][cos]);

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_add returned error: %d\r\n", rc);

            return rc;
        }
        else
        {
     //       printf ("opennsl_cosq_gport_add Successful. c_Mqueue[port][%d]: 0x%x\r\n", cos, c_Mqueue[port][cos]);
        }
    }

    /* Attach L0 gport to port scheduler at input 0 */
    cos = 0;

    rc = opennsl_cosq_gport_attach(unit, c_l0_sched[port][cos], c_port_sched[port], 
                               -1);

    if (rc != OPENNSL_E_NONE)
    {
        printf ("opennsl_cosq_gport_attach returned error: %d\r\n", rc);

        return rc;
    }
    else
    {
     //   printf ("opennsl_cosq_gport_attach btw c_l0_sched[port][%d]: 0x%x and c_port_sched: 0x%x Successful\r\n", cos, c_l0_sched[port][cos], c_port_sched[port]);
    }

    /* Attach L1[0] to L0 [0] */
    rc = opennsl_cosq_gport_attach(unit, c_l1_sched[port][cos], 
                               c_l0_sched[port][0], -1); 

    if (rc != OPENNSL_E_NONE)
    {
        printf ("opennsl_cosq_gport_attach returned error: %d\r\n", rc);

        return rc;
    }
    else
    {
     //   printf ("opennsl_cosq_gport_attach btw c_l1_sched[port][%d]: 0x%x and c_l0_sched[port][0]: 0x%x Successful. \r\n", cos, c_l1_sched[port][cos], c_l0_sched[port][0]);
    }

    /* Multicast queues 0-11 are attached to L1 nodes 0 respectively. */ 
    for (cos = 0; cos < numCpuCosQ; cos++) 
    {
        rc = opennsl_cosq_gport_attach(unit, c_Mqueue[port][cos], c_l1_sched[port][0], 
                              -1);

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_attach returned error: %d\r\n", rc);

            return rc;
        }
        else
        {
     //       printf ("opennsl_cosq_gport_attach btw c_Mqueue[port] [%d]: 0x%x and c_l1_sched[port][0]: 0x%x Successful. \r\n", cos, c_Mqueue[port][cos], c_l1_sched[port][0]);
        }
    }

    return rc;
}

int opennslCintHqosDelete (int unit, int port)
{
    int rc = 0;

    /* Delete the default hierarchy of given port */ 
    rc = opennsl_port_gport_get(unit, port, &c_gport [port]);

    if (rc != OPENNSL_E_NONE)
    {
        printf ("opennsl_port_gport_get returned error: %d\r\n", rc);

        return rc;
    }
    else
    {
 //       printf ("opennsl_port_gport_get Successful. c_gport: 0x%x\r\n", 
   //             c_gport [port]);
    }

    rc = opennsl_cosq_gport_delete(unit, c_gport[port]);

    if (rc != OPENNSL_E_NONE)
    {
        printf ("opennsl_cosq_gport_delete returned error: %d\r\n", rc);

        return rc;
    }

    return 0;
}

int opennslCintHqosDfltInit (int unit, int port)
{
    int rc = 0;
    int cos;
    int mode = 0;
    int numChild = 0;
    int numPortChild = 4;
    int L0CtrlIndex = 3;
    int isHspPort = opennslDrvIsHspPort(unit,port);

    /*
     * This function gets and stores the GPORT for the given port in 
     * c_gport[port].
     */
    rc = opennslCintHqosDelete (unit, port);

    if (rc != OPENNSL_E_NONE)
    {
        printf ("HQoS port delete failed for port: %d, rc: %d\r\n", 
                port, rc);

    }
    else
    {
 //       printf ("HQoS port deleted for port: %d\r\n", port);
    }
    if(isHspPort)
    {
        numPortChild = 5;
        L0CtrlIndex = 4;
         c_l0_data_node_idx = 1;
    }
    else
    {
         c_l0_data_node_idx = 0;
    }

    /* Create port scheduler. Specify port scheduler with 1 input */ 
    rc = opennsl_cosq_gport_add(unit, c_gport[port], numPortChild, 0, &c_port_sched[port]);

    if (rc != OPENNSL_E_NONE)
    {
        printf ("opennsl_cosq_gport_add returned error: %d\r\n", rc);

        return rc;
    }
    else
    {
   //     printf ("opennsl_cosq_gport_add successful. c_port_sched: 0x%x\r\n", 
     //           c_port_sched[port]);
    }

    /* Create L0 scheduler gport object with 8 input */ 
    for (cos = 0; cos < numPortChild; cos++) 
    {
        ((cos == (numPortChild-1))?(numChild = 1):(numChild=8));
        rc = opennsl_cosq_gport_add (unit, c_gport[port],numChild, 
                                 OPENNSL_COSQ_GPORT_SCHEDULER, &c_l0_sched[port] [cos]);

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_add returned error: %d\r\n", rc);

            return rc;
        }
        else
        {
     //       printf ("opennsl_cosq_gport_add Successful. c_l0_sched[port][%d]: 0x%x\r\n",
       //             cos, c_l0_sched[port] [cos]);
        }
    }

    /* Create first 10 L1 gport objects */ 
    for (cos = 0;  cos < 10; cos++) 
    {
        rc = opennsl_cosq_gport_add(unit, c_gport[port], ((cos !=9)? c_num_l1_child:c_num_l1_child_ctrl), 
                                OPENNSL_COSQ_GPORT_SCHEDULER, &c_l1_sched[port][cos]);

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_add returned error: %d\r\n", rc);

            return rc;
        }
        else
        {
     //       printf ("opennsl_cosq_gport_add Successful. c_l1_sched[port][%d]: 0x%x\r\n", cos, c_l1_sched[port][cos]);
        }
    } 

    /* Create UC queue gports */ 
    if(isHspPort)
        c_num_l2_uc_nodes = 9;
    else
        c_num_l2_uc_nodes = 12;

    for (cos = 0; cos < c_num_l2_uc_nodes; cos++) 
    {
        rc = opennsl_cosq_gport_add(unit, c_gport[port], 1, 
                                OPENNSL_COSQ_GPORT_UCAST_QUEUE_GROUP, 
                                &c_Uqueue[port][cos]);

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_add returned error: %d\r\n", rc);

            return rc;
        }
        else
        {
     //       printf ("opennsl_cosq_gport_add Successful. c_Uqueue[port][%d]: 0x%x\r\n", 
       //             cos, c_Uqueue[port][cos]);
        }
    }

    /* Create 9 MC queue gports and attach to correspoding L1 gports at COS1 */ 
    for (cos = 0; cos < 9; cos++) 
    {
        rc = opennsl_cosq_gport_add(unit, c_gport[port], 1, 
                                OPENNSL_COSQ_GPORT_MCAST_QUEUE_GROUP, 
                                &c_Mqueue[port][cos]);

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_add returned error: %d\r\n", rc);

            return rc;
        }
        else
        {
     //       printf ("opennsl_cosq_gport_add Successful. c_Mqueue[port][%d]: 0x%x\r\n", 
       //             cos, c_Mqueue[port][cos]);
        }
    }

    /* Attach L0 gport to port scheduler at input 0 */ 
    for (cos = 0; cos < numPortChild; cos++) 
    {
        rc = opennsl_cosq_gport_attach(unit, c_l0_sched[port][cos], c_port_sched[port], 
                                   -1);

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_attach returned error: %d\r\n", rc);

            return rc;
        }
        else
        {
     //       printf ("opennsl_cosq_gport_attach btw c_l0_sched[port][%d]: 0x%x and c_port_sched: 0x%x Successful.\r\n", cos, c_l0_sched[port][cos], c_port_sched[port]);
        }
    }

    /* Attach L1[0] to L1[7]  gport to L0 data node Index */
    for (cos = 0; cos < 8; cos++)
    {
        rc = opennsl_cosq_gport_attach(unit, c_l1_sched[port][cos], c_l0_sched[port][c_l0_data_node_idx], -1); 

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_attach returned error: %d\r\n", rc);

            return rc;
        }
        else
        {
     //       printf ("opennsl_cosq_gport_attach btw c_l1_sched[port][%d]: 0x%x and c_l0_sched[port][%d]: 0x%x Successful. \r\n", cos, c_l1_sched[port][cos], c_l0_data_node_idx,
       //             c_l0_sched[port][c_l0_data_node_idx]);
        }
    } 

    /* L1 control node attached to L0 [4] */
    rc = opennsl_cosq_gport_attach(unit, c_l1_sched[port][c_l1_ctrl_node_idx], c_l0_sched[port][L0CtrlIndex], -1); 

    if (rc != OPENNSL_E_NONE)
    {
        printf ("opennsl_cosq_gport_attach returned error: %d\r\n", rc);

        return rc;
    }
    else
    {
     //   printf ("opennsl_cosq_gport_attach btw c_l1_sched[port][%d]: 0x%x and c_l0_sched[port][3]: 0x%x Successful. \r\n", c_l1_ctrl_node_idx, c_l1_sched[port][c_l1_ctrl_node_idx], 
       //         c_l0_sched[port][L0CtrlIndex]);
    }

    /* Unicast queues 0-8 are attached to L1 nodes 0-8 respectively. */
    for (cos = 0; cos < 8; cos++) 
    {
        rc = opennsl_cosq_gport_attach(unit, c_Uqueue[port][cos], c_l1_sched[port][cos], -1); 

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_attach returned error: %d\r\n", rc);

            return rc;
        }
        else
        {
     //       printf ("opennsl_cosq_gport_attach btw c_Uqueue[port] [%d]: 0x%x and c_l1_sched[port][%d]: 0x%x Successful. \r\n", cos, 
       //             c_Uqueue[port][cos], cos, c_l1_sched[port][cos]);
        }
    } 

    /* Multicast queues 0-8 are attached to L1 nodes 0-8 respectively. */ 
    for (cos = 0; cos < 8; cos++) 
    {
        rc = opennsl_cosq_gport_attach(unit, c_Mqueue[port][cos], c_l1_sched[port][cos], -1);

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_attach returned error: %d\r\n", rc);

            return rc;
        }
        else
        {
     //       printf ("opennsl_cosq_gport_attach btw c_Mqueue[port] [%d]: 0x%x and c_l1_sched[port][%d]: 0x%x Successful. \r\n", cos, 
       //             c_Mqueue[port][cos], cos, c_l1_sched[port][cos]);
        }
    }


    /* Unicast queues 8-11 are attached to L1 node 9. */   
    for (cos = 8; cos < ((isHspPort)?9:12); cos++)
    {
        rc = opennsl_cosq_gport_attach(unit, c_Uqueue[port][cos], c_l1_sched[port][9], -1); 

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_attach returned error: %d\r\n", rc);

            return rc;
        }
        else
        {
     //       printf ("opennsl_cosq_gport_attach btw c_Uqueue[port] [%d]: 0x%x and c_l1_sched[port][9]: 0x%x Successful. \r\n", cos, 
       //             c_Uqueue[port][cos], c_l1_sched[port][9]);
        }
    }

    /* Multicast queue 8 is attached to L1 node 9. */   
    rc = opennsl_cosq_gport_attach(unit, c_Mqueue[port][8], c_l1_sched[port][9], -1); 

    if (rc != OPENNSL_E_NONE)
    {
        printf ("opennsl_cosq_gport_attach returned error: %d\r\n", rc);

        return rc;
    }
    else
    {
     //   printf ("opennsl_cosq_gport_attach btw c_Mqueue[port] [8]: 0x%x and c_l1_sched[port][9]: 0x%x Successful. \r\n", c_Mqueue[port][8], 
       //         c_l1_sched[port][9]);
    }
    
    for (cos = 0; cos < numPortChild; cos++)
    {    
        mode = ((c_port_child_wt [cos])? (OPENNSL_COSQ_DEFICIT_ROUND_ROBIN) : 
                (OPENNSL_COSQ_STRICT)); 

        rc = opennsl_cosq_gport_sched_set(unit, c_port_sched[port], cos, mode, 
                                      c_port_child_wt [cos]);

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_sched_set failed with err=%d for portSchd=0x%x idx=%d\r\n", rc, c_port_sched[port], cos);
        }
        else
        {
     //       printf ("opennsl_cosq_gport_sched_set success for portSchd=0x%x idx=%d\r\n", c_port_sched[port], cos);
        }
    }
    
    for (cos = 0; cos < 8; cos++)
    {
        mode = ((c_l0_0_child_wt [cos])? (OPENNSL_COSQ_DEFICIT_ROUND_ROBIN) : 
                (OPENNSL_COSQ_STRICT)); 

        rc = opennsl_cosq_gport_sched_set(unit, c_l0_sched[port][c_l0_data_node_idx], cos, mode,  
                                      c_l0_0_child_wt [cos]);

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_sched_set failed with err=%d for L0[%d]=0x%x idx=%d\r\n", rc, c_l0_data_node_idx, c_l0_sched[port][c_l0_data_node_idx], cos);
        }
        else
        {
     //       printf ("opennsl_cosq_gport_sched_set success for L0[%d]=0x%x idx=%d\r\n", c_l0_data_node_idx, c_l0_sched[port][c_l0_data_node_idx], cos);
        }
    }
    
    rc = opennsl_cosq_gport_sched_set(unit, c_l0_sched[port][L0CtrlIndex], 0, OPENNSL_COSQ_STRICT, 0);
    
    if (rc != OPENNSL_E_NONE)
    {
        printf ("opennsl_cosq_gport_sched_set failed with err=%d for L0[3]=0x%x idx=%d\r\n", rc, c_l0_sched[port][L0CtrlIndex], 0);
    }
    else
    {
     //   printf ("opennsl_cosq_gport_sched_set success for L0[3]=0x%x idx=%d\r\n", 
       //         c_l0_sched[port][L0CtrlIndex], 0);
    }

    for (cos = 0; cos < 8; cos++)
    {
        rc = opennsl_cosq_gport_sched_set(unit, c_l1_sched[port][cos], 0, 
                                      OPENNSL_COSQ_DEFICIT_ROUND_ROBIN, 1);

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_sched_set failed with err=%d for L1[%d]=0x%x idx=%d\r\n", rc, cos, c_l1_sched[port][cos], 0);
        }
        else
        {
     //       printf ("opennsl_cosq_gport_sched_set success for L1[%d]=0x%x idx=%d\r\n", cos, c_l1_sched[port][cos], 0);
        }
    }

    for (cos = 0; cos < 8; cos++)
    {
        rc = opennsl_cosq_gport_sched_set(unit, c_l1_sched[port][cos], 1, 
                                      OPENNSL_COSQ_DEFICIT_ROUND_ROBIN, 1);

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_sched_set failed with err=%d for L1[%d]=0x%x idx=%d\r\n", rc, cos, c_l1_sched[port][cos], 1);
        }
        else
        {
     //       printf ("opennsl_cosq_gport_sched_set success for L1[%d]=0x%x idx=%d\r\n", cos, c_l1_sched[port][cos], 1);
        }
    }


    /*
     * 5 Child attached to c_l1_sched[port][9]. Assign WDRR weights to each of
     * them - 4 L2UC [8 to 11] and 1 L2MC L2MC[8].
     */
    for (cos = 0; cos < ((isHspPort)?2:5); cos++)
    {
        rc = opennsl_cosq_gport_sched_set(unit, c_l1_sched[port][9], cos, 
                                      OPENNSL_COSQ_DEFICIT_ROUND_ROBIN, 1);

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_sched_set failed with err=%d for L1[9]=0x%x idx=%d\r\n", rc, c_l1_sched[port][9], cos);
        }
        else
        {
     //       printf ("opennsl_cosq_gport_sched_set success for L1[9]=0x%x idx=%d\r\n", c_l1_sched[port][9], cos);
        }
    }

    return 0;    
}

int opennslCintHqosDeleteAll (int unit)
{
    int        rc = 0;
    int        port = 0;
    opennsl_port_config_t c;
    opennsl_port_config_get(unit, &c);

    OPENNSL_PBMP_ITER (c.all, port)
    {
      if( port < BVIEW_ASIC_MAX_PORTS)
      {
        printf ("Deleting HQoS for port %d...\r\n", port);

        /*
         * This function gets and stores the GPORT for the given port in
         * c_gport[port].
         */
        rc = opennslCintHqosDelete (unit, port);
       
        if (rc != 0)
        {
            printf ("HQoS Delete FAILED for Port %d\r\n", port);
        }
        else
        {
 //           printf ("HQoS Delete SUCCESS for Port %d\r\n", port);
        }
      }
    }

    return rc;
}

int opennslCintHqosDfltInitAll (int unit)
{
    int        rc = 0;
    int        port = 0;

    opennsl_port_config_t c;
    opennsl_port_config_get(unit, &c);

    opennslCintTablesInit ();

    rc = opennslCintCpuHqosInit (unit);

    if (rc != 0)
    {
        printf ("HQoS Init FAILED for CPU Port\r\n");
    }
    else
    {
 //       printf ("HQoS Init SUCCESS for CPU Port\r\n");
    }

    OPENNSL_PBMP_ITER (c.all, port)
    {

      if ((port == 0) || (port == 105))
         continue;
      if (port < BVIEW_ASIC_MAX_PORTS)
      {
        rc = opennslCintHqosDfltInit (unit, port);
       
        if (rc != 0)
        {
            printf ("HQoS Init FAILED for Port %d\r\n", port);
        }
        else
        {
   //         printf ("HQoS Init SUCCESS for Port %d\r\n", port);
        }
      }
    }

    return rc;
}

int opennslCintHqosReOrderForSPQueue (int unit, int port, int oldSpQueue, 
                                  int newSpQueue)
{    
    int rc = OPENNSL_E_NONE;
    int l0_sp_idx = 2;
    int l1_sp_idx = 8;

    if (oldSpQueue == newSpQueue)
    {
        printf ("oldSpQueue == OPENNSL_COS_COUNT. Ignore and return.\r\n");

        return 0;
    }

    if ((rc = opennsl_port_control_set(
               unit,port,opennslPortControlMmuTrafficEnable,FALSE)) != OPENNSL_E_NONE)
    {
        printf ("opennsl_port_control_set returned error: %d\r\n", rc);

        return rc;
    }
    if ((rc = opennsl_port_control_set(
               unit,port,opennslPortControlMmuDrain,TRUE)) != OPENNSL_E_NONE)
    {
        printf ("opennsl_port_control_set returned error: %d\r\n", rc);

        return rc;
    }

    if (oldSpQueue != OPENNSL_COS_COUNT)
    {
        printf ("oldSpQueue != OPENNSL_COS_COUNT\r\n");

        printf ("Detaching L2UC Queue %d from L1 idx %d\r\n", 
                oldSpQueue, l1_sp_idx);

        rc = opennsl_cosq_gport_detach (unit, c_Uqueue[port][oldSpQueue], 
                                    c_l1_sched[port][l1_sp_idx], 0); 

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_detach returned error: %d on detaching c_Uqueue[port] [%d]: 0x%x and c_l1_sched[port][%d]: 0x%x\r\n", rc, 
                    oldSpQueue, c_Uqueue[port][oldSpQueue], l1_sp_idx, 
                    c_l1_sched[port][l1_sp_idx]);

            return rc;
        }
        else
        {
   //         printf ("opennsl_cosq_gport_detach btw c_Uqueue[port] [%d]: 0x%x and c_l1_sched[port][%d]: 0x%x Successful. \r\n", oldSpQueue, 
 //                   c_Uqueue[port][oldSpQueue], l1_sp_idx, c_l1_sched[port][l1_sp_idx]);
        }

        printf ("Attaching L2UC Queue %d to L1 idx %d\r\n", 
                oldSpQueue, oldSpQueue);

        rc = opennsl_cosq_gport_attach(unit, c_Uqueue[port][oldSpQueue], 
                                   c_l1_sched[port][oldSpQueue], -1);

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_detach returned error: %d on attaching c_Uqueue[port] [%d]: 0x%x to c_l1_sched[port][%d]: 0x%x\r\n", rc, 
                    oldSpQueue, c_Uqueue[port][oldSpQueue], oldSpQueue, 
                    c_l1_sched[port][oldSpQueue]);

            return rc;
        }
        else
        {
     //       printf ("opennsl_cosq_gport_attach btw c_Uqueue[port] [%d]: 0x%x and c_l1_sched[port][%d]: 0x%x Successful. \r\n", oldSpQueue, 
       //             c_Uqueue[port][oldSpQueue], oldSpQueue, c_l1_sched[port][oldSpQueue]);
        }

        rc = opennsl_cosq_gport_sched_set(unit, c_l1_sched[port][oldSpQueue], 0, 
                                      OPENNSL_COSQ_DEFICIT_ROUND_ROBIN, 1);

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_sched_set failed with err=%d for L1[%d]=0x%x idx=%d\r\n", rc, oldSpQueue, 
                    c_l1_sched[port][oldSpQueue], 0);
        }
        else
        {
     //       printf ("opennsl_cosq_gport_sched_set success for L1[%d]=0x%x idx=%d\r\n", oldSpQueue, c_l1_sched[port][oldSpQueue], 0);
        }
    }
    else
    {
        printf ("oldSpQueue == OPENNSL_COS_COUNT\r\n");

        rc = opennsl_cosq_gport_sched_set(unit, c_port_sched[port], l0_sp_idx,  
                                      OPENNSL_COSQ_STRICT, 0);

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_sched_set failed with err=%d for L0[%d]=0x%x idx=%d\r\n", rc, l0_sp_idx, 
                    c_l0_sched[port][l0_sp_idx], 0);
        }
        else
        {
     //       printf ("opennsl_cosq_gport_sched_set success for L0[%d]=0x%x idx=%d\r\n", l0_sp_idx, c_l0_sched[port][l0_sp_idx], 0);
        }
        
        printf ("Attaching L1 node Idx %d to L0 node idx %d\r\n", 
                l1_sp_idx, l0_sp_idx);

        rc = opennsl_cosq_gport_attach(unit,c_l1_sched[port][l1_sp_idx], 
                                   c_l0_sched[port][l0_sp_idx], -1);

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_attach returned error: %d on attaching c_l1_sched[port][%d]: 0x%x to c_l0_sched[port][%d]: 0x%x\r\n", rc, 
                    l1_sp_idx, c_l1_sched[port][l1_sp_idx], l0_sp_idx, 
                    c_l0_sched[port][l0_sp_idx]);

            return rc;
        }
        else
        {
       //     printf ("opennsl_cosq_gport_attach success on attaching c_l1_sched[port][%d]: 0x%x to c_l0_sched[port][%d]: 0x%x\r\n", 
         //           l1_sp_idx, c_l1_sched[port][l1_sp_idx], l0_sp_idx, 
           //         c_l0_sched[port][l0_sp_idx]);
        }

        rc = opennsl_cosq_gport_sched_set(unit, c_l0_sched[port][l0_sp_idx], 0,  
                                      OPENNSL_COSQ_STRICT, 0);

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_sched_set failed with err=%d for L0[%d]=0x%x idx=%d\r\n", rc, l0_sp_idx, 
                    c_l0_sched[port][l0_sp_idx], 0);
        }
        else
        {
     //       printf ("opennsl_cosq_gport_sched_set success for L0[%d]=0x%x idx=%d \r\n", l0_sp_idx, c_l0_sched[port][l0_sp_idx], 0);
        }
    }

    if (newSpQueue != OPENNSL_COS_COUNT)
    {
        printf ("newSpQueue != OPENNSL_COS_COUNT\r\n");

        printf ("Detaching L2UC Queue %d from L1 idx %d\r\n", 
                newSpQueue, newSpQueue);

        rc = opennsl_cosq_gport_detach (unit, c_Uqueue[port][newSpQueue], 
                                    c_l1_sched[port][newSpQueue], 0); 

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_detach returned error: %d on detaching c_Uqueue[port] [%d]: 0x%x and c_l1_sched[port][%d]: 0x%x\r\n", rc, 
                    newSpQueue, c_Uqueue[port][newSpQueue], newSpQueue, 
                    c_l1_sched[port][newSpQueue]);

            return rc;
        }
        else
        {
     //       printf ("opennsl_cosq_gport_detach btw c_Uqueue[port] [%d]: 0x%x and c_l1_sched[port][%d]: 0x%x Successful. \r\n", newSpQueue, 
       //             c_Uqueue[port][newSpQueue], newSpQueue, c_l1_sched[port][newSpQueue]);
        }

        printf ("Attaching L2UC Queue %d to L1 idx %d\r\n", 
                newSpQueue, l1_sp_idx);

        rc = opennsl_cosq_gport_attach(unit, c_Uqueue[port][newSpQueue], 
                                   c_l1_sched[port][l1_sp_idx], -1);

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_detach returned error: %d on attaching c_Uqueue[port] [%d]: 0x%x to c_l1_sched[port][%d]: 0x%x\r\n", rc, 
                    newSpQueue, c_Uqueue[port][newSpQueue], l1_sp_idx, 
                    c_l1_sched[port][l1_sp_idx]);

            return rc;
        }
        else
        {
         //   printf ("opennsl_cosq_gport_attach btw c_Uqueue[port] [%d]: 0x%x and c_l1_sched[port][%d]: 0x%x Successful. \r\n", newSpQueue, 
           //         c_Uqueue[port][newSpQueue], l1_sp_idx, c_l1_sched[port][l1_sp_idx]);
        }

        rc = opennsl_cosq_gport_sched_set(unit, c_l1_sched[port][l1_sp_idx], 0, 
                                      OPENNSL_COSQ_STRICT, 0);

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_sched_set failed with err=%d for L1[%d]=0x%x idx=%d\r\n", rc, l1_sp_idx, 
                    c_l1_sched[port][l1_sp_idx], 0);
        }
        else
        {
     //       printf ("opennsl_cosq_gport_sched_set success for L1[%d]=0x%x idx=%d \r\n", l1_sp_idx, c_l1_sched[port][l1_sp_idx], 0);
        }
    }
    else
    {
        printf ("newSpQueue == OPENNSL_COS_COUNT\r\n");

        rc = opennsl_cosq_gport_sched_set(unit, c_port_sched[port], l0_sp_idx,  
                                      OPENNSL_COSQ_DEFICIT_ROUND_ROBIN, 1);

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_sched_set failed with err=%d for L0[%d]=0x%x idx=%d\r\n", rc, l0_sp_idx, 
                    c_l0_sched[port][l0_sp_idx], 0);
        }
        else
        {
     //       printf ("opennsl_cosq_gport_sched_set success for L0[%d]=0x%x idx=%d\r\n", l0_sp_idx, c_l0_sched[port][l0_sp_idx], 0);
        }
        
        printf ("Detaching L1 node Idx %d to L0 node idx %d\r\n", 
                l1_sp_idx, l0_sp_idx);

        rc = opennsl_cosq_gport_detach(unit,c_l1_sched[port][l1_sp_idx], 
                                   c_l0_sched[port][l0_sp_idx], 0);

        if (rc != OPENNSL_E_NONE)
        {
            printf ("opennsl_cosq_gport_detach returned error: %d on detaching c_l1_sched[port][%d]: 0x%x from c_l0_sched[port][%d]: 0x%x\r\n", rc, 
                    l1_sp_idx, c_l1_sched[port][l1_sp_idx], l0_sp_idx, 
                    c_l0_sched[port][l0_sp_idx]);

            return rc;
        }
        else
        {
     //       printf ("opennsl_cosq_gport_detach success on detaching c_l1_sched[port][%d]: 0x%x from c_l0_sched[port][%d]: 0x%x\r\n", 
       //             l1_sp_idx, c_l1_sched[port][l1_sp_idx], l0_sp_idx, 
         //           c_l0_sched[port][l0_sp_idx]);
        }

    }
    if ((rc = opennsl_port_control_set(
               unit,port,opennslPortControlMmuTrafficEnable,TRUE)) != OPENNSL_E_NONE)
    {
        printf ("opennsl_port_control_set returned error: %d\r\n", rc);

        return rc;
    }

    return rc;
}
int opennslCintHqosReOrderForSPQueueAllPorts(int unit, int oldSpQueue,
                                         int newSpQueue)
{
    int        rc = 0;
    int        port = 0;
    opennsl_port_config_t c;
    opennsl_port_config_get(unit, &c);

    OPENNSL_PBMP_ITER (c.all, port)
    {
      if (port < BVIEW_ASIC_MAX_PORTS)
      {
        printf ("Scheduler change HQoS for port %d...\r\n", port);

        rc = opennslCintHqosReOrderForSPQueue (unit, port, oldSpQueue, newSpQueue);

        if (rc != 0)
        {
            printf ("Scheduler change HQoS Init FAILED for Port %d\r\n", port);
        }
        else
        {
            printf ("Scheduler change HQoS Init SUCCESS for Port %d\r\n", port);
        }
      }
    }

    return rc;
}
/*****************************************************************//**
* \brief Include all ports to default vlan
*
* \param unit   [IN]    unit number
*
* \return OPENNSL_E_XXX     OpenNSL API return code
  ********************************************************************/
int switch_default_vlan_config(int unit)
{
  opennsl_port_config_t pcfg;
  int rv;

  /*
   * Create VLAN with id DEFAULT_VLAN and
   * add ethernet ports to the VLAN
   */
  rv = opennsl_port_config_get(unit, &pcfg);
  if (rv != OPENNSL_E_NONE) {
    printf("Failed to get port configuration. Error %s\n", opennsl_errmsg(rv));
    return rv;
  }

  rv = opennsl_vlan_port_add(unit, DEFAULT_VLAN, pcfg.e, pcfg.e);
  if (rv != OPENNSL_E_NONE) {
    printf("Failed to add ports to VLAN. Error %s\n", opennsl_errmsg(rv));
    return rv;
  }

  return 0;
}

int switch_default_port_config(int unit)
{
  opennsl_port_config_t pcfg;
  opennsl_port_info_t info;
  int rv;
  int port;
  int stp_state = OPENNSL_STG_STP_FORWARD;
  int stg = 1;

  /*
   * Create VLAN with id DEFAULT_VLAN and
   * add ethernet ports to the VLAN
   */
  rv = opennsl_port_config_get(unit, &pcfg);
  if (rv != OPENNSL_E_NONE)
  {
    printf("Failed to get port configuration. Error %s\n", opennsl_errmsg(rv));
    return rv;
  }

  /* Set the STP state to forward in default STG for all ports */
  OPENNSL_PBMP_ITER(pcfg.e, port)
  {
    rv = opennsl_stg_stp_set(unit, stg, port, stp_state);
    if (rv != OPENNSL_E_NONE)
    {
      printf("Failed to set STP state for unit %d port %d, Error %s\n",
          unit, port, opennsl_errmsg(rv));
      return rv;
    }
  }

  /* Setup default configuration on the ports */
  opennsl_port_info_t_init(&info);

  info.speed        = 0;
  info.duplex       = OPENNSL_PORT_DUPLEX_FULL;
  info.pause_rx     = OPENNSL_PORT_ABILITY_PAUSE_RX;
  info.pause_tx     = OPENNSL_PORT_ABILITY_PAUSE_TX;
  info.linkscan     = OPENNSL_LINKSCAN_MODE_SW;
  info.autoneg      = FALSE;
  info.enable = 1;

  info.action_mask |= ( OPENNSL_PORT_ATTR_AUTONEG_MASK |
      OPENNSL_PORT_ATTR_SPEED_MASK    |
      OPENNSL_PORT_ATTR_DUPLEX_MASK   |
      OPENNSL_PORT_ATTR_PAUSE_RX_MASK |
      OPENNSL_PORT_ATTR_PAUSE_TX_MASK |
      OPENNSL_PORT_ATTR_LINKSCAN_MASK |
      OPENNSL_PORT_ATTR_ENABLE_MASK   );

  OPENNSL_PBMP_ITER(pcfg.e, port)
  {
    rv = opennsl_port_selective_set(unit, port, &info);
    if (OPENNSL_FAILURE(rv))
    {
      printf("Failed to set port config for unit %d, port %d, Error %s",
          unit, port, opennsl_errmsg(rv));
      return rv;
    }
  }

  return OPENNSL_E_NONE;
}


int openapps_driver_init(bool debug, bool menu)
{
  int       rc = 1;
  int i= 0, pri = 0;
  static int systemMappingInitialized = 0;
#ifndef CDP_EXCLUDE
  pthread_t agentThread;
#endif
  if(systemMappingInitialized != 0)
  {
    printf("\r\nDriver is already initialized.\r\n");
    return 0;
  }
  rc = opennsl_driver_init((opennsl_init_t *) NULL);
  if (rc != 0)
  {
    return rc;
  }
  /* create the default vlan */

  rc = switch_default_vlan_config(0);

  for (i = 0; i < soc_ndev; i++) {
    for (pri=0;pri<8;pri++){
     opennsl_cosq_mapping_set (i,pri,pri); 
    }
  }

  if (0 != rc)
  {
    printf("\r\nError initializing driver, rc = %d.\r\n", rc);
    return -1;
  }
  systemMappingInitialized = 1;

#if defined(BVIEW_CHIP_TD2)
  opennslCintHqosDfltInitAll(0);
#endif

  rc = switch_default_port_config(0);
  if (rc != OPENNSL_E_NONE) {
    printf("\r\nFailed to apply default config on ports, rc = %d (%s).\r\n",
           rc, opennsl_errmsg(rc));
  }


  if (true == debug)
  {
#ifndef CDP_EXCLUDE
    rc = pthread_create(&agentThread, NULL, (void *)&bview_system_init_ph2, (void *)NULL);
    if (rc != 0)
    {
      printf("Failed to create Agent Thread \n");
    }

    example_server_main();
#endif
  }
   return 0;
}


