#ifndef PROTOCOL_DR1_H
#define PROTOCOL_DR1_H

#include "protocol.h"



/**
 *  Size of a DR1 frame.
 */
#define PROTOCOL_DR1_SIZE          (   (PROTOCOL_HEADER_SIZE)                              + \
                                       (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                       (PROTOCOL_FRAME_TIME_SIZE)                          + \
                                       (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                       (PROTOCOL_FRAME_FSR_SIZE) *  (PROTOCOL_FSR_NUMBER)   + \
                                       (PROTOCOL_FRAME_END_SIZE)                             \
                                   )


/**
 *  Container for DR1 frame data.
 */
struct sProtocolDR1
{
    uint32_t time;                              /**< Time of acquisition from millis(). */
    uint16_t fsrValues[PROTOCOL_FSR_NUMBER];    /**< FSR's data samples. */
};

typedef struct sProtocolDR1 tProtocol_DR1_data;
PROTOCOL_GENERATE_TYPEDEF(DR1)

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @copydoc protocol_AAA_setCallBackFunc()
 */
inline void protocol_DR1_setCallBackFunc(tProtocol_DR1_callBackFunc newCallBackFunc);

/**
 *  @copydoc protocol_AAA_setAskCallBackFunc()
 */
inline void protocol_DR1_setAskCallBackFunc(tProtocol_AskCallBackFunc newAskCallBackFunc);


/**
 *  @copydoc protocol_AAA_parse()
 */
bool protocol_DR1_parse(tProtocol_DR1_data* sData);

/**
 *  @copydoc protocol_AAA_parseAndCall()
 */
void protocol_DR1_parseAndCall(void);

/**
 *  @copydoc protocol_AAA_create()
 */
uint16_t protocol_DR1_create(tProtocol_DR1_buffer buffer, tProtocol_DR1_data const *  sData);

#ifdef __cplusplus
}
#endif
#endif