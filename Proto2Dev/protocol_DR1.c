
#include "protocol_DR1.h"
#include "protocol_api_private.h"

static tProtocol_DR1_callBackFunc callBackFunc;
static tProtocol_AskCallBackFunc askCallBackFunc;

PROTOCOL_GENERATE_FUNCVAR_SETTER(DR1)

PROTOCOL_GENERATE_PARSE_AND_CALL(DR1)

bool protocol_DR1_parse(tProtocol_DR1_data* sData)
{
    bool bOk;
    
    assert(sData != NULL);
    
    if (getCursorSize() >= PROTOCOL_DR1_SIZE - PROTOCOL_HEADER_SIZE)
    {
        if (isSeparator())
        {
            nextCursor();
            sData->time = read32();
            
            if (isSeparator())
            {
                nextCursor();
                readFSR(sData->fsrValues);
                
                if (isEndOfFrame())
                {
                    nextCursor();
                    bOk = true;
                }
            }
        }
    }
    
    return bOk;
}

uint16_t protocol_DR1_create(tProtocol_DR1_buffer buffer, tProtocol_DR1_data const *  sData)
{
    /* $DR1,<TIME>,<R0><R1>...<RN>\n */
    uint16_t pos;
    
    assert(sData != NULL);
    assert(buffer != NULL);
    
    pos = 0;
    
    addStart(buffer, &pos);
    
    addFrameId(buffer, &pos, cProtocolFrameDR1);
    
    addSep(buffer, &pos);
    
    endian_copyToB(buffer + pos, &(sData->time), PROTOCOL_FRAME_TIME_SIZE, PROTOCOL_FRAME_TIME_SIZE);
    pos += PROTOCOL_FRAME_TIME_SIZE;
    
    addSep(buffer, &pos);
    
    endian_copyToB(buffer + pos, sData->fsrValues, PROTOCOL_FRAME_FSR_SIZE *  PROTOCOL_FSR_NUMBER, PROTOCOL_FRAME_FSR_SIZE);
    pos += PROTOCOL_FRAME_FSR_SIZE *  PROTOCOL_FSR_NUMBER;
    
    addEnd(buffer, &pos);
    
    assert(pos == PROTOCOL_DR1_SIZE);
    
    return pos;
}