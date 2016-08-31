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
#include "bstapp.h"

BSTAPP_REST_MENU_MSG_t bstRestMenuMessages[] = {
  {
    .httpMethod = "POST",
    .descr = "Setting up BST Feature",
    .method = "configure-bst-feature",
    .json = "{ \
               \"jsonrpc\": \"2.0\", \
               \"method\": \"configure-bst-feature\", \
               \"asic-id\": \"%s\", \
               \"params\": { \
               \"bst-enable\": %d, \
               \"send-async-reports\": %d, \
               \"collection-interval\": %d, \
               \"stat-units-in-cells\": %d \
               }, \
              \"id\": 1 \
             }"
  },
  {
    .httpMethod = "POST",
    .descr = "Setting up BST Tracking ",
    .method = "configure-bst-tracking",
    .json = "{ \
               \"jsonrpc\": \"2.0\", \
               \"method\": \"configure-bst-tracking\", \
               \"asic-id\": \"%s\", \
               \"params\": { \
               \"track-peak-stats\" :%d, \
               \"track-ingress-port-priority-group\" : %d, \
               \"track-ingress-port-service-pool\" : %d, \
               \"track-ingress-service-pool\" : %d, \
               \"track-egress-port-service-pool\" : %d, \
               \"track-egress-service-pool\" : %d, \
               \"track-egress-uc-queue\" :%d , \
               \"track-egress-uc-queue-group\" : %d, \
               \"track-egress-mc-queue\" : %d, \
               \"track-egress-cpu-queue\" : %d, \
               \"track-egress-rqe-queue\" : %d, \
               \"track-device\" : %d \
                }, \
                \"id\": 2 \
             } "
  },
  {
    .httpMethod = "POST",
    .descr = "Setting up few thresholds to enable trigger",
    .method = "configure-bst-thresholds",
    .json = "{ \
               \"jsonrpc\": \"2.0\", \
               \"method\": \"configure-bst-thresholds\", \
               \"asic-id\": \"%s\", \
               \"params\": { \
                 \"realm\": \"device\", \
                 \"threshold\": %d \
               }, \
               \"id\": 3 \
             }"
  },
  {
    .httpMethod = "POST",
    .descr = "Clearing Statistics ",
    .method = "clear-bst-statistics",
    .json = "{ \
               \"jsonrpc\": \"2.0\", \
               \"method\": \"clear-bst-statistics\", \
               \"asic-id\": \"%s\", \
               \"params\": { \
               }, \
               \"id\": 4 \
             }"
  },
  {
    .httpMethod = "POST",
    .descr = "Clearing Thresholds ",
    .method = "clear-bst-thresholds",
    .json = "{ \
               \"jsonrpc\": \"2.0\", \
               \"method\": \"clear-bst-thresholds\", \
               \"asic-id\": \"%s\", \
               \"params\": { \
               }, \
               \"id\": 5 \
             }"
  },

  {
    .httpMethod = "GET",
    .descr = "Obtaining current feature status ",
    .method = "get-bst-feature",
    .json = "{ \
               \"jsonrpc\": \"2.0\", \
               \"method\": \"get-bst-feature\", \
               \"asic-id\": \"%s\", \
               \"params\": { \
               }, \
               \"id\": 6 \
             }"
  },
{
    .httpMethod = "GET",
    .descr = "Obtaining current tracking status ",
    .method = "get-bst-tracking",
    .json = "{ \
               \"jsonrpc\": \"2.0\", \
               \"method\": \"get-bst-tracking\", \
               \"asic-id\": \"%s\", \
               \"params\": { \
               }, \
               \"id\": 7 \
             }"
  },
  {
    .httpMethod = "GET",
    .descr = "Obtaining the thresholds ",
    .method = "get-bst-thresholds",
    .json = "{ \
               \"jsonrpc\": \"2.0\", \
               \"method\": \"get-bst-thresholds\", \
               \"asic-id\": \"%s\", \
               \"params\": { \
                 \"include-ingress-port-priority-group\" : 1, \
                 \"include-ingress-port-service-pool\" : 0, \
                 \"include-ingress-service-pool\" : 0, \
                 \"include-egress-port-service-pool\" : 0, \
                 \"include-egress-service-pool\" : 1, \
                 \"include-egress-uc-queue\" : 1, \
                 \"include-egress-uc-queue-group\" : 0, \
                 \"include-egress-mc-queue\" : 1, \
                 \"include-egress-cpu-queue\" : 1, \
                 \"include-egress-rqe-queue\" : 0, \
                 \"include-device\" : 1 \
                 }, \
               \"id\": 8 \
            }"
  },
  {
    .httpMethod = "GET",
    .descr = "Obtaining a snapshot report ",
    .method = "get-bst-report",
    .json = "{ \
               \"jsonrpc\": \"2.0\", \
               \"method\": \"get-bst-report\", \
               \"asic-id\" : \"%s\", \
               \"params\": { \
                 \"include-ingress-port-priority-group\" : 1, \
                 \"include-ingress-port-service-pool\" : 0, \
                 \"include-ingress-service-pool\" : 0, \
                 \"include-egress-port-service-pool\" : 0, \
                 \"include-egress-service-pool\" : 1, \
                 \"include-egress-uc-queue\" : 1, \
                 \"include-egress-uc-queue-group\" : 0, \
                 \"include-egress-mc-queue\" : 1, \
                 \"include-egress-cpu-queue\" : 1, \
                 \"include-egress-rqe-queue\" : 0, \
                 \"include-device\" : 1 \
                 }, \
              \"id\": 9\
             }"
  },
};


