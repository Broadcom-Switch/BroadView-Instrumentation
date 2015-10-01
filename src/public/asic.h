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

#ifndef INCLUDE_ASIC_H
#define	INCLUDE_ASIC_H

#ifdef	__cplusplus
extern "C"
{
#endif

#define BVIEW_ASIC_MAX_PORTS            130
#define BVIEW_ASIC_MAX_UC_QUEUES        4096
#define BVIEW_ASIC_MAX_UC_QUEUE_GROUPS  128
#define BVIEW_ASIC_MAX_MC_QUEUES        1040
#define BVIEW_ASIC_MAX_SERVICE_POOLS    4
#define BVIEW_ASIC_MAX_COMMON_POOLS     1
#define BVIEW_ASIC_MAX_CPU_QUEUES       8
#define BVIEW_ASIC_MAX_RQE_QUEUES       11
#define BVIEW_ASIC_MAX_RQE_QUEUE_POOLS  4
#define BVIEW_ASIC_MAX_PRIORITY_GROUPS  8

#define BVIEW_ASIC_MAX_INGRESS_SERVICE_POOLS  \
    (BVIEW_ASIC_MAX_SERVICE_POOLS + BVIEW_ASIC_MAX_COMMON_POOLS)

/** Different scalability numbers for the ASIC */
typedef struct _capabilities_
{
        /** Number of ports of Asic */
        int numPorts;
        /** Number of unicast queues */
        int numUnicastQueues;
        /** Number of unicast queue groups */
        int numUnicastQueueGroups;
        /** Number of multicast queues */
        int numMulticastQueues;
        /** Number of service pools */
        int numServicePools;
        /** Number of common pools */
        int numCommonPools;
        /** Number of CPU queues */
        int numCpuQueues;
        /** Number of RQE queues */
        int numRqeQueues;
        /** Number of RQE queue pools */
        int numRqeQueuePools;
        /** Number of priority groups */
        int numPriorityGroups;
        /** can this ASIC provide accurate time ? */
        bool support1588;
        /** MMU Cell to BYTE conversion*/
        int cellToByteConv;
        
} BVIEW_ASIC_CAPABILITIES_t;


/** This structure defines an ASIC from an instrumentation perspective */
typedef struct _bview_asic_
{
    /** Asic/Unit number */
    int unit;
    /** The ASIC model number */
    BVIEW_ASIC_TYPE  asicType;
    /** Asic Capabilities */
    BVIEW_ASIC_CAPABILITIES_t scalingParams;
} BVIEW_ASIC_t;


#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_ASIC_H */

