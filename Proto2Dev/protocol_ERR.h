#ifndef PROTOCOL_ERR_H
#define PROTOCOL_ERR_H

#include "protocol.h"

/**
 *  Size of an ERR frame.
 */
#define PROTOCOL_ERR_SIZE          (   (PROTOCOL_HEADER_SIZE)                              + \
                                       (PROTOCOL_FRAME_END_SIZE)                             \
                                   )


typedef void tProtocol_ERR_data;
PROTOCOL_GENERATE_TYPEDEF(ERR)

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @copydoc protocol_AAA_setCallBackFunc()
 */
inline void protocol_ERR_setCallBackFunc(tProtocol_ERR_callBackFunc newCallBackFunc);

/**
 *  @copydoc protocol_AAA_setAskCallBackFunc()
 */
inline void protocol_ERR_setAskCallBackFunc(tProtocol_AskCallBackFunc newAskCallBackFunc);


/**
 *  @copydoc protocol_AAA_parse()
 */
bool protocol_ERR_parse(tProtocol_ERR_data* sData);

/**
 *  @copydoc protocol_AAA_parseAndCall()
 */
void protocol_ERR_parseAndCall(void);

/**
 *  @copydoc protocol_AAA_create()
 */
uint16_t protocol_ERR_create(tProtocol_ERR_buffer buffer, tProtocol_ERR_data* sData);

#ifdef __cplusplus
}
#endif
#endif