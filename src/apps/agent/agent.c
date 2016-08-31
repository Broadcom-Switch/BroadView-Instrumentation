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

#include <string.h>
#include "system.h"
#include "version.h"

int main(int argc, char **argv)
{
  bool debug = false;
  bool menu = false;

  if (argc > 2)
  {
    printf("Invalid number of arguments\n");
    return -1;
  }

  if (argc == 2)
  {
    if (strcmp(argv[1], "-d") == 0) 
    {
      debug = true;
    }
    else if (strcmp(argv[1], "-c") == 0)
    {
      debug = true;
      menu = true;
    }
    else if (strcmp(argv[1], "-h") == 0)
    {
      printf("Usage %s [OPTION]\n", argv[0]);
      printf("       -d    Driver debug mode\n");
      printf("       -c    Driver menu mode\n");
      return 0;
    }
    else
    {
      printf("Invalid option! Try -h for help\n");
      return -1;
    }
  }
  printf("BroadViewAgent Version %s\n",RELEASE_STRING);
  bview_system_init_ph1(debug, menu);
  return 0;
}
