#include "protocol_DCN.h"
#include "protocol_api_private.h"

static tProtocol_DCN_callBackFunc callBackFunc;
static tProtocol_AskCallBackFunc askCallBackFunc;

PROTOCOL_GENERATE_FUNCVAR_SETTER(DCN)

PROTOCOL_GENERATE_PARSE_AND_CALL(DCN)

bool protocol_DCN_parse(tProtocol_DCN_data* sData)
{
    bool bOk = false;
    uint16_t nbSamples;
    
    assert(sData != NULL);
    
    if (getCursorSize() >= PROTOCOL_DCN_MIN_SIZE - PROTOCOL_HEADER_SIZE)
    {
        if (isSeparator())
        {
            nextCursor();
            sData->time = read32();
            if (isSeparator())
            {
                nextCursor();
                sData->delta = read32();
                if (isSeparator())
                {
                    nbSamples = 0;
                    do
                    {
                        nextCursor();
                        /* At least one wave of sampling. */
                        readFSC(sData->fscValues[nbSamples]);
                        
                        /* if there is a separator then parse an other wave. */
                    }
                    while (isSeparator() &&
                            getCursorSize() >= PROTOCOL_DCN_VAR_SIZE + PROTOCOL_FRAME_END &&
                            ++nbSamples <= PROTOCOL_DCN_SAMPLE_MAX);
                    /* Too much waves ? */
                    if (isEndOfFrame())
                    {
                        sData->nbSamples = nbSamples;
                        nextCursor();
                        bOk = true;
                    }
                }
            }
        }
        
    }
    return bOk;
}

uint16_t protocol_DCN_create(tProtocol_DCN_buffer buffer, tProtocol_DCN_data const *  sData)
{
    /* $DCN,<TIME>,<DELTA>,<C0><C1>...<CN>[,<C0><C1>...<CN>]\n */
    uint16_t pos;
    uint16_t iterSamples;
    
    assert(sData != NULL);
    assert(buffer != NULL);
    assert(sData->nbSamples <= PROTOCOL_DCN_SAMPLE_MAX);
    
    pos = 0;
    
    addStart(buffer, &pos);
    
    addFrameId(buffer, &pos, cProtocolFrameDCN);
    
    addSep(buffer, &pos);
    
    endian_copyToB(buffer + pos, &(sData->time), PROTOCOL_FRAME_TIME_SIZE, PROTOCOL_FRAME_TIME_SIZE);
    pos += PROTOCOL_FRAME_TIME_SIZE;
    
    addSep(buffer, &pos);
    
    endian_copyToB(buffer + pos, &(sData->delta), PROTOCOL_FRAME_TIME_SIZE, PROTOCOL_FRAME_TIME_SIZE);
    pos += PROTOCOL_FRAME_TIME_SIZE;
    
    for (iterSamples = 0 ; iterSamples < sData->nbSamples ; iterSamples++)
    {
        buffer[pos] = PROTOCOL_FRAME_SEP;
        pos += PROTOCOL_FRAME_SEP_SIZE;
        
        endian_copyToB(buffer + pos, sData->fscValues[iterSamples], PROTOCOL_FRAME_FSC_SIZE *  PROTOCOL_FSC_NUMBER, PROTOCOL_FRAME_FSC_SIZE);
        pos += PROTOCOL_FRAME_FSC_SIZE *  PROTOCOL_FSC_NUMBER;
    }
    
    addEnd(buffer, &pos);
    
    assert(pos >= PROTOCOL_DCN_MIN_SIZE);
    assert(pos < PROTOCOL_DATA_SIZE_MAX);
    
    return pos;
}

uint16_t protocol_DCN_init(uint8_t buffer[PROTOCOL_DCN_MIN_SIZE - PROTOCOL_FRAME_END], tProtocol_DCN_init const* sData, const uint32_t delta)
{
    /* $DCN,<TIME>,<DELTA>,<C0><C1>...<CN> */
    uint16_t pos;
    
    assert(sData != NULL);
    assert(buffer != NULL);
    
    pos = 0;
    
    addStart(buffer, &pos);
    
    addFrameId(buffer, &pos, cProtocolFrameDCN);
    
    addSep(buffer, &pos);
    
    endian_copyToB(buffer + pos, &(sData->time), PROTOCOL_FRAME_TIME_SIZE, PROTOCOL_FRAME_TIME_SIZE);
    pos += PROTOCOL_FRAME_TIME_SIZE;
    
    addSep(buffer, &pos);
    
    endian_copyToB(buffer + pos, &delta, PROTOCOL_FRAME_TIME_SIZE, PROTOCOL_FRAME_TIME_SIZE);
    pos += PROTOCOL_FRAME_TIME_SIZE;
    
    pos += protocol_DCN_extend(buffer + pos, sData->fscValues);
    
    assert(pos == PROTOCOL_DCN_MIN_SIZE - PROTOCOL_FRAME_END);
    
    return pos;
}

uint16_t protocol_DCN_extend(uint8_t buffer[PROTOCOL_DCN_VAR_SIZE], uint16_t const fscValues[PROTOCOL_FSC_NUMBER])
{
    uint16_t pos;
    
    assert(fscValues != NULL);
    assert(buffer != NULL);
    
    pos = 0;
    
    addSep(buffer, &pos);
    
    endian_copyToB(buffer + pos, fscValues, PROTOCOL_FRAME_FSC_SIZE *  PROTOCOL_FSC_NUMBER, PROTOCOL_FRAME_FSC_SIZE);
    pos += PROTOCOL_FRAME_FSC_SIZE *  PROTOCOL_FSC_NUMBER;
    
    assert(pos == PROTOCOL_DCN_VAR_SIZE);
    
    return pos;
}

uint16_t protocol_DCN_end(uint8_t buffer[PROTOCOL_FRAME_END_SIZE])
{
    buffer[0] = PROTOCOL_FRAME_END;
    
    return PROTOCOL_FRAME_END_SIZE;
}