/**
 *  Header of protocol_api_private.c
 *
 *  @file protocol_api_private.c
 *  @date 21 jul. 2014
 *  @copyright PAWM International
 *
 *  @author Mickael Germain
 *  @todo complete documentation
 */

#ifndef PROTOCOL_API_PRIVATE_H_
#define PROTOCOL_API_PRIVATE_H_

#include "protocol.h"
#include <inttypes.h>
#include <assert.h>


#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


/**
 *  @todo doc
 */
inline bool is(uint8_t c);

/**
 *  @todo doc
 */
inline bool isSeparator(void);

/**
 *  @todo doc
 */
inline bool isAskByte(void);

/**
 *  @todo doc
 */
inline bool isStartOfFrame(void);

/**
 *  @todo doc
 */
inline bool isEndOfFrame(void);

/**
 *  @todo doc
 */
inline  uint8_t peek8(void);

/**
 *  @todo doc
 */
uint16_t peek16(void);

/**
 *  @todo doc
 */
uint32_t peek32(void);
/**
 *  @todo doc
 */
bool parseAskFrame(void);

/**
 *  Read an integer of 1 byte from set stream.
 *
 *  @internal
 *
 *  @return The integer read.
 *
 *  @see protocol_read16()
 *  @see protocol_read32()
 */
uint8_t read8(void);

/**
 *  Read an integer of 2 bytes from set stream.
 *
 *  @internal
 *
 *  @note The integer should be in big endian format and
 *    will be convert to the host format.
 *
 *  @return The integer read.
 *
 *  @see protocol_read8()
 *  @see protocol_read32()
 */
uint16_t read16(void);

/**
 *  Read an integer of 4 bytes from set stream.
 *
 *  @internal
 *
 *  @note The integer should be in big endian format and
 *    will be convert to the host format.
 *
 *  @return The integer read.
 *
 *  @see protocol_read8()
 *  @see protocol_read16()
 */
uint32_t read32(void);


/**
 *  @todo doc
 */
inline void initCursor(uint8_t* data, uint8_t dataSize);

/**
 *  @todo doc
 */
inline void moveCursor(uint8_t i);

/**
 *  @todo doc
 */
inline void nextCursor(void);

/**
 *  @todo doc
 */
inline uint8_t getCursor(void);

/**
 *  @todo doc
 */
inline uint8_t getCursorSize(void);

/**
 *  @todo doc
 */
void addSep(uint8_t* buffer, uint16_t* pos);

/**
 *  @todo doc
 */
void addFrameId(uint8_t* buffer, uint16_t* pos, enum eProtocolFrame id);

/**
 *  @todo doc
 */
void addStart(uint8_t* buffer, uint16_t* pos);

/**
 *  @todo doc
 */
void addEnd(uint8_t* buffer, uint16_t* pos);

/**
 *  Read FSC's values from set stream.
 *
 *  @internal
 *
 *  @param [out] fscValues
 *  Reference where store values.
 */
void readFSC(uint16_t* fscValues);

/**
 *  Read FSR's values from set stream.
 *
 *  @internal
 *
 *  @param [out] fsrValues
 *  Reference where store values.
 */
void readFSR(uint16_t* fsrValues);

/**
* Identifies the type of frame.
*
*   @param [in] buffer
*     String containing the frame identifier to be analyzed.
*
*   @return The corresponding value of the identified frame hopefully, cProtocolFrameUnknow otherwise.
*/
enum eProtocolFrame frameIdentification(char const buffer[PROTOCOL_FRAME_TYPE_SIZE]);

#ifdef __cplusplus
}
#endif
#endif