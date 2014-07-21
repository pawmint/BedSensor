/**
 *  Header example for frames handling.
 *
 *  This file is a pattern for developing support
 *  for new kind of frame. For more information,
 *  refer to @ref pageProtoFrame .
 *
 *  @file protocol_AAA.h
 *  @date 16/07/2014
 *  @copyright PAWM International
 *  @author Mickaël Germain
 *
 *  @warning This file is an example. Functions aren't supported and can't be used.
 */

#ifndef PROTOCOL_AAA_H
#define PROTOCOL_AAA_H

#include "protocol.h"

//////////////////////////////////////////////////////////////////////////
// Other includes :


//////////////////////////////////////////////////////////////////////////
// Defines :

/**
 *  Maximum size in bytes of an entire frame.
 */
#define PROTOCOL_AAA_SIZE   2


//////////////////////////////////////////////////////////////////////////
// Typedef :

/**
 *  Container for frame's data.
 */
struct sProtocolAAA
{
    uint32_t var;   /**< Description. */
};


/* Adds tProtocol_AAA_buffer and tProtocol_AAA_callBackFunc type. */
PROTOCOL_GENERATE_TYPEDEF(AAA)

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Sets function call when a suitable frame is received
 *
 *  @param [in] newCallBackFunc
 *  The reference to the function to call when the frame is received. Can be NULL.
 */
inline void protocol_AAA_setCallBackFunc(tProtocol_AAA_callBackFunc newCallBackFunc);

/**
 *  Sets function call when a suitable ask frame is received
 *
 *  @param [in] newAskCallBackFunc
 *  The reference to the function to call when the frame is received. Can be NULL.
 */
inline void protocol_AAA_setAskCallBackFunc(tProtocol_AskCallBackFunc newAskCallBackFunc);

/**
 *  Tries to parse a frame.
 *
 *  Tries to parse a a frame from the current cursor.
 *  The parser expect to start parsing after the identification
 *  of frame tag (after $AAA)
 *
 *  @param [out] sData
 *  Reference to the memory area where store data extracted from the AAA frame.
 *  If no data are expected, use NULL.
 *
 *  @return true, if the frame have been correctly parsed, false otherwise.
 *
 */
bool protocol_AAA_parse(tProtocol_AAA_data* sData);

/**
 *  Parses the frame and then call the callback function with extracted data.
 *
 *  @note This function is a wrapper to allow protocol_parseFrameCluster() to work with all
 *  kind of frame without modification.
 */
void protocol_AAA_parseAndCall(void);

/**
 *  Creates a suitable frame.
 *
 *  @param [in,out] buffer
 *  Reference where store the created frame.
 *  @param [in] sData
 *  Reference to data to be putted in the frame.
 *
 *  @return The length of the frame.
 */
uint16_t protocol_AAA_create(tProtocol_AAA_buffer buffer, tProtocol_AAA_data const *  sData);

//////////////////////////////////////////////////////////////////////////
// Other functions:

#ifdef __cplusplus
}
#endif
#endif