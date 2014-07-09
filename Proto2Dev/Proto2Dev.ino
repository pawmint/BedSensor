#include <Wire.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ADS7828.h"
#include "_24XX1026.h"
#include "protocol.h"

#include "xbee_api.h"

#define BUFFER_SIZE 100

bool mysleep(uint32_t delay, uint32_t timeout);
uint8_t sleepMode(const uint32_t timeMax, const uint32_t delay, const uint16_t delta, uint8_t buffer[BUFFER_SIZE], uint16_t* bufferPos);
void manager(void);
bool setAPIMode(uint8_t mode);

void sendBytesFunc(unsigned char* buffer, unsigned long size)
{
    Serial.write(buffer, size);
}

void flushFunc(void)
{
  Serial.flush();
}
bool getFSRSensor(uint16_t* values)
{
	ADS7828_getAllValues(0b00, ADS7828_AD_CONVERTER_ON | ADS7828_INTERNAL_REF_ON | ADS7828_SINGLE_ENDED_I, false, values);

	return true;
}


char monRead(void){
	char rep;
	scanf("%c%*c", &rep);
	return rep;
}

char (*protocol_readChar)(void) = &monRead;


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
	protocol_createACK(ackBuf);
	bufferPos += protocol_createYOP(buffer + bufferPos);
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
        *id |= (((uint32_t) buf1[j]) << (28 - (j + (8 - i)) * 4));
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
		*id |= (((uint32_t) buf1[j]) << (28 - (j + (8 - i)) * 4));
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
	struct sProtocolDR1 sDr1;
	
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
			getFSRSensor(sDr1.fsrValues);
			i = 0;
			do
			{
				bActivity = ( (((oldValues[i] > sDr1.fsrValues[i]) ? oldValues[i] : sDr1.fsrValues[i]) - ((oldValues[i] < sDr1.fsrValues[i]) ? oldValues[i] : sDr1.fsrValues[i])) >= delta);
				i++;
			} while (!bActivity && i < PROTOCOL_FSR_NUMBER);
			if (bActivity)
			{
				
				sDr1.time = millis();
				*bufferPos += protocol_createDR1(&sDr1, buffer + *bufferPos);
			}
			else
			{
				bStop = mysleep(delay, timeout);
			}
			memcpy(oldValues, sDr1.fsrValues, PROTOCOL_FSR_NUMBER * PROTOCOL_FRAME_FSR_SIZE);
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
