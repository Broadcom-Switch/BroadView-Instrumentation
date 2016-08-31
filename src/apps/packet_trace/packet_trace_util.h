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

#ifndef INCLUDE_PACKET_TRACE_UTIL_H
#define INCLUDE_PACKET_TRACE_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <time.h>
#include <ctype.h>

  typedef struct _pt_pcap_hdr_ {
    unsigned int   magic_number;   /* magic number */
    unsigned short version_major;  /* major version number */
    unsigned short version_minor;  /* minor version number */
    int            thiszone;       /* GMT to local correction */
    unsigned int   sigfigs;        /* accuracy of timestamps */
    unsigned int   snaplen;        /* max length of captured packets, in octets */
    unsigned int   network;        /* data link type */
  } PT_PCAP_HDR_t;

  typedef struct _pt_pcaprec_hdr_ {
    unsigned int ts_sec;         /* timestamp seconds */
    unsigned int ts_usec;        /* timestamp microseconds */
    unsigned int incl_len;       /* number of octets of packet saved in file */
    unsigned int orig_len;       /* actual length of packet */
  } PT_PCAPREC_HDR_t;

#define PT_PCAP_ETHERNET              0x1

#define PT_PCAP_MAGIC_NUMBER           0xa1b2c3d4
#define PT_PCAP_VERSION_NUM_MAJOR      0x2
#define PT_PCAP_VERSION_NUM_MINOR      0x4
#define PT_PCAP_THIS_ZONE              0x0
#define PT_PCAP_SIGFIGS                0x0
#define PT_PCAP_SNAPLEN                0xffff
#define PT_PCAP_NETWORK                PT_PCAP_ETHERNET  /* Ethernet */

#define PT_MAX_RANGE_STR_LEN 256
#define PT_MIN_PKT_SIZE  64
#define PT_MAX_PKT_SIZE  (4 * ((BVIEW_PT_MAX_PACKET_SIZE + 2) / 3))
#define PT_PCAP_MAJOR_NUM 2
#define PT_PCAP_MINOR_NUM 4

#define PT_MAX_IP_ADDR_LENGTH 20



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
#if 0
/* FLMASKBIT finds the index of the least-significant bit turned-on in
   mask j and returns that index in k.  Since this is a 1-based
   mask, 0 is returned for "no bits set". */
#define BVIEW_FLMASKBIT(j, k, _len_) {                                    \
  unsigned int x;                                                  \
  unsigned int _tmp_len_;                                                  \
  \
  _tmp_len_ = ((_len_*8)-1)/(BVIEW_MASKWID);       \
  for (x = 0; x <= (_tmp_len_ - 1); x++) {               \
    if ( j.value[x] )                                    \
    break;                                       \
  };                                                           \
  k = 0;                                                       \
  if (x < _tmp_len_) {                                   \
      unsigned int i;                                          \
      for (i = 0; i <= ((8 * sizeof(BVIEW_MASK_BASE_UNIT))-1); i++) {                           \
        if ( j.value[x] & (1 << i)) {                    \
          k = i + 1 + (x * (8 * sizeof(BVIEW_MASK_BASE_UNIT)));    \
            break;                                        \
        }                                                \
      }                                                    \
  };                                                           \
}
#endif
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


BVIEW_STATUS pt_base64_decode(const char *data,
        unsigned int input_length, char *decoded_data, unsigned int max_len,
            unsigned int *output_length);



#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_PACKET_TRACE_UTIL_H */
