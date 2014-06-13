/**
 *  Bed sensor's communication protocol.
 *    
 *  Provide functions and structures in order 
 *  to creates and manages communication's 
 *  frames between sensor and platform.
 *	
 *  @file protocol.c
 *  @date 13 juin 2014
 *  @copyright PAWM International
 *	
 *  @author Mickael Germain
 *	
 */

#include "protocol.h"

/**
 *	Read an integer of 1 byte from set stream.
 *  
 *  @internal
 *  
 *  @return The integer read.
 *
 *  @see protocol_read16()
 *  @see protocol_read32()
 */
#define protocol_read8() protocol_readChar()

/**
 *	Read an integer of 2 bytes from set stream.
 *  
 *  @internal
 *  
 *  @note The integer should be in big endian format and 
 *        will be convert to the host format.
 *  
 *  @return The integer read.
 *  
 *  @see protocol_read8()
 *  @see protocol_read32()
 */
static uint16_t protocol_read16(void);

/**
 *	Read an integer of 4 bytes from set stream.
 *  
 *  @internal
 *
 *  @note The integer should be in big endian format and
 *        will be convert to the host format.
 *  
 *  @return The integer read.
 *  
 *  @see protocol_read8()
 *  @see protocol_read16()
 */
static uint32_t protocol_read32(void);

/**
 *	Read FSC's values from set stream.
 *  
 *  @internal
 *  
 *  @param [out] fscValues
 *      Reference where store values.
 */
static void protocol_readFSC(uint16_t* fscValues);

/**
 *	Read FSR's values from set stream.
 *  
 *  @internal
 *  
 *  @param [out] fsrValues
 *      Reference where store values.
 */
static void protocol_readFSR(uint16_t* fsrValues);

char const protocol_FrameId[cProtocolFrameNumber][PROTOCOL_FRAME_TYPE_SIZE + 1] = {"ACK", 
                                                                                   "YOP", 
                                                                                   "SYN",
                                                                                   "ERR", 
                                                                                   "BAT", 
                                                                                   "MOD",
                                                                                   "DR1", 
                                                                                   "DC1", 
                                                                                   "DCN",
                                                                                   "DA1", 
                                                                                   "DAN"
                                                                                   /* ^-insert new frames id at the end-^ */
                                                                                   /* According to eProtocolFrame order.  */
                                                                                   };

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
        fscValues[iterFsc] = protocol_read16();
}

static void protocol_readFSR(uint16_t* fsrValues)
{
    uint8_t iterFsr;

    assert(fsrValues != NULL);

    for (iterFsr = 0 ; iterFsr < PROTOCOL_FSR_NUMBER ; iterFsr++)
        fsrValues[iterFsr] = protocol_read16();
}

eProtocolFrame protocol_FrameIdentification(char const buffer[PROTOCOL_FRAME_TYPE_SIZE])
{
    assert(buffer != NULL);

    eProtocolFrame iterFrame = 0;
    while (iterFrame < cProtocolFrameNumber && strncmp(buffer, protocol_FrameId[iterFrame], PROTOCOL_FRAME_TYPE_SIZE) != 0)
        iterFrame++;
        
    assert(iterFrame <= cProtocolFrameNumber);
    
    if (iterFrame == cProtocolFrameNumber)
        iterFrame = cProtocolFrameUnknow;

    return iterFrame;
}

bool protocol_parseYOP(uint8_t* fsrNumber, uint8_t* fscNumber)
{
	bool bOk;
	char buf[3];
	
	assert(fsrNumber != NULL);
	assert(fscNumber != NULL);
	
	buf[2] = '\0';
	
	if (bOk = protocol_isSeparator())
	{
		/* [00;99] */
		buf[0] = protocol_readChar();
		buf[1] = protocol_readChar();
		
		*fsrNumber = atoi(buf);
		bOk = (*fsrNumber != 0);
	}
	if (bOk = (bOk && protocol_isSeparator()))
	{
        /* [00;99] */
		buf[0] = protocol_readChar();
		buf[1] = protocol_readChar();
				
		*fscNumber = atoi(buf);
		bOk = (*fscNumber != 0);
	}
	
	return bOk && protocol_isEndOfFrame();
}

uint16_t protocol_createYOP(tProtocol_bufferYOP buffer)
{
	/* $YOP,<FSR>,<FSC>\n */
	uint16_t pos;
	
	assert(buffer != 0);
	assert(PROTOCOL_FSR_NUMBER <= 99);
	assert(PROTOCOL_FSR_NUMBER <= 99);
	
	pos = 0;
	
	buffer[pos] = PROTOCOL_FRAME_START;
	pos += PROTOCOL_FRAME_START_SIZE;

	memcpy(buffer + pos, protocol_FrameId[cProtocolFrameYOP], PROTOCOL_FRAME_TYPE_SIZE);
	pos += PROTOCOL_FRAME_TYPE_SIZE;

	buffer[pos] = PROTOCOL_FRAME_SEP;
	pos += PROTOCOL_FRAME_SEP_SIZE;

	buffer[pos++] = PROTOCOL_FSR_NUMBER / 10 + '0';
	buffer[pos++] = PROTOCOL_FSR_NUMBER % 10 + '0';
	
	buffer[pos] = PROTOCOL_FRAME_SEP;
	pos += PROTOCOL_FRAME_SEP_SIZE;
	
	buffer[pos++] = PROTOCOL_FSC_NUMBER / 10 + '0';
	buffer[pos++] = PROTOCOL_FSC_NUMBER % 10 + '0';
	
	buffer[pos] = PROTOCOL_FRAME_END;
	pos += PROTOCOL_FRAME_END_SIZE;

	assert(pos == PROTOCOL_YOP_SIZE);

	return pos;
	
}

uint16_t protocol_createACK(tProtocol_bufferACK buffer)
{
	uint16_t pos;

	assert(buffer != NULL);

	pos = 0;

	buffer[pos] = PROTOCOL_FRAME_START;
	pos += PROTOCOL_FRAME_START_SIZE;

	memcpy(buffer + pos, protocol_FrameId[cProtocolFrameACK], PROTOCOL_FRAME_TYPE_SIZE);
	pos += PROTOCOL_FRAME_TYPE_SIZE;

	buffer[pos] = PROTOCOL_FRAME_END;
	pos += PROTOCOL_FRAME_END_SIZE;

	assert(pos == PROTOCOL_ACK_SIZE);

	return pos;

}

bool protocol_parseSYN(uint32_t* timeData)
{
    assert(timeData != NULL);

    *timeData = protocol_read32();

    return protocol_isEndOfFrame();
}

uint16_t protocol_createSYN(const uint32_t timeData, tProtocol_bufferSYN buffer)
{
    /* $SYN,<TIME>\n */
    uint16_t pos;

    assert(buffer != NULL);

    pos = 0;

    buffer[pos] = PROTOCOL_FRAME_START;
    pos += PROTOCOL_FRAME_START_SIZE;

    memcpy(buffer + pos, protocol_FrameId[cProtocolFrameSYN], PROTOCOL_FRAME_TYPE_SIZE);
    pos += PROTOCOL_FRAME_TYPE_SIZE;

    buffer[pos] = PROTOCOL_FRAME_SEP;
    pos += PROTOCOL_FRAME_SEP_SIZE;

    memcpy(buffer + pos, &timeData, PROTOCOL_FRAME_TIME_SIZE);
    pos += PROTOCOL_FRAME_TIME_SIZE;

    buffer[pos] = PROTOCOL_FRAME_END;
    pos += PROTOCOL_FRAME_END_SIZE;

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

    return bOk && protocol_isEndOfFrame();
}

uint16_t protocol_createERR(const eProtocolError errNum, tProtocol_bufferERR buffer)
{
    /* $ERR,<NUM>\n */
    uint16_t pos;

    assert(errNum < cProtocolErrorNumber);
    assert(buffer != NULL);

    pos = 0;

    buffer[pos] = PROTOCOL_FRAME_START;
    pos += PROTOCOL_FRAME_START_SIZE;

    memcpy(buffer + pos, protocol_FrameId[cProtocolFrameERR], PROTOCOL_FRAME_TYPE_SIZE);
    pos += PROTOCOL_FRAME_TYPE_SIZE;

    buffer[pos] = PROTOCOL_FRAME_SEP;
    pos += PROTOCOL_FRAME_SEP_SIZE;

    memcpy(buffer + pos, &errNum, PROTOCOL_FRAME_ERR_SIZE);
    pos += PROTOCOL_FRAME_TIME_SIZE;

    buffer[pos] = PROTOCOL_FRAME_END;
    pos += PROTOCOL_FRAME_END_SIZE;

    assert(pos == PROTOCOL_ERR_SIZE);

    return pos;
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

    return bOk && protocol_isEndOfFrame();
}

uint16_t protocol_createMOD(const eProtocolMode modeNum, tProtocol_bufferMOD buffer)
{
    /* $MOD,<NUM>\n */
    uint16_t pos;

    assert(modeNum < cProtocolModeNumber);
    assert(buffer != NULL);

    pos = 0;

    buffer[pos] = PROTOCOL_FRAME_START;
    pos += PROTOCOL_FRAME_START_SIZE;

    memcpy(buffer + pos, protocol_FrameId[cProtocolFrameMOD], PROTOCOL_FRAME_TYPE_SIZE);
    pos += PROTOCOL_FRAME_TYPE_SIZE;

    buffer[pos] = PROTOCOL_FRAME_SEP;
    pos += PROTOCOL_FRAME_SEP_SIZE;

    memcpy(buffer + pos, &modeNum, PROTOCOL_FRAME_MOD_SIZE);
    pos += PROTOCOL_FRAME_TIME_SIZE;

    buffer[pos] = PROTOCOL_FRAME_END;
    pos += PROTOCOL_FRAME_END_SIZE;

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

    return bOk && protocol_isEndOfFrame();
}

uint16_t protocol_createDR1(struct sProtocolDR1 const* sDr1, tProtocol_bufferDR1 buffer)
{
    /* $DR1,<TIME>,<R0><R1>...<RN>\n */
    uint16_t pos;

    assert(sDr1 != NULL);
    assert(buffer != NULL);

    pos = 0;

    buffer[pos] = PROTOCOL_FRAME_START;
    pos += PROTOCOL_FRAME_START_SIZE;

    memcpy(buffer + pos, protocol_FrameId[cProtocolFrameDR1], PROTOCOL_FRAME_TYPE_SIZE);
    pos += PROTOCOL_FRAME_TYPE_SIZE;

    buffer[pos] = PROTOCOL_FRAME_SEP;
    pos += PROTOCOL_FRAME_SEP_SIZE;

    endian_copyToB(buffer + pos, &(sDr1->time), PROTOCOL_FRAME_TIME_SIZE, PROTOCOL_FRAME_TIME_SIZE);
    pos += PROTOCOL_FRAME_TIME_SIZE;

    buffer[pos] = PROTOCOL_FRAME_SEP;
    pos += PROTOCOL_FRAME_SEP_SIZE;
    
    endian_copyToB(buffer + pos, sDr1->fsrValues, PROTOCOL_FRAME_FSR_SIZE * PROTOCOL_FSR_NUMBER, PROTOCOL_FRAME_FSR_SIZE);
    pos += PROTOCOL_FRAME_FSR_SIZE * PROTOCOL_FSR_NUMBER;

    buffer[pos] = PROTOCOL_FRAME_END;
    pos += PROTOCOL_FRAME_END_SIZE;

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

    return bOk && protocol_isEndOfFrame();
}

uint16_t protocol_createDC1(struct sProtocolDC1 const* sDc1, tProtocol_bufferDC1 buffer)
{
    /* $DC1,<TIME>,<C0><C1>...<CN>\n */
    uint16_t pos;

    assert(sDc1 != NULL);
    assert(buffer != NULL);

    pos = 0;

    buffer[pos] = PROTOCOL_FRAME_START;
    pos += PROTOCOL_FRAME_START_SIZE;

    memcpy(buffer + pos, protocol_FrameId[cProtocolFrameDC1], PROTOCOL_FRAME_TYPE_SIZE);
    pos += PROTOCOL_FRAME_TYPE_SIZE;

    buffer[pos] = PROTOCOL_FRAME_SEP;
    pos += PROTOCOL_FRAME_SEP_SIZE;

    endian_copyToB(buffer + pos, &(sDc1->time), PROTOCOL_FRAME_TIME_SIZE, PROTOCOL_FRAME_TIME_SIZE);
    pos += PROTOCOL_FRAME_TIME_SIZE;

    buffer[pos] = PROTOCOL_FRAME_SEP;
    pos += PROTOCOL_FRAME_SEP_SIZE;
    
    endian_copyToB(buffer + pos, sDc1->fscValues, PROTOCOL_FRAME_FSC_SIZE * PROTOCOL_FSC_NUMBER, PROTOCOL_FRAME_FSC_SIZE);
    pos += PROTOCOL_FRAME_FSC_SIZE * PROTOCOL_FSC_NUMBER;

    buffer[pos] = PROTOCOL_FRAME_END;
    pos += PROTOCOL_FRAME_END_SIZE;

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
                /* At least one wave of sampling. */
                protocol_readFSC(sDcn->fscValues[nbSamples]);
                buf = protocol_readChar();
                bOk = (buf == PROTOCOL_FRAME_SEP || buf == PROTOCOL_FRAME_END);
            /* if there is a separator then parse an other wave. */
            } while (   bOk &&
                        buf == PROTOCOL_FRAME_SEP &&
                        ++nbSamples <= PROTOCOL_DCN_SAMPLE_MAX);
            /* Too much waves ? */
            if (nbSamples > PROTOCOL_DCN_SAMPLE_MAX)
                bOk = false;
            else
                sDcn->nbSamples = nbSamples;
        }
    }

    return bOk && protocol_isEndOfFrame();
}

uint16_t protocol_createDCN(struct sProtocolDCN const* sDcn, tProtocol_bufferDCN buffer)
{
    /* $DCN,<TIME>,<DELTA>,<C0><C1>...<CN>[,<C0><C1>...<CN>]\n */
    uint16_t pos;
    uint16_t iterSamples;
    
    assert(sDcn != NULL);
    assert(buffer != NULL);
    assert(sDcn->nbSamples <= PROTOCOL_DCN_SAMPLE_MAX);

    pos = 0;

    buffer[pos] = PROTOCOL_FRAME_START;
    pos += PROTOCOL_FRAME_START_SIZE;

    memcpy(buffer + pos, protocol_FrameId[cProtocolFrameDCN], PROTOCOL_FRAME_TYPE_SIZE);
    pos += PROTOCOL_FRAME_TYPE_SIZE;

    buffer[pos] = PROTOCOL_FRAME_SEP;
    pos += PROTOCOL_FRAME_SEP_SIZE;

    endian_copyToB(buffer + pos, &(sDcn->time), PROTOCOL_FRAME_TIME_SIZE, PROTOCOL_FRAME_TIME_SIZE);
    pos += PROTOCOL_FRAME_TIME_SIZE;

    buffer[pos] = PROTOCOL_FRAME_SEP;
    pos += PROTOCOL_FRAME_SEP_SIZE;

    endian_copyToB(buffer + pos, &(sDcn->delta), PROTOCOL_FRAME_TIME_SIZE, PROTOCOL_FRAME_TIME_SIZE);
    pos += PROTOCOL_FRAME_TIME_SIZE;

    for (iterSamples = 0 ; iterSamples < sDcn->nbSamples ; iterSamples++)
    {
        buffer[pos] = PROTOCOL_FRAME_SEP;
        pos += PROTOCOL_FRAME_SEP_SIZE;
        
        endian_copyToB(buffer + pos, sDcn->fscValues[iterSamples], PROTOCOL_FRAME_FSC_SIZE * PROTOCOL_FSC_NUMBER, PROTOCOL_FRAME_FSC_SIZE);
        pos += PROTOCOL_FRAME_FSC_SIZE * PROTOCOL_FSC_NUMBER;
    }

    buffer[pos] = PROTOCOL_FRAME_END;
    pos += PROTOCOL_FRAME_END_SIZE;

    assert(pos >= PROTOCOL_DCN_MIN_SIZE);
    assert(pos < PROTOCOL_DATA_SIZE_MAX);

    return pos;

}

uint16_t protocol_initDCN(struct sProtocolDC1 const* sDc1, const uint32_t delta, uint8_t buffer[PROTOCOL_DCN_MIN_SIZE - PROTOCOL_FRAME_END])
{
    /* $DCN,<TIME>,<DELTA>,<C0><C1>...<CN> */
    uint16_t pos;

    assert(sDc1 != NULL);
    assert(buffer != NULL);

    pos = 0;

    buffer[pos] = PROTOCOL_FRAME_START;
    pos += PROTOCOL_FRAME_START_SIZE;

    memcpy(buffer + pos, protocol_FrameId[cProtocolFrameDCN], PROTOCOL_FRAME_TYPE_SIZE);
    pos += PROTOCOL_FRAME_TYPE_SIZE;

    buffer[pos] = PROTOCOL_FRAME_SEP;
    pos += PROTOCOL_FRAME_SEP_SIZE;

    endian_copyToB(buffer + pos, &(sDc1->time), PROTOCOL_FRAME_TIME_SIZE, PROTOCOL_FRAME_TIME_SIZE);
    pos += PROTOCOL_FRAME_TIME_SIZE;

    buffer[pos] = PROTOCOL_FRAME_SEP;
    pos += PROTOCOL_FRAME_SEP_SIZE;

    endian_copyToB(buffer + pos, &delta, PROTOCOL_FRAME_TIME_SIZE, PROTOCOL_FRAME_TIME_SIZE);
    pos += PROTOCOL_FRAME_TIME_SIZE;

    pos += protocol_extendDCN(sDc1->fscValues, buffer + pos);

    assert(pos == PROTOCOL_DCN_MIN_SIZE - PROTOCOL_FRAME_END);

    return pos;
}

uint16_t protocol_extendDCN(uint16_t const fscValues[PROTOCOL_FSC_NUMBER], uint8_t buffer[PROTOCOL_DCN_VAR_SIZE])
{
    uint16_t pos;

    assert(fscValues != NULL);
    assert(buffer != NULL);

    pos = 0;
    
    buffer[pos] = PROTOCOL_FRAME_SEP;
    pos += PROTOCOL_FRAME_SEP_SIZE;
    
    endian_copyToB(buffer + pos, fscValues, PROTOCOL_FRAME_FSC_SIZE * PROTOCOL_FSC_NUMBER, PROTOCOL_FRAME_FSC_SIZE);
    pos += PROTOCOL_FRAME_FSC_SIZE * PROTOCOL_FSC_NUMBER;

    assert(pos == PROTOCOL_DCN_VAR_SIZE);

    return pos;
}

inline uint16_t protocol_endDCN(uint8_t buffer[PROTOCOL_FRAME_END_SIZE])
{
    buffer[0] = PROTOCOL_FRAME_END;
    
    return PROTOCOL_FRAME_END_SIZE;
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

    return bOk && protocol_isEndOfFrame();
}

uint16_t protocol_createDA1(struct sProtocolDA1 const* sDa1, tProtocol_bufferDA1 buffer)
{
    /* $DA1,<TIME>,<R0><R1>...<RN>,<C0><C1>....<CN>\n */
    uint16_t pos;

    assert(sDa1 != NULL);
    assert(buffer != NULL);

    pos = 0;

    buffer[pos] = PROTOCOL_FRAME_START;
    pos += PROTOCOL_FRAME_START_SIZE;

    memcpy(buffer + pos, protocol_FrameId[cProtocolFrameDA1], PROTOCOL_FRAME_TYPE_SIZE);
    pos += PROTOCOL_FRAME_TYPE_SIZE;

    buffer[pos] = PROTOCOL_FRAME_SEP;
    pos += PROTOCOL_FRAME_SEP_SIZE;

    endian_copyToB(buffer + pos, &(sDa1->time), PROTOCOL_FRAME_TIME_SIZE, PROTOCOL_FRAME_TIME_SIZE);
    pos += PROTOCOL_FRAME_TIME_SIZE;

    buffer[pos] = PROTOCOL_FRAME_SEP;
    pos += PROTOCOL_FRAME_SEP_SIZE;
    
    endian_copyToB(buffer + pos, sDa1->fsrValues, PROTOCOL_FRAME_FSR_SIZE * PROTOCOL_FSR_NUMBER, PROTOCOL_FRAME_FSR_SIZE);
    pos += PROTOCOL_FRAME_FSR_SIZE * PROTOCOL_FSR_NUMBER;

    buffer[pos] = PROTOCOL_FRAME_SEP;
    pos += PROTOCOL_FRAME_SEP_SIZE;
    
    endian_copyToB(buffer + pos, sDa1->fscValues, PROTOCOL_FRAME_FSC_SIZE * PROTOCOL_FSC_NUMBER, PROTOCOL_FRAME_FSC_SIZE);
    pos += PROTOCOL_FRAME_FSC_SIZE * PROTOCOL_FSC_NUMBER;

    buffer[pos] = PROTOCOL_FRAME_END;
    pos += PROTOCOL_FRAME_END_SIZE;

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
                /* At least one wave of sampling. */
                protocol_readFSR(sDan->fsrValues[nbSamples]);
                bOk = protocol_isSeparator();
                if (bOk)
                {
                    protocol_readFSC(sDan->fscValues[nbSamples]);
                    buf = protocol_readChar();
                    bOk = (buf == PROTOCOL_FRAME_SEP || buf == PROTOCOL_FRAME_END);
                }
            /* if there is a separator then parse an other wave. */
            } while (   bOk &&
                        buf == PROTOCOL_FRAME_SEP &&
                        ++nbSamples < PROTOCOL_DAN_SAMPLE_MAX
                    );
            /* Too much waves ? */
            if (nbSamples > PROTOCOL_DCN_SAMPLE_MAX)
                bOk = false;
            else
                sDan->nbSamples = nbSamples;
        }
    }

    return bOk && protocol_isEndOfFrame();
}

uint16_t protocol_createDAN(struct sProtocolDAN const* sDan, tProtocol_bufferDAN buffer)
{
    /* $DCN,<TIME>,<DELTA>,<C0><C1>...<CN>[,<C0><C1>...<CN>]\n */
    uint16_t pos;
    uint16_t iterSamples;

    assert(sDan != NULL);
    assert(buffer != NULL);
    assert(sDan->nbSamples <= PROTOCOL_DAN_SAMPLE_MAX);

    pos = 0;

    buffer[pos] = PROTOCOL_FRAME_START;
    pos += PROTOCOL_FRAME_START_SIZE;

    memcpy(buffer + pos, protocol_FrameId[cProtocolFrameDAN], PROTOCOL_FRAME_TYPE_SIZE);
    pos += PROTOCOL_FRAME_TYPE_SIZE;

    buffer[pos] = PROTOCOL_FRAME_SEP;
    pos += PROTOCOL_FRAME_SEP_SIZE;

    endian_copyToB(buffer + pos, &(sDan->time), PROTOCOL_FRAME_TIME_SIZE, PROTOCOL_FRAME_TIME_SIZE);
    pos += PROTOCOL_FRAME_TIME_SIZE;

    buffer[pos] = PROTOCOL_FRAME_SEP;
    pos += PROTOCOL_FRAME_SEP_SIZE;

    endian_copyToB(buffer + pos, &(sDan->delta), PROTOCOL_FRAME_TIME_SIZE, PROTOCOL_FRAME_TIME_SIZE);
    pos += PROTOCOL_FRAME_TIME_SIZE;

    for (iterSamples = 0 ; iterSamples < sDan->nbSamples ; iterSamples++)
    {
        pos += protocol_extendDAN(sDan->fsrValues[iterSamples], sDan->fscValues[iterSamples], buffer + pos);
    }

    pos += protocol_endDAN(buffer + pos);

    assert(pos >= PROTOCOL_DCN_MIN_SIZE);
    assert(pos < PROTOCOL_DATA_SIZE_MAX);

    return pos;
}

uint16_t protocol_initDAN(struct sProtocolDA1 const* sDa1, const uint32_t delta, uint8_t buffer[PROTOCOL_DAN_MIN_SIZE - PROTOCOL_FRAME_END])
{
    /* $DAN,<TIME>,<DELTA>,<C0><C1>...<CN> */
    uint16_t pos;

    assert(sDa1 != NULL);
    assert(buffer != NULL);

    pos = 0;

    buffer[pos] = PROTOCOL_FRAME_START;
    pos += PROTOCOL_FRAME_START_SIZE;

    memcpy(buffer + pos, protocol_FrameId[cProtocolFrameDAN], PROTOCOL_FRAME_TYPE_SIZE);
    pos += PROTOCOL_FRAME_TYPE_SIZE;

    buffer[pos] = PROTOCOL_FRAME_SEP;
    pos += PROTOCOL_FRAME_SEP_SIZE;

    endian_copyToB(buffer + pos, &(sDa1->time), PROTOCOL_FRAME_TIME_SIZE, PROTOCOL_FRAME_TIME_SIZE);
    pos += PROTOCOL_FRAME_TIME_SIZE;

    buffer[pos] = PROTOCOL_FRAME_SEP;
    pos += PROTOCOL_FRAME_SEP_SIZE;

    endian_copyToB(buffer + pos, &delta, PROTOCOL_FRAME_TIME_SIZE, PROTOCOL_FRAME_TIME_SIZE);
    pos += PROTOCOL_FRAME_TIME_SIZE;

    pos += protocol_extendDAN(sDa1->fsrValues, sDa1->fscValues, buffer + pos);

    assert(pos == PROTOCOL_DAN_MIN_SIZE - PROTOCOL_FRAME_END);

    return pos;
}

uint16_t protocol_extendDAN(uint16_t const fsrValues[PROTOCOL_FSR_NUMBER], uint16_t const fscValues[PROTOCOL_FSC_NUMBER], uint8_t buffer[PROTOCOL_DAN_VAR_SIZE])
{
    uint16_t pos;

    assert(fsrValues != NULL);
    assert(fscValues!= NULL);
    assert(buffer != NULL);

    pos = 0;

    buffer[pos] = PROTOCOL_FRAME_SEP;
    pos += PROTOCOL_FRAME_SEP_SIZE;
        
    endian_copyToB(buffer + pos, fsrValues, PROTOCOL_FRAME_FSR_SIZE * PROTOCOL_FSR_NUMBER, PROTOCOL_FRAME_FSR_SIZE);
    pos += PROTOCOL_FRAME_FSR_SIZE * PROTOCOL_FSR_NUMBER;
    
    buffer[pos] = PROTOCOL_FRAME_SEP;
    pos += PROTOCOL_FRAME_SEP_SIZE;
    
    endian_copyToB(buffer + pos, fscValues, PROTOCOL_FRAME_FSC_SIZE * PROTOCOL_FSC_NUMBER, PROTOCOL_FRAME_FSC_SIZE);
    pos += PROTOCOL_FRAME_FSC_SIZE * PROTOCOL_FSC_NUMBER;

    assert(pos == PROTOCOL_DAN_VAR_SIZE);

    return pos;
}

inline uint16_t protocol_endDAN(uint8_t buffer[PROTOCOL_FRAME_END_SIZE])
{
    buffer[0] = PROTOCOL_FRAME_END;
    
    return PROTOCOL_FRAME_END_SIZE;
}
