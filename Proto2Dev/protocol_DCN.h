#ifndef PROTOCOL_DCN_H
#define PROTOCOL_DCN_H

#include "protocol.h"
#include "protocol_DC1.h"
/**
 *  Size between a DCN frame of n elements and a DCN frame of n + 1 elements.
 */
#define PROTOCOL_DCN_VAR_SIZE      (   (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                       (PROTOCOL_FRAME_FSC_SIZE) *  (PROTOCOL_FSC_NUMBER)     \
                                   )
/**
 *  Minimum size of a DCN frame.
 */
#define PROTOCOL_DCN_MIN_SIZE      (   (PROTOCOL_HEADER_SIZE)                              + \
                                       (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                       (PROTOCOL_FRAME_TIME_SIZE)                          + \
                                       (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                       (PROTOCOL_FRAME_TIME_SIZE)                          + \
                                       (PROTOCOL_DCN_VAR_SIZE)                             + \
                                       (PROTOCOL_FRAME_END_SIZE)                             \
                                   )


/**
 *  Maximum number of samples in a DCN frame.
 */
#define PROTOCOL_DCN_SAMPLE_MAX    (((PROTOCOL_DATA_SIZE_MAX) - (PROTOCOL_DCN_MIN_SIZE)) / (PROTOCOL_DCN_VAR_SIZE) + 1)

#define PROTOCOL_DCN_SIZE   PROTOCOL_DATA_SIZE_MAX


/**
 *  Container for DCN frame data.
 */
struct sProtocolDCN
{
    uint32_t time;                                                      /**< Time of the first acquisition from millis(). */
    uint32_t delta;                                                     /**< Time between both consecutive sampling waves. */
    uint16_t fscValues[PROTOCOL_DCN_SAMPLE_MAX][PROTOCOL_FSC_NUMBER];   /**< FSC's data from sampling waves sort by time of acquisition. */
    uint16_t nbSamples;                                                 /**< Number of sampling waves in the frame. */
};

typedef struct sProtocolDCN tProtocol_DCN_data;
typedef struct sProtocolDC1 tProtocol_DCN_init;
PROTOCOL_GENERATE_TYPEDEF(DCN)

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @copydoc protocol_AAA_setCallBackFunc()
 */
inline void protocol_DCN_setCallBackFunc(tProtocol_DCN_callBackFunc newCallBackFunc);

/**
 *  @copydoc protocol_AAA_setAskCallBackFunc()
 */
inline void protocol_DCN_setAskCallBackFunc(tProtocol_AskCallBackFunc newAskCallBackFunc);

/**
 *  @copydoc protocol_AAA_parse()
 */
bool protocol_DCN_parse(tProtocol_DCN_data* sData);

/**
 *  @copydoc protocol_AAA_parseAndCall()
 */
void protocol_DCN_parseAndCall(void);

/**
 *  @copydoc protocol_AAA_create()
 */
uint16_t protocol_DCN_create(tProtocol_DCN_buffer buffer, tProtocol_DCN_data const *  sData);

uint16_t protocol_DCN_init(uint8_t buffer[PROTOCOL_DCN_MIN_SIZE - PROTOCOL_FRAME_END], tProtocol_DCN_init const* sData, const uint32_t delta);
uint16_t protocol_DCN_extend(uint8_t buffer[PROTOCOL_DCN_VAR_SIZE], uint16_t const fscValues[PROTOCOL_FSC_NUMBER]);
uint16_t protocol_DCN_end(uint8_t buffer[PROTOCOL_FRAME_END_SIZE]);

#ifdef __cplusplus
}
#endif
#endif