/**
 *  Bed sensor's communication protocol.
 *
 *  Provide functions and structures in order
 *  to creates and manages communication's
 *  frames between sensor and platform.
 *
 *  @file protocol.c
 *  @date 13 juin 2014
 *  @copyright PAWM International
 *
 *  @author Mickael Germain
 *
 */
#include "protocol_api_private.h"
#include "protocol.h"

/**
 *  Read the buffer while there is no start of frame tag.
 *
 *  @return true, if a start of frame tag have been found
 *          in the buffer, false otherwise.
 */
static bool waitStartOfFrame(void);

//static const void (*parseAndCallFunc[])(void) =
// ^-- Possible ??

/**
 *  void function pointer.
 */
typedef void (*voidFunc)(void);

/**
 *  Array of parseAndCall functions for frames.
 *
 *  Each line containing a pointer of functions to the
 *  parse and call function corresponding to the frame
 *  according to the eProtocolFrame enumeration.
 *
 *  @note Content of this array are generated at compilation time
 *  according to protocol_manifest.h file.
 *
 */
static const voidFunc parseAndCallFunc[cProtocolFrameNumber] =
{
    /// @cond
#define PROTOCOL_ADD(x) &protocol_ ## x ## _parseAndCall,
#include "protocol_manifest.h"
#undef PROTOCOL_ADD
    /// @endcond
};


static bool waitStartOfFrame(void)
{
    bool bRet = false;
    while (getCursorSize() != 0 && !isStartOfFrame())
    {
        nextCursor();
    }
    if (getCursorSize() != 0)
    {
        nextCursor();
        bRet = true;
    }
    
    return bRet;
}


void protocol_parseFrameCluster(uint8_t* data, uint8_t dataSize)
{
    char buffer[PROTOCOL_FRAME_TYPE_SIZE];
    enum eProtocolFrame frameType;
    uint8_t i;
    
    assert(data != NULL);
    
    initCursor(data, dataSize);
    if (waitStartOfFrame())
    {
        if (getCursorSize() > PROTOCOL_FRAME_TYPE_SIZE)
        {
            // Get frame type tag.
            for (i = 0 ; i < PROTOCOL_FRAME_TYPE_SIZE ; i++)
            {
                buffer[i] = getCursor();
                nextCursor();
            }
            
            frameType = frameIdentification(buffer);
            if (frameType != cProtocolFrameUnknow)
            {
                parseAndCallFunc[frameType]();
            }
        }
    }
}

