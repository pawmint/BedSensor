#include <Wire.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ADS7828.h"
#include "_24XX1026.h"
#include "protocol.h"

#define BUFFER_SIZE 128

bool mysleep(uint32_t delay, uint32_t timeout);
bool acquisitionMode(const uint32_t timeMax, const uint32_t fsrDelay, const uint32_t fscDelay, uint8_t buffer[BUFFER_SIZE], uint16_t* bufferPos);
uint8_t sleepMode(const uint32_t timeMax, const uint32_t delay, const uint16_t delta, uint8_t buffer[BUFFER_SIZE], uint16_t* bufferPos);
void manager(void);

bool sendData(uint8_t* buffer, uint16_t size)
{
	Serial.write(buffer, size);
	return true;
}

bool getFSRSensor(uint16_t* values)
{
	uint8_t i;
	ADS7828_getAllValues(0b00, ADS7828_AD_CONVERTER_ON | ADS7828_INTERNAL_REF_ON | ADS7828_SINGLE_ENDED_I, false, values);

	return true;
}

bool getFSCSensor(uint16_t* values)
{
	uint16_t i;
	for (i = 0 ; i < PROTOCOL_FSC_NUMBER ; i++)
		values[i] = random() / RAND_MAX * 2048;

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

void setup()
{
	Serial.begin(9600);
	ADS7828_init();
	protocol_createACK(ackBuf);
	bufferPos += protocol_createYOP(buffer + bufferPos);
	sendData(buffer, bufferPos);
	bufferPos = 0;
}

void loop()
{
	
	old = bufferPos;
	if (sleepMode(10000, 1000, 512, buffer, &bufferPos))
	{
		sendData(buffer, bufferPos);
		bufferPos = 0;
	}
	else if (bufferPos != old)
	{
		sendData(buffer, bufferPos);
		bufferPos=0;
		delay(1000);
	}
	else
		sendData(ackBuf, PROTOCOL_ACK_SIZE);
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

bool acquisitionMode(const uint32_t timeMax, const uint32_t fsrDelay, const uint32_t fscDelay, uint8_t buffer[BUFFER_SIZE], uint16_t* bufferPos)
{
	static uint32_t globalTimeout;
	static uint32_t fsrTimeout;
	static uint32_t fscTimeout;
	static bool bStarted = false;
	uint32_t millisSave;
	bool bStop = false;
	bool bFull = false;
	struct sProtocolDR1 sDr1;
	struct sProtocolDC1 sDc1;

	assert(fsrDelay > fscDelay);
	assert(buffer != NULL);
	assert(bufferPos != NULL);

	if (!bStarted)
	{
		if (*bufferPos + PROTOCOL_DR1_SIZE + PROTOCOL_DC1_SIZE > BUFFER_SIZE)
			bFull = true;
		else
		{
			sDr1.time = sDc1.time = millisSave = millis();
			getFSRSensor(sDr1.fsrValues);
			
			getFSCSensor(sDc1.fscValues);
			*bufferPos += protocol_createDR1(&sDr1, buffer + *bufferPos);
			*bufferPos += protocol_createDC1(&sDc1, buffer + *bufferPos);
			bStarted = true;
			globalTimeout   = millisSave + timeMax;
			fsrTimeout      = millisSave + fsrDelay;
			fscTimeout      = millisSave + fscTimeout;
			bStop = mysleep(fscDelay, globalTimeout);

		}
	}
	if (bStarted)
	{
		bool bDcnInit = false;
		do
		{
			if (fscTimeout > fsrTimeout)
			{
				if (bDcnInit)
				{
					*bufferPos = protocol_endDCN(buffer + *bufferPos);
					bDcnInit = false;
				}
				if (*bufferPos + PROTOCOL_DR1_SIZE > BUFFER_SIZE)
					bFull = true;
				else
				{
					sDr1.time = millis();
					getFSRSensor(sDr1.fsrValues);
					*bufferPos += protocol_createDR1(&sDr1, buffer + *bufferPos);
					fsrTimeout += fsrDelay;
				}
			}
			else
			{
				if (!bDcnInit)
				{
					// if (!(bFull = (*bufferPos + PROTOCOL_DCN_MIN_SIZE > BUFFER_SIZE)))
					if (*bufferPos + PROTOCOL_DCN_MIN_SIZE > BUFFER_SIZE)
						bFull = true;
					else
					{
						sDc1.time = millis();
						getFSCSensor(sDc1.fscValues);
						*bufferPos += protocol_initDCN(&sDc1, fscDelay, buffer + *bufferPos);
						bDcnInit = true;
					}
				}
				else
				{
					if (*bufferPos + PROTOCOL_DCN_VAR_SIZE + PROTOCOL_FRAME_END_SIZE > BUFFER_SIZE)
					bFull = true;
					else
					{
						getFSCSensor(sDc1.fscValues);
						*bufferPos += protocol_extendDCN(sDc1.fscValues, buffer + *bufferPos);
					}
				}
				// fscTimeout += !bFull * fscDelay;
				if (!bFull)
					fscTimeout += fscDelay;
				
			}
			if (!bFull)
			{
				millisSave = millis();
				if (millisSave > globalTimeout)
					bStop = true;
				else if ((millisSave < fscTimeout) && (millisSave < fsrTimeout))
					bStop = mysleep(((fscTimeout < fsrTimeout) ? fscTimeout : fsrTimeout) - millisSave, globalTimeout);
			}
		} while (!bFull && !bStop);
		if (bDcnInit)
		{
			*bufferPos = protocol_endDCN(buffer + *bufferPos);
			bDcnInit = false;
		}
		if (bStarted && bStop)
		bStarted = false;
	}

	return bFull;
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
