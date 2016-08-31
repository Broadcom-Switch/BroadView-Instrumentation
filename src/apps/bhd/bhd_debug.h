/************************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename bhd_debug.h 
  *
  * @purpose BroadView BHD Application debug definitions 
  *
  * @component Black hole detection 
  *
  * @comments
  *
  * @create 3/7/2016
  *
  * @author 
  * @end
  *
  **********************************************************************/


#ifndef INCLUDE_BHD_DEBUG_H
#define INCLUDE_BHD_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include "modulemgr.h"




#define _BHD_DEBUG
#define _BHD_DEBUG_LEVEL        (0x0) 

#define _BHD_DEBUG_TRACE        (0x1)
#define _BHD_DEBUG_INFO         (0x01 << 1)
#define _BHD_DEBUG_ERROR        (0x01 << 2)
#define _BHD_DEBUG_ALL          (0xFF)

#ifdef _BHD_DEBUG
#define _BHD_LOG(level, format,args...)   do { \
              if ((level) & _BHD_DEBUG_LEVEL) { \
                                printf(format, ##args); \
                            } \
          }while(0)
#else
#define _BHD_LOG(level, format,args...)
#endif



#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_BHD_DEBUG_H */

