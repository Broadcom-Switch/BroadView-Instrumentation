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

#ifndef INCLUDE_PORT_UTILS_H
#define INCLUDE_PORT_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "broadview.h"
#include "asic.h"

/* Base type for declarations */
#define     BVIEW_MASK_BASE_UNIT     unsigned int
#define     BVIEW_MASKWID            (8*sizeof(BVIEW_MASK_BASE_UNIT)) 

/* (internal) Number of BVIEW_MASK_BASE_UNITs needed to contain _max bits */
#define     BVIEW_MASK_SIZE(_max)    (((_max) + BVIEW_MASKWID - 1) / BVIEW_MASKWID)

#define PT_MAX_RANGE_STR_LEN 256

/* Interface storage */
typedef struct
{
  BVIEW_MASK_BASE_UNIT   value[BVIEW_MASK_SIZE(BVIEW_ASIC_MAX_PORTS)];
} BVIEW_PORT_MASK_t;


/* Interface storage */
typedef struct
{
  BVIEW_MASK_BASE_UNIT   value[BVIEW_MASK_SIZE(BVIEW_MAX_QUEUES_PORT)];
} BVIEW_QUEUE_MASK_t;



  /* SETMASKBIT turns on bit index # k in mask j. */
#define BVIEW_SETMASKBIT(j, k)                                     \
                ((j).value[((k-1)/(8*sizeof(BVIEW_MASK_BASE_UNIT)))]                \
                                                         |= 1 << ((k-1) % (8*sizeof(BVIEW_MASK_BASE_UNIT))))


    /* CLRMASKBIT turns off bit index # k in mask j. */
#define BVIEW_CLRMASKBIT(j, k)                                     \
               ((j).value[((k-1)/(8*sizeof(BVIEW_MASK_BASE_UNIT)))]                 \
                                                      &= ~(1 << ((k-1) % (8*sizeof(BVIEW_MASK_BASE_UNIT)))))

  /* MASKOREQ or's on the bits in mask j that are on in either mask j or k. */
#define BVIEW_MASKOREQ(j, k, _len_) {                                     \
  unsigned int x;                                                 \
  unsigned int _tmp_len_;                                                  \
  _tmp_len_ = ((_len_*8)-1)/(BVIEW_MASKWID);       \
  \
  for (x = 0; x < _tmp_len_; x++) {                      \
    (j).value[x] |= (k).value[x];                        \
  }                                                            \
}


/* ISMASKBITSET returns 0 if the interface k is not set in mask j */
#define BVIEW_ISMASKBITSET(j, k)                                   \
        ((j).value[((k-1)/(8*sizeof(BVIEW_MASK_BASE_UNIT)))]                    \
                         & ( 1 << ((k-1) % (8*sizeof(BVIEW_MASK_BASE_UNIT)))) )

#define BVIEW_SETMASKBIT(j, k)                                     \
                ((j).value[((k-1)/(8*sizeof(BVIEW_MASK_BASE_UNIT)))]                \
                            |= 1 << ((k-1) % (8*sizeof(BVIEW_MASK_BASE_UNIT))))

#if 0

  /* SETMASKBIT turns on bit index # k in mask j. */
#define BVIEW_SETMASKBIT(j, k)                                     \
              ((j).value[((k-1)/(8*sizeof(char)))]                \
                                        |= 1 << ((k-1) % (8*sizeof(char))))


  /* CLRMASKBIT turns off bit index # k in mask j. */
#define BVIEW_CLRMASKBIT(j, k)                                     \
             ((j).value[((k-1)/(8*sizeof(char)))]                 \
                                      &= ~(1 << ((k-1) % (8*sizeof(char)))))

#endif

/* FHMASKBIT finds the index of the most-significant bit turned-on in
      mask j and returns that index in k.  Since this is a 1-based
         mask, 0 is returned for "no bits set". */

#define BVIEW_FHMASKBIT(j, k, _len_) {                                    \
  int x;                                                  \
  unsigned int _tmp_len_;                                                  \
  _tmp_len_ = ((_len_*8)-1)/(BVIEW_MASKWID);       \
  for (x = _tmp_len_-1; x >= 0; x--) {               \
    if ( j.value[x] )                                    \
    break;                                       \
  };                                                           \
  k = 0;                                                       \
  if (x >= 0) {                                                \
    int i;                                          \
    for (i = 31; i >= 0; i--) {                           \
      if ( j.value[x] & (1 << i)) {                    \
        k = i + 1 + (x * (8 * sizeof(BVIEW_MASK_BASE_UNIT)));    \
        break;                                        \
      }                                                \
    }                                                    \
  };                                                           \
}

/* FLMASKBIT finds the index of the least-significant bit turned-on in
   mask j and returns that index in k.  Since this is a 1-based
   mask, 0 is returned for "no bits set". */
#define BVIEW_FLMASKBIT(j, k, _len_) {                                    \
  unsigned int x;                                                  \
  unsigned int _tmp_len_;                                                  \
  \
  _tmp_len_ = ((_len_*8)-1)/(BVIEW_MASKWID);       \
  for (x = 0; x <= (_tmp_len_ ); x++) {               \
    if ( j.value[x] )                                    \
    break;                                       \
  }                                                           \
  k = 0;                                                       \
  if (x <= _tmp_len_) {                                   \
      unsigned int i;                                          \
      for (i = 0; i <= ((8 * sizeof(BVIEW_MASK_BASE_UNIT))-1); i++) {                           \
        if ( j.value[x] & (1 << i)) {                    \
          k = i + 1 + (x * (8 * sizeof(BVIEW_MASK_BASE_UNIT)));    \
            break;                                        \
        }                                                \
      }                                                    \
  }                                                           \
}

/*  Byte swap unsigned short */
#define BVIEW_SWAP_UINT16(_val_)  ((_val_ << 8) | (_val_ >> 8 ))
/*  Byte swap unsigned short */
#define BVIEW_SWAP_UINT32(_val_, _ret_val_) \
    { \
    _val_ = ((_val_ << 8) & 0xFF00FF00 ) | ((_val_ >> 8) & 0xFF00FF ); \
     _ret_val_ = (_val_ << 16) | (_val_ >> 16); \
} 
#if 0
#define BVIEW_SWAP_UINT32(_val_) \
    ((_val_ = ((_val_ << 8) & 0xFF00FF00 ) | ((_val_ >> 8) & 0xFF00FF )), ((_val_ << 16) | (_val_ >> 16)));
#endif

#define BVIEW_PT_PRINT_PORT_MASK(_buf_) \
   do { \
 \
  BVIEW_PORT_MASK_t _temp_mask; \
  unsigned int _ii_ = 0, _len = 0; \
\
\
  _len = strlen(_buf_);\
  if (0 == _len)\
  {\
    printf("length is 0 \r\n");\
    return BVIEW_STATUS_INVALID_PARAMETER;\
  }\
\
  memset(&_temp_mask, 0, sizeof(BVIEW_PORT_MASK_t));\
  memcpy (_temp_mask.value, _buf_, _len);\
\
  BVIEW_FHMASKBIT(_temp_mask, _ii_, sizeof(BVIEW_PORT_MASK_t));\
\
  while (0 != _ii_)\
  {\
    printf("%d,",_ii_);\
    BVIEW_CLRMASKBIT(_temp_mask, _ii_);\
    BVIEW_FHMASKBIT(_temp_mask, _ii_, sizeof(BVIEW_PORT_MASK_t));\
  }\
} while (0)


#define BVIEW_PT_PRINT_CHAR_ARRAY(_buf, _len) do {\
  unsigned int ii =0, j = 0; \
  unsigned int num_col = 16;\
  int mydata = 0;\
\
  if ((NULL == _buf) || (0 == _len)) \
  {\
    printf("length is 0 or input is null ptr\r\n");\
    return BVIEW_STATUS_INVALID_PARAMETER;\
  }\
\
  printf (" printing packet \n");\
  j = 0;\
  for (ii = 0; ii < _len; ii++)\
  {\
    mydata = (int)  _buf[ii];\
    printf("%x ", mydata);\
    j++;\
    if (num_col == j)\
    {\
      j = 0;\
      printf ("\n");\
    }\
  }\
  printf ("\n");\
} while (0)


#define PT_PORT_LIST_TO_MASK_CONVERT(_buf, _mask) \
 do {\
\
  BVIEW_PORT_MASK_t _temp_mask__;\
  char _temp_buf__[PT_MAX_RANGE_STR_LEN] = {0};\
\
  int _start, _end, _length = 0, _ii=0;\
  char *_str_ptr = NULL, *_end_ptr = NULL;\
  char *_ptr = NULL;\
\
  _length = strlen(_buf);\
  if (0 == _length)\
  {\
    return BVIEW_STATUS_INVALID_PARAMETER;\
  }\
\
  memcpy (_temp_buf__, _buf, _length);\
  if(0 == strcmp (_temp_buf__, ""))\
  {\
    return BVIEW_STATUS_INVALID_PARAMETER;\
  }\
\
  _ptr = strtok_r (_temp_buf__, ",", &(_str_ptr));\
\
  memset(&_temp_mask__, 0, sizeof(BVIEW_PORT_MASK_t));\
\
  while (NULL != _ptr)\
  {\
    _end_ptr =  strstr(_ptr, "-");\
     if (NULL != _end_ptr)\
    {\
       JSON_PORT_MAP_FROM_NOTATION(_start, _ptr);\
       _end_ptr++;\
       JSON_PORT_MAP_FROM_NOTATION(_end, _end_ptr);\
     }\
     else\
      {\
       JSON_PORT_MAP_FROM_NOTATION(_start, _ptr);\
      _end = _start;\
      }\
      if ((_start > BVIEW_ASIC_MAX_PORTS) || \
          (_end > BVIEW_ASIC_MAX_PORTS))\
        return BVIEW_STATUS_INVALID_PARAMETER;\
      if (_start > _end )\
      {\
        return BVIEW_STATUS_INVALID_PARAMETER;\
      }\
      for (_ii = _start; _ii <= _end; _ii++)\
      {\
        BVIEW_SETMASKBIT(_temp_mask__, _ii);\
      }\
    _ptr = strtok_r(NULL, ",", &_str_ptr);\
\
    _start = 0;\
    _end = 0;\
  }\
\
  memcpy(_mask, &_temp_mask__, sizeof(BVIEW_PORT_MASK_t));\
\
} while (0)



#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_PORT_UTILS_H */
