#ifndef ARDUINO_MAIN
#include "protocol_ACK.h"
#include "protocol_api_private.h"

static tProtocol_ACK_callBackFunc callBackFunc;
static tProtocol_AskCallBackFunc askCallBackFunc;

PROTOCOL_GENERATE_FUNCVAR_SETTER(ACK)

PROTOCOL_GENERATE_PARSE_AND_CALL_VOID(ACK)

bool protocol_ACK_parse(tProtocol_ACK_data* sData)
{
    bool bOk = false;
    if (getCursorSize() >= PROTOCOL_ACK_SIZE - PROTOCOL_HEADER_SIZE)
    {
        if (isEndOfFrame())
        {
            nextCursor();
            bOk = true;
        }
    }
    
    return bOk;
}

uint16_t protocol_ACK_create(tProtocol_ACK_buffer buffer, tProtocol_ACK_data* sData)
{
    uint16_t pos;
    
    assert(buffer != NULL);
    
    pos = 0;
    
    addStart(buffer, &pos);
    
    addFrameId(buffer, &pos, cProtocolFrameACK);
    
    addEnd(buffer, &pos);
    
    assert(pos == PROTOCOL_ACK_SIZE);
    
    return pos;
    
}
#endif