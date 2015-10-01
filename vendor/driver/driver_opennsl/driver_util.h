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

#ifndef DRIVER_UTIL_H
#define DRIVER_UTIL_H

#include <stdint.h>
#include <stdbool.h>

#define CMD_LEN_MAX 256

int openapps_driver_init(bool debug, bool menu);
extern int opennsl_driver_init(void)   ;
int driverStart(void);

int driverSwitchIdGet(uint16_t *chipId, uint8_t *revision);

int driverPhysicalPortCountGet(void);
int driverPhysicalPortNumNextGet(int portNum, int *nextPortNum);

int opennsldriver_shell_command(char *commandBuf)  ;
int driverBcmCommand(char *commandBuf);
int opennsl_driver_shell()  ;
#endif  /* DRIVER_UTIL_H */
