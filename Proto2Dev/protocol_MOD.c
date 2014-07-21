#include "protocol_MOD.h"
#include "protocol_api_private.h"

static tProtocol_MOD_callBackFunc callBackFunc;
static tProtocol_AskCallBackFunc askCallBackFunc;

PROTOCOL_GENERATE_FUNCVAR_SETTER(MOD)

PROTOCOL_GENERATE_PARSE_AND_CALL(MOD)

bool protocol_MOD_parse(tProtocol_MOD_data* sData)
{
    bool bOk = false;
    
    assert(sData != NULL);
    
    if (getCursorSize() >= PROTOCOL_MOD_SIZE - PROTOCOL_HEADER_SIZE)
    {
        if (isSeparator())
        {
            nextCursor();
            *sData = peek8();
            
            if (*sData < cProtocolModeNumber)
            {
                moveCursor(PROTOCOL_FRAME_MOD_SIZE);
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

uint16_t protocol_MOD_create(tProtocol_MOD_buffer buffer, tProtocol_MOD_data const *  sData)
{
    /* $MOD,<NUM>\n */
    uint16_t pos;
    
    assert(buffer != NULL);
    assert(sData != NULL);
    assert(*sData < cProtocolModeNumber);
    
    pos = 0;
    
    addStart(buffer, &pos);
    
    addFrameId(buffer, &pos, cProtocolFrameMOD);
    
    addSep(buffer, &pos);
    
    memcpy(buffer + pos, sData, PROTOCOL_FRAME_MOD_SIZE);
    pos += PROTOCOL_FRAME_MOD_SIZE;
    
    addEnd(buffer, &pos);
    
    assert(pos == PROTOCOL_MOD_SIZE);
    
    return pos;
    
}