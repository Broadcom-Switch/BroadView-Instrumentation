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

#ifndef INCLUDE_OVSDB_BST_CTL_H
#define INCLUDE_OVSDB_BST_CTL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
* @brief           Set default connection parameters                                 
*
* @param[in,out]   connectMode  - Connection Type 
*
* @notes
*
* @retval          none
*
*********************************************************************/

void ovsdb_set_default(char * connectMode);

/*********************************************************************
* @brief           Read OVSDB Config File 
*
* @param[in,out]   connectMode  - Connection Type 
*
* @notes       
*
* @retval          BVIEW_STATUS_SUCCESS for successful execution
*
*********************************************************************/

BVIEW_STATUS ovsdb_file_read(char * connectMode);

/*********************************************************************
* @brief       Commit column "trigger_threshold" in table "bufmon" to 
*              OVSDB database.
*
* @param[in]   asic             -  ASIC ID
* @param[in]   port             -  Port 
* @param[in]   index            -  Index 
* @param[in]   bid              -  Stat ID
* @param[in]   threshold      -   Threshold.
*
* @notes       
*          
*
*
*********************************************************************/
BVIEW_STATUS bst_ovsdb_threshold_commit (int asic , int port, int index,
                                         int bid, uint64_t threshold);

/*********************************************************************
* @brief   Commit Table "System" columns to OVSDB database.
*
* @param[in]   asic              -   ASIC ID
* @param[in]   config            -   Pointer to BST config Data.
*
* @retval      
* 
* @notes    
*          
*
*
*********************************************************************/
BVIEW_STATUS bst_ovsdb_bst_config_commit (int asic ,
                                          BVIEW_OVSDB_CONFIG_DATA_t *config);

/*********************************************************************
* @brief   BST OVSDB monitor thread
*
*
*
* @notes   Receive JSON notification from OVSDB-SERVER and Update the 
*          SB PLUGIN cache.
*
*
*********************************************************************/
void bst_ovsdb_monitor();

#ifdef __cplusplus
}
#endif
#endif

