#ifndef PROTOCOL_STA_H
#define PROTOCOL_STA_H

#include "protocol.h"

/**
 *  Size of a STA frame.
 */
#define PROTOCOL_STA_SIZE          (   (PROTOCOL_HEADER_SIZE)                              + \
                                       (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                       (PROTOCOL_FRAME_TIME_SIZE)                          + \
                                       (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                       (PROTOCOL_FRAME_BAT_SIZE)                           + \
                                       (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                       (PROTOCOL_FRAME_FSR_NUMBER_SIZE)                    + \
                                       (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                       (PROTOCOL_FRAME_FSC_NUMBER_SIZE)                    + \
                                       (PROTOCOL_FRAME_END_SIZE)                             \
                                   )


/**
 *  Container for STA frame data.
 */
struct sProtocolSTA
{
    uint32_t syncTime;      /**< Time from millis(). */
    uint8_t batLvl;         /**< Battery level in % [000;100]. */
    uint8_t nbFsr;          /**< Number of FSR sensors. [00;99]*/
    uint8_t nbFsc;          /**< Number of FSC sensors. [00;99]*/
};

typedef struct sProtocolSTA tProtocol_STA_data;
PROTOCOL_GENERATE_TYPEDEF(STA)

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @copydoc protocol_AAA_setCallBackFunc()
 */
inline void protocol_STA_setCallBackFunc(tProtocol_STA_callBackFunc newCallBackFunc);

/**
 *  @copydoc protocol_AAA_setAskCallBackFunc()
 */
inline void protocol_STA_setAskCallBackFunc(tProtocol_AskCallBackFunc newAskCallBackFunc);

/**
 *  @copydoc protocol_AAA_parse()
 */
bool protocol_STA_parse(tProtocol_STA_data* sData);

/**
 *  @copydoc protocol_AAA_parseAndCall()
 */
void protocol_STA_parseAndCall(void);

/**
 *  @copydoc protocol_AAA_create()
 */
uint16_t protocol_STA_create(tProtocol_STA_buffer buffer, tProtocol_STA_data const *  sData);

#ifdef __cplusplus
}
#endif
#endif