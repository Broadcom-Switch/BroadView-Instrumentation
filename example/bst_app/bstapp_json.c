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

#include "bstapp.h"
BSTAPP_REST_MSG_t bstRestMessages[] = {
    {
     .httpMethod = "POST",
     .descr = "Setting up BST Feature",
     .method = "configure-bst-feature",
     .json = "{     \
                \"jsonrpc\": \"2.0\", \
                \"method\": \"configure-bst-feature\", \
                \"asic-id\": \"1\", \
                \"params\": { \
                \"bst-enable\": 1, \
                \"send-async-reports\": 0, \
                \"collection-interval\": 10, \
                \"stats-in-percentage\": 0, \
                \"stat-units-in-cells\": 1, \
                \"send-snapshot-on-trigger\": 1, \
                \"trigger-rate-limit\": 1, \
                \"trigger-rate-limit-interval\": 5, \
                \"async-full-reports\": 0 \
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
                     \"asic-id\": \"1\", \
                     \"params\": { \
		     \"track-peak-stats\" : 1, \
		     \"track-ingress-port-priority-group\" : 1, \
		     \"track-ingress-port-service-pool\" : 1, \
		     \"track-ingress-service-pool\" : 1, \
		     \"track-egress-port-service-pool\" : 1, \
		     \"track-egress-service-pool\" : 1, \
		     \"track-egress-uc-queue\" : 1, \
		     \"track-egress-uc-queue-group\" : 1, \
		     \"track-egress-mc-queue\" : 1, \
		     \"track-egress-cpu-queue\" : 1, \
		     \"track-egress-rqe-queue\" : 1, \
		     \"track-device\" : 1 \
		     }, \
		     \"id\": 2 \
		     } "
    },
    {
     .httpMethod = "GET",
     .descr = "Obtaining current feature status ",
     .method = "get-bst-feature",
     .json = "{ \
                       \"jsonrpc\": \"2.0\", \
                       \"method\": \"get-bst-feature\", \
                       \"asic-id\": \"1\", \
                       \"params\": { \
                       }, \
                       \"id\": 3 \
                       }"
    },


    {
     .httpMethod = "GET",
     .descr = "Obtaining current tracking status ",
     .method = "get-bst-tracking",
     .json = "{ \
                       \"jsonrpc\": \"2.0\", \
                       \"method\": \"get-bst-tracking\", \
                       \"asic-id\": \"1\", \
                       \"params\": { \
                       }, \
                       \"id\": 4 \
                       }"
    },

    {
     .httpMethod = "GET",
     .descr = "Obtaining a snapshot report ",
     .method = "get-bst-report",
     .json = "{ \
		       \"jsonrpc\": \"2.0\", \
		       \"method\": \"get-bst-report\", \
		       \"asic-id\": \"1\", \
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
              \"id\": 5\
            } "
    },

    {
     .httpMethod = "GET",
     .descr = "Obtaining the thresholds ",
     .method = "get-bst-thresholds",
     .json = "{ \
		       \"jsonrpc\": \"2.0\", \
		       \"method\": \"get-bst-thresholds\", \
		       \"asic-id\": \"1\", \
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
              \"id\": 6\
            } "
    },
    {
     .httpMethod = "POST",
     .descr = "Setting up few thresholds to enable trigger",
     .method = "configure-bst-thresholds",
     .json = "{ \
			\"jsonrpc\": \"2.0\", \
                             \"method\": \"configure-bst-thresholds\", \
			\"asic-id\": \"1\", \
                       \"params\": { \
                             \"realm\": \"device\", \
                             \"threshold\": 1000 \
                       }, \
                       \"id\": 7 \
                       }"
    },
    
    {
     .httpMethod = "GET",
     .descr = "Obtaining current feature status ",
     .method = "get-bst-feature",
     .json = "{ \
                       \"jsonrpc\": \"2.0\", \
                       \"method\": \"get-bst-feature\", \
                       \"asic-id\": \"1\", \
                       \"params\": { \
                       }, \
                       \"id\": 8 \
                       }"
    },
    {
     .httpMethod = "GET",
     .descr = "Obtaining current feature status ",
     .method = "get-bst-feature",
     .json = "{ \
                       \"jsonrpc\": \"2.0\", \
                       \"method\": \"get-bst-feature\", \
                       \"asic-id\": \"1\", \
                       \"params\": { \
                       }, \
                       \"id\": 9 \
                       }"
    },

    {
     .httpMethod = "POST",
     .descr = "configure bst feature with different params",
     .method = "configure-bst-feature",
     .json = "{     \
                \"jsonrpc\": \"2.0\", \
                \"method\": \"configure-bst-feature\", \
                \"asic-id\": \"1\", \
                \"params\": { \
                \"bst-enable\": 1, \
                \"send-async-reports\": 0, \
                \"collection-interval\": 3, \
                \"stats-in-percentage\": 0, \
                \"stat-units-in-cells\": 1, \
                \"send-snapshot-on-trigger\": 1, \
                \"trigger-rate-limit\": 2, \
                \"trigger-rate-limit-interval\": 10, \
                \"async-full-reports\": 0 \
                }, \
                \"id\": 10 \
                }"
    },


    {
     .httpMethod = "GET",
     .descr = "Obtaining current feature status ",
     .method = "get-bst-feature",
     .json = "{ \
                              \"jsonrpc\": \"2.0\", \
                       \"method\": \"get-bst-feature\", \
                              \"asic-id\": \"1\", \
                              \"params\": { \
                              },\
                               \"id\": 11 \
                              }"
    },
    {
     .httpMethod = "POST",
     .descr = "Clearing Statistics ",
     .method = "clear-bst-statistics",
     .json = "{ \
			\"jsonrpc\": \"2.0\", \
			\"method\": \"clear-bst-statistics\", \
			\"asic-id\": \"1\", \
                       \"params\": { \
                       }, \
                       \"id\": 12 \
                       }"
    },
    {
     .httpMethod = "POST",
     .descr = "Enabling ASYNC Reports",
     .method = "configure-bst-feature to turn on ASYNC reports",
     .json = "{     \
                \"jsonrpc\": \"2.0\", \
                \"method\": \"configure-bst-feature\", \
                \"asic-id\": \"1\", \
                \"params\": { \
                \"bst-enable\": 1, \
                \"send-async-reports\": 1, \
                \"collection-interval\": 1, \
                \"stats-in-percentage\": 0, \
                \"stat-units-in-cells\": 1, \
                \"send-snapshot-on-trigger\": 1, \
                \"trigger-rate-limit\": 2, \
                \"trigger-rate-limit-interval\": 3, \
                \"async-full-reports\": 0 \
                }, \
                \"id\": 13 \
                }"
    },

};

