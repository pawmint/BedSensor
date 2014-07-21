#ifndef PROTOCOL_DC1_H
#define PROTOCOL_DC1_H

#include "protocol.h"



/**
 *  Size of a DC1 frame.
 */
#define PROTOCOL_DC1_SIZE          (   (PROTOCOL_HEADER_SIZE)                              + \
                                       (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                       (PROTOCOL_FRAME_TIME_SIZE)                          + \
                                       (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                       (PROTOCOL_FRAME_FSC_SIZE) *  (PROTOCOL_FSC_NUMBER)   + \
                                       (PROTOCOL_FRAME_END_SIZE)                             \
                                   )


/**
 *  Container for DC1 frame data.
 */
struct sProtocolDC1
{
    uint32_t time;                              /**< Time of acquistion from millis(). */
    uint16_t fscValues[PROTOCOL_FSC_NUMBER];    /**< FSC's data samples. */
};

typedef struct sProtocolDC1 tProtocol_DC1_data;
PROTOCOL_GENERATE_TYPEDEF(DC1)

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @copydoc protocol_AAA_setCallBackFunc()
 */
inline void protocol_DC1_setCallBackFunc(tProtocol_DC1_callBackFunc newCallBackFunc);

/**
 *  @copydoc protocol_AAA_setAskCallBackFunc()
 */
inline void protocol_DC1_setAskCallBackFunc(tProtocol_AskCallBackFunc newAskCallBackFunc);

/**
 *  @copydoc protocol_AAA_parse()
 */
bool protocol_DC1_parse(tProtocol_DC1_data* sData);

/**
 *  @copydoc protocol_AAA_parseAndCall()
 */
void protocol_DC1_parseAndCall(void);

/**
 *  @copydoc protocol_AAA_create()
 */
uint16_t protocol_DC1_create(tProtocol_DC1_buffer buffer, tProtocol_DC1_data const *  sData);

#ifdef __cplusplus
}
#endif
#endif