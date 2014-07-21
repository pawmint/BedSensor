#ifndef PROTOCOL_YOP_H
#define PROTOCOL_YOP_H

#include "protocol.h"

/**
 *  Size of an YOP frame.
 */
#define PROTOCOL_YOP_SIZE          (   (PROTOCOL_HEADER_SIZE)                              + \
                                       (PROTOCOL_FRAME_END_SIZE)                             \
                                   )



typedef void tProtocol_YOP_data;

PROTOCOL_GENERATE_TYPEDEF(YOP)

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @copydoc protocol_AAA_setCallBackFunc()
 */
inline void protocol_YOP_setCallBackFunc(tProtocol_YOP_callBackFunc newCallBackFunc);

/**
 *  @copydoc protocol_AAA_setAskCallBackFunc()
 */
inline void protocol_YOP_setAskCallBackFunc(tProtocol_AskCallBackFunc newAskCallBackFunc);

/**
 *  @copydoc protocol_AAA_create()
 */
uint16_t protocol_YOP_create(tProtocol_YOP_buffer buffer, tProtocol_YOP_data* sData);

/**
 *  @copydoc protocol_AAA_parse()
 */
bool protocol_YOP_parse(tProtocol_YOP_data* sData);

/**
 *  @copydoc protocol_AAA_parseAndCall()
 */
void protocol_YOP_parseAndCall(void);


#ifdef __cplusplus
}
#endif
#endif