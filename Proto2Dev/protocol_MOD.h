#ifndef PROTOCOL_MOD_H
#define PROTOCOL_MOD_H

#include "protocol.h"

/**
 *  Size of a runtime mode identifier in a frames.
 */
#define PROTOCOL_FRAME_MOD_SIZE    sizeof(enum eProtocolMode)

/**
 *  Size of an MOD frame.
 */
#define PROTOCOL_MOD_SIZE          (   (PROTOCOL_HEADER_SIZE)                              + \
                                       (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                       (PROTOCOL_FRAME_MOD_SIZE)                           + \
                                       (PROTOCOL_FRAME_END_SIZE)                             \
                                   )


/**
 *  Enumeration of bed sensor's runtime mode.
 */
enum eProtocolMode
{
    cProtocolModeSleep = 0, /**< Waiting for a person lies. */
    cProtocolModeNormal,    /**< Sampling with a normal accuracy in order to monitor person's breathing. */
    cProtocolModeAccurate,  /**< Sampling with a high resolution in order to detect and analyze problems. */
    /* ^-insert new runtime mode at the end-^ */
    /*-----------END OF ENUMERATION-----------*/
    cProtocolModeNumber,    /**< Number of runtime modes. */
    cProtocolModeUnknow     /**< Value for an unknown runtime modes. */
};

typedef enum eProtocolMode tProtocol_MOD_data;
PROTOCOL_GENERATE_TYPEDEF(MOD)

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @copydoc protocol_AAA_setCallBackFunc()
 */
inline void protocol_MOD_setCallBackFunc(tProtocol_MOD_callBackFunc newCallBackFunc);

/**
 *  @copydoc protocol_AAA_setAskCallBackFunc()
 */
inline void protocol_MOD_setAskCallBackFunc(tProtocol_AskCallBackFunc newAskCallBackFunc);

/**
 *  @copydoc protocol_AAA_parse()
 */
bool protocol_MOD_parse(tProtocol_MOD_data* sData);

/**
 *  @copydoc protocol_AAA_parseAndCall()
 */
void protocol_MOD_parseAndCall(void);

/**
 *  @copydoc protocol_AAA_create()
 */
uint16_t protocol_MOD_create(tProtocol_MOD_buffer buffer, tProtocol_MOD_data const *  sData);

#ifdef __cplusplus
}
#endif
#endif