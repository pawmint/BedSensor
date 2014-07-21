#include "protocol_ERR.h"
#include "protocol_api_private.h"

static tProtocol_ERR_callBackFunc callBackFunc;
static tProtocol_AskCallBackFunc askCallBackFunc;

PROTOCOL_GENERATE_FUNCVAR_SETTER(ERR)

PROTOCOL_GENERATE_PARSE_AND_CALL_VOID(ERR)

bool protocol_ERR_parse(tProtocol_ERR_data* sData)
{
    bool bOk = false;
    if (getCursorSize() >= PROTOCOL_ERR_SIZE - PROTOCOL_HEADER_SIZE)
    {
        if (isEndOfFrame())
        {
            nextCursor();
            bOk = true;
        }
    }
    
    return bOk;
}

uint16_t protocol_ERR_create(tProtocol_ERR_buffer buffer, tProtocol_ERR_data* sData)
{
    uint16_t pos;
    
    assert(buffer != NULL);
    
    pos = 0;
    
    addStart(buffer, &pos);
    
    addFrameId(buffer, &pos, cProtocolFrameERR);
    
    addEnd(buffer, &pos);
    
    assert(pos == PROTOCOL_ERR_SIZE);
    
    return pos;
    
}