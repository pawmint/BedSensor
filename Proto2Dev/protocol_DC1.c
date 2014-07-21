#include "protocol_DC1.h"
#include "protocol_api_private.h"

static tProtocol_DC1_callBackFunc callBackFunc;
static tProtocol_AskCallBackFunc askCallBackFunc;

PROTOCOL_GENERATE_FUNCVAR_SETTER(DC1)

PROTOCOL_GENERATE_PARSE_AND_CALL(DC1)

bool protocol_DC1_parse(tProtocol_DC1_data* sData)
{
    bool bOk;
    
    assert(sData != NULL);
    
    if (getCursorSize() >= PROTOCOL_DC1_SIZE - PROTOCOL_HEADER_SIZE)
    {
        if (isSeparator())
        {
            nextCursor();
            sData->time = read32();
            
            if (isSeparator())
            {
                nextCursor();
                readFSC(sData->fscValues);
                
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

uint16_t protocol_DC1_create(tProtocol_DC1_buffer buffer, tProtocol_DC1_data const *  sData)
{
    /* $DC1,<TIME>,<R0><R1>...<RN>\n */
    uint16_t pos;
    
    assert(sData != NULL);
    assert(buffer != NULL);
    
    pos = 0;
    
    addStart(buffer, &pos);
    
    addFrameId(buffer, &pos, cProtocolFrameDC1);
    
    addSep(buffer, &pos);
    
    endian_copyToB(buffer + pos, &(sData->time), PROTOCOL_FRAME_TIME_SIZE, PROTOCOL_FRAME_TIME_SIZE);
    pos += PROTOCOL_FRAME_TIME_SIZE;
    
    addSep(buffer, &pos);
    
    endian_copyToB(buffer + pos, sData->fscValues, PROTOCOL_FRAME_FSC_SIZE *  PROTOCOL_FSC_NUMBER, PROTOCOL_FRAME_FSC_SIZE);
    pos += PROTOCOL_FRAME_FSC_SIZE *  PROTOCOL_FSC_NUMBER;
    
    addEnd(buffer, &pos);
    
    assert(pos == PROTOCOL_DC1_SIZE);
    
    return pos;
}