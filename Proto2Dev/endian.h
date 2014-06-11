#ifndef ENDIAN_H
 #define ENDIAN_H
 
 #include <inttypes.h>
 #include <string.h>

 #ifdef ENDIAN_BIG_ENDIAN
  #define endian_htons(n)           (n)
  #define endian_htonl(n)           (n)
 #else
  #define endian_htons(n)           (((((uint16_t)(n) & 0xFF)) << 8) | (((uint16_t)(n) & 0xFF00) >> 8))
  #define endian_htonl(n)           (   ((((uint32_t)(n) & 0xFF))<< 24)         | \
                                        ((((uint32_t)(n) & 0xFF00)) << 8)       | \
                                        ((((uint32_t)(n) & 0xFF0000)) >> 8)     | \
                                        ((((uint32_t)(n) & 0xFF000000)) >> 24)    \
                                    )
 #endif

 #define endian_ntohs(n)            endian_htons(n)
 #define endian_ntohl(n)            endian_htonl(n)

 #ifdef __cplusplus
  extern "C"{
 #endif
 
 void* endian_copyToB16(void* dest, const void* src, size_t n);
 void* endian_copyToB32(void* dest, const void* src, size_t n);
 void* endian_copyToB(void* dest, void const* src, const size_t n, const size_t size);
 
 #ifdef __cplusplus
  }
 #endif
#endif
