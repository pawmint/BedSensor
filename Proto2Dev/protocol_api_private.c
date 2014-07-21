/**
 *  Parsing and creating functions for protocol handling.
 *
 *  @file protocol_api_private.c
 *  @date 21 jul. 2014
 *  @copyright PAWM International
 *
 *  @author Mickael Germain
 *
 */

#include "protocol_api_private.h"

/**
 *  Array of frames tag.
 *
 *  Each line containing a string corresponding to the
 *  tag of the frame according to the eProtocolFrame enumeration.
 *
 *  @note Content of this array are generated at compilation time
 *  according to protocol_manifest.h file.
 *
 */
static char const protocol_frameId[cProtocolFrameNumber][PROTOCOL_FRAME_TYPE_SIZE + 1] =
{
    /// @cond
#define PROTOCOL_ADD(x) #x ,
#include "protocol_manifest.h"
#undef PROTOCOL_ADD
    /// @endcond
};

//////////////////////////////////////////////////////////////////////////
// Cursor utilities :

typedef struct
{
    uint8_t* data;
    uint8_t dataSize;
} tCursor;

static tCursor cur;


inline void initCursor(uint8_t* data, uint8_t dataSize)
{
    cur.data = data;
    cur.dataSize = dataSize;
}

inline void moveCursor(uint8_t i)
{
    assert(i < getCursorSize());
    cur.data += i;
    cur.dataSize -= i;
}

inline void nextCursor(void)
{
    moveCursor(1);
}

inline uint8_t getCursorSize(void)
{
    return cur.dataSize;
}

inline uint8_t getCursor(void)
{
    assert(getCursorSize() != 0);
    return cur.data[0];
}

//////////////////////////////////////////////////////////////////////////

inline bool is(uint8_t c)
{
    assert(getCursorSize() != 0);
    
    return (getCursor() == c);
}

inline bool isStartOfFrame(void)
{
    return is(PROTOCOL_FRAME_START);
}

inline bool isEndOfFrame(void)
{
    return is(PROTOCOL_FRAME_END);
}

inline bool isAskByte(void)
{
    return is(PROTOCOL_FRAME_ASK);
}

inline bool isSeparator(void)
{
    return is(PROTOCOL_FRAME_SEP);
}

bool parseAskFrame(void)
{
    bool bOk = false;
    
    if (getCursorSize() >= PROTOCOL_FRAME_ASK_SIZE + PROTOCOL_FRAME_END_SIZE)
    {
        if (isAskByte())
        {
            nextCursor();
            if (isEndOfFrame())
            {
                nextCursor();
                bOk = true;
            }
        }
    }
    
    return bOk;
}

//////////////////////////////////////////////////////////////////////////

inline uint8_t peek8(void)
{

    assert(getCursorSize() >= sizeof(uint8_t));
    return endian_ntohs(getCursor());
}

uint16_t peek16(void)
{
    uint16_t value;
    
    assert(getCursorSize() >= sizeof(uint16_t));
    
    value  = ((uint16_t)cur.data[0] << 8);
    value += ((uint16_t)cur.data[1]);
    
    return endian_ntohs(value);
}

uint32_t peek32(void)
{
    uint32_t value;
    
    assert(getCursorSize() >= sizeof(uint32_t));
    
    value  = ((uint32_t)cur.data[0]) << 24;
    value += ((uint32_t)cur.data[1]) << 16;
    value += ((uint32_t)cur.data[2]) << 8;
    value += ((uint32_t)cur.data[3]);
    
    return endian_ntohl(value);
}

uint8_t read8(void)
{
    uint8_t value = peek8();
    moveCursor(sizeof(uint8_t));
    return value;
}

uint16_t read16(void)
{
    uint16_t value = peek16();
    moveCursor(sizeof(uint16_t));
    return value;
}

uint32_t read32(void)
{
    uint32_t value = peek32();
    moveCursor(sizeof(uint32_t));
    return value;
}

void readFSC(uint16_t* fscValues)
{
    uint8_t iterFsc;
    
    assert(fscValues != NULL);
    
    for (iterFsc = 0 ; iterFsc < PROTOCOL_FSC_NUMBER ; iterFsc++)
        fscValues[iterFsc] = read16();
        
    moveCursor(sizeof(uint16_t) *  PROTOCOL_FSC_NUMBER);
}

void readFSR(uint16_t* fsrValues)
{
    uint8_t iterFsr;
    
    assert(fsrValues != NULL);
    
    for (iterFsr = 0 ; iterFsr < PROTOCOL_FSR_NUMBER ; iterFsr++)
        fsrValues[iterFsr] = read16();
        
    moveCursor(sizeof(uint16_t) *  PROTOCOL_FSR_NUMBER);
}

//////////////////////////////////////////////////////////////////////////

void addSep(uint8_t* buffer, uint16_t* pos)
{
    buffer[*pos] = PROTOCOL_FRAME_SEP;
    *pos += PROTOCOL_FRAME_SEP_SIZE;
}

void addFrameId(uint8_t* buffer, uint16_t* pos, enum eProtocolFrame id)
{
    memcpy(buffer + *pos, protocol_frameId[id], PROTOCOL_FRAME_TYPE_SIZE);
    *pos += PROTOCOL_FRAME_TYPE_SIZE;
}

void addStart(uint8_t* buffer, uint16_t* pos)
{
    buffer[*pos] = PROTOCOL_FRAME_START;
    *pos += PROTOCOL_FRAME_START_SIZE;
}

void addEnd(uint8_t* buffer, uint16_t* pos)
{
    buffer[*pos] = PROTOCOL_FRAME_END;
    *pos += PROTOCOL_FRAME_END_SIZE;
}

//////////////////////////////////////////////////////////////////////////

enum eProtocolFrame frameIdentification(char const buffer[PROTOCOL_FRAME_TYPE_SIZE])
{
    assert(buffer != NULL);
    
    enum eProtocolFrame iterFrame = 0;
    while (iterFrame < cProtocolFrameNumber && strncmp(buffer, protocol_frameId[iterFrame], PROTOCOL_FRAME_TYPE_SIZE) != 0)
        iterFrame++;
        
    assert(iterFrame <= cProtocolFrameNumber);
    
    if (iterFrame == cProtocolFrameNumber)
        iterFrame = cProtocolFrameUnknow;
        
    return iterFrame;
}