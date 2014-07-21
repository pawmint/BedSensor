/**
 *  Bed sensor's communication protocol.
 *
 *  Provide functions and structures in order
 *  to creates and manages communication's
 *  frames between sensor and platform.
 *
 *  @file protocol.h
 *  @date 17 juillet 2014
 *  @copyright PAWM International
 *
 *  @author Mickael Germain
 *
 *  @todo global tutorial.
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "endian.h"

/**
 *  Leading character of frames.
 */
#define PROTOCOL_FRAME_START       '$'
/**
 *  Terminating character of frames.
 */
#define PROTOCOL_FRAME_END         '\n'
/**
 *  Separating character between argument in frames.
 */
#define PROTOCOL_FRAME_SEP         ','
/**
 *  Asking character for a frame.
 */
#define PROTOCOL_FRAME_ASK         '?'

/**
 *  Size of the start of frames.
 */
#define PROTOCOL_FRAME_START_SIZE  sizeof(char)
/**
 *  Size of a ask character.
 */
#define PROTOCOL_FRAME_ASK_SIZE    sizeof(char)
/**
 *  Size of the end of frames.
 */
#define PROTOCOL_FRAME_END_SIZE    sizeof(char)
/**
 *  Size of a separator in frames.
 */
#define PROTOCOL_FRAME_SEP_SIZE    sizeof(char)
/**
 *  Size of a frame type identifier.
 */
#define PROTOCOL_FRAME_TYPE_SIZE   sizeof(char) *  3

/**
* Size of the leading common part of frames.
*/
#define PROTOCOL_HEADER_SIZE       (   (PROTOCOL_FRAME_START_SIZE) +                             \
                                       (PROTOCOL_FRAME_TYPE_SIZE)                                \
                                   )


#define PROTOCOL_FRAME_TIME_SIZE   sizeof(uint32_t)

/**
 *  Maximum size of a frame in this protocol.
 */
#define PROTOCOL_DATA_SIZE_MAX     1024

/**
 *  @define PROTOCOL_FSR_NUMBER
 *  Number of FSR sensors in the bed.
 *
 *  @todo Think about an extern constant.
 */
#if 0
#ifdef ONESENSOR
#define PROTOCOL_FSR_NUMBER        1
#else
#define PROTOCOL_FSR_NUMBER        8
#endif
#endif
#define PROTOCOL_FSR_NUMBER        8
/**
 *  Number of FSC sensors in the bed.
 *
 *  @todo Think about an extern constant.
 */
#define PROTOCOL_FSC_NUMBER        2

/**
 *  Size of an FSR sensor's data in frames.
 *
 *  @todo put it in an fsr header.
 */
#define PROTOCOL_FRAME_FSR_SIZE    sizeof(uint16_t)

/**
 *  Size of an FSC sensor's data in frames.
 */
#define PROTOCOL_FRAME_FSC_SIZE    sizeof(uint16_t)

/**
 *  Generates variable for storing reference of callback function in frames handling files.
 */
#define PROTOCOL_GENERATE_FUNCVAR(x)                                                                \
    static tProtocol_ ## x ## _callBackFunc callBackFunc = NULL;                                    \
    static tProtocol_AskCallBackFunc askCallBackFunc = NULL;

/**
 *  Generates setter for variable define with PROTOCOL_GENERATE_FUNCVAR() .
 */
#define PROTOCOL_GENERATE_FUNCVAR_SETTER(x)                                                         \
    inline void protocol_ ## x ## _setCallBackFunc(tProtocol_ ## x ## _callBackFunc newCallBackFunc)\
    {                                                                                               \
        callBackFunc = newCallBackFunc;                                                             \
    }                                                                                               \
    \
    inline void protocol_ ## x ## _setAskCallBackFunc(tProtocol_AskCallBackFunc newAskCallBackFunc) \
    {                                                                                               \
        askCallBackFunc = newAskCallBackFunc;                                                       \
    }

/**
 *  Generates prototype for setter define with PROTOCOL_GENERATE_FUNCVAR_SETTER() .
 */
#define PROTOCOL_GENERATE_FUNCVAR_PROTOTYPE(x)                                                      \
    void protocol_ ## x ## _setCallBackFunc(tProtocol_ ## x ## _callBackFunc newCallBackFunc);      \
    void protocol_ ## x ## _setAskCallBackFunc(tProtocol_AskCallBackFunc newAskCallBackFunc);


/**
 *  Generates typedef for frame buffer and frame callback function.
 */
#define PROTOCOL_GENERATE_TYPEDEF(x)                                                                \
    typedef uint8_t tProtocol_ ## x ## _buffer [PROTOCOL_ ## x ## _SIZE];                           \
    typedef void (*tProtocol_ ## x ## _callBackFunc)(tProtocol_ ## x ## _data*);

/**
 *  Generates wrapper function to parse a frame and then call the right callback functions with data.
 */
#define PROTOCOL_GENERATE_PARSE_AND_CALL(x)                                                         \
    void protocol_ ## x ## _parseAndCall(void)                                                      \
    {                                                                                               \
        if (parseAskFrame())                                                                        \
        {                                                                                           \
            if (askCallBackFunc != NULL)                                                            \
                askCallBackFunc();                                                                  \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            tProtocol_ ## x ## _data data;                                                          \
            if (protocol_ ## x ## _parse(&data))                                                    \
            {                                                                                       \
                if (callBackFunc != NULL)                                                           \
                    callBackFunc(&data);                                                            \
            }                                                                                       \
        }                                                                                           \
    }

/**
 *  Generates wrapper function to parse a frame and then call the right callback functions without data.
 */
#define PROTOCOL_GENERATE_PARSE_AND_CALL_VOID(x)                                                    \
    void protocol_ ## x ## _parseAndCall(void)                                                      \
    {                                                                                               \
        if (parseAskFrame())                                                                        \
        {                                                                                           \
            if (askCallBackFunc != NULL)                                                            \
                askCallBackFunc();                                                                  \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            if (protocol_ ## x ## _parse(NULL))                                                     \
            {                                                                                       \
                if (callBackFunc != NULL)                                                           \
                    callBackFunc(NULL);                                                             \
            }                                                                                       \
        }                                                                                           \
    }

/**
 *  Enumeration of all frame's type.
 *
 *  This enumeration allow to map frame's type
 *  to an ID. Values are generated at compilation time
 *  according to protocol_anifests.h . Each member respect
 *  the pattern cProtocolFrameIDF where IDF is the ID of the
 *  considered frame.
 *
 *  @note this enumeration should be used as an index for array
 *  who groups data from frames.
 *
 *  @see protocol_frameIdentification()
 *  @todo STA
 */
enum eProtocolFrame
{
    ///@cond
#define PROTOCOL_ADD(x) cProtocolFrame ## x,
#include "protocol_manifest.h"
#undef PROTOCOL_ADD
    ///@endcond
    cProtocolFrameNumber,
    cProtocolFrameUnknow
};

/**
 *  @var eProtocolFrame cProtocolFrameNumber
 *  Number of frame's type.
 */

/**
 *  @var eProtocolFrame cProtocolFrameUnknow
 *  Value for an unknown type of frame.
 */

/**
 *  Type of function who ca be call when a ask frame is received.
 */
typedef void (*tProtocol_AskCallBackFunc)(void);

#include "protocol_ACK.h"
#include "protocol_YOP.h"
#include "protocol_ERR.h"
#include "protocol_MOD.h"
#include "protocol_DR1.h"
#include "protocol_DC1.h"
#include "protocol_DCN.h"

/**
 *  Extract frame from a buffer and call function for processing.
 *
 *  @note Cut frames will be treat as incorrect frames.
 *
 *  @param [in] data
 *      Reference to the buffer to be parsed.
 *  @param [in] dataSize
 *      Size of the buffer in bytes.
 *
 */
void protocol_parseFrameCluster(uint8_t* data, uint8_t dataSize);
#endif
