#include "protocol_YOP.h"
#include "protocol_api_private.h"

static tProtocol_YOP_callBackFunc callBackFunc;
static tProtocol_AskCallBackFunc askCallBackFunc;

PROTOCOL_GENERATE_FUNCVAR_SETTER(YOP)

PROTOCOL_GENERATE_PARSE_AND_CALL_VOID(YOP)

bool protocol_YOP_parse(tProtocol_YOP_data* sData)
{
    bool bOk = false;
    if (getCursorSize() >= PROTOCOL_YOP_SIZE - PROTOCOL_HEADER_SIZE)
    {
        if (isEndOfFrame())
        {
            nextCursor();
            bOk = true;
        }
    }
    
    return bOk;
}

uint16_t protocol_YOP_create(tProtocol_YOP_buffer buffer, tProtocol_YOP_data* sData)
{
    uint16_t pos;
    
    assert(buffer != NULL);
    
    pos = 0;
    
    addStart(buffer, &pos);
    
    addFrameId(buffer, &pos, cProtocolFrameYOP);
    
    addEnd(buffer, &pos);
    
    assert(pos == PROTOCOL_YOP_SIZE);
    
    return pos;
    
}