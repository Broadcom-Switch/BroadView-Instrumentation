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

#ifndef INCLUDE_PLATFORM_H
#define INCLUDE_PLATFORM_H

#ifdef __cplusplus
extern "C"
{
#endif

/* BST:*/
/*Default The BST_Threshold for the Egress UC Queues in units of bytes.*/
#define BVIEW_BST_UCAST_THRES_DEFAULT                 (0xFFF * 208)

/*Default The BST_Threshold for the Egress MC Queues*/
#define BVIEW_BST_MCAST_THRES_DEFAULT                 (0x7FFF * 208)


/*Default BST_Threshold for the Egress Per (Port, SP) UC shared in units of bytes.*/
#define BVIEW_BST_E_P_SP_UC_THRES_DEFAULT             (0x7FFF * 208)



/*Default BST_Threshold for the Egress Per (Port, SP) MC/UC+MC shared in units of bytes*/
#define BVIEW_BST_E_P_SP_UCMC_THRES_DEFAULT           (0x7FFF * 208)

 /*  Default BST_Threshold for each of the 4 Egress
  *  SPs Shared use-counts (UC+MC)in units of bytes.
  */

#define BVIEW_BST_E_SP_UCMC_THRES_DEFAULT             (0x7FFF * 208)

 /*  Default BST_Threshold for each of the 4 Egress
  * SPs Shared use-counts (MC) in units of bytes.
  */

#define BVIEW_BST_E_SP_MC_THRES_DEFAULT               (0x7FFF * 208)


 /*  Default BST_Threshold for each of the Egress
  *  CPU queues use-counts (UC+MC)in units of bytes.
  */

#define BVIEW_BST_E_CPU_UCMC_THRES_DEFAULT            (0x7FFF * 208)

 /*  Default BST_Threshold for each of the Egress
  *  RQE queues use-counts  in units of bytes.
  */

#define BVIEW_BST_E_RQE_THRES_DEFAULT                 (0x7FFF * 208)

/*  Default The BST_Threshold for the
 * (Ingress Port, PG) UC plus MC Shared use-count in units of bytes.
 */

#define  BVIEW_BST_I_P_PG_UCMC_SHARED_THRES_DEFAULT   (0x7FFF * 208)


/*  Default The BST_Threshold for the
 * (Ingress Port, PG) UC plus MC Headroom use-count in units of bytes.
 */

#define  BVIEW_BST_I_P_PG_UCMC_HDRM_THRES_DEFAULT     (0x7FFF * 208)


/* Default The BST_Threshold for the
 * (Ingress Port, SP) UC plus MC shared use-count in units of bytes.
 */
#define  BVIEW_BST_I_P_SP_UCMC_SHARED_THRES_DEFAULT   (0x7FFF * 208)
/* Default The BST_Threshold for the Ingress SP
 * UC plus MC use-count in units of bytes.
 */

#define  BVIEW_BST_I_SP_UCMC_SHARED_THRES_DEFAULT     (0xFFF * 208)


/* Default The BST_Threshold for Device Use-Countin units of bytes.*/

#define  BVIEW_BST_DEVICE_THRES_DEFAULT               (0x7FFF * 208)
       

/* Default BST_Threshold for Unicast Queue Group*/
#define  BVIEW_BST_UCAST_QUEUE_GROUP_DEFAULT          (0xFFF * 208)

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_PLATFORM_H */

