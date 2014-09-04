#include <Wire.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ONESENSOR

#ifdef ONESENSOR
    #include "ADS7828.h"
#endif

// #include "_24XX1026.h"
#include "protocol.h"
#include "communicating.h"
#include "xbee_api.h"
#include "sampling.h"



#define BUFFER_SIZE 100

bool mysleep(uint32_t delay, uint32_t timeout);
uint8_t sleepMode(const uint32_t timeMax, const uint32_t delay, const uint16_t delta, uint8_t buffer[BUFFER_SIZE], uint16_t* bufferPos);
void manager(void);
bool setAPIMode(uint8_t mode);

void sendBytesFunc(unsigned char* buffer, unsigned long size)
{
    Serial.write(buffer, size);
}

static inline void flushFunc(void)
{
    Serial.flush();
}

uint8_t buffer[BUFFER_SIZE];
uint8_t ackBuf[PROTOCOL_ACK_SIZE];
uint16_t bufferPos = 0;
uint16_t old;
xbee_TxFrame64 tx64;


void setup()
{
    tx64.frameId = 1;
    tx64.destinationAddress = 0xFFFFLL;
    tx64.options = 0;

	Serial.begin(9600);
	ADS7828_init();
    xbee_Init(NULL, NULL, NULL, NULL, NULL, NULL, &sendBytesFunc, &flushFunc);
	protocol_ACK_create(ackBuf, NULL);
	bufferPos += protocol_YOP_create(buffer + bufferPos, NULL);
        setAPIMode(1);
        
        tx64.data = buffer;
    tx64.dataSize = bufferPos;
    xbee_SendTxFrame64(&tx64);
	bufferPos = 0;
}

void loop()
{
	
	old = bufferPos;
	if (sleepMode(10000, 1000, 512, buffer, &bufferPos))
	{
        tx64.data = buffer;
        tx64.dataSize = bufferPos;
        xbee_SendTxFrame64(&tx64);
		bufferPos=0;
	}
	else if (bufferPos != old)
	{
        tx64.data = buffer;
        tx64.dataSize = bufferPos;
        xbee_SendTxFrame64(&tx64);
		bufferPos=0;
		delay(1000);
	}
	else
    {
        tx64.data = ackBuf;
        tx64.dataSize = PROTOCOL_ACK_SIZE;
        xbee_SendTxFrame64(&tx64);
    }
}

uint8_t readSerial()
{
  while(!Serial.available());
  return Serial.read();
}

bool readOK()
{
  	if (readSerial() != 'O')
        return false;
	if (readSerial() != 'K')
		return false;
	if (readSerial() != '\r')
		return false;
  return true;
}

bool startAT()
{
  delay(2000);
  Serial.print("+++");
  delay(2000);
  return readOK();
}
bool stopAT()
{
  Serial.print("ATCN\r");
  return readOK();
}

bool getATSL(uint32_t* id)
{
	bool stop = false;
	uint8_t i = 0;
	uint8_t j;
	uint8_t buf;
	uint8_t buf1[8];

        *id=0;
	Serial.print("ATSL\r");
	do 
	{
		buf = readSerial();
		stop = (buf == '\r');
		if (!stop)
		{
                        if(i > 7)
			  return false;
			buf1[i++] = buf;
		}
	} while (!stop);
	for ( j = 0 ; j < i ; j++)
	{
		buf1[j] -= (isDigit(buf1[j]) ? '0' : ('A' - 10));
        *id |= (((uint32_t) buf1[j]) << (28 - (j + (8 - i)) *  4));
	}
	
	*id = endian_htonl(*id);
	return true;
}

bool getATSH(uint32_t* id)
{
        
	bool stop = false;
	uint8_t i = 0;
	uint8_t j;
	uint8_t buf;
	uint8_t buf1[8];

        *id=0;
	Serial.print("ATSH\r");
	do 
	{
		buf = readSerial();
		stop = (buf == '\r');
		if (!stop)
		{
                        if(i > 7)
			  return false;
			buf1[i++] = buf;
		}
	} while (!stop);
	for ( j = 0 ; j < i ; j++)
	{
		buf1[j] -= (isDigit(buf1[j]) ? '0' : ('A' - 10));
		*id |= (((uint32_t) buf1[j]) << (28 - (j + (8 - i)) *  4));
	}
	
	*id = endian_htonl(*id);
	return true;
}

bool setAPIMode(uint8_t mode)
{
    bool bOk = false;
    
    if (mode < 3)
    {
        startAT();
        Serial.print("ATAP ");
        Serial.print(mode);
        Serial.print("\r");
        bOk = readOK();
        stopAT();
        
    }
    return bOk;

}

uint8_t sleepMode(const uint32_t timeMax, const uint32_t delay, const uint16_t delta, uint8_t buffer[BUFFER_SIZE], uint16_t* bufferPos)
{
	uint32_t timeout = timeMax + millis();
	static bool bFirst = true;
	static uint16_t oldValues[PROTOCOL_FSR_NUMBER];
	bool bFull = false;
	bool bStop = false;
	bool bActivity = false;
	uint8_t i;
	struct sProtocolDR1 sData;
	
	assert(delta < 4096);
	
	if (bFirst)
	{
		getFSRSensor(oldValues);
		bFirst = false;
	}
	if (*bufferPos + PROTOCOL_DR1_SIZE > BUFFER_SIZE)
	bFull = true;
	else
	{
		bActivity = false;
		do {
			getFSRSensor(sData.fsrValues);
			i = 0;
			do
			{
				bActivity = ( (((oldValues[i] > sData.fsrValues[i]) ? oldValues[i] : sData.fsrValues[i]) - ((oldValues[i] < sData.fsrValues[i]) ? oldValues[i] : sData.fsrValues[i])) >= delta);
				i++;
			} while (!bActivity && i < PROTOCOL_FSR_NUMBER);
			if (bActivity)
			{
				
				sData.time = millis();
				*bufferPos += protocol_DR1_create(buffer + *bufferPos, &sData);
			}
			else
			{
				bStop = mysleep(delay, timeout);
			}
			memcpy(oldValues, sData.fsrValues, PROTOCOL_FSR_NUMBER *  PROTOCOL_FRAME_FSR_SIZE);
		} while (!bStop && !bActivity);
	}

	return bFull;
}


bool mysleep(uint32_t time, uint32_t timeout)
{
	bool ret;
	uint32_t timeoutDelay = timeout - millis();
	uint32_t ref = (ret = (timeoutDelay < time)) ? timeoutDelay : time;
	delay(ref);

	return ret;
}
