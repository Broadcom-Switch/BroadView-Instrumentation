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

#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include "openapps_log_api.h"
#include "broadview.h"
#include "packet_multiplexer.h"
#include "packet_multiplexer_db.h"
#include "sbplugin_redirect_system.h"

pktMuxInfo_t            pkt_mux_info;
char                    pkt_mux_Initialized = false;


  /* Macro to acquire lock */
#define PKT_MUX_LOCK_TAKE()                                                  \
  {                                                                          \
    if (0 != pthread_mutex_lock (&pkt_mux_info.pkt_mux_mutex))               \
    {                                                                        \
      return BVIEW_STATUS_FAILURE;                                           \
    }                                                                        \
  }
  /* Macro to release lock*/
#define PKT_MUX_LOCK_GIVE()                                                  \
  {                                                                          \
    if (0 != pthread_mutex_unlock(&pkt_mux_info.pkt_mux_mutex))              \
    {                                                                        \
      return BVIEW_STATUS_FAILURE;                                           \
    }                                                                        \
  }

/*********************************************************************
* @brief :   Packet Multiplexer thread.
*
* @param[in] : none
*
* @retval  : 
*            
*            
* @retval  : 
*
* @note  : This api is the processing thread of the packet multiplexer 
*          application. All the incoming requests are processed and 
*          notify the applciations registered. 
*
*********************************************************************/
BVIEW_STATUS pkt_mux_main (void)
{
  BVIEW_PACKET_MSG_t msg_data;
  int appIndex = 0;

  while (1)
  {
    if (-1 != (msgrcv (pkt_mux_info.recvMsgQid, &msg_data, sizeof (msg_data), 0, 0)))
    {
      for (appIndex = 0; appIndex < BVIEW_PKT_MUX_MAX_HOOKS; appIndex++)
      {
        if (msg_data.appMask & pkt_mux_info.pktMuxHooks[appIndex].appID)
        {
          pkt_mux_info.pktMuxHooks[appIndex].callbackFunc(&msg_data);
        }
      }
    }
    else 
    {
      /* LOG a message*/  
    }
  } /* End of While */                                                
  return BVIEW_STATUS_SUCCESS;
}   

/*********************************************************************
* @brief  De-Register with packet multiplexer
*
* @param    pkt_mux_hook @b{(input)} Pointer to application registration info
*
* @returns  BVIEW_STATUS_SUCCESS  if successful de-registation
* @returns  BVIEW_STATUS_FAILURE  if 1) hooks uninitialized,
*                                    2) or no registation present
*
* @notes    none
*
* @end
*********************************************************************/
BVIEW_STATUS pkt_mux_packet_handler (void *cookie,
                                     BVIEW_PACKET_MSG_t *msg)
{
  /* Process the packet and return */
  BVIEW_PACKET_MSG_t msg_data;

  memcpy (&msg_data, msg, sizeof (msg_data));
  if (-1 == msgsnd (pkt_mux_info.recvMsgQid, &msg_data,
            sizeof (BVIEW_PACKET_MSG_t), 0))
  {
    return BVIEW_STATUS_FAILURE;
  }
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Initialize the Packet Multiplexer hook structure
*
* @param    none
*
* @returns  BVIEW_STATUS_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
BVIEW_STATUS pkt_mux_init()
{
  int recvMsgQid;
  int cookie = 0;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;

  /* locks */
  pthread_mutex_init (&pkt_mux_info.pkt_mux_mutex, NULL);

  pkt_mux_info.key = 0x103;
   /* Check if message queue is already exitsing */
  if (0 > (recvMsgQid = msgget(pkt_mux_info.key, IPC_EXCL | IPC_CREAT | 0777)))
  {
    if (EEXIST == errno)
    {
      /* get the already existing  message queue id for pt */
      if ((recvMsgQid = msgget (pkt_mux_info.key, IPC_CREAT | 0777)) < 0)
      {
        LOG_POST (BVIEW_LOG_EMERGENCY,
            "(Packet Multiplexer) Failed to create msgQ: error:%d\n", errno);
        return  BVIEW_STATUS_FAILURE;
      }

      /* message queue exits . Delete the message queue */
      if  (msgctl(recvMsgQid, IPC_RMID, (struct msqid_ds *)NULL) <0)
      {
        LOG_POST (BVIEW_LOG_EMERGENCY,
            "(Packet Multiplexer) Failed to destroy msgQ id, error:%d\n", errno);
        return  BVIEW_STATUS_FAILURE;
      }

      /* create the message queue for pt */
      if ((recvMsgQid = msgget (pkt_mux_info.key, IPC_CREAT | 0777)) < 0)
      {
        LOG_POST (BVIEW_LOG_EMERGENCY,
            "(Packet Multiplexer) Failed to create  msgQ. error:%d\n", errno);
        return  BVIEW_STATUS_FAILURE;
      }
    }
  }
  pkt_mux_info.recvMsgQid = recvMsgQid;

   /* create pthread for pt application */
  if (0 != pthread_create (&pkt_mux_info.pt_thread, NULL,
                           (void *) &pkt_mux_main, NULL))
  {
    LOG_POST (BVIEW_LOG_EMERGENCY,
              "(Packet Multiplexer) Thread creation failed error:%d\r\n", errno);
    return  BVIEW_STATUS_FAILURE;
  }

  /* Registration with silicon for CPU packets*/
  rv = sbapi_system_packet_rx_register (0, pkt_mux_packet_handler, 
                                        "pkt_mux_packet_handler", &cookie);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_EMERGENCY,
           "(Packet Multiplexer) Failed to Register with silicon for CPU packets");
    return BVIEW_STATUS_FAILURE;
  }
  pkt_mux_Initialized = true;
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Register for a Packet
*
* @param    pkt_mux_hook @b{(input)} Pointer to application registration info
*
* @returns  BVIEW_STATUS_SUCCESS    if registration is successful
* @returns  BVIEW_STATUS_DUPLICATE  if application is already registered
* @returns  BVIEW_STATUS_FAILURE    if packet multiplexer is not initialized
*                                  
*
* @notes    none
*
* @end
*********************************************************************/
BVIEW_STATUS pkt_mux_register(pktMuxRegister_t *pkt_mux_hook)
{
  int index =0;

  if ((pkt_mux_Initialized == false) && 
      (pkt_mux_init() != BVIEW_STATUS_SUCCESS))
  {
    return BVIEW_STATUS_FAILURE;
  }

  PKT_MUX_LOCK_TAKE();

  for (index = 0; index < BVIEW_PKT_MUX_MAX_HOOKS; index++)
  {
    if (pkt_mux_hook->appID == pkt_mux_info.pktMuxHooks[index].appID)
    {
      /*hook already registered */
      PKT_MUX_LOCK_GIVE();
      return BVIEW_STATUS_DUPLICATE;
    }
    
    if (pkt_mux_info.pktMuxHooks[index].appID == 0)
    {
      break;
    }
  }
  if (index == BVIEW_PKT_MUX_MAX_HOOKS)
  {
    PKT_MUX_LOCK_GIVE();
    return BVIEW_STATUS_TABLE_FULL;
  } 

  strcpy(pkt_mux_info.pktMuxHooks[index].appFuncName,
         pkt_mux_hook->appFuncName);
  pkt_mux_info.pktMuxHooks[index].callbackFunc = 
                              pkt_mux_hook->callbackFunc;
  pkt_mux_info.pktMuxHooks[index].appID = 
                              pkt_mux_hook->appID;

  PKT_MUX_LOCK_GIVE();
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  De-Register with packet multiplexer
*
* @param    pkt_mux_hook @b{(input)} Pointer to application registration info
*
* @returns  BVIEW_STATUS_SUCCESS  if successful de-registation
* @returns  BVIEW_STATUS_FAILURE  if 1) hooks uninitialized, 
*                                    2) or no registation present
*
* @notes    none
*
* @end
*********************************************************************/
BVIEW_STATUS pkt_mux_deregister (pktMuxRegister_t *pkt_mux_hook)
{
  int index = 0;

  if (pkt_mux_Initialized == false)
  {
    return BVIEW_STATUS_FAILURE;
  }

  PKT_MUX_LOCK_TAKE();
  for (index = 0; index < BVIEW_PKT_MUX_MAX_HOOKS; index++)
  {
    if (pkt_mux_hook->appID == pkt_mux_info.pktMuxHooks[index].appID)
    {
      break;
    }
  }

  if (index == BVIEW_PKT_MUX_MAX_HOOKS)
  {
    PKT_MUX_LOCK_GIVE();
    return BVIEW_STATUS_FAILURE;
  }

  pkt_mux_info.pktMuxHooks[index].appID = 0;
  pkt_mux_info.pktMuxHooks[index].callbackFunc = NULL;
  memset (pkt_mux_info.pktMuxHooks[index].appFuncName, 0x00, 
          BVIEW_PKT_MUX_MAX_FUNC_NAME);
  PKT_MUX_LOCK_GIVE();
  return BVIEW_STATUS_SUCCESS;
}



