/**
 *	A simple implementation of arpa/inet functions.
 *	
 *  @file endian.h
 *  @date 13 juin 2014
 *  @copyright PAWM International
 *	
 *  @author Mickael Germain
 *	
 */

#ifndef ENDIAN_H
 #define ENDIAN_H
 
 #include <inttypes.h>
 #include <string.h>
 
 /* Define if the host processor use big endian order. */
// #define ENDIAN_BIG_ENDIAN
 
 /**
  * @def    endian_htons(n)
  * @brief  Convert 2 bytes integer from host order to big endian (network) order.
  */
 
 /**
  * @def    endian_htonl(n)
  *	@brief  Convert 4 bytes integer from host order to big endian (network) order.
  */
 
 /**
  *  @def    endian_ntohs(n)
  *	 @brief  Convert 2 bytes integer from  big endian to host order.
  */
 
 /**
  * @def    endian_ntohs(n)
  * @brief  Convert 4 bytes integer from  big endian to host order.
  */
 
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
 
 /**
  *	Copies memory area to big endian.
  * 
  * Copies memory area divided on 2 bytes elements from host order to big endian.
  * 
  * @param [out] dest
  *     Reference to the area where data must be copied.
  * @param [in] src
  *     Reference where find data who must be copied.
  * @param [in] n
  *     Size of area in bytes.
  * 
  * @return Reference to the area where data must be copied.
  * 
  * @see endian_copyToB32()
  * @see endian_copyToB()
  */
 void* endian_copyToB16(void* dest, const void* src, size_t n);
 
  /**
  *	Copies memory area to big endian.
  * 
  * Copies memory area divided on 4 bytes elements from host order to big endian.
  * 
  * @param [out] dest
  *     Reference to the area where data must be copied.
  * @param [in] src
  *     Reference where find data who must be copied.
  * @param [in] n
  *     Size of area in bytes.
  * 
  * @return Reference to the area where data must be copied.
  * 
  * @see endian_copyToB16()
  * @see endian_copyToB()
  */
 void* endian_copyToB32(void* dest, const void* src, size_t n);
 
 /**
  *	Copies memory area to big endian.
  * 
  * Copies memory area divided on "size" bytes elements from host order to big endian.
  * 
  * @param [out] dest
  *     Reference to the area where data must be copied.
  * @param [in] src
  *     Reference where find data who must be copied.
  * @param [in] n
  *     Size of area in bytes.
  * @param [in] size
  *     Size of elements in the memory area in bytes.
  * 
  * @return Reference to the area where data must be copied.
  * 
  * @warning Support only 8, 16 and 32 bit.
  * 
  * @see endian_copyToB16()
  * @see endian_copyToB32()
  */
 void* endian_copyToB(void* dest, void const* src, const size_t n, const size_t size);
 
 #ifdef __cplusplus
   }
 #endif
#endif
