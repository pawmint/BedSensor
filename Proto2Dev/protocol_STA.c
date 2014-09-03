#include "protocol_STA.h"
#include "protocol_api_private.h"


static tProtocol_STA_callBackFunc callBackFunc;
static tProtocol_AskCallBackFunc askCallBackFunc;

//PROTOCOL_GENERATE_FUNCVAR_SETTER(STA)

inline void protocol_STA_setAskCallBackFunc(void (*test)())
{

}

PROTOCOL_GENERATE_PARSE_AND_CALL(STA)

bool protocol_STA_parse(tProtocol_STA_data* sData)
{
    bool bOk = false;
    uint16_t num;
    
    assert(sData != NULL);
    
    if (getCursorSize() >= PROTOCOL_STA_SIZE - PROTOCOL_HEADER_SIZE)
    {
        if (isSeparator())
        {
            nextCursor();
            sData->syncTime = read32();
            
            if (isSeparator())
            {
                nextCursor();
                if (read999(&num) && num <= 100)
                {
                    sData->batLvl = (uint8_t)num;
                    if (isSeparator())
                    {
                        nextCursor();
                        if (read99(&(sData->nbFsr)))
                        {
                            if (isSeparator())
                            {
                                nextCursor();
                                bOk = read99(&(sData->nbFsc));
                                
                            }
                        }
                    }
                }
            }
        }
    }
    
    return bOk;
}

uint16_t protocol_STA_create(tProtocol_STA_buffer buffer, tProtocol_STA_data const * sData)
{
    /* $STA,<TIME>,<R0><R1>...<RN>\n */
    uint16_t pos;
    
    assert(sData != NULL);
    assert(buffer != NULL);
    
    pos = 0;
    
    addStart(buffer, &pos);
    
    addFrameId(buffer, &pos, cProtocolFrameSTA);
    
    addSep(buffer, &pos);
    
    endian_copyToB(buffer + pos, &(sData->syncTime), PROTOCOL_FRAME_TIME_SIZE, PROTOCOL_FRAME_TIME_SIZE);
    pos += PROTOCOL_FRAME_TIME_SIZE;
    
    addSep(buffer, &pos);
    
    buffer[pos] = sData->batLvl / 100 + '0';
    buffer[pos + 1] = (sData->batLvl / 10) % 10 + '0';
    buffer[pos + 2] = sData->batLvl % 10 + '0';
    pos += PROTOCOL_FRAME_BAT_SIZE;
    
    addSep(buffer, &pos);
    
    buffer[pos] = sData->nbFsr / 10 + '0';
    buffer[pos + 1] = sData->nbFsr % 10 + '0';
    pos += PROTOCOL_FRAME_FSR_NUMBER_SIZE;
    
    addSep(buffer, &pos);
    
    buffer[pos] = sData->nbFsc / 10 + '0';
    buffer[pos + 1] = sData->nbFsc % 10 + '0';
    pos += PROTOCOL_FRAME_FSC_NUMBER_SIZE;
    
    addEnd(buffer, &pos);
    
    assert(pos == PROTOCOL_STA_SIZE);
    
    return pos;
}