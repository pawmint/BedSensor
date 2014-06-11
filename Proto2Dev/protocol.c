#include "protocol.h"

#define protocol_read8() protocol_readChar()

static uint16_t protocol_read16(void);
static uint32_t protocol_read32(void);

static void protocol_readFSC(uint16_t* fscValues);
static void protocol_readFSR(uint16_t* fsrValues);

char const protocol_trameId[cProtocolTrameNumber][PROTOCOL_TRAME_TYPE_SIZE + 1] = {"ACK", "YOP", "SYN",
                                                                                   "ERR", "BAT", "MOD",
                                                                                   "DR1", "DC1", "DCN",
                                                                                   "DA1", "DAN"};

static uint16_t protocol_read16(void)
{
    uint16_t value;

    value  = protocol_read8() << 8;
    value += protocol_read8();

    return endian_ntohs(value);
}

static uint32_t protocol_read32(void)
{
    uint32_t value;

    value  = ((uint32_t)protocol_read8()) << 24;
    value += ((uint32_t)protocol_read8()) << 16;
    value += ((uint32_t)protocol_read8()) << 8;
    value += ((uint32_t)protocol_read8());

    return endian_ntohl(value);
}

static void protocol_readFSC(uint16_t* fscValues)
{
    uint8_t iterFsc;

    assert(fscValues != NULL);

    for (iterFsc = 0 ; iterFsc < PROTOCOL_FSC_NUMBER ; iterFsc++)
    {
        fscValues[iterFsc] = protocol_read16();
    }
}

static void protocol_readFSR(uint16_t* fsrValues)
{
    uint8_t iterFsr;

    assert(fsrValues != NULL);

    for (iterFsr = 0 ; iterFsr < PROTOCOL_FSR_NUMBER ; iterFsr++)
    {
        fsrValues[iterFsr] = protocol_read16();
    }
}

eProtocolTrame protocol_trameIdentification(char const buffer[PROTOCOL_TRAME_TYPE_SIZE])
{
    assert(buffer != NULL);

    eProtocolTrame iterTrame = 0;
    while (iterTrame < cProtocolTrameNumber && strncmp(buffer, protocol_trameId[iterTrame], PROTOCOL_TRAME_TYPE_SIZE) != 0)
    {
        iterTrame++;
    }
    assert(iterTrame <= cProtocolTrameNumber);
    if (iterTrame == cProtocolTrameNumber)
    {
        iterTrame = cProtocolTrameUnknow;
    }

    return iterTrame;
}

bool protocol_parseSYN(uint32_t* timeData)
{
    assert(timeData != NULL);

    *timeData = protocol_read32();

    return protocol_isEndOfTrame();
}

uint16_t protocol_createSYN(const uint32_t timeData, uint8_t buffer[PROTOCOL_SYN_SIZE])
{
    /* $SYN,<TIME>\n */
    uint16_t pos;

    assert(buffer != NULL);

    pos = 0;

    buffer[pos] = PROTOCOL_TRAME_START;
    pos += PROTOCOL_TRAME_START_SIZE;

    memcpy(buffer + pos, protocol_trameId[cProtocolTrameSYN], PROTOCOL_TRAME_TYPE_SIZE);
    pos += PROTOCOL_TRAME_TYPE_SIZE;

    buffer[pos] = PROTOCOL_TRAME_SEP;
    pos += PROTOCOL_TRAME_SEP_SIZE;

    memcpy(buffer + pos, &timeData, PROTOCOL_TRAME_TIME_SIZE);
    pos += PROTOCOL_TRAME_TIME_SIZE;

    buffer[pos] = PROTOCOL_TRAME_END;
    pos += PROTOCOL_TRAME_END_SIZE;

    assert(pos == PROTOCOL_SYN_SIZE);

    return pos;

}

bool protocol_parseERR(eProtocolError* errNum)
{
    bool bOk = false;

    assert(errNum != NULL);

    *errNum = protocol_read16();
    if (*errNum < cProtocolErrorNumber)
        bOk = true;

    return bOk && protocol_isEndOfTrame();
}

uint16_t protocol_createERR(const eProtocolError errNum, uint8_t buffer[PROTOCOL_ERR_SIZE])
{
    /* $ERR,<NUM>\n */
    uint16_t pos;

    assert(errNum < cProtocolErrorNumber);
    assert(buffer != NULL);

    pos = 0;

    buffer[pos] = PROTOCOL_TRAME_START;
    pos += PROTOCOL_TRAME_START_SIZE;

    memcpy(buffer + pos, protocol_trameId[cProtocolTrameERR], PROTOCOL_TRAME_TYPE_SIZE);
    pos += PROTOCOL_TRAME_TYPE_SIZE;

    buffer[pos] = PROTOCOL_TRAME_SEP;
    pos += PROTOCOL_TRAME_SEP_SIZE;

    memcpy(buffer + pos, &errNum, PROTOCOL_TRAME_ERR_SIZE);
    pos += PROTOCOL_TRAME_TIME_SIZE;

    buffer[pos] = PROTOCOL_TRAME_END;
    pos += PROTOCOL_TRAME_END_SIZE;

    assert(pos == PROTOCOL_ERR_SIZE);

    return pos;
}

/** 
 *  @todo
 */
/*bool protocol_parseYOP(void)
{
    return true;
}*/

/** 
 *  @todo
 */
uint16_t protocol_createYOP(void)
{
    return 0;
}

/** 
 *  @todo
 */
bool protocol_parseBAT(void)
{
    return true;
}

/** 
 *  @todo
 */
uint16_t protocol_createBAT(void)
{
    return 0;
}

bool protocol_parseMOD(eProtocolMode* modeNum)
{
    bool bOk = false;

    assert(modeNum != NULL);

    *modeNum = protocol_read8();
    if (*modeNum < cProtocolModeNumber)
        bOk = true;

    return bOk && protocol_isEndOfTrame();
}

uint16_t protocol_createMOD(const eProtocolMode modeNum, uint8_t buffer[PROTOCOL_MOD_SIZE])
{
    /* $MOD,<NUM>\n */
    uint16_t pos;

    assert(modeNum < cProtocolModeNumber);
    assert(buffer != NULL);

    pos = 0;

    buffer[pos] = PROTOCOL_TRAME_START;
    pos += PROTOCOL_TRAME_START_SIZE;

    memcpy(buffer + pos, protocol_trameId[cProtocolTrameMOD], PROTOCOL_TRAME_TYPE_SIZE);
    pos += PROTOCOL_TRAME_TYPE_SIZE;

    buffer[pos] = PROTOCOL_TRAME_SEP;
    pos += PROTOCOL_TRAME_SEP_SIZE;

    memcpy(buffer + pos, &modeNum, PROTOCOL_TRAME_MOD_SIZE);
    pos += PROTOCOL_TRAME_TIME_SIZE;

    buffer[pos] = PROTOCOL_TRAME_END;
    pos += PROTOCOL_TRAME_END_SIZE;

    assert(pos == PROTOCOL_MOD_SIZE);

    return pos;

}

bool protocol_parseDR1(struct sProtocolDR1* sDr1)
{
    bool bOk = false;

    assert(sDr1 != NULL);

    sDr1->time = protocol_read32();
    if (protocol_isSeparator())
    {
        protocol_readFSR(sDr1->fsrValues);
        bOk = true;
    }

    return bOk && protocol_isEndOfTrame();
}

uint16_t protocol_createDR1(struct sProtocolDR1 const* sDr1, uint8_t buffer[PROTOCOL_DR1_SIZE])
{
    /* $DR1,<TIME>,<R0><R1>...<RN>\n */
    uint16_t pos;

    assert(sDr1 != NULL);
    assert(buffer != NULL);

    pos = 0;

    buffer[pos] = PROTOCOL_TRAME_START;
    pos += PROTOCOL_TRAME_START_SIZE;

    memcpy(buffer + pos, protocol_trameId[cProtocolTrameDR1], PROTOCOL_TRAME_TYPE_SIZE);
    pos += PROTOCOL_TRAME_TYPE_SIZE;

    buffer[pos] = PROTOCOL_TRAME_SEP;
    pos += PROTOCOL_TRAME_SEP_SIZE;

    endian_copyToB(buffer + pos, &(sDr1->time), PROTOCOL_TRAME_TIME_SIZE, PROTOCOL_TRAME_TIME_SIZE);
    pos += PROTOCOL_TRAME_TIME_SIZE;

    buffer[pos] = PROTOCOL_TRAME_SEP;
    pos += PROTOCOL_TRAME_SEP_SIZE;
    
    endian_copyToB(buffer + pos, sDr1->fsrValues, PROTOCOL_TRAME_FSR_SIZE * PROTOCOL_FSR_NUMBER, PROTOCOL_TRAME_FSR_SIZE);
    pos += PROTOCOL_TRAME_FSR_SIZE * PROTOCOL_FSR_NUMBER;

    buffer[pos] = PROTOCOL_TRAME_END;
    pos += PROTOCOL_TRAME_END_SIZE;

    assert(pos == PROTOCOL_DR1_SIZE);

    return pos;

}

bool protocol_parseDC1(struct sProtocolDC1* sDc1)
{

    bool bOk = false;

    assert(sDc1 != NULL);

    sDc1->time = protocol_read32();
    if (protocol_isSeparator())
    {
        protocol_readFSC(sDc1->fscValues);
        bOk = true;
    }

    return bOk && protocol_isEndOfTrame();
}

uint16_t protocol_createDC1(struct sProtocolDC1 const* sDc1, uint8_t buffer[PROTOCOL_DC1_SIZE])
{
    /* $DC1,<TIME>,<C0><C1>...<CN>\n */
    uint16_t pos;

    assert(sDc1 != NULL);
    assert(buffer != NULL);

    pos = 0;

    buffer[pos] = PROTOCOL_TRAME_START;
    pos += PROTOCOL_TRAME_START_SIZE;

    memcpy(buffer + pos, protocol_trameId[cProtocolTrameDC1], PROTOCOL_TRAME_TYPE_SIZE);
    pos += PROTOCOL_TRAME_TYPE_SIZE;

    buffer[pos] = PROTOCOL_TRAME_SEP;
    pos += PROTOCOL_TRAME_SEP_SIZE;

    endian_copyToB(buffer + pos, &(sDc1->time), PROTOCOL_TRAME_TIME_SIZE, PROTOCOL_TRAME_TIME_SIZE);
    pos += PROTOCOL_TRAME_TIME_SIZE;

    buffer[pos] = PROTOCOL_TRAME_SEP;
    pos += PROTOCOL_TRAME_SEP_SIZE;
    
    endian_copyToB(buffer + pos, sDc1->fscValues, PROTOCOL_TRAME_FSC_SIZE * PROTOCOL_FSC_NUMBER, PROTOCOL_TRAME_FSC_SIZE);
    pos += PROTOCOL_TRAME_FSC_SIZE * PROTOCOL_FSC_NUMBER;

    buffer[pos] = PROTOCOL_TRAME_END;
    pos += PROTOCOL_TRAME_END_SIZE;

    assert(pos == PROTOCOL_DC1_SIZE);

    return pos;

}

bool protocol_parseDCN(struct sProtocolDCN* sDcn)
{
    bool bOk = false;
    uint16_t nbSamples;
    char buf;

    assert(sDcn != NULL);

    sDcn->time = protocol_read32();
    if (protocol_isSeparator())
    {
        sDcn->delta = protocol_read32();
        if (protocol_isSeparator())
        {
            bOk = true;
            nbSamples = 0;
            do
            {
                protocol_readFSC(sDcn->fscValues[nbSamples]);
                buf = protocol_readChar();
                bOk = (buf == PROTOCOL_TRAME_SEP || buf == PROTOCOL_TRAME_END);
            } while (   bOk &&
                        buf == PROTOCOL_TRAME_SEP &&
                        ++nbSamples < PROTOCOL_DCN_SAMPLE_MAX);
            sDcn->nbSamples = nbSamples;
        }
    }

    return bOk && protocol_isEndOfTrame();
}

uint16_t protocol_createDCN(struct sProtocolDCN const* sDcn, uint8_t buffer[PROTOCOL_DATA_SIZE_MAX])
{
    /* $DCN,<TIME>,<DELTA>,<C0><C1>...<CN>[,<C0><C1>...<CN>]\n */
    uint16_t pos;
    uint16_t iterSamples;
    assert(sDcn != NULL);
    assert(buffer != NULL);
    assert(sDcn->nbSamples <= PROTOCOL_DCN_SAMPLE_MAX);

    pos = 0;

    buffer[pos] = PROTOCOL_TRAME_START;
    pos += PROTOCOL_TRAME_START_SIZE;

    memcpy(buffer + pos, protocol_trameId[cProtocolTrameDCN], PROTOCOL_TRAME_TYPE_SIZE);
    pos += PROTOCOL_TRAME_TYPE_SIZE;

    buffer[pos] = PROTOCOL_TRAME_SEP;
    pos += PROTOCOL_TRAME_SEP_SIZE;

    endian_copyToB(buffer + pos, &(sDcn->time), PROTOCOL_TRAME_TIME_SIZE, PROTOCOL_TRAME_TIME_SIZE);
    pos += PROTOCOL_TRAME_TIME_SIZE;

    buffer[pos] = PROTOCOL_TRAME_SEP;
    pos += PROTOCOL_TRAME_SEP_SIZE;

    endian_copyToB(buffer + pos, &(sDcn->delta), PROTOCOL_TRAME_TIME_SIZE, PROTOCOL_TRAME_TIME_SIZE);
    pos += PROTOCOL_TRAME_TIME_SIZE;

    for (iterSamples = 0 ; iterSamples < sDcn->nbSamples ; iterSamples++)
    {
        buffer[pos] = PROTOCOL_TRAME_SEP;
        pos += PROTOCOL_TRAME_SEP_SIZE;
        
        endian_copyToB(buffer + pos, sDcn->fscValues[iterSamples], PROTOCOL_TRAME_FSC_SIZE * PROTOCOL_FSC_NUMBER, PROTOCOL_TRAME_FSC_SIZE);
        pos += PROTOCOL_TRAME_FSC_SIZE * PROTOCOL_FSC_NUMBER;
    }

    buffer[pos] = PROTOCOL_TRAME_END;
    pos += PROTOCOL_TRAME_END_SIZE;

    assert(pos >= PROTOCOL_DCN_MIN_SIZE);
    assert(pos < PROTOCOL_DATA_SIZE_MAX);

    return pos;

}

uint16_t protocol_initDCN(struct sProtocolDC1 const* sDc1, const uint32_t delta, uint8_t buffer[PROTOCOL_DCN_MIN_SIZE - PROTOCOL_TRAME_END])
{
    /* $DCN,<TIME>,<DELTA>,<C0><C1>...<CN> */
    uint16_t pos;

    assert(sDc1 != NULL);
    assert(buffer != NULL);

    pos = 0;

    buffer[pos] = PROTOCOL_TRAME_START;
    pos += PROTOCOL_TRAME_START_SIZE;

    memcpy(buffer + pos, protocol_trameId[cProtocolTrameDCN], PROTOCOL_TRAME_TYPE_SIZE);
    pos += PROTOCOL_TRAME_TYPE_SIZE;

    buffer[pos] = PROTOCOL_TRAME_SEP;
    pos += PROTOCOL_TRAME_SEP_SIZE;

    endian_copyToB(buffer + pos, &(sDc1->time), PROTOCOL_TRAME_TIME_SIZE, PROTOCOL_TRAME_TIME_SIZE);
    pos += PROTOCOL_TRAME_TIME_SIZE;

    buffer[pos] = PROTOCOL_TRAME_SEP;
    pos += PROTOCOL_TRAME_SEP_SIZE;

    endian_copyToB(buffer + pos, &delta, PROTOCOL_TRAME_TIME_SIZE, PROTOCOL_TRAME_TIME_SIZE);
    pos += PROTOCOL_TRAME_TIME_SIZE;

    pos += protocol_extendDCN(sDc1->fscValues, buffer + pos);

    assert(pos == PROTOCOL_DCN_MIN_SIZE - PROTOCOL_TRAME_END);

    return pos;
}

uint16_t protocol_extendDCN(uint16_t const fscValues[PROTOCOL_FSC_NUMBER], uint8_t buffer[PROTOCOL_DCN_VAR_SIZE])
{
    uint16_t pos;

    assert(fscValues != NULL);
    assert(buffer != NULL);

    pos = 0;
    
    buffer[pos] = PROTOCOL_TRAME_SEP;
    pos += PROTOCOL_TRAME_SEP_SIZE;
    
    endian_copyToB(buffer + pos, fscValues, PROTOCOL_TRAME_FSC_SIZE * PROTOCOL_FSC_NUMBER, PROTOCOL_TRAME_FSC_SIZE);
    pos += PROTOCOL_TRAME_FSC_SIZE * PROTOCOL_FSC_NUMBER;

    assert(pos == PROTOCOL_DCN_VAR_SIZE);

    return pos;
}

inline uint16_t protocol_endDCN(uint8_t buffer[PROTOCOL_TRAME_END_SIZE])
{
    buffer[0] = PROTOCOL_TRAME_END;
    
    return PROTOCOL_TRAME_END_SIZE;
}

bool protocol_parseDA1(struct sProtocolDA1* sDa1)
{

    bool bOk = false;

    assert(sDa1 != NULL);

    sDa1->time = protocol_read32();
    if (protocol_isSeparator())
    {
        protocol_readFSR(sDa1->fsrValues);
        if (protocol_isSeparator())
        {
            protocol_readFSC(sDa1->fscValues);
            bOk = true;
        }
    }

    return bOk && protocol_isEndOfTrame();
}

uint16_t protocol_createDA1(struct sProtocolDA1 const* sDa1, uint8_t buffer[PROTOCOL_DA1_SIZE])
{
    /* $DA1,<TIME>,<R0><R1>...<RN>,<C0><C1>....<CN>\n */
    uint16_t pos;

    assert(sDa1 != NULL);
    assert(buffer != NULL);

    pos = 0;

    buffer[pos] = PROTOCOL_TRAME_START;
    pos += PROTOCOL_TRAME_START_SIZE;

    memcpy(buffer + pos, protocol_trameId[cProtocolTrameDA1], PROTOCOL_TRAME_TYPE_SIZE);
    pos += PROTOCOL_TRAME_TYPE_SIZE;

    buffer[pos] = PROTOCOL_TRAME_SEP;
    pos += PROTOCOL_TRAME_SEP_SIZE;

    endian_copyToB(buffer + pos, &(sDa1->time), PROTOCOL_TRAME_TIME_SIZE, PROTOCOL_TRAME_TIME_SIZE);
    pos += PROTOCOL_TRAME_TIME_SIZE;

    buffer[pos] = PROTOCOL_TRAME_SEP;
    pos += PROTOCOL_TRAME_SEP_SIZE;
    
    endian_copyToB(buffer + pos, sDa1->fsrValues, PROTOCOL_TRAME_FSR_SIZE * PROTOCOL_FSR_NUMBER, PROTOCOL_TRAME_FSR_SIZE);
    pos += PROTOCOL_TRAME_FSR_SIZE * PROTOCOL_FSR_NUMBER;

    buffer[pos] = PROTOCOL_TRAME_SEP;
    pos += PROTOCOL_TRAME_SEP_SIZE;
    
    endian_copyToB(buffer + pos, sDa1->fscValues, PROTOCOL_TRAME_FSC_SIZE * PROTOCOL_FSC_NUMBER, PROTOCOL_TRAME_FSC_SIZE);
    pos += PROTOCOL_TRAME_FSC_SIZE * PROTOCOL_FSC_NUMBER;

    buffer[pos] = PROTOCOL_TRAME_END;
    pos += PROTOCOL_TRAME_END_SIZE;

    assert(pos == PROTOCOL_DA1_SIZE);

    return pos;

}

bool protocol_parseDAN(struct sProtocolDAN* sDan)
{
    bool bOk = false;
    uint16_t nbSamples;
    char buf;

    assert(sDan != NULL);

    sDan->time = protocol_read32();
    if (protocol_isSeparator())
    {
        sDan->delta = protocol_read32();
        if (protocol_isSeparator())
        {
            bOk = true;
            nbSamples = 0;
            do
            {
                protocol_readFSR(sDan->fsrValues[nbSamples]);
                bOk = protocol_isSeparator();
                if (bOk)
                {
                    protocol_readFSC(sDan->fscValues[nbSamples]);
                    buf = protocol_readChar();
                    bOk = (buf == PROTOCOL_TRAME_SEP || buf == PROTOCOL_TRAME_END);
                }
            } while (   bOk &&
                        buf == PROTOCOL_TRAME_SEP &&
                        ++nbSamples < PROTOCOL_DAN_SAMPLE_MAX
                    );
            sDan->nbSamples = nbSamples;
        }
    }

    return bOk && protocol_isEndOfTrame();
}

uint16_t protocol_createDAN(struct sProtocolDAN const* sDan, uint8_t buffer[PROTOCOL_DATA_SIZE_MAX])
{
    /* $DCN,<TIME>,<DELTA>,<C0><C1>...<CN>[,<C0><C1>...<CN>]\n */
    uint16_t pos;
    uint16_t iterSamples;

    assert(sDan != NULL);
    assert(buffer != NULL);
    assert(sDan->nbSamples <= PROTOCOL_DAN_SAMPLE_MAX);

    pos = 0;

    buffer[pos] = PROTOCOL_TRAME_START;
    pos += PROTOCOL_TRAME_START_SIZE;

    memcpy(buffer + pos, protocol_trameId[cProtocolTrameDAN], PROTOCOL_TRAME_TYPE_SIZE);
    pos += PROTOCOL_TRAME_TYPE_SIZE;

    buffer[pos] = PROTOCOL_TRAME_SEP;
    pos += PROTOCOL_TRAME_SEP_SIZE;

    endian_copyToB(buffer + pos, &(sDan->time), PROTOCOL_TRAME_TIME_SIZE, PROTOCOL_TRAME_TIME_SIZE);
    pos += PROTOCOL_TRAME_TIME_SIZE;

    buffer[pos] = PROTOCOL_TRAME_SEP;
    pos += PROTOCOL_TRAME_SEP_SIZE;

    endian_copyToB(buffer + pos, &(sDan->delta), PROTOCOL_TRAME_TIME_SIZE, PROTOCOL_TRAME_TIME_SIZE);
    pos += PROTOCOL_TRAME_TIME_SIZE;

    for (iterSamples = 0 ; iterSamples < sDan->nbSamples ; iterSamples++)
    {
        pos += protocol_extendDAN(sDan->fsrValues[iterSamples], sDan->fscValues[iterSamples], buffer + pos);
    }

    pos += protocol_endDAN(buffer + pos);

    assert(pos >= PROTOCOL_DCN_MIN_SIZE);
    assert(pos < PROTOCOL_DATA_SIZE_MAX);

    return pos;
}

uint16_t protocol_initDAN(struct sProtocolDA1 const* sDa1, const uint32_t delta, uint8_t buffer[PROTOCOL_DAN_MIN_SIZE - PROTOCOL_TRAME_END])
{
    /* $DAN,<TIME>,<DELTA>,<C0><C1>...<CN> */
    uint16_t pos;

    assert(sDa1 != NULL);
    assert(buffer != NULL);

    pos = 0;

    buffer[pos] = PROTOCOL_TRAME_START;
    pos += PROTOCOL_TRAME_START_SIZE;

    memcpy(buffer + pos, protocol_trameId[cProtocolTrameDAN], PROTOCOL_TRAME_TYPE_SIZE);
    pos += PROTOCOL_TRAME_TYPE_SIZE;

    buffer[pos] = PROTOCOL_TRAME_SEP;
    pos += PROTOCOL_TRAME_SEP_SIZE;

    endian_copyToB(buffer + pos, &(sDa1->time), PROTOCOL_TRAME_TIME_SIZE, PROTOCOL_TRAME_TIME_SIZE);
    pos += PROTOCOL_TRAME_TIME_SIZE;

    buffer[pos] = PROTOCOL_TRAME_SEP;
    pos += PROTOCOL_TRAME_SEP_SIZE;

    endian_copyToB(buffer + pos, &delta, PROTOCOL_TRAME_TIME_SIZE, PROTOCOL_TRAME_TIME_SIZE);
    pos += PROTOCOL_TRAME_TIME_SIZE;

    pos += protocol_extendDAN(sDa1->fsrValues, sDa1->fscValues, buffer + pos);

    assert(pos == PROTOCOL_DAN_MIN_SIZE - PROTOCOL_TRAME_END);

    return pos;
}

uint16_t protocol_extendDAN(uint16_t const fsrValues[PROTOCOL_FSR_NUMBER], uint16_t const fscValues[PROTOCOL_FSC_NUMBER], uint8_t buffer[PROTOCOL_DAN_VAR_SIZE])
{
    uint16_t pos;

    assert(fsrValues != NULL);
    assert(fscValues!= NULL);
    assert(buffer != NULL);

    pos = 0;

    buffer[pos] = PROTOCOL_TRAME_SEP;
    pos += PROTOCOL_TRAME_SEP_SIZE;
        
    endian_copyToB(buffer + pos, fsrValues, PROTOCOL_TRAME_FSR_SIZE * PROTOCOL_FSR_NUMBER, PROTOCOL_TRAME_FSR_SIZE);
    pos += PROTOCOL_TRAME_FSR_SIZE * PROTOCOL_FSR_NUMBER;
    
    buffer[pos] = PROTOCOL_TRAME_SEP;
    pos += PROTOCOL_TRAME_SEP_SIZE;
    
    endian_copyToB(buffer + pos, fscValues, PROTOCOL_TRAME_FSC_SIZE * PROTOCOL_FSC_NUMBER, PROTOCOL_TRAME_FSC_SIZE);
    pos += PROTOCOL_TRAME_FSC_SIZE * PROTOCOL_FSC_NUMBER;

    assert(pos == PROTOCOL_DAN_VAR_SIZE);

    return pos;
}

inline uint16_t protocol_endDAN(uint8_t buffer[PROTOCOL_TRAME_END_SIZE])
{
    buffer[0] = PROTOCOL_TRAME_END;
    
    return PROTOCOL_TRAME_END_SIZE;
}
