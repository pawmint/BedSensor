#ifndef PROTOCOL_ACK_H
#define PROTOCOL_ACK_H

#include "protocol.h"

/**
 *  Size of an ACK frame.
 */
#define PROTOCOL_ACK_SIZE          (   (PROTOCOL_HEADER_SIZE)                              + \
                                       (PROTOCOL_FRAME_END_SIZE)                             \
                                   )

typedef void tProtocol_ACK_data;
PROTOCOL_GENERATE_TYPEDEF(ACK)

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @copydoc protocol_AAA_setCallBackFunc()
 */
inline void protocol_ACK_setCallBackFunc(tProtocol_ACK_callBackFunc newCallBackFunc);

/**
 *  @copydoc protocol_AAA_setAskCallBackFunc()
 */
inline void protocol_ACK_setAskCallBackFunc(tProtocol_AskCallBackFunc newAskCallBackFunc);

/**
 *  @copydoc protocol_AAA_parse()
 */
bool protocol_ACK_parse(tProtocol_ACK_data* sData);

/**
 *  @copydoc protocol_AAA_parseAndCall()
 */
void protocol_ACK_parseAndCall(void);

/**
 *  @copydoc protocol_AAA_create()
 */
uint16_t protocol_ACK_create(tProtocol_ACK_buffer buffer, tProtocol_ACK_data* sData);

#ifdef __cplusplus
}
#endif
#endif